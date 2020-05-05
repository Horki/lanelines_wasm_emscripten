# Finding Lane Lines on the Road

## Lane Lines helper
This is a helper project to [Lane Lines project](https://github.com/Horki/CarND-LaneLines-P1)

## Run project
This project uses [WASM OpenCV](https://docs.opencv.org/4.3.0/d4/da1/tutorial_js_setup.html) and [Emscripten](https://emscripten.org/).
core functionality was written in C++, JS for DOM manipulation and wrapper around C++ calls.

More details on how to setup environment locally [here](BUILD.md).

```bash
mkdir build
cd build
${HOME}/emsdk/fastcomp/emscripten/emconfigure cmake ..
# nproc for linux; sysctl -n hw.ncpu for OsX
${HOME}/emsdk/fastcomp/emscripten/emmake make -j $(nproc)
```

Or run make command

```bash
make build
# make debug # build debug
make serve
```

## Pipeline

### 1. Convert to Gray
![convert2gray](img/res/01.png)

### 2. Gaussian Blur
![convert2gauss](img/res/02.png)

### 3. Canny
![convert2canny](img/res/03.png)

### 4. Select region of interest
![convert2region](img/res/04.png)

### 5. Result
![convert2result](img/res/05.png)

## TODO
- [ ] Build with [Conan](https://conan.io/)
- [ ] Upload photo feature
- [ ] Better post build script
- [ ] Exception handling (both C++ and JS)
- [ ] Unit Tests
- [ ] Performance Tests
- [ ] Check memory leaks
- [ ] Add better UX/UI (without 3rd party lib, or JS framework)
- [ ] Create static instance of app
- [ ] Rewrite to Rust
