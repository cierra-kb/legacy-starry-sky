# Usage:
# export ANDROID_NDK_ROOT=<>
# make build

ifndef ANDROID_NDK_ROOT
$(error ANDROID_NDK_ROOT is not set)
endif

.PHONY: build

build:
	cmake -B build -DCMAKE_TOOLCHAIN_FILE=$(ANDROID_NDK_ROOT)/build/cmake/android.toolchain.cmake -DCMAKE_BUILD_TYPE:STRING=Release
	cmake --build build --config Release
clean:
	rm -rf ./build
