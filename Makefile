.PHONY: all
all: format build

.PHONY: format
format:
	clang-format src/* include/* example/*.cc -i

.PHONY: build
build:
	mkdir -p build
	cd build && \
	${HOME}/emsdk/fastcomp/emscripten/emconfigure cmake .. && \
	${HOME}/emsdk/fastcomp/emscripten/emmake make
.PHONY: debug
debug:
	mkdir -p build
	cd build && \
	${HOME}/emsdk/fastcomp/emscripten/emconfigure cmake -DCMAKE_BUILD_TYPE=debug .. && \
	${HOME}/emsdk/fastcomp/emscripten/emmake make

.PHONY: serve
serve: build
	mkdir -p web
	cp static/* web/
	cp img/img.jpg web/
	cp build/LaneLines.* web/
	basic-http-server web

.PHONY: clean
clean:
	rm -rf build web
