UNAME := $(shell uname)
NPROC=4
ifeq ($(UNAME), Darwin)
	NPROC=$(shell sysctl -n hw.ncpu)
else ifeq ($(UNAME), Linux)
	NPROC=$(shell nproc)
endif

.PHONY: all
all: format build

.PHONY: format
format:
	clang-format src/* include/* example/*.cc -i

.PHONY: build
build:
	mkdir -p build
	cd build && \
	${HOME}/emsdk/upstream/emscripten/emcmake cmake .. && \
	${HOME}/emsdk/upstream/emscripten/emmake make -j ${NPROC}
.PHONY: debug
debug:
	mkdir -p build
	cd build && \
	${HOME}/emsdk/upstream/emscripten/emcmake cmake -DCMAKE_BUILD_TYPE=debug .. && \
	${HOME}/emsdk/upstream/emscripten/emmake make -j ${NPROC}

.PHONY: serve
serve: build
	mkdir -p web
	cp static/* web/
	cp img/img.webp web/
	cp build/LaneLines.* web/
	basic-http-server web

.PHONY: clean
clean:
	rm -rf build web
