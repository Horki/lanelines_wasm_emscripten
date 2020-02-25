'use strict';

let laneLines;

class Lanes {
    constructor(module, image) {
        console.log("Start");
        this.LaneLines = new module.LaneLines(image);
        this.width  = image.width;
        this.height = image.height;
        console.log("Load done");
    }
    toGray() {
               this.LaneLines.toGray();
        return this.LaneLines.getImaag();
    }
    toGaussian(form) {
        let kernel  = +form["kernel"].value;
        let sigma_x = +form["sigma-x"].value;
        let sigma_y = +form["sigma-y"].value;
        console.assert((kernel >= 0) && (kernel <= 125), "Kernel should be [0, 125]");
        console.assert((sigma_x >= 0) && (sigma_x <= 300), "Sigma X should be [0, 300]");
        console.assert((sigma_y >= 0) && (sigma_y <= 300), "Sigma Y should be [0, 300]");

               this.LaneLines.toGaussian(kernel, sigma_x, sigma_y);
        return this.LaneLines.getImaag();
    }
    toCanny(form) {
        let threshold_1 = +form["threshold-1"].value;
        let threshold_2 = +form["threshold-2"].value;
        let aperture    = +form["aperture"].value;
        console.assert((threshold_1 >= 0) && (threshold_1 <= 300), "Min Threshold should be [0, 300]");
        console.assert((threshold_2 >= 0) && (threshold_2 <= 300), "Max Threshold should be [0, 300]");
        console.assert(threshold_2 > threshold_1, "Max threshold should be bigger");
        console.assert((aperture >= 0) && (aperture <= 125), "Aperture should be [0, 150]");

               this.LaneLines.toCanny(threshold_1, threshold_2, aperture);
        return this.LaneLines.getImaag();
    }
    toRegion(form) {
        let x_1 = +form["x-1"].value;
        let y_1 = +form["y-1"].value;
        let x_2 = +form["x-2"].value;
        let y_2 = +form["y-2"].value;
        console.assert((x_1 >= 0) && (x_1 <= this.width), `Point 1 X should be [0, ${this.width}]`);
        console.assert((y_1 >= 0) && (y_1 <= this.height), `Point 1 Y should be [0, ${this.height}]`);
        console.assert((x_2 >= 0) && (x_2 <= this.width), `Point 2 X should be [0, ${this.width}]`);
        console.assert((y_2 >= 0) && (y_2 <= this.height), `Point 2 Y should be [0, ${this.height}]`);

               this.LaneLines.toRegion(x_1, y_1, x_2, y_2);
        return this.LaneLines.getImaag();
    }
    toHoughes(form) {
        let rho       =  +form["rho"].value;
        let threshold =  +form["threshold"].value;
        let min_theta =  +form["min-theta"].value;
        let max_theta =  +form["max-theta"].value;
        let thickness =  +form["thickness"].value;
        console.assert((rho >= 0) && (rho <= 10), "Rho should be [0, 10]");
        console.assert((threshold >= 0) && (threshold <= 100), "Threshold should be [0, 100]");
        console.assert((min_theta >= 0) && (min_theta <= 300), "Min theta should be [0, 300]");
        console.assert((max_theta >= 0) && (max_theta <= 300), "Max theta should be [0, 300]");
        console.assert(min_theta < max_theta, "Max Theta is bigger!!!");
        console.assert((thickness >= 0) && (thickness <= 10), "Thickness should be [0, 10]");

               this.LaneLines.toHoughes(rho, threshold, min_theta, max_theta, thickness);
        return this.LaneLines.getImaag();
    }
    toNext()   {        this.LaneLines.toNext();  }
}


window.Module = {
    noInitialRun: true,
    preInit: () => {
        console.log("preInit");
    },
    onRuntimeInitialized: () => {
        const {data, width, height} = getImage();
        let i = imageDataFrom4Channels(data, width, height);
        console.log("W = ", width, " H = ", height);
        laneLines = new Lanes(window.Module, i);
    },
    // Custom function to process stdout: show in console as debug
    print:      stdout => console.debug(stdout),
    // Custom function to process stderr: show in console as errors
    printError: stderr => console.error(stderr),
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
    // console.memory
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

// https://shockry.blogspot.com/2017/04/webassembly-sending-javascript-array-to.html
const returnImage = (Imaag) => {
    const buffer =
        window.Module.HEAPU8.slice(Imaag.p_addr, Imaag.p_addr + Imaag.size);
    const imageData1 = Imaag.channels === 1
        ? imageDataFrom1Channel(buffer,  Imaag.width, Imaag.height)
        : imageDataFrom4Channels(buffer, Imaag.width, Imaag.height);
    // TODO: free heap?
    // window.Module.HEAP8._fr
    console.log("before", buffer);
    window.Module._free(buffer);
    console.log("after", buffer);
    return imageData1;
};

const convertTo = (form, preview = false) => {
    const id = +form.id;
    let a = laneLines[form.name](form);
    let img = returnImage(a);
    n(img, id, preview);
};

const n = (img, did = 1, preview = false) => {
    if (!preview) {
        drawOutputImage(img, `output-image-${did + 1}`);
        document.getElementById(`step-${did}`).style.display = 'none';
        document.getElementById(`step-${did + 1}`).style.display = 'block';
        laneLines.toNext();
    } else {
        drawOutputImage(img, `preview-image-${did}`);
    }
};

const getImage = () => {
    let canvas  = document.createElement('canvas');
    let context = canvas.getContext('2d');
    let img     = document.getElementById('image-1');
    canvas.width  = img.width;
    canvas.height = img.height;
    context.drawImage(img, 0, 0 );
    {
        let x_1 = document.getElementById('x-1');
        let y_1 = document.getElementById('y-1');
        let x_2 = document.getElementById('x-2');
        let y_2 = document.getElementById('y-2');
        let x_size = img.width;
        let y_size = img.height;
        let x_a = Math.trunc(x_size * 0.5);
        let x_b = Math.trunc(x_size * 0.05);
        let y_a = Math.trunc(y_size * 0.62);
        x_1.setAttribute('max',   String(x_size));
        y_1.setAttribute('max',   String(y_size));
        x_2.setAttribute('max',   String(x_size));
        y_2.setAttribute('max',   String(y_size));
        x_1.setAttribute('value', String(Math.trunc(x_a - x_b)));
        y_1.setAttribute('value', String(y_a));
        x_2.setAttribute('value', String(Math.trunc(x_a + x_b)));
        y_2.setAttribute('value', String(y_a));
        document.getElementById(`step-1`).style.display = 'block';
    }
    return context.getImageData(0, 0, img.width, img.height);
};
