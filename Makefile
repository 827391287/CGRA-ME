#.SUFFIXES: # no built-in suffix rules
#MAKEFLAGS += -r # no other built-in rules

# CMake Build Type. Possible values: Debug, Release, RelWithDebInfo and MinSizeRel
BUILD_TYPE = release
#BUILD_TYPE_NORMALIZED = $(shell printf '%s' '$(BUILD_TYPE)' | tr [:upper:] [:lower:])

# Passes & code for generating DFGs from annotated C files.
# Requires LLVM development headers
BUILD_LLVM_PASSES = ON

# Build directory
BUILD_DIR = ./build

# Makefile generated from CMake
GENERATED_MAKEFILE = $(BUILD_DIR)/Makefile

# CMake flags
CMAKE_FLAGS =\
	-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
	-DBUILD_LLVM_PASSES=$(BUILD_LLVM_PASSES) \
	-DCGRAME_CXX_FLAGS=$(CXXFLAGS) \

# Command to invoke CMake. Will be empty if not in $PATH
CMAKE ?= $(shell command -v cmake 2> /dev/null)

# Changes in these files trigger a run of CMake
CMAKE_FILES = CMakeLists.txt\
	$(shell find src doc inc thirdparty -name 'CMakeLists.txt') \
	$(shell find cmake -type f) \

.PHONY: all
all: $(GENERATED_MAKEFILE)
	@ echo "Running build all..."
	@ $(MAKE) -C $(BUILD_DIR) --no-print-directory all

.PHONY: clean
clean: $(GENERATED_MAKEFILE)
	@ echo "Running clean, removing generated files..."
	@ $(MAKE) -C $(BUILD_DIR) --no-print-directory clean

.PHONY: fullclean
fullclean: 
	@ echo "Running fullclean, removing the build results completely..."
	$(MAKE) clean -C thirdparty/coreir
	rm -rf $(BUILD_DIR)

$(GENERATED_MAKEFILE): $(CMAKE_FILES)
	@ if [ -z "$(CMAKE)" ]; then echo 'CMake not found in $$PATH. Please install cmake and/or run with Makefile with CMAKE=/my/path/to/cmake'; exit 1; fi
	@ echo "Running CMake with arguments $(CMAKE_FLAGS)"
	@ mkdir -p $(BUILD_DIR)
	@ echo $(CMAKE_FLAGS)
	@ cd $(BUILD_DIR) && $(CMAKE) $(CMAKE_FLAGS) ..


