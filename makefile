
ROOT_DIR:=$(PWD)
SRC_ROOT:=$(ROOT_DIR)/src

BUILD_TYPE:="Release"
BUILD_DIR=$(ROOT_DIR)/build
CONAN_MARKER=$(BUILD_DIR)/conanbuildinfo.cmake


MAIN_EXE=build/bin/mapmerge

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

format:
	@echo clang-tidy src

.PHONY: main
main: debug
	${MAIN_EXE}

.PHONY: test testgrid
test: test-all

test-bounds: debug
	build/bin/geometry-tests BoundTests

test-frame: debug
	build/bin/geometry-tests FrameTests

test-polygon: debug
	build/bin/geometry-tests PolygonTests

test-rolling: debug
	build/bin/rolling-grid-tests

test-qtree: debug
	build/bin/quad-tree-tests

test-static: debug
	build/bin/static-grid-tests

test-search: debug
	build/bin/search-tests

test-all: 
	clear
	build/bin/geometry-tests
	build/bin/rolling-grid-tests
#	build/bin/search-tests

