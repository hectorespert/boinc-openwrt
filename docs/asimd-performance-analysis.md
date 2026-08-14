# ASIMD (NEON) Performance Analysis — PeriodSearch for ARM Routers

## Purpose

This document is intended to be consumed by a coding agent tasked with implementing
performance improvements to the ASIMD (ARM NEON) code path of the
[PeriodSearch](https://github.com/AsteroidsAtHome/PeriodSearch/tree/dev) upstream project,
as used by the `period-search` OpenWrt package in this repository.

---

## Target Hardware

All improvements focus on the `aarch64` code path compiled with `-DARM64`, targeting:

| SoC | Core | Router examples |
|-----|------|----------------|
| BCM2837 | Cortex-A53 | RPi 3 + OpenWrt |
| BCM2711 | Cortex-A72 | RPi 4 + OpenWrt |
| BCM2712 | Cortex-A76 | RPi 5 + OpenWrt |
| Generic | ARMv8-A | aarch64_generic devices |

Key microarchitecture constraints:
- A53: **in-order** pipeline, 1 NEON FMA unit, `vfmaq_f64` = 1 cycle throughput / 4 cycle latency
- A72/A76: out-of-order, 2 NEON FMA units, better at exploiting multiple accumulators
- L1 data cache: 32 KB (all); cache line = 64 bytes = **8 × double**
- `vdivq_f64` latency: **15–23 cycles** on A53, **10–15 cycles** on A72 — avoid in hot paths

---

## Source File Locations (upstream repo)

All files are in `period_search_optimization_simd/period_search/` of the
`AsteroidsAtHome/PeriodSearch` repository, branch `dev`.

| File | Class | Role |
|------|-------|------|
| `bright_asimd.cpp` | `CalcStrategyAsimd` | Brightness + derivatives per observation point |
| `mrqcof_asimd.cpp` | `CalcStrategyAsimd` | Marquardt coefficient matrix accumulation |
| `gauss_errc_asimd.cpp` | `CalcStrategyAsimd` | Gaussian elimination for linear system |
| `conv_asimd.cpp` | `CalcStrategyAsimd` | Convexity regularization |
| `curv_asimd.cpp` | `CalcStrategyAsimd` | Curvature / facet area from Laplace series |
| `CpuInfoArm64.cpp` | free functions | Runtime SIMD capability detection |
| `CalcStrategyAsimd.hpp` | header | Class declaration + private storage |

### Key data structure (`globals` struct, `arrayHelpers.hpp`)

```cpp
// All arrays are 64-byte aligned via __attribute__((aligned(64)))
double Nor[3][MAX_N_FAC + 8];              // facet normals, layout [component][facet]
double Area[MAX_N_FAC + 8];                // facet areas
double Darea[MAX_N_FAC + 8];              // area differentials
double Dg[MAX_N_FAC + 16][MAX_N_PAR + 8]; // shape derivatives [facet][param]
double dyda[MAX_N_PAR + 16];              // current point derivatives [param]
AlignedOuterVector alpha;                  // Marquardt alpha matrix (mfit × mfit)
AlignedOuterVector covar;                  // covariance matrix
```

Relevant constants from `constants.h`:
```cpp
#define MAX_N_FAC   1000    // max facets (Numfac typically 512–2048)
#define MAX_N_PAR    200    // max parameters (ma typically 100–400)
#define MAX_LM        10    // max Laplace degree/order
#define TINY         1e-8   // visibility threshold for lmu, lmu0
```

---

## Improvement 1 — `mrqcof_asimd.cpp`: Replace division with reciprocal multiply

### Priority: 🔴 HIGH — highest impact, lowest effort

### Problem

Inside the `Lcurves` outer loop, when `gl.Inrel[i] == 1` (relative lightcurve), the
normalization step for each observation point `jp` contains:

```cpp
float64x2_t avx_ave = vdupq_n_f64(gl.ave);   // computed ONCE per lightcurve i

for (jp = 1; jp <= gl.Lpoints[i]; jp++) {
    // ...
    float64x2_t avx_ytemp = vld1q_dup_f64(&gl.ytemp[jp]);
    for (l = 1; l <= ma; l += 2) {
        float64x2_t avx_dytemp = vld1q_f64(&gl.dytemp[jp][l]);
        float64x2_t avx_dave   = vld1q_f64(&gl.dave[l]);
        // ↓ vdivq_f64 here — 15–23 cycle latency on A53, called ma/2 times per jp
        avx_dytemp = vsubq_f64(avx_dytemp,
                         vdivq_f64(vmulq_f64(avx_ytemp, avx_dave), avx_ave));
        avx_dytemp = vmulq_f64(avx_dytemp, avx_coef);
        vst1q_f64(&gl.dytemp[jp][l], avx_dytemp);
    }
}
```

`avx_ave` is a **loop-invariant** scalar (constant for the entire `jp` loop).
The division `x / ave` is computed `(ma/2) × Lpoints[i]` times, but divides by the
same constant every time.

### Fix

Compute the reciprocal **once** before the `jp` loop:

```cpp
// Compute once per lightcurve i — ave is constant for all jp in this i
float64x2_t avx_inv_ave = vdupq_n_f64(1.0 / gl.ave);

for (jp = 1; jp <= gl.Lpoints[i]; jp++) {
    float64x2_t avx_ytemp = vld1q_dup_f64(&gl.ytemp[jp]);
    // Pre-multiply ytemp * inv_ave once per jp (scalar cost, not per-l)
    float64x2_t avx_yt_over_ave = vmulq_f64(avx_ytemp, avx_inv_ave);

    for (l = 1; l <= ma; l += 2) {
        float64x2_t avx_dytemp = vld1q_f64(&gl.dytemp[jp][l]);
        float64x2_t avx_dave   = vld1q_f64(&gl.dave[l]);
        // ↓ Replace vdivq_f64 with vmulq_f64 — 1 cycle throughput
        avx_dytemp = vsubq_f64(avx_dytemp, vmulq_f64(avx_dave, avx_yt_over_ave));
        avx_dytemp = vmulq_f64(avx_dytemp, avx_coef);
        vst1q_f64(&gl.dytemp[jp][l], avx_dytemp);
    }
}
```

### Expected gain

- On A53: saves ~14–22 cycles × (ma/2) per `(jp, i)` pair where `Inrel == 1`
- On A72: saves ~9–14 cycles × (ma/2)
- Relative lightcurves are the common case in Asteroids@home data sets

---

## Improvement 2 — `conv_asimd.cpp`: Use scalar FMA to eliminate redundant vector multiply

### Priority: 🔴 HIGH — simple change, applies to every period candidate tested

### Problem

```cpp
for (auto i = 0; i < Numfac; i++) {
    double *Dg_row = gl.Dg[i];
    float64x2_t avx_Darea = vdupq_n_f64(gl.Darea[i]);     // scalar → vector
    float64x2_t avx_Nor   = vdupq_n_f64(gl.Nor[nc-1][i]); // scalar → vector

    for (auto j = 0; j < Ncoef; j += 2) {
        float64x2_t avx_dres = vld1q_f64(&gl.dyda[j]);
        float64x2_t avx_Dg   = vld1q_f64(&Dg_row[j]);
        // ↓ vmulq_f64(avx_Darea, avx_Dg) creates a temp vector — wasted instruction
        avx_dres = vfmaq_f64(avx_dres, vmulq_f64(avx_Darea, avx_Dg), avx_Nor);
        vst1q_f64(&gl.dyda[j], avx_dres);
    }
}
```

`Darea[i]` and `Nor[nc-1][i]` are both loop-invariant scalars for the inner `j` loop.
Their product `factor = Darea[i] * Nor[nc-1][i]` can be computed once as a `double`
and used with `vfmaq_n_f64` (FMA with scalar broadcast), avoiding the intermediate vector.

### Fix

```cpp
for (auto i = 0; i < Numfac; i++) {
    double *Dg_row = gl.Dg[i];
    // Compute scalar factor once — replaces two vdupq_n_f64 + one vmulq_f64
    const double factor = gl.Darea[i] * gl.Nor[nc - 1][i];

    for (auto j = 0; j < Ncoef; j += 2) {
        float64x2_t avx_dres = vld1q_f64(&gl.dyda[j]);
        float64x2_t avx_Dg   = vld1q_f64(&Dg_row[j]);
        // vfmaq_n_f64: dst = dst + Dg * factor  (scalar broadcast is free on ARM)
        avx_dres = vfmaq_n_f64(avx_dres, avx_Dg, factor);
        vst1q_f64(&gl.dyda[j], avx_dres);
    }
}
```

### Expected gain

- Removes 1 `vmulq_f64` + 2 `vdupq_n_f64` per outer iteration `i`
- `conv` is called once per observation point for the convexity lightcurve — O(N_obs) total
- Saves approximately Numfac × 3 instructions per `conv` call

---

## Improvement 3 — `bright_asimd.cpp`: Unaligned stores for rotation-parameter derivatives

### Priority: 🟠 MEDIUM

### Problem

At the end of `bright`, five `vst1q_f64` stores write to potentially misaligned
positions in `gl.dyda`. The upstream source itself acknowledges this with comments:

```cpp
vst1q_f64(&gl.dyda[ncoef0-3+1-1], avx_dyda1);  //unaligned memory because of odd index
vst1q_f64(&gl.dyda[ncoef0-3+3-1], avx_dyda3);  //unaligned memory because of odd index
vst1q_f64(&gl.dyda[ncoef-1-1],    avx_d);       //unaligned memory because of odd index
```

`vst1q_f64` does not fault on unaligned ARM addresses but incurs a **1-cycle penalty**
on A53 and a potential **cache-line split** (2× cache traffic) if the 16-byte store spans
a 64-byte boundary.

### Fix

Replace each unaligned 2-lane store with two individual 1-lane stores:

```cpp
// Instead of: vst1q_f64(&gl.dyda[idx], avx_val);  // potentially misaligned
// Use:
vst1q_lane_f64(&gl.dyda[idx],     avx_val, 0);
vst1q_lane_f64(&gl.dyda[idx + 1], avx_val, 1);
```

`vst1q_lane_f64` stores a single `double` which is always naturally 8-byte aligned
(guaranteed by `double`'s alignment). This eliminates cache-line splits.

### Prerequisite check

Verify that `ncoef0` is always odd for real Asteroids@home work units before applying,
to confirm the misalignment is systematic and the fix is necessary.

---

## Improvement 4 — `gauss_errc_asimd.cpp`: Increase inner loop unrolling from ×2 to ×4

### Priority: 🟡 LOW-MEDIUM

### Problem

The two performance-critical inner loops process rows of the `n × n` covariance matrix
with stride-2 (2 doubles = 16 bytes per iteration), leaving 3/4 of each 64-byte cache
line unused per iteration:

```cpp
// Scale pivot row:
for (l = 0; l < (n - 1); l += 2) {
    float64x2_t avx_a1 = vld1q_f64(&a[icol][l]);
    avx_a1 = vmulq_f64(avx_a1, avx_pivinv);
    vst1q_f64(&a[icol][l], avx_a1);
}

// Eliminate column:
for (l = 0; l < (n - 1); l += 2) {
    float64x2_t avx_a  = vld1q_f64(&a[ll][l]);
    float64x2_t avx_aa = vld1q_f64(&a[icol][l]);
    float64x2_t avx_result = vmlsq_f64(avx_a, avx_aa, avx_dum);
    vst1q_f64(&a[ll][l], avx_result);
}
```

With `n` up to ~400 and the outer `i` loop running `n` times, this is O(n²) work.

### Fix

Unroll to 4 doubles per iteration with interleaved loads to hide FPU latency on A53:

```cpp
// Scale pivot row — unroll ×4:
int l = 0;
for (; l <= n - 4; l += 4) {
    float64x2_t a0 = vld1q_f64(&a[icol][l]);
    float64x2_t a1 = vld1q_f64(&a[icol][l + 2]);
    a0 = vmulq_f64(a0, avx_pivinv);
    a1 = vmulq_f64(a1, avx_pivinv);
    vst1q_f64(&a[icol][l],     a0);
    vst1q_f64(&a[icol][l + 2], a1);
}
for (; l < n; l++) a[icol][l] *= pivinv;   // tail (0–3 elements)

// Eliminate column — unroll ×4:
l = 0;
for (; l <= n - 4; l += 4) {
    float64x2_t a0  = vld1q_f64(&a[ll][l]);
    float64x2_t a1  = vld1q_f64(&a[ll][l + 2]);
    float64x2_t aa0 = vld1q_f64(&a[icol][l]);
    float64x2_t aa1 = vld1q_f64(&a[icol][l + 2]);
    a0 = vmlsq_f64(a0, aa0, avx_dum);
    a1 = vmlsq_f64(a1, aa1, avx_dum);
    vst1q_f64(&a[ll][l],     a0);
    vst1q_f64(&a[ll][l + 2], a1);
}
for (; l < n; l++) a[ll][l] -= a[icol][l] * dum;  // tail
```

The interleaved pattern (load `a0`, load `a1`, compute `a0`, compute `a1`) hides the
4-cycle FPU latency on A53 by keeping the load unit busy.

---

## Improvement 5 — `bright_asimd.cpp`: Early exit for zero visible facets

### Priority: 🟠 MEDIUM

### Problem

When no facets are visible (`incl_count == 0`), the accumulator variables
`avx_dyda1..3`, `avx_d`, `avx_d1` are all zero, but `bright` continues to:
1. Execute the `vpaddq_f64` + `vst1q_lane_f64` for `ymod` (trivial)
2. Execute the **entire** g-coefficient derivative loop (`ncoef03` elements)
3. Write zeros to `gl.dyda[0..ncoef03]` through the full loop structure

```cpp
// This entire section runs even when incl_count == 0:
for (i = 0; i < cyklus1; i += 10) {
    // ... 5 tmp accumulators, inner j loop over incl_count (skipped if 0)
    vst1q_f64(&gl.dyda[i],   tmp1 * Scale);  // writes 0 * Scale = 0
    // ... 4 more stores
}
```

Low-angle observations (sun or observer near asteroid's orbital plane) can result in
`incl_count == 0` for many period candidates, making this a significant wasted path.

### Fix

Add an explicit early-exit after the main facet loop:

```cpp
// After the Numfac loop ends:
if (incl_count == 0) {
    // All derivatives are zero, ymod is zero
    gl.ymod = 0.0;
    for (int ii = 0; ii < ncoef; ii++) gl.dyda[ii] = 0.0;
    return;
}
```

Place this check immediately after the `for (i = 0; i < Numfac; i += 2)` loop and
before the g-coefficient derivative section.

---

## Improvement 6 — `curv_asimd.cpp`: Align Dg store base offset

### Priority: 🟡 LOW

### Problem

```cpp
for (k = 1; k < n; k += 2) {
    float64x2_t avx_pom = vld1q_f64(&Dsph[i][k]);     // load Dsph[i][k]
    avx_pom = vmulq_f64(avx_pom, avx_g);
    vst1q_f64(&gl.Dg[i - 1][k - 1], avx_pom);          // store Dg[i-1][k-1]
}
```

The loop starts at `k=1` with load from `Dsph[i][1]` (8 bytes into the row = aligned
for a 16-byte NEON load) and stores to `Dg[i-1][0]` (row start = 64-byte aligned).
The asymmetric offset between load and store is a minor but addressable issue.

### Fix

Rewrite the loop to start at `k=0` and adjust indices explicitly:

```cpp
int k;
for (k = 0; k + 1 < n; k += 2) {
    float64x2_t avx_pom = vld1q_f64(&Dsph[i][k + 1]);
    avx_pom = vmulq_f64(avx_pom, avx_g);
    vst1q_f64(&gl.Dg[i - 1][k], avx_pom);
}
// Handle last odd element if n is even (original: k == n)
if (k < n) {
    gl.Dg[i - 1][k] = g * Dsph[i][k + 1];
}
```

The store `&gl.Dg[i-1][k]` at `k=0` is at the 64-byte aligned row start.

---

## Implementation Notes for the Agent

### Files to modify (all in upstream `AsteroidsAtHome/PeriodSearch`, branch `dev`)

1. `period_search_optimization_simd/period_search/mrqcof_asimd.cpp` — Improvement 1
2. `period_search_optimization_simd/period_search/conv_asimd.cpp` — Improvement 2
3. `period_search_optimization_simd/period_search/bright_asimd.cpp` — Improvements 3, 5
4. `period_search_optimization_simd/period_search/gauss_errc_asimd.cpp` — Improvement 4
5. `period_search_optimization_simd/period_search/curv_asimd.cpp` — Improvement 6

### Files NOT to modify

- `CalcStrategyAsimd.hpp` — No changes to private member layout needed
- `CpuInfoArm64.cpp` — Out of scope for this set of improvements
- `bright.cpp`, `mrqcof.cpp`, etc. (`CalcStrategyNone` scalar path) — Out of scope

### Constraints

- All NEON intrinsics must use `<arm_neon.h>` types (`float64x2_t`, `uint64x2_t`, etc.)
- Do **not** remove the `__attribute__((__target__("arch=armv8-a+simd")))` guard from
  `bright_asimd.cpp`, `mrqcof_asimd.cpp`, `conv_asimd.cpp`, `curv_asimd.cpp`.
  Only `gauss_errc_asimd.cpp` has this attribute already removed by the OpenWrt patch
  `001-drop-gcc14-asimd-target-attribute.patch`.
- All changes must be **numerically equivalent** — the reciprocal multiply in Improvement 1
  replaces `x / ave` with `x * (1/ave)` which is floating-point equivalent given that `ave`
  is a fixed positive scalar computed before the loop.
- Code must compile with GCC 13+ and Clang 17+ with `-march=armv8-a -O3 -std=gnu++2a`

### Verification

For each improvement:
1. Compile with `aarch64-openwrt-linux-musl-g++` using `-march=armv8-a -O3 -std=gnu++2a`
   and check for zero new warnings
2. Run a known-good input/output pair through the modified binary to confirm identical results
3. Optionally use `perf stat -e cycles,instructions` on an A53 or A72 board to measure speedup

---

## Priority Summary

| # | File | Change | Priority | Effort |
|---|------|--------|----------|--------|
| 1 | `mrqcof_asimd.cpp` | Replace `vdivq_f64` with reciprocal multiply | 🔴 HIGH | Low |
| 2 | `conv_asimd.cpp` | `vfmaq_n_f64` with scalar factor | 🔴 HIGH | Low |
| 3 | `bright_asimd.cpp` | Aligned lane stores for rotation derivatives | 🟠 MEDIUM | Low |
| 4 | `gauss_errc_asimd.cpp` | Unroll ×4 with interleaved loads | 🟡 LOW-MED | Medium |
| 5 | `bright_asimd.cpp` | Early exit when `incl_count == 0` | 🟠 MEDIUM | Low |
| 6 | `curv_asimd.cpp` | Align `Dg` store base offset | 🟡 LOW | Low |
