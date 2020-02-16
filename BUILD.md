# Build

Finding Lane Lanes with Cpp

## Install depedencies

To run this project, we need to install [Emscripten](https://emscripten.org/) and [OpenCV](https://opencv.org/).

### Emscripten SDK

Install and activate legacy (fastcomp) backend.

```sh
cd $HOME
git clone git@github.com:emscripten-core/emsdk.git
source ${HOME}/emsdk/emsdk_env.sh
emsdk install latest-fastcomp
emsdk activate latest-fastcomp
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

```sh
cd $HOME
wget --tries=5 "https://github.com/opencv/opencv/archive/3.4.7.zip"
unzip 3.4.7.zip
rm 3.4.7.zip             # optional
mv opencv-3.4.7/ opencv/ # optional
cd $HOME/opencv
# Add WASM mem flags to build.js
python ./platforms/js/build_js.py --build_wasm --emscripten_dir $HOME/emsdk/fastcomp/emscripten build_wasm
```

## Run Project

### Compile to emscripten target

```sh
git clone git@github.com:Horki/lanelines_wasm_emscripten.git
cd lanelines_wasm_emscripten
mkdir build && cd build
${HOME}/emsdk/fastcomp/emscripten/emconfigure cmake ..
# On Linux
${HOME}/emsdk/fastcomp/emscripten/emmake make -j $(nproc)
# On OsX
# ${HOME}/emsdk/fastcomp/emscripten/emmake make -j $(sysctl -n hw.ncpu)
```
