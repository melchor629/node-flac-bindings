const api = require('bindings')('flac-bindings');

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
