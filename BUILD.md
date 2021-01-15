# Build

Finding Lane Lanes with Cpp

## Install dependencies

To run this project, we need to install [Emscripten](https://emscripten.org/) and [OpenCV](https://opencv.org/).

### Emscripten SDK

Install and activate backend.

```sh
cd $HOME
git clone git@github.com:emscripten-core/emsdk.git
source ${HOME}/emsdk/emsdk_env.sh
emsdk install latest
emsdk activate latest
source ${HOME}/emsdk/emsdk_env.sh
```

### OpenCV for WASM target

Install and build OpenCV for [emscripten](https://github.com/opencv/opencv/tree/master/platforms/js) wasm target.

In [build_js.py](https://github.com/opencv/opencv/blob/master/platforms/js/build_js.py#L171-#L174) under "build_wasm" add flags (or other memory limit) `-s WASM_MEM_MAX=512MB -s DEMANGLE_SUPPORT=0`

before:

```py
def get_build_flags(self):
    flags = ""
    if self.options.build_wasm:
        flags += "-s WASM=1 "
```

after:

```py
def get_build_flags(self):
    flags = ""
    if self.options.build_wasm:
        flags += "-s WASM=1 -s WASM_MEM_MAX=512MB -s DEMANGLE_SUPPORT=0 "
```

## Run Project

### Compile to emscripten target

```sh
git clone git@github.com:Horki/lanelines_wasm_emscripten.git
cd lanelines_wasm_emscripten
# Add dependency OpenCV
git submodule update --init --recursive
# Build OpenCV
cd opencv
# Make sure python v3.4+
python ./platforms/js/build_js.py --build_wasm --emscripten_dir $HOME/emsdk/upstream/emscripten build_wasm
source ${HOME}/emsdk/emsdk_env.sh
cd ..
# Build
mkdir -p build && cd build
emcmake cmake ..
# On Linux
emmake make -j $(nproc)
# On OsX
# emmake make -j $(sysctl -n hw.ncpu)
```
