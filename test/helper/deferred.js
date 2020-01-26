const createDeferredScope = () => {
    const deferredFunctions = [];
    return {
        defer: (fn) => {
            deferredFunctions.push(fn);
        },
        finalize: () => Promise.all(deferredFunctions.map((fn) => fn())),
    };
};

module.exports = createDeferredScope;
