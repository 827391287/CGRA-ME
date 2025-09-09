#remove all implicit rules
MAKEFLAGS += -r

## BEGIN INTERFACE ##
# This makefile is also sensitive to CGRA_MAPPER, CGRA_MAPPER_ARGS and CGRA_ARCH_ARGS.

# Space separated list of loop names to extract. Should only modify if extracting multiple loops
LOOP_TAGS ?= loop

# Passed directly to the compiler when compiling the code; before DFG generation
CFLAGS ?= -O3

# Optimizations that are usually not wanted -- they assume a more complicated ISA, and may remove loops altogether.
OPT_DISABLE_FLAGS ?= -fno-vectorize -fno-slp-vectorize -fno-unroll-loops

# Extra flags pased when creating the DFG
DFG_FLAGS ?=

## END INTERFACE ##

BENCHNAME ?= $(shell basename `pwd`)

LLVM_DFG_PLUGIN ?= $(wildcard $(CGRA_ME_LIBDIR)llvm-pass-dfggen.*)
LLVM_MAJOR_VERSION ?= $(shell llvm-config --version | cut -d. -f1)
USE_NEW_PASS_MANAGER = $(shell test '$(LLVM_MAJOR_VERSION)' -ge 14 && echo yes || echo no)
ifeq ($(USE_NEW_PASS_MANAGER),yes)
# v14 requires --load and --load-pass-plugin to add the cmd-line args
RUN_DFG_PLUGIN_FLAGS = --load '$(LLVM_DFG_PLUGIN)' --load-pass-plugin '$(LLVM_DFG_PLUGIN)' --passes=dfg-out
else
RUN_DFG_PLUGIN_FLAGS = --load '$(LLVM_DFG_PLUGIN)' --dfg-out
endif

FINAL_DFG_FLAGS = $(RUN_DFG_PLUGIN_FLAGS) -loop-tags '$(LOOP_TAGS)' --in-tag-pairs '$(BENCHNAME).tag' $(DFG_FLAGS)

LOOP_PARSER ?= $(CGRA_ME_SCRIPTSDIR)LoopParser.py

DFG_TARGETS = $(patsubst %,graph_%.dot,$(LOOP_TAGS))

## COMMON TARGETS ##

.PHONY: build run ll

build: $(DFG_TARGETS)

run: run_mapper

ll: $(BENCHNAME).ll

## FOR BUILDING ##

.INTERMEDIATE: $(BENCHNAME).bc $(BENCHNAME).tag

ifeq ($(LLVM_DFG_PLUGIN),)
%.dot: pre-gen-%.dot
	@msg='WARNING: Using pre-generated graph loop. DFG will not reflect changes to the source file(s)'; echo "$$msg"; echo "$$msg" >&2
	cp $< $@
else
$(DFG_TARGETS): $(BENCHNAME).bc $(BENCHNAME).tag
	@echo LLVM_MAJOR_VERSION=$(LLVM_MAJOR_VERSION)
	@echo USE_NEW_PASS_MANAGER=$(USE_NEW_PASS_MANAGER)
	opt '$(BENCHNAME).bc' -o '/dev/null' $(FINAL_DFG_FLAGS)
endif

%.bc: %.tagged.c
	clang -emit-llvm -c '$*.tagged.c' -o '$*.bc' $(CFLAGS) $(OPT_DISABLE_FLAGS)

%.ll: %.bc
	llvm-dis '$*.bc'

%.tagged.c %.tag: %.c
	'$(LOOP_PARSER)' '$*.c' '$*.tagged.c' '$*.tag'

clean:
	rm -f '$(BENCHNAME).bc' '$(BENCHNAME).ll' $(DFG_TARGETS) '$(BENCHNAME).tag' '$(BENCHNAME).tagged.c'

## FOR RUNNING ##

.PHONY: prepare_run run_mapper

prepare_run: build

run_mapper:
	'$(CGRA_MAPPER)' $(CGRA_MAPPER_ARGS) $(CGRA_ARCH_ARGS) -g $(firstword $(DFG_TARGETS))
