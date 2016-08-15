//jshint esversion: 6
const flac_bindings = require('bindings')('flac-bindings');
const encoders = require('./lib/encoder');
const decoders = require('./lib/decoder');

module.exports = {
    bindings: flac_bindings,
    StreamEncoder: encoders.StreamEncoder,
    FileEncoder: encoders.FileEncoder,
    StreamDecoder: decoders.StreamDecoder,
    FileDecoder: decoders.FileDecoder
};

if('function' === typeof flac_bindings.load && process.platform === 'darwin') {
    //Try to load libFLAC installed by Homebrew on macOS
    try {
        flac_bindings.load('/usr/local/lib/libFLAC.dylib');
    } catch(e) {}
}

if('function' === typeof flac_bindings.load && process.env.FLAC_LIBRARY !== undefined) {
    //Try to load libFLAC from environment variable
    try {
        flac_bindings.load(process.env.FLAC_LIBRARY);
    } catch(e) {}
}
