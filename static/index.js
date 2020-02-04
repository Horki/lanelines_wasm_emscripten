Module = {
    // Don't run main on page load
    noInitialRun: true,
    // Run custom function on page load
    preInit: () => {
        //
    },
    onRuntimeInitialized: () => {
        //
    },
    print: stdout => output.push(stdout),
    // Custom function to process stderr: keep in memory
    print: stderr => console.error(stderr),
};

document.getElementById('upload')
    .addEventListener('change', function(event) {
        let f = this.files[0];
        let reader = new FileReader();
        reader.onloadend = evt => {
            const uint8_t_arr = new Uint8Array(evt.target.result);
            const uint8_t_ptr = window.Module._malloc(uint8_t_arr.length);
            window.Module.HEAPU8.set(uint8_t_arr, uint8_t_ptr);
            window.Module.print_file(uint8_t_ptr, uint8_t_arr.length);
            const returnArr = new Uint8Array(uint8_t_arr.length);
            returnArr.set(window.Module.HEAPU8.subarray(uint8_t_ptr, uint8_t_ptr + uint8_t_arr.length));
            window.Module._free(uint8_t_ptr);
        };
        reader.readAsArrayBuffer(f);
    }
);