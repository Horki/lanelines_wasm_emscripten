Module = {
    // Don't run main on page load
    noInitialRun: true,
    // Run custom function on page load
    onRuntimeInitialized: () => {
        console.log(window.Module.test());
        console.log(window.Module.incr(101))
    },
    // // Custom function to process stdout: keep in memory
    // print: stdout => output.push(stdout),
    // // Custom function to process stderr: keep in memory
    // print: stderr => console.error(stderr),
};
