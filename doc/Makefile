# User command-line options
SPHINXOPTS    ?=
SPHINXBUILD   ?= python3 -msphinx
SPHINXPROJ    ?= CGRA-ME
SOURCEDIR     ?= source
BUILDDIR      ?= build
DOXYGENDIRS   ?= ../src/ ../inc doxygen-extra
# building the PDF takes a while... you may want to disable it when building locally
BUILD_REFMAN ?= ON

DOXYGEN_REFMAN = $(BUILDDIR)/doxygen/latex/refman.pdf

# Remove all implicit rules and suffixes
.SUFFIXES:
MAKEFLAGS += -r

# Main/default target
.PHONY: build-docs
build-docs: $(BUILDDIR)/sphinx_html $(BUILDDIR)/doxygen_build

ifeq ($(BUILD_REFMAN),ON)
build-docs: $(DOXYGEN_REFMAN)
endif

# Cleaning target
.PHONY: clean
clean:
	-rm -r $(BUILDDIR)

# Calls sphinx with the builder set to the pattern leaf. Tocuhes a file to mark the competion time.
# run `$(SPHINXBUILD) -M help a b c` to see a list
$(BUILDDIR)/sphinx_%: Makefile $(shell find $(SOURCEDIR) -not -type d)
	$(SPHINXBUILD) -qb $* $(SOURCEDIR) $(BUILDDIR) $(SPHINXOPTS)
	@touch $@

$(BUILDDIR)/doxygen_build: Makefile $(shell find $(DOXYGENDIRS) -not -type d) Doxyfile
	doxygen
	@touch $@

$(DOXYGEN_REFMAN): $(BUILDDIR)/doxygen_build
	$(MAKE) -C $(BUILDDIR)/doxygen/latex > /dev/null
