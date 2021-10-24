
ROOT_DIR:=$(PWD)
SRC_ROOT:=$(ROOT_DIR)/src

BUILD_TYPE:="Release"
BUILD_DIR=$(ROOT_DIR)/build
CONAN_MARKER=$(BUILD_DIR)/conanbuildinfo.cmake


SANDBOX_EXE=build/bin/sandbox

#-------------------------------------------------------------------
#  Part 2: Invoke the call to make in the build directory
#-------------------------------------------------------------------

.PHONY: debug
debug: BUILD_TYPE=Debug
debug: configure
	cd $(BUILD_DIR) && ninja

.PHONY: release
release: BUILD_TYPE=Release
release: configure
	cd $(BUILD_DIR) && ninja

.PHONY: conan
conan: $(CONAN_MARKER)

$(CONAN_MARKER): 
	@ echo "did not find conan-build-info file... rebuilding: "
	cd $(BUILD_DIR) && conan install --build=missing --profile=gcc-profile ..

configure: $(CONAN_MARKER)
	cd $(BUILD_DIR) && \
		cmake .. -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -GNinja

clean:
	rm -rf build/*
	rm -f *.png

format:
	@echo clang-tidy src

.PHONY: sandbox
sandbox: debug
	${SANDBOX_EXE}

.PHONY: test testgrid
test: test-all

test-dyn: debug
	build/bin/dynamic-layer-tests

test-frame: debug
	build/bin/geometry-tests FrameTests

test-geometry: debug
	build/bin/geometry-tests
	
test-layer: debug
	build/bin/common-layer-tests
	build/bin/dynamic-layer-tests
	build/bin/rolling-layer-tests
	build/bin/static-layer-tests

test-rolling: debug
	build/bin/rolling-grid-tests

test-qtree: debug
	build/bin/quad-tree-tests

test-static: debug
	build/bin/static-grid-tests

test-search: debug
	build/bin/search-tests

test-all:
	build/bin/geometry-tests
	build/bin/dynamic-layer-tests
	build/bin/common-layer-tests
#	build/bin/quad-tree-tests
	build/bin/rolling-layer-tests
	build/bin/static-layer-tests
#	build/bin/search-tests

