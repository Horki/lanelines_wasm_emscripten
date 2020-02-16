'use strict';

let LaneLines;

class Lanes {
    constructor(module, image) {
        console.log("tusam");
        this.LaneLines = new module.LaneLines(image);
        console.log("Load done");
    }
    to_gray() {
        this.LaneLines.to_gray();
        return this.to_imaag();
    }
    to_gaussian(kernel, sigma_x = 0.0, sigma_y = 0.0) {
        // TODO: Check numbers
        this.LaneLines.to_gaussian(kernel, sigma_x, sigma_y);
        return this.to_imaag();
    }
    to_canny(threshold_1 = 0.0, threshold_2 = 0.0, aparture = 0) {
        // TODO: Check numbers
        this.LaneLines.to_canny(threshold_1, threshold_2, aparture);
        return this.to_imaag();
    }
    to_next()   {        this.LaneLines.to_next();  }
    to_imaag()  { return this.LaneLines.to_imaag(); }
    // Helpers
    // static imageDataFrom4Channels(data, width, height) {
    //     if (data instanceof Uint8ClampedArray) {
    //         return new ImageData(data, width, height);
    //     }
    //     const array = new Uint8ClampedArray(data);
    //     return new ImageData(array, width, height);
    // }
    //
    // static imageDataFrom1Channel(data, width, height) {
    //     const cb = width * height * 4;
    //     const array = new Uint8ClampedArray(cb);
    //     data.forEach((pixelValue, index) => {
    //         const base = index * 4;
    //         array[base] = pixelValue;
    //         array[base + 1] = pixelValue;
    //         array[base + 2] = pixelValue;
    //         array[base + 3] = 255;
    //     });
    //     return new ImageData(array, width, height);
    // }
}


window.Module = {
    noInitialRun: true,
    preInit: () => {
        console.log("preInit");
    },
    onRuntimeInitialized: () => {
        const {data, width, height} = get_image();
        let i = imageDataFrom4Channels(data, width, height);
        console.log("W = ", width, " H = ", height);
        LaneLines = new Lanes(window.Module, i);
    },
};


const imageDataFrom4Channels = (data, width, height) => {
    console.log('[imageDataFrom4Channels]');
    console.log("data", data);
    if (data instanceof Uint8ClampedArray) {
        return new ImageData(data, width, height);
    }
    const array     = new Uint8ClampedArray(data);
    console.log("array", array);
    return imageData;
};

const imageDataFrom1Channel = (data, width, height) => {
    console.log('[imageDataFrom1Channel]');
    const cb = width * height * 4;
    const array = new Uint8ClampedArray(cb);
    data.forEach((pixelValue, index) => {
        const base = index * 4;
        array[base] = pixelValue;
        array[base + 1] = pixelValue;
        array[base + 2] = pixelValue;
        array[base + 3] = 255;
    });
    const imageData = new ImageData(array, width, height);
    return imageData;
};

const drawOutputImage = (imageData, canvasId) => {
    console.log('[drawOutputImage]');
    const canvas = document.getElementById(canvasId);
    console.log("canvas", canvas);
    canvas.width  = imageData.width;
    canvas.height = imageData.height;
    console.log(imageData.constructor.name);
    console.log('draw:: w', canvas.width, ', h = ', canvas.height);
    const ctx = canvas.getContext('2d');
    ctx.putImageData(imageData, 0, 0);
};


const return_image = (Imaag) => {
    const outImage1Data =
        window.Module.HEAPU8.slice(Imaag.p_addr, Imaag.p_addr + Imaag.size);
    const imageData1 = Imaag.channels === 1
        ? imageDataFrom1Channel(outImage1Data,  Imaag.width, Imaag.height)
        : imageDataFrom4Channels(outImage1Data, Imaag.width, Imaag.height);
    // TODO: free heap?
    // module._free(imageData1);
    return imageData1;
};



const convert_to_gray = () => {
    console.log("Convert to gray");
    let a = LaneLines.to_gray();
    let img = return_image(a);
    LaneLines.to_next();
    drawOutputImage(img, 'output-image-1');
    document.getElementById('step-1').style.display = 'block';
};

const convert_to_gaussian = () => {
    console.log("Convert to gaussian");
    let kernel  = Number(document.getElementById('kernel').value);
    let sigma_x = Number(document.getElementById('sigma_x').value);
    let sigma_y = Number(document.getElementById('sigma_y').value);
    console.log("kernel", kernel, "sigma", sigma_x, "y", sigma_y);
    let a = LaneLines.to_gaussian(kernel, sigma_x, sigma_y);
    let img = return_image(a);
    console.log("bbb");
    drawOutputImage(img, 'output-image-2');
    document.getElementById('step-1').style.display = 'none';
    document.getElementById('step-2').style.display = 'block';
    LaneLines.to_next();
};

const convert_to_canny = () => {
    console.log("Convert to canny");
    let threshold_1 = 1;
    let threshold_2 = 100;
    let aperture   =  3;
    LaneLines.to_canny(threshold_1, threshold_2, aperture);
    let a = LaneLines.to_imaag();
    let img = return_image(a);
    console.log("ccc");
    drawOutputImage(img, 'output-image-3');
    document.getElementById('step-2').style.display = 'none';
    document.getElementById('step-3').style.display = 'block';
    LaneLines.to_next();
};


const get_image = () => {
    let canvas  = document.createElement('canvas');
    let context = canvas.getContext('2d');
    let img     = document.getElementById('image1');
    canvas.width  = img.width;
    canvas.height = img.height;
    context.drawImage(img, 0, 0 );
    return context.getImageData(0, 0, img.width, img.height);
};
