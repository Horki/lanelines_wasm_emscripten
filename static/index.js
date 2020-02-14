window.Module = {
    // Don't run main on page load
    noInitialRun: true,
    // Run custom function on page load
    preInit: () => {
        //
    },
    onRuntimeInitialized: () => {
        console.log('[onRuntimeInitialized]');
        init(Module);
    },
    // print: stdout => output.push(stdout),
};


const imageDataFrom4Channels = (data, width, height) => {
    console.log('[imageDataFrom4Channels]');
    console.log("data", data);
    const array     = new Uint8ClampedArray(data);
    console.log("array", array);
    const imageData = new ImageData(array, width, height);
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
    // const outputImageOverlay = document.getElementById(`${canvasId}-overlay`);
    // outputImageOverlay.width = imageData.width;
    // outputImageOverlay.height = imageData.height;
};

let LaneLines;

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


const init = (module) => {
    console.log('[init]');
    const {data, width, height} = get_image();
    let i = imageDataFrom4Channels(data, width, height);
    console.log("W = ", width, " H = ", height);
    console.log(i);
    LaneLines = new module.LaneLines(i);
};

const convert_to_gray = () => {
    console.log("Convert to gray");
    let a = LaneLines.to_gray();
    let img = return_image(a);
    console.log("aaa");
    drawOutputImage(img, 'output-image-1');
};

const convert_to_gaussian = () => {
    console.log("Convert to gaussian");
    let kernel = 25;
    let sigma_x = 0.0;
    let sigma_y = 0.0;
    let a = LaneLines.to_gaussian(kernel, sigma_x, sigma_y);
    let img = return_image(a);
    console.log("bbb");
    drawOutputImage(img, 'output-image-2');
};

const convert_to_canny = () => {
    console.log("Convert to canny");
    let threshold_1 = 1;
    let threshold_2 = 100;
    let aperture   =  3;
    let a = LaneLines.to_canny(threshold_1, threshold_2, aperture);
    let img = return_image(a);
    console.log("ccc");
    drawOutputImage(img, 'output-image-3');
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
