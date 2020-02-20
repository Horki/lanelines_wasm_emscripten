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
        return this.LaneLines.to_imaag();
    }
    to_gaussian(kernel, sigma_x = 0.0, sigma_y = 0.0) {
        // TODO: Check numbers
               this.LaneLines.to_gaussian(kernel, sigma_x, sigma_y);
        return this.LaneLines.to_imaag();
    }
    to_canny(threshold_1 = 0.0, threshold_2 = 0.0, aparture = 0) {
        // TODO: Check numbers
               this.LaneLines.to_canny(threshold_1, threshold_2, aparture);
        return this.LaneLines.to_imaag();
    }
    to_region(x_1, y_1, x_2, y_2) {
        // TODO: Check numbers
               this.LaneLines.to_region(x_1, y_1, x_2, y_2);
        return this.LaneLines.to_imaag();
    }
    to_houghes() {
               this.LaneLines.to_houghes();
        return this.LaneLines.to_imaag();
    }
    to_next()   {        this.LaneLines.to_next();  }
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
    return new ImageData(array, width, height);
};

const imageDataFrom1Channel = (data, width, height) => {
    console.log('[imageDataFrom1Channel]');
    const cb = width * height * 4;
    const array = new Uint8ClampedArray(cb);
    data.forEach((pixelValue, index) => {
        const base = index * 4;
        array[base    ] = pixelValue;
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

const convert_to_gray = (n = false) => {
    console.log("Convert to gray");
    let a = LaneLines.to_gray();
    let img = return_image(a);
    if (n) {
        LaneLines.to_next();
        drawOutputImage(img, 'output-image-2');
        document.getElementById('step-1').style.display = 'none';
        document.getElementById('step-2').style.display = 'block';
    } else {
        drawOutputImage(img, 'preview-image-1');
    }
};

const n = (img, did, preview = false) => {
    if (!preview) {
        drawOutputImage(img, `output-image-${did + 1}`);
        document.getElementById(`step-${did}`).style.display = 'none';
        document.getElementById(`step-${did + 1}`).style.display = 'block';
        LaneLines.to_next();
    } else {
        drawOutputImage(img, `preview-image-${did}`);
    }
};

const convert_to_gaussian = (form, preview = false) => {
    // console.log("Convert to gaussian");
    let kernel  = Number(form.kernel.value)  | 1;
    let sigma_x = Number(form.sigma_x.value) | 0;
    let sigma_y = Number(form.sigma_y.value) | 0;
    console.log("kernel", kernel, "sigma", sigma_x, "y", sigma_y);
    let a = LaneLines.to_gaussian(kernel, sigma_x, sigma_y);
    let img = return_image(a);
    console.log("bbb");
    n(img, 2, preview);
};

const convert_to_canny = (form, preview = false) => {
    console.log("Convert to canny");
    let threshold_1 = Number(form.threshold_1.value);
    let threshold_2 = Number(form.threshold_2.value);
    let aperture    = Number(form.aperture.value);
    console.log("min threshold", threshold_1, "max threshold", threshold_2, "aperture", aperture);
    let a = LaneLines.to_canny(threshold_1, threshold_2, aperture);
    let img = return_image(a);
    console.log("ccc");
    if (!preview) {
        // TODO: refactor to something more meaningful!
        let x_1 = document.getElementById('x_1');
        let y_1 = document.getElementById('y_1');
        let x_2 = document.getElementById('x_2');
        let y_2 = document.getElementById('y_2');
        let x_size = img.width;
        let y_size = img.height;
        let x_a = Math.trunc(x_size * 0.5);
        let x_b = Math.trunc(x_size * 0.05);
        let y_a = Math.trunc(y_size * 0.62);
        x_1.setAttribute('max', String(x_size));
        y_1.setAttribute('max', String(y_size));
        x_2.setAttribute('max', String(x_size));
        y_2.setAttribute('max', String(y_size));
        x_1.setAttribute('value', String(Math.trunc(x_a - x_b)));
        y_1.setAttribute('value', String(y_a));
        x_2.setAttribute('value', String(Math.trunc(x_a + x_b)));
        y_2.setAttribute('value', String(y_a));
    }
    n(img, 3, preview);
};

const convert_to_region = (form, preview = false) => {
    console.log("Select region");
    let x_1 = Number(form.x_1.value);
    let y_1 = Number(form.y_1.value);
    let x_2 = Number(form.x_2.value);
    let y_2 = Number(form.y_2.value);
    console.log(`Point1(${x_1}, ${y_1}), Point2(${x_2}, ${y_2})`);
    let a   = LaneLines.to_region(x_1, y_1, x_2, y_2);
    let img = return_image(a);
    console.log("ddd");
    n(img, 4, true);
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
