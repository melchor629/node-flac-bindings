//jshint esversion: 6
const stream = require('stream');
const flac = require('bindings')('flac-bindings');
const Buffer = require('buffer').Buffer;

const convertToInt32Buffer = (chunk, inBPS) => {
    let buffer = Buffer.allocUnsafe(chunk.length / inBPS * 4);
    for(let i = 0; i < Math.trunc(chunk.length / inBPS); i++) {
        buffer.writeInt32LE(chunk.readIntLE(i * inBPS, inBPS), i * 4);
    }
    return buffer;
};

class FlacStreamEncoder extends stream.Transform {
    constructor(options) {
        options = options || {};
        super(options);
        this._enc = flac.encoder.new();
        this._encoderInit = false;
        this._oggStream = options.isOggStream || false;
        this._channels = options.channels || 2;
        this._bitsPerSample = options.bitsPerSample || 16;
        this._samplerate = options.samplerate || 44100;
        this._oggSerialNumber = options.oggSerialNumber || null;
        this._totalSamplesEstimate = options.totalSamplesEstimate;
    }

    _transform(chunk, encoding, callback) {
        if(!this._encoderInit) {
            this._encoderInit = true;
            let err;
            flac.encoder.set_channels(this._enc, this._channels);
            flac.encoder.set_bits_per_sample(this._enc, this._bitsPerSample);
            flac.encoder.set_sample_rate(this._enc, this._samplerate);
            if(this._oggStream)
                flac.encoder.set_ogg_serial_number(this._enc, this._oggSerialNumber);
            if(this._totalSamplesEstimate)
                flac.encoder.set_total_samples_estimate(this._enc, this._totalSamplesEstimate);
            if(this._oggStream) {
                err = flac.encoder.init_ogg_stream(this._enc, null, this._writeFlac.bind(this), null, null, null);
            } else {
                err = flac.encoder.init_stream(this._enc, this._writeFlac.bind(this), null, null, null);
            }
            if(err) {
                callback(new Error(flac.encoder.InitStatusString[err]));
                return;
            }
        }

        let _chunk = convertToInt32Buffer(chunk, this._bitsPerSample / 8);
        let samples = _chunk.length / this._channels / 4;
        if(!flac.encoder.process_interleaved(this._enc, _chunk, samples)) {
            let err = flac.encoder.get_state();
            let errStr = flac.encoder.State[err];
            callback(new Error(errStr));
        } else {
            callback(null);
        }
    }

    _flush(callback) {
        if(!flac.encoder.finish(this._enc)) {
            let err = flac.encoder.get_state();
            let errStr = flac.encoder.State[err];
            callback(new Error(errStr));
        } else {
            callback(null);
        }
        flac.encoder.delete(this._enc);
        this._enc = null;
    }

    _encoderInit() {
        this._encoderInit = true;
        if(this._oggStream) {
            return flac.encoder.init_ogg_stream(this._enc, null, this._writeFlac, null, null, null);
        } else {
            return flac.encoder.init_stream(this._enc, this._writeFlac, null, null, null);
        }
    }

    _writeFlac(buffer, bytes, samples, currentFrame) {
        let buffer2 = Buffer.from(buffer);
        this.push(buffer2);
        return 0;
    }
}

class FlacFileEncoder extends stream.Writable {
    constructor(options) {
        options = options || {};
        super(options);
        this._enc = flac.encoder.new();
        this._encoderInit = false;
        this._oggStream = options.isOggStream || false;
        this._file = options.file;
        if(!this._file) throw new Error("No file passed as argument");
        this._oggStream = options.isOggStream || false;
        this._channels = options.channels || 2;
        this._bitsPerSample = options.bitsPerSample || 16;
        this._samplerate = options.samplerate || 44100;
        this._oggSerialNumber = options.oggSerialNumber || null;
        this._totalSamplesEstimate = options.totalSamplesEstimate;
    }

    _write(chunk, encoding, callback) {
        if(!this._encoderInit) {
            this._encoderInit = true;
            let err;
            flac.encoder.set_channels(this._enc, this._channels);
            flac.encoder.set_bits_per_sample(this._enc, this._bitsPerSample);
            flac.encoder.set_sample_rate(this._enc, this._samplerate);
            if(this._oggStream)
                flac.encoder.set_ogg_serial_number(this._enc, this._oggSerialNumber);
            if(this._totalSamplesEstimate)
                flac.encoder.set_total_samples_estimate(this._enc, this._totalSamplesEstimate);
            if(this._oggStream) {
                err = flac.encoder.init_ogg_file(this._enc, this._file, null);
            } else {
                err = flac.encoder.init_file(this._enc, this._file, null);
            }
            if(err) {
                callback(new Error(flac.encoder.InitStatusString[err]));
                return;
            }
        }

        let _chunk = convertToInt32Buffer(chunk, this._bitsPerSample / 8);
        let samples = _chunk.length / this._channels / 4;
        if(!flac.encoder.process_interleaved(this._enc, _chunk, samples)) {
            let err = flac.encoder.get_state();
            let errStr = flac.encoder.State[err];
            callback(new Error(errStr));
        } else {
            callback(null);
        }
    }

    end(chunk, encoding, callback) {
        super.end(chunk, encoding, callback);
        flac.encoder.finish(this._enc);
        flac.encoder.delete(this._enc);
        this._enc = null;
    }
}

module.exports = {
    FileEncoder: FlacFileEncoder,
    StreamEncoder: FlacStreamEncoder
};
