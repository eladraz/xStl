# Doxygen documentation progress

Legend: `[x]` done and verified, `[ ]` pending.

## Phase 1 - Baseline
- [x] Doxygen 1.18.0 installed
- [x] Baseline configs created (`docs/baseline/`)
- [x] Baseline HTML/XML generated (EXTRACT_ALL=YES)
- [x] Warning audit generated (EXTRACT_ALL=NO)
- [x] Metrics scripts written (`docs/scripts/doc-metrics.ps1`, `doc-warnings.ps1`)
- [x] `docs/doxygen-baseline.md` written

## Phase 2 - Documentation upgrade
- [x] Comment converter written and validated
- [x] core (types, operators, refCountObject, remoteAddress, exceptions)
- [x] data
- [x] enc
- [x] except
- [x] os
- [x] parser
- [x] stream
- [x] utils
- [x] Module groups (`@defgroup`) and `@file` headers
- [x] Remaining undocumented public symbols documented or recorded

## Phase 3 - Generate and validate
- [x] Production `Doxyfile`
- [x] Warning-free (or classified) EXTRACT_ALL=NO audit
- [x] HTML + XML generated

## Phase 4 - Reports
- [x] `docs/doxygen-report.md`

## Phase 5 - Repeatable pipeline
- [x] Build entry points (`docs-api`, `docs-html`, `docs-xml`, `docs-check`, `docs-all`)
- [x] `.gitignore`, regeneration instructions

## Phase 6 - Final verification
- [x] Windows build + tests still pass
- [x] Executable code unchanged (comment-stripped diff identical)

## Notes
- Final measurements and the list of remaining limitations: [doxygen-report.md](doxygen-report.md).
- `docs/Makefile` was written but not executed (GNU make is not installed on the Windows verification machine).
