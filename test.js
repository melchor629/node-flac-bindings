//jshint esversion: 6
const flapa = require('./index');
const fs = require('fs');
const Buffer = require('buffer').Buffer;

let dec = new flapa.FileDecoder({file: '/Volumes/OSX/Música/Floating Points/Nuits Sonores - Nectarines/01 - Floating Points - Nuits Sonores.flac'});
let enc = new flapa.FileEncoder({
    file: 'out.flac',
    channels: 2,
    bitsPerSample: 16,
    samplerate: 44100,
    totalSamplesEstimate: 31569584
});
dec.on('error', (error) => {
    console.log(error);
});
enc.on('error', (error) => {
    console.log(error);
});
dec.pipe(enc);
