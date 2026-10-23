# xStl Doxygen documentation report

All numbers in this report were measured with the scripts in `docs/scripts` and the commands in
`docs/README.md`; nothing is estimated. Doxygen 1.18.0, Windows 11, MSVC 2022 Build Tools (v143).

## 1. Scope

* 103 public headers under `Include/xStl/` (102 carry documentation; `xStlPrecompiled.h` is an internal
  precompiled-header shim and was left untouched).
* Only comments changed. No `.cpp`/`.inl` file, no macro, template, signature, or ABI-relevant line was edited
  (see section 5).

## 2. API documentation coverage (before / after)

Measurement: `docs/scripts/doc-metrics.ps1` on the Doxygen XML generated with `EXTRACT_ALL=YES` and the same
`PREDEFINED = WIN32 XSTL_UNICODE XSTL_64BIT DOXYGEN` for both runs. Public members only; include guards are not
counted; overloads are counted separately.

| Metric | Before | After |
| --- | ---: | ---: |
| Public API symbols | 1355 | 1355 |
| Symbols with a brief or detailed description | 20 (1.5 %) | 1355 (100 %) |
| Function parameters documented (`@param`) | 0 / 827 | 827 / 827 |
| Functions with `@return` (non-void, non-constructor) | 0 / 494 | 494 / 494 |
| Template parameters documented (`@tparam`) | 0 / 41 | 41 / 41 |
| Doxygen warnings, `EXTRACT_ALL=NO`, all `WARN_*` on | 1292 | 0 |
| Doxygen warnings, `EXTRACT_ALL=YES` | 0 | 0 |
| Broken references / parse errors | 0 | 0 |
| Doxygen comment blocks in the headers (`/**` or `///`) | 28 | 2010 |
| Modules / groups | none | 19 (`docs/groups.dox`) |
| Compiled examples | 0 | 4 (`docs/examples`) |

By kind (documented / total): class 106/106, struct 5/5, union 1/1, function 895/895, define 137/137,
typedef 57/57, enum 18/18, enum value 64/64, variable 72/72.
By module: core 92, data 419, enc 92, except 39, os 214, parser 88, stream 358, utils 53 - all 100 %.

Note on the baseline numbers: the first baseline (`docs/doxygen-baseline.md`) reported 1244 symbols and 19
documented, because the first version of the metrics script merged overloaded functions that share a name. The
script was corrected (overloads are kept apart) and the baseline XML was re-measured with the corrected script;
the table above uses the corrected values for both columns. The warning count (1292) is unaffected.

"Documented" in this table means that a brief or detailed description exists (and that every parameter, return
value and template parameter has its tag). It is a completeness measure, not a proof that every sentence is
perfect; see section 4.

## 3. Quality of the result

* License headers are unchanged.
* Existing comments were converted to Doxygen (1441 blocks, by a converter that was validated by the
  "code unchanged" check), then reviewed module by module and completed by hand: missing `@param`, `@return`,
  `@tparam`, `@throws`, undocumented macros, enum values, typedefs, variables and overloads.
* Prose that the converter had wrongly wrapped into `@code` blocks was repaired (e.g. `os.h`, `arguments.h`,
  `string.h`, `parser.h`).
* Behavioural statements were written only when they could be read from the code (e.g. `cHashFunction`
  semantics, `cList::iterator::operator <`, digest sizes in 32-bit words, `compare()` being a stub).
* Modules (`@defgroup`): Core, Data (containers, strings, pointers, serialization, dump), Encryption (digest,
  cipher, random), Exceptions, OS (+ Windows / UNIX / XDK backends), Parser, Streams, Utilities. Each header joins
  its group with `@addtogroup`.
* Examples are compiled and executed by `docs/examples/build-examples.ps1` and included in the group pages:
  containers, digest (RFC test vectors), endianness, exceptions.

## 4. Remaining issues

Nothing is open in the Doxygen audit (0 warnings). Known limitations:

1. **Platform code that Doxygen cannot see.** The configuration predefines `WIN32 XSTL_UNICODE XSTL_64BIT`.
   Code under `#ifdef XSTL_LINUX`, `XSTL_MACOSX`, `XSTL_NTDDK`/XDK, `XSTL_CE` and `DOT_NET` is only documented
   where it is visible to the preprocessor (`unixOSdef.h`, `ddkOSdef.h`, `DbgCom.h` are documented but their
   inner `#ifdef` branches were not compiled or verified on those platforms). `os/WindowsOS/*.h` is wrapped in
   `#ifdef WIN32` and is only visible because `WIN32` is predefined.
