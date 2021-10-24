
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

.PHONY: test
test: test-all

test-a-star: debug
	build/bin/a-star-search-tests

test-all: test-geometry test-layer test-search

test-geometry: debug
	build/bin/geometry-tests

test-layer: debug
	build/bin/common-layer-tests
	build/bin/dynamic-layer-tests
	build/bin/rolling-layer-tests
	build/bin/simple-layer-tests

test-search: debug
	build/bin/a-star-search-tests
