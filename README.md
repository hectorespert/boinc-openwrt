# boinc-openwrt

Custom [OpenWrt](https://openwrt.org/) package repository providing [BOINC](https://boinc.berkeley.edu/) client packages and applications for OpenWrt 25.12.

## Available packages

- **boinc-upstream** — BOINC client (upstream, tracks BOINC releases directly)
- **period-search** — Asteroids@home Period Search BOINC application
- **binary-radio-pulsar-search** — Einstein@Home Binary Radio Pulsar Search BOINC application (CPU)

## Supported architectures

| Architecture | Example devices |
|---|---|
| `aarch64_cortex-a53` | Raspberry Pi 3 with OpenWrt |
| `aarch64_cortex-a72` | Raspberry Pi 4 with OpenWrt |
| `aarch64_cortex-a76` | Raspberry Pi 5 with OpenWrt |
| `aarch64_generic` | Generic ARM64 devices |
| `arm_cortex-a7_neon-vfpv4` | Banana Pi R2, MediaTek MT7623 |
| `arm_cortex-a9_vfpv3-d16` | Turris Omnia, Linksys WRT1900AC |
| `arm_cortex-a15_neon-vfpv4` | Netgear R7800, Linksys EA8500 |
| `x86_64` | PC / server |
| `riscv64_generic` | Generic RISC-V 64-bit devices |

MIPS architectures are deliberately not built. Neither `mipsel_24kc` nor
`mips_mips32` has an FPU (24K**c**, not 24K**f**), so OpenWrt builds them
soft-float and every floating-point operation is emulated in software. The
science applications are double-precision bound, so a workunit would never
finish inside its deadline.

The 32-bit ARM architectures run the science applications with hardware
floating point but without SIMD: the vector paths use AArch64-only intrinsics
(`float64x2_t`), and armv7 NEON has no double precision.

## Usage

For instructions on how to install the packages, please visit the [package repository website](https://hectorespert.github.io/boinc-openwrt/).