2. **Not verifiable on this machine:** builds and tests on Linux/macOS, the XDK/CE targets, and `make` (GNU make is
   not installed on the Windows machine, so `docs/Makefile` was written but **not executed**; the PowerShell
   pipeline `docs/build-docs.ps1` was executed for every target).
3. **Trivial members** (copy constructors, default constructors, `operator =`) have short generic descriptions
   such as "Copy-constructor." - they are complete, but not informative.
4. **The `.inl` template implementations** and `Source/` were not touched (the task covered public headers).
5. `cMessageQueue` / `cNullGroup` (`data/messageQueue.h`) declare `template <cClientID, cGroupID>` without
   `class`/`typename`. Doxygen accepts it; a compiler would not. Documented as is (no code changes allowed).

## 5. Code integrity

* `docs/scripts/verify-code-unchanged.ps1` strips all comments from every changed header at `HEAD` and in the
  working tree and compares: **0 of 102 files differ**.
* `git status`: only `Include/xStl/**/*.h` (102 files), `.gitignore` and the new `docs/` tree changed.
* Windows build + test (MSBuild, toolset v143, `XSTL_PATH` set), all four configurations:

| Configuration | Build | `xStlTests.exe` |
| --- | --- | --- |
| Debug Win32 | success | `OK!` |
| Debug x64 | success | `OK!` |
| Release Win32 | success | `OK!` |
| Release x64 | success | `OK!` |

## 6. Suspected bugs and inaccurate original comments (documentation only; code not changed)

| Where | Observation |
| --- | --- |
| `data/array.h` `cArray::iterator::operator +` / `-` | The original briefs were swapped ("Decrease" on `+`). Documentation now matches the code. |
| `operators.h` `MAKE_INCREASE_OPERATORS` | The postfix `++`/`--` return a reference to the object itself (after the change), not the previous value. Documented with a `@note`; probably a bug. |
| `data/list.h` `cList::iterator::operator <` | Returns true when both iterators are equal (it scans from this position inclusive). Documented. |
| `data/graph.h` `GraphNode::m_chosenColor` | The comment says "Initialized as 0" but the constructor initializes it with `-1` (stored in a `uint`). |
| `utils/algorithm.h` `compare()` | Not implemented: `ASSERT(false)` and returns `true`. Documented with `@warning`. |
| `data/hash.h` `cHashFunction(float)` | Hashes through a pointer cast of the float bits (`*(uint*)&index`) (strict-aliasing). |
| `stream/socketStream.h` | "network order bits (Little-endian)": network byte order is big-endian; the class uses `littleEndianImpl`. |
| `types.h` | A comment says "Linux is big-endian platform" next to a definition of `XSTL_LITTLE_ENDIAN`. |
| `os/os.h` `smallMemoryAllocation` | The text says the block must be freed with `smallMemoryRealloc()`; the matching function is `smallMemoryFree()`. |
| `stream/basicIO.h` | `readPascal8String` was listed twice; the second entry is the 16 bit variant (fixed in the new text). |
| `enc/digest/sha1.h` | `SHA1_*` comment said "MD5 algorithm" (copy and paste). Fixed in the docs. |
| `data/string.h` `cString(const char ch, ...)` | Carried the documentation of the `const char* string` constructor. Fixed. |
| `enc/digest/md5.h` | The author name in the header comment was a single Latin-1 byte (invalid UTF-8, it broke the LaTeX build). Converted to UTF-8 (comment only). |

## 7. Deliverables

| Item | Location |
| --- | --- |
| Baseline report | `docs/doxygen-baseline.md` |
| This report | `docs/doxygen-report.md` |
| Progress tracking | `docs/documentation-progress.md` |
| Production Doxyfile / audit Doxyfile | `docs/Doxyfile`, `docs/Doxyfile.check` |
| HTML (561 pages) / XML (260 files) | `docs/build/html`, `docs/build/xml` (generated, ignored by git) |
| Scripts | `docs/build-docs.ps1`, `docs/Makefile`, `docs/scripts/*`, `docs/examples/build-examples.ps1` |
| Regeneration instructions | `docs/README.md` |

Regenerate everything: `powershell -File docs\build-docs.ps1` (or `make -f docs/Makefile docs-all`).
