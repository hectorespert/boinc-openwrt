# AGENTS.md

Guidance for coding agents working in this repository.

## What this repository is

An OpenWrt **package feed** (not an application). Every directory at the top level is one OpenWrt
package: a `Makefile` written in OpenWrt's package language plus a `files/` (installed assets) and
optional `patches/` directory. There is no source code of our own beyond a handful of shim files —
the Makefiles fetch upstream sources and cross-compile them.

Target: OpenWrt **25.12**, packaged as APK (`.adb`), published to GitHub Pages as a signed feed at
`https://hectorespert.github.io/boinc-openwrt/<arch>/packages.adb`.

## Build and test

There is no local build system; builds only happen through the OpenWrt SDK. CI
([build-pr.yml](.github/workflows/build-pr.yml) on PRs, [build-and-publish.yml](.github/workflows/build-and-publish.yml)
on `main`) runs `openwrt/gh-action-sdk` across an 8-architecture matrix with:

```
ARCH=<arch>-openwrt-25.12
FEEDNAME=boinc_openwrt
PACKAGES="boinc-upstream period-search binary-radio-pulsar-search fftw3 gsl libxml2-static"
```

To build locally, mount this repo as a feed inside an OpenWrt 25.12 SDK container and run
`make package/<name>/compile V=s`. Building a single package still requires its `PKG_BUILD_DEPENDS`
to be staged first (`boinc` before the apps; `fftw3 gsl libxml2-static zlib` before
binary-radio-pulsar-search).

Running `make` from the repository root does nothing useful: every package `Makefile` starts with
`include $(TOPDIR)/rules.mk` and is only valid inside an SDK tree. Editing a Makefile therefore has no
local verification step — review the source list and flags by hand; the PR build is the real check.

The architecture matrix is duplicated in **both** workflows and again in the `index.html` heredoc and
the README table — adding an architecture means editing all four places.

## Architecture of the feed

**Dependency layering.** `boinc-upstream` builds the BOINC client *and* installs headers and static
libs into the staging dir via `Build/InstallDev`. It declares `PROVIDES:=boinc`, so the science
applications depend on the abstract name `boinc` and link against `-lboinc_api -lboinc` from
`$(STAGING_DIR)/usr/lib`. `gsl`, `fftw3` and `libxml2-static` exist only because the OpenWrt feeds'
versions do not ship what binary-radio-pulsar-search needs (static archives, single-precision FFTW,
`libgslcblas`); they are build-time dependencies linked with `-Wl,-Bstatic`, not runtime ones.

**Applications are compiled by hand, not by upstream build systems.** Both `period-search` and
`binary-radio-pulsar-search` stub out `Build/Configure` and invoke `$(TARGET_CXX)` directly with an
explicit list of source files. Upstream's autotools/CMake/MSVC build is bypassed entirely. Consequence:
when a source file is added, renamed, or removed upstream, the `Build/Compile` block must be updated
by hand or the link fails.

`period-search` branches on `$(ARCH)` in `Build/Compile` because the SIMD source set differs:
`aarch64` compiles the `*_asimd.cpp` files with `-DARM64`, `x86_64` compiles the SSE/AVX/FMA/AVX512
variants, and every other architecture (mips, riscv) uses [FallbackCpuInfo.cpp](period-search/files/FallbackCpuInfo.cpp),
a local no-SIMD stand-in for upstream's `CpuInfo*.cpp`, and the scalar sources only.

**Anonymous-platform deployment.** The applications are not distributed by the BOINC project for
these architectures, so they are installed as anonymous-platform apps: a binary named
`*_openwrt_$(ARCH_PACKAGES)` plus an `app_info.xml` under `/opt/<app>/`. The `app_info.xml` files in
`files/` are templates — `Package/*/install` rewrites them with `$(SED)`, substituting the literal
`aarch64` in the file name for the real `$(ARCH_PACKAGES)` and `@BOINC_API_VERSION@` for the version
read out of the staged `boinc/version.h` by the `BOINC_API_VERSION` awk one-liner. Keep those two
placeholders exactly as they are when editing `app_info.xml`.

The app init scripts ([period-search.init](period-search/files/period-search.init),
[binary-radio-pulsar-search.init](binary-radio-pulsar-search/files/binary-radio-pulsar-search.init))
do not start daemons: at `START=98` they copy `/opt/<app>/*` into the BOINC project directory
(`/opt/boinc/projects/<project-host>`) and chown it to `boinc`, before the client starts at `START=99`.

**The client runs procd-jailed.** [boinc-client.init](boinc-upstream/files/boinc-client.init) uses
`procd_add_jail ... requirejail`, so the client sees only what is explicitly mounted. Anything the
client (or a science app) shells out to or reads must be added as a `procd_add_jail_mount` and, for
binaries, pulled in as a package `DEPENDS` — this is why `boinc-upstream` depends on `coreutils-stat`.
A missing mount shows up as a runtime failure inside the jail, invisible at build time.

**Patches.** `boinc-upstream/patches/` are plain quilt patches applied to the upstream git checkout:
one avoids linking freetype when the manager is disabled, one makes the client honour the
`HOSTTYPE`/`HOSTTYPEALT` platform names passed via `--with-boinc-platform` /
`--with-boinc-alt-platform` instead of guessing. The alt-platform string
(`$(ARCH)-$(BOARD)-$(DEVICE_TYPE)-openwrt-$(TARGET_SUFFIX)`) is what BOINC projects match work
against, so changing it changes which work units the client is offered.

## Version bumps

- `boinc-upstream`: git source; `PKG_SOURCE_VERSION` is derived from `PKG_VERSION`
  (`client_release/<major.minor>/<full>`), and `PKG_MIRROR_HASH` must be updated with it.
- `period-search`: a *branch snapshot* tarball (`dev.tar.gz`), so `PKG_HASH` changes whenever
  upstream pushes; `PKG_VERSION` is a date stamp and `PKG_BUILD_DIR` is pinned to `PeriodSearch-dev`.
- `binary-radio-pulsar-search`: `brp-src-release.zip` from einsteinathome.org, unpacked by a custom
  `Build/Prepare` (the zip has no single root dir). That same hook injects
  [erp_git_version.h](binary-radio-pulsar-search/files/erp_git_version.h) (upstream generates it from
  git, absent in the release zip) and seds a `-Wformat-security` violation in `erp_utilities.cpp`.
- Bumping any package's contents requires bumping `PKG_RELEASE` so apk sees a new version.

## Signing and publishing

`main` builds are signed with the `SIGNING_KEY` secret (base64 EC private key); the publish workflow
derives `public-key.pem` with `openssl ec -pubout` and ships it alongside the packages, and users add
it to `/etc/apk/keys/`. PR builds run unsigned. The user-facing site (`index.html`, `robots.txt`,
`sitemap.xml`) is generated inline in the publish workflow, not stored in the repo.
