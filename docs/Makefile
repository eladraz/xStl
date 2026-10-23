# Doxygen documentation targets for xStl.
#
#   make -f docs/Makefile docs-all        (from the repository root)
#   make docs-all                         (from the docs directory)
#
# The Windows PowerShell equivalent is docs/build-docs.ps1 (same target names).

DOXYGEN ?= doxygen
ROOT    := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/..)
PWSH    ?= pwsh

.PHONY: docs-all docs-api docs-html docs-xml docs-pdf docs-check docs-metrics docs-clean

docs-all: docs-check docs-api docs-metrics

# HTML + XML in one run
docs-api:
	cd $(ROOT) && rm -rf docs/build/html docs/build/xml && $(DOXYGEN) docs/Doxyfile

docs-html:
	cd $(ROOT) && rm -rf docs/build/html && ( cat docs/Doxyfile; echo "GENERATE_XML = NO" ) | $(DOXYGEN) -

docs-xml:
	cd $(ROOT) && rm -rf docs/build/xml && ( cat docs/Doxyfile; echo "GENERATE_HTML = NO" ) | $(DOXYGEN) -

# PDF reference manual (needs LaTeX: pdflatex and makeindex, e.g. TeX Live or MacTeX)
# Output: docs/build/xStl-reference.pdf
docs-pdf:
	cd $(ROOT) && rm -rf docs/build/latex && ( cat docs/Doxyfile; \
	    echo "GENERATE_HTML = NO"; echo "GENERATE_XML = NO"; echo "GENERATE_LATEX = YES"; \
	    echo "LATEX_OUTPUT = latex"; echo "USE_PDFLATEX = YES"; echo "PDF_HYPERLINKS = YES"; \
	    echo "LATEX_BATCHMODE = YES"; echo "COMPACT_LATEX = YES" ) | $(DOXYGEN) -
	$(MAKE) -C $(ROOT)/docs/build/latex
	cp $(ROOT)/docs/build/latex/refman.pdf $(ROOT)/docs/build/xStl-reference.pdf
	@echo "PDF: docs/build/xStl-reference.pdf"

# Audit with EXTRACT_ALL=NO; fails if Doxygen reports any warning
docs-check:
	cd $(ROOT) && rm -rf docs/build/check && $(DOXYGEN) docs/Doxyfile.check
	@cd $(ROOT) && if [ -s docs/build/check/doxygen-warnings.log ]; then \
	    head -n 50 docs/build/check/doxygen-warnings.log; echo "docs-check failed"; exit 1; \
	 else echo "docs-check passed: 0 Doxygen warnings with EXTRACT_ALL=NO"; fi

# Coverage metrics (uses PowerShell 7; on Windows use docs/build-docs.ps1)
docs-metrics:
	cd $(ROOT) && $(PWSH) -NoProfile -File docs/scripts/doc-metrics.ps1 -XmlDir docs/build/xml

docs-clean:
	rm -rf $(ROOT)/docs/build
