const api = require('bindings')('flac-bindings');
const { StreamEncoder, FileEncoder } = require('./encoder');
const { StreamDecoder, FileDecoder } = require('./decoder');

module.exports = {
    api,
    StreamEncoder,
    FileEncoder,
    StreamDecoder,
    FileDecoder,
};

if('function' === typeof api.load && process.platform === 'darwin') {
    //Try to load libFLAC installed by Homebrew on macOS
    try {
        api.load('/usr/local/lib/libFLAC.dylib');
    } catch(e) {}
}

if('function' === typeof api.load && process.env.FLAC_LIBRARY !== undefined) {
    //Try to load libFLAC from environment variable
    try {
        api.load(process.env.FLAC_LIBRARY);
    } catch(e) {}
}
