# xStl API documentation

The API reference is generated from the Doxygen comments in `Include/xStl/**/*.h`.

## Quick start (Windows, PowerShell)

```powershell
winget install DimitriVanHeesch.Doxygen        # once
powershell -File docs\build-docs.ps1            # = docs-all: audit, HTML + XML, coverage metrics
start docs\build\html\index.html
```

## Quick start (Linux / macOS)

```sh
make -f docs/Makefile docs-all                  # needs doxygen and GNU make; docs-metrics also needs PowerShell 7 (pwsh)
```

## Targets

| Target | What it does | Output |
| --- | --- | --- |
| `docs-api` | HTML + XML in one Doxygen run (`docs/Doxyfile`) | `docs/build/html`, `docs/build/xml` |
| `docs-html` | HTML only | `docs/build/html` |
| `docs-xml` | XML only (input for Sphinx + Breathe, doxybook2, ...) | `docs/build/xml` (`index.xml`) |
| `docs-pdf` | PDF reference manual through LaTeX. Needs `pdflatex`, `makeindex` and `epstopdf` (TeX Live, MacTeX or MiKTeX) | `docs/build/xStl-reference.pdf` |
| `docs-check` | Audit with `EXTRACT_ALL=NO` and all `WARN_*` options. **Fails on any warning** | `docs/build/check` |
| `docs-metrics` | Documentation coverage measured from the XML | console |
| `docs-examples` | Compile and run `docs/examples/*.cpp` (MSVC x64, needs a built `xStl.lib`) | `docs/build/examples` |
| `docs-all` | `docs-check`, `docs-api`, `docs-metrics` | |
| `docs-clean` | Remove `docs/build` | |

`docs/build-docs.ps1 -Target <name>` and `make -f docs/Makefile <name>` accept the same names
(the Makefile has no `docs-examples`; `docs-pdf` is not part of `docs-all`).

## Files

| File | Purpose |
| --- | --- |
| `Doxyfile` | Production configuration (`EXTRACT_ALL=YES`, HTML + XML, source browser) |
| `Doxyfile.check` | Includes `Doxyfile`, switches to `EXTRACT_ALL=NO` and generates warnings only |
| `groups.dox` | Module groups (`xstl_core`, `xstl_data`, `xstl_stream`, ...). Headers join them with `@addtogroup` |
| `examples/` | Compiled examples that are included in the group pages (`@include`) |
| `scripts/doc-metrics.ps1` | Coverage (symbols, `@param`, `@return`, `@tparam`) from the XML |
| `scripts/doc-worklist.ps1` | Lists symbols with missing documentation, per header and line |
| `scripts/doc-warnings.ps1` | Classifies a Doxygen warning log |
| `scripts/verify-code-unchanged.ps1` | Proves that only comments differ from a git revision (code, macros and strings identical) |
| `baseline/` | The configurations used for the "before" measurement |
| `doxygen-baseline.md`, `doxygen-report.md`, `documentation-progress.md` | Reports |

## Writing documentation

* Use `/** ... */` or `///` comments (plain `/* */` comments are ignored by Doxygen).
* Document every public symbol: `@brief`, then `@param`, `@tparam`, `@return`, `@throws` as they apply.
* Put a new header into a group with `@addtogroup <group> @{` ... `@}` just inside the include guard.
* Run `docs-check` before committing: it must report 0 warnings.
* Documentation changes must not change code. Verify with
  `powershell -File docs\scripts\verify-code-unchanged.ps1` (compares against `HEAD`).
