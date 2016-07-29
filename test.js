//jshint esversion: 6
const flapa = require('./index');
const fs = require('fs');
const Buffer = require('buffer').Buffer;

flapa.bindings.load('/usr/local/lib/libFLAC.dylib');
let dec = new flapa.FileDecoder({file: '/Volumes/OSX/Música/Floating Points/Nuits Sonores - Nectarines/01 - Floating Points - Nuits Sonores.flac'});
let outFile = fs.createWriteStream('out.raw');

dec.on('metadata', (metadata) => {
    console.log(require('util').inspect(metadata));
});

dec.on('error', (error) => {
    console.error("Error %d: %s", error.code, error);
});

dec.on('data', (data) => {
    console.log(data);
});

dec.on('end', () => {
    console.log("sacabó");
});

/*let dec = flapa.bindings.decoder.new();
let totalS = 0;
let outFile = fs.createWriteStream('out.raw', {defaultEncoding: null});
let is = flapa.bindings.decoder.init_file(dec, '/Volumes/OSX/Música/Floating Points/Nuits Sonores - Nectarines/01 - Floating Points - Nuits Sonores.flac',
    (frame, buffers) => {
        totalS += buffers[0].length / 4;//frame.header.blocksize;
        let outBuffer = Buffer.allocUnsafe(buffers[0].length / 2);
        for(let sample = 0; sample < frame.header.blocksize; sample++) {
            let value = buffers[0].readInt32LE(sample * 4);
            outBuffer.writeInt16LE(value, sample * 2);
        }
        outFile.write(outBuffer);
        return 0;
    },
    (metadata) => {
        console.log(require('util').inspect(metadata));
    },
    (error) => {
        console.error(flapa.bindings.decoder.ErrorStatusString[error]);
    }
);
console.log(flapa.bindings.decoder.InitStatusString[is]);

flapa.bindings.decoder.process_until_end_of_metadata(dec);

flapa.bindings.decoder.process_until_end_of_stream(dec);

console.log(flapa.bindings.decoder.get_resolved_state_string(dec));
console.log(totalS);
outFile.end();

flapa.bindings.decoder.delete(dec);*/
