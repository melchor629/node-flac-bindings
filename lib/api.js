const api = require('bindings')('flac-bindings');

if('function' === typeof api.load && process.env.FLAC_LIBRARY !== undefined) {
    //Try to load libFLAC from environment variable
    try {
        api.load(process.env.FLAC_LIBRARY);
    } catch(e) {} //eslint-disable-line no-empty
}

api._helpers.handleAsync = async (promise, resolve, reject) => {
    try {
        const res = await promise;
        resolve(res);
    } catch(e) {
        reject(e);
    }
};

Object.freeze(api._helpers);

module.exports = api;
