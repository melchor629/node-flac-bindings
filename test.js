//jshint esversion: 6
let flapa = require('./index');

flapa.bindings.load('/usr/local/lib/libFLAC.dylib');
/*let dec = new flapa.FileDecoder({file: '/Volumes/OSX/Música/Floating Points/Nuits Sonores - Nectarines/01 - Floating Points - Nuits Sonores.flac'});

dec.on('metadata', (metadata) => {
    console.log(require('util').inspect(metadata));
});

dec.on('error', (error) => {
    console.error("Error %d !! %s", error.code, error);
});

dec.on('data', (data) => {
    console.log(data);
});

console.log(dec.read());*/

let dec = flapa.bindings.decoder.new();
let totalS = 0;
let is = flapa.bindings.decoder.init_file(dec, '/Volumes/OSX/Música/Floating Points/Nuits Sonores - Nectarines/01 - Floating Points - Nuits Sonores.flac',
    (dec, frame, buffers) => {
        totalS += frame.header.blocksize;
    },
    (dec, metadata) => {
        console.log(require('util').inspect(metadata));
    },
    (dec, error) => {
        console.error(flapa.bindings.decoder.ErrorStatusString[error]);
    }
);
console.log(flapa.bindings.decoder.InitStatusString[is]);

flapa.bindings.decoder.process_until_end_of_metadata(dec);

/*let ret = true;
for(let i = 0; i < 1 << 8 && ret; i++) {
    ret = flapa.bindings.decoder.process_single(dec);
}*/
flapa.bindings.decoder.process_until_end_of_stream(dec);

console.log(flapa.bindings.decoder.get_resolved_state_string(dec));
console.log(totalS);
