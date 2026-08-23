# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

**Read [AGENTS.md](AGENTS.md) first** — it holds the main instructions (what this feed is, how it is
built, the dependency layering, the hand-written compile blocks, the procd jail, version bumps and
publishing). Keep it as the single source of truth; add project knowledge there, not here. This file
is only for Claude Code specifics.

## Claude Code specifics

- **Nothing here is verifiable locally.** There are no tests, no linter, and no runnable build (see
  the build section in AGENTS.md). Do not spend turns trying to make `make` work in the working
  directory — read the upstream sources or the SDK docs instead, and say plainly that a Makefile
  change is unverified until CI builds it.
- **Cross-file edits are the usual failure mode.** Adding or removing an architecture, a package, or
  a source file touches several places at once (both workflows, the `index.html` heredoc, the README
  table; or `Build/Compile` in all its `$(ARCH)` branches). Grep for the string being changed across
  the repo before declaring the edit complete.
- Run `/code-review` on Makefile and workflow changes before opening a PR — a wrong `$(SED)` pattern
  or a dropped source file is exactly the kind of defect it catches and CI reports only after a full
  cross-compile.
- Upstream sources (BOINC, PeriodSearch, Einstein@Home) are not checked into this repo. When a
  `Build/Compile` source list needs updating, fetch the upstream tree rather than guessing file names.
