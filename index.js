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
