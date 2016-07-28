//jshint esversion: 6
const stream = require('stream');
const flac = require('bindings')('flac-bindings');
const Buffer = require('buffer');

class FlacStreamEncoder extends stream.Transform {
    constructor(options) {
        super(options);
        this._enc = flac.encoder.new();
        this._encoderInit = false;
        this._oggStream = options.isOggStream || false;
        this._inputInterleaved = options.isInputInterleaved || false;
    }

    _transform(chunk, encoding, callback) {
        if(!this._encoderInit) {
            let err = this._encoderInit();
            if(err) {
                callback(new Error(flac.encoder.InitStatusString[err]));
                return;
            }
        }

        let processFunc;
        if(this._inputInterleaved) {
            processFunc = flac.encoder.process_interleaved;
        } else {
            processFunc = flac.encoder.process;
        }

        processFunc(chunk, chunk.length / 4, (done) => {
            if(!done) {
                let err = flac.encoder.get_state();
                let errStr = flac.encoder.State[err];
                callback(new Error(errStr));
            } else {
                callback();
            }
        });
    }

    _flush(callback) {
        flac.encoder.finish(this._enc, (done) => {
            if(!done) {
                let err = flac.encoder.get_state();
                let errStr = flac.encoder.State[err];
                callback(new Error(errStr));
            } else {
                callback(null);
            }
        });
    }

    _initDecoder() {
        this._encoderInit = true;
        if(this._oggStream) {
            return flac.encoder.init_ogg_stream(this._enc, null, this._writeFlac, null, null, null);
        } else {
            return flac.encoder.init_stream(this._enc, this._writeFlac, null, null, null);
        }
    }

    _writeFlac(enc, buffer, bytes, samples, currentFrame) {
        let buffer2 = Buffer.from(buffer);
        super.push(buffer2);
    }
}

class FlacFileEncoder extends stream.Writable {
    constructor(options) {
        super(options);
        this._enc = flac.encoder.new();
        this._encoderInit = false;
        this._oggStream = options.isOggStream || false;
        this._inputInterleaved = options.isInputInterleaved || false;
        this._file = options._file;
        if(!this._file) throw new Error("No file passed as argument");
    }

    _write(chunk, encoding, callback) {
        if(!this._encoderInit) {
            let err = this._encoderInit();
            if(err) {
                callback(new Error(flac.encoder.InitStatusString[err]));
                return;
            }
        }

        let processFunc;
        if(this._inputInterleaved) {
            processFunc = flac.encoder.process_interleaved;
        } else {
            processFunc = flac.encoder.process;
        }

        processFunc(chunk, chunk.length / 4, (done) => {
            if(!done) {
                let err = flac.encoder.get_state();
                let errStr = flac.encoder.State[err];
                callback(new Error(errStr));
            } else {
                callback(null);
            }
        });
    }

    _initDecoder() {
        this._encoderInit = true;
        if(this._oggStream) {
            return flac.encoder.init_ogg_file(this._enc, this._file, null);
        } else {
            return flac.encoder.init_file(this._enc, this._file, null);
        }
    }
}

module.exports = {
    FileEncoder: FlacFileEncoder,
    StreamEncoder: FlacStreamEncoder
};
