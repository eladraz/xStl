# xStl Doxygen baseline

Measured **before any documentation change** (source at commit `156d7df`), with
Doxygen 1.18.0 on the public headers under `Include/xStl/` (103 `.h` files).

Baseline configurations are kept in `docs/baseline/`:

| File | Purpose |
|---|---|
| `Doxyfile.all` | `EXTRACT_ALL=YES`, XML + HTML, no predefined macros |
| `Doxyfile.audit` | `EXTRACT_ALL=NO`, warnings only, no predefined macros |
| `Doxyfile.pd-all` | as `Doxyfile.all` plus `PREDEFINED = WIN32 XSTL_UNICODE XSTL_64BIT DOXYGEN` |
| `Doxyfile.pd-audit` | as `Doxyfile.audit` plus the same `PREDEFINED` |

The `pd-*` pair is the one used for the before/after comparison, because the
final production `Doxyfile` uses the same predefined macros (they expose the
Windows-only headers and the Unicode/64-bit API variants that would otherwise
be hidden behind `#ifdef`).

Metrics are computed from the Doxygen XML by `docs/scripts/doc-metrics.ps1`
and warnings are classified by `docs/scripts/doc-warnings.ps1`.

## 1. API inventory

"Public API symbol" = public class/struct/union, public member of one, or a
file-level macro, typedef, enum, function or variable (enum values included).

| Kind | Symbols | Documented |
|---|---:|---:|
| class | 106 | 4 |
| struct | 5 | 0 |
| union | 1 | 0 |
| function (incl. constructors, destructors, operators) | 784 | 10 |
| variable / constant | 72 | 3 |
| macro (`#define`) | 137 | 0 |
| typedef | 57 | 2 |
| enum | 18 | 0 |
| enum value | 64 | 0 |
| **Total** | **1244** | **19** |

Without predefined macros the total is 1235 (4 Windows-only headers, `winOSdef.h`
and `winFilename.h` among them, produce no symbols because of `#ifdef WIN32`).
`osdef.h` and `xStlPrecompiled.h` only select/include other headers and define
no symbols.

## 2. Documentation coverage

| Metric | Baseline |
|---|---:|
| Public API symbols | 1244 |
| Symbols with a brief or detailed description | 19 |
| **Documentation coverage** | **1.5 %** |
| Function parameters (named) | 666 |
| Parameters with a `@param` description | 0 |
| Non-void functions needing `@return` | 441 |
| Functions with `@return` | 0 |
| Template parameters | 41 |
| Template parameters with `@tparam` | 0 |

### By module

| Module | Symbols | Documented | Coverage |
|---|---:|---:|---:|
| core (`types.h`, `operators.h`, `refCountObject.h`, `remoteAddress.h`, `exceptions.h`) | 91 | 0 | 0.0 % |
| data | 352 | 1 | 0.3 % |
| enc | 88 | 8 | 9.1 % |
| except | 37 | 0 | 0.0 % |
| os | 206 | 3 | 1.5 % |
| parser | 86 | 1 | 1.2 % |
| stream | 333 | 0 | 0.0 % |
| utils | 51 | 6 | 11.8 % |

The 19 documented symbols come from the few places that already use
Doxygen comment forms: `enc/digest/Crc16.h`, `enc/digest/Crc64.h`,
`utils/TimeoutMonitor.h`, plus single symbols in `parser/types.h`, `data/char.h`, `utils/callbacker.h`, `os/file.h` and `os/filename.h` (all `///` and `/** */` comments).

## 3. Warnings and errors

With `EXTRACT_ALL=YES` Doxygen reports **0 warnings** (it suppresses the
"not documented" warnings in that mode), no parse errors, and no broken
references. The `EXTRACT_ALL=NO` audit reports **1292 warnings**:

| Warning category | Count |
|---|---:|
| undocumented function | 827 |
| undocumented compound (class/struct/union) | 110 |
| undocumented variable | 89 |
| parameters not (all) documented | 249 |
| undocumented enum | 7 |
| undocumented typedef | 2 |
| undocumented other member | 4 |
| return type not documented | 4 |
| broken references | 0 |
| **Total** | **1292** |

Macros and enum values are not reported by Doxygen's warnings even when
undocumented, which is why the symbol-level coverage above (computed from the
XML) is the more reliable measure.

## 4. Existing documentation quality

The headers already contain a lot of useful explanatory text, but it is
invisible to Doxygen:

| Comment form in the headers | Count | Recognised by Doxygen |
|---|---:|---|
| `/* ... */` blocks | 1249 | no (needs `/**` or `/*!`) |
| `//` comment lines | 578 | no (needs `///` or `//!`) |
| `/** ... */` blocks | 13 | yes |
| `///` lines | 15 | yes |

Observed conventions that the upgrade preserves:

* Each declaration has a `/* ... */` block directly above it: a short
  description, then `parameter - description` lines, then a `Return ...` /
  `Throws ...` sentence. These map directly to `@brief`, `@param`,
  `@return` and `@throws`.
* Class blocks start with the class name on its own line.
* Several headers contain `Usage:`/`Using:` examples (hash, list, callbacker,
  dumpMemory, ...), written as indented text.
* File headers contain the file name, a description and `Author: ...`.
* The wording of many comments was non-native English; spelling and grammar
  were corrected in commits `643f47a`/`156d7df` before this work.

Quality gaps found:

* No parameter, return value, template parameter or exception is described in
  a form Doxygen understands (0 of 666 / 441 / 41).
* Ownership, thread safety and platform behaviour are only mentioned in
  prose, and only for some classes (for example `cCounter`, `cStringerStream`,
  `cSmartPtr`).
* No `@defgroup` organisation; the HTML reference would be a flat list.
* A few comments describe the wrong thing (copy-paste), e.g. the socket
  stream header says network order is little-endian. These are recorded in
  `docs/doxygen-report.md` instead of being silently rewritten.

## 5. Prioritised recommendations

1. Convert existing `/* */` comments to `/** */` with `@brief`/`@param`/
   `@return`/`@throws`/`@note`, keeping the original wording (mechanical,
   covers most of the 1249 blocks).
2. Add `@file` headers and `@addtogroup` per architectural module
   (`core`, `data`, `enc`, `except`, `os`, `parser`, `stream`, `utils`).
3. Document the symbols that have no comment at all (mostly operators,
   overloads and constants) only where the behaviour is verifiable from the
   implementation or tests; record the rest as open items.
4. Escape characters that Doxygen treats as markup (`<`, `>`, `\`, `&`, `@`).
5. Add compiled examples taken from the tests.
6. Provide a production `Doxyfile`, `make`-style entry points and a coverage
   gate so that regressions are detected.


## Addendum: corrected counts

The first version of `doc-metrics.ps1` merged overloaded functions that share a name. With overloads counted separately the baseline is **1355** public symbols, **20** documented (1.5 %), **827** parameters, **494** non-void functions, **41** template parameters (all with no `@param`/`@return`/`@tparam`). The 1292 warnings are unaffected. `doxygen-report.md` uses these corrected numbers.
