//jshint esversion: 6
const api = require('bindings')('flac-bindings');
const encoders = require('./encoder');
const decoders = require('./decoder');

module.exports = {
    api,
    StreamEncoder: encoders.StreamEncoder,
    FileEncoder: encoders.FileEncoder,
    StreamDecoder: decoders.StreamDecoder,
    FileDecoder: decoders.FileDecoder
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
        apo.load(process.env.FLAC_LIBRARY);
    } catch(e) {}
}
