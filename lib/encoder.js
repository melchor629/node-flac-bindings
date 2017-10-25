//jshint esversion: 6
const stream = require('stream');
const flac = require('bindings')('flac-bindings');
const Buffer = require('buffer').Buffer;

const convertToInt32Buffer = (chunk, self) => {
    if(self._inputAs32) return chunk;

    let inBPS = self._bitsPerSample / 8;
    if(self._storedChunk) {
        chunk = Buffer.concat([ self._storedChunk, chunk ], self._storedChunk.length + chunk.length);
    }

    let samples = Math.trunc(chunk.length / inBPS);
    let buffer = Buffer.allocUnsafe(samples * 4);
    for(let i = 0; i < samples; i++) {
        buffer.writeInt32LE(chunk.readIntLE(i * inBPS, inBPS), i * 4);
    }

    if(samples !== chunk.length / inBPS) {
        self._storedChunk = chunk.slice(samples * inBPS);
    } else {
        self._storedChunk = null;
    }
    return buffer;
};

const setOptions = (self, options) => {
    self._enc = flac.encoder.new();
    self._encoderInit = false;
    self._oggStream = !!options.isOggStream;
    self._channels = Number(options.channels) || 2;
    self._bitsPerSample = Number(options.bitsPerSample) || 16;
    self._inputAs32 = Boolean(options.inputAs32) || (self._bitsPerSample === 24);
    self._samplerate = Number(options.samplerate) || 44100;
    self._oggSerialNumber = Number(options.oggSerialNumber) || null;
    self._totalSamplesEstimate = Number(options.totalSamplesEstimate) || null;
    self._compressionLevel = Number(options.compressionLevel) || null;
    self._doMidSideStereo = options.channels === 2 ? !!options.doMidSideStereo : undefined;
    self._looseMidSideStereo = options.channels === 2 ? !!options.looseMidSideStereo : undefined;
    self._apodization = 'string' === typeof options.apodization ? options.apodization : undefined;
    self._maxLpcOrder = Number(options.maxLpcOrder) || null;
    self._qlpCoeffPrecision = Number(options.qlpCoeffPrecision) || null;
    self._doQlpCoeffPrecSearch = options.doQlpCoeffPrecSearch;
    self._doExhaustiveModelSearch = options.doExhaustiveModelSearch;
    self._minResidualPartitionOrder = Number(options.minResidualPartitionOrder) || null;
    self._maxResidualPartitionOrder = Number(options.maxResidualPartitionOrder) || null;
    self._riceParameterSearchDist = Number(options.riceParameterSearchDist) || null;
    self._blocksize = Number(options.blocksize) || null;
    self._metadata = options._metadata;
};

const setOptionsToEncoder = (self) => {
    flac.encoder.set_channels(self._enc, self._channels);
    flac.encoder.set_bits_per_sample(self._enc, self._bitsPerSample);
    flac.encoder.set_sample_rate(self._enc, self._samplerate);
    if(self._oggStream && self._oggSerialNumber)
        flac.encoder.set_ogg_serial_number(self._enc, self._oggSerialNumber);
    if(self._totalSamplesEstimate)
        flac.encoder.set_total_samples_estimate(self._enc, self._totalSamplesEstimate);
    if(self._compressionLevel)
        flac.encoder.set_compression_level(self._enc, self._compressionLevel);
    if(self._doMidSideStereo !== undefined)
        flac.encoder.set_do_mid_side_stereo(self._enc, self._doMidSideStereo);
    if(self._looseMidSideStereo !== undefined)
        flac.encoder.set_loose_mid_side_stereo(self._enc, self._looseMidSideStereo);
    if(self._apodization)
        flac.encoder.set_apodization(self._enc, self._apodization);
    if(self._maxLpcOrder)
        flac.encoder.set_max_lpc_order(self._enc, self._maxLpcOrder);
    if(self._qlpCoeffPrecision)
        flac.encoder.set_qlp_coeff_precision(self._enc, self._qlpCoeffPrecision);
    if(self._doQlpCoeffPrecSearch !== undefined)
        flac.encoder.set_do_qlp_coeff_prec_search(self._enc, self._doQlpCoeffPrecSearch);
    if(self._doExhaustiveModelSearch !== undefined)
        flac.encoder.set_do_exhaustive_model_search(self._enc, self._doExhaustiveModelSearch);
    if(self._minResidualPartitionOrder)
        flac.encoder.set_min_residual_partition_order(self._enc, self._minResidualPartitionOrd);
    if(self._maxResidualPartitionOrder)
        flac.encoder.set_max_residual_partition_order(self._enc, self._maxResidualPartitionOrd);
    if(self._riceParameterSearchDist)
        flac.encoder.set_rice_parameter_search_dist(self._enc, self._riceParameterSearchDist);
    if(self._blocksize)
        flac.encoder.set_blocksize(self._enc, self._blocksize);
    if(self._metadata)
        flac.encoder.set_metadata(self._enc, self._metadata);
};

class FlacStreamEncoder extends stream.Transform {
    constructor(options) {
        options = options || {};
        super(options);
        setOptions(this, options);
    }

    _transform(chunk, encoding, callback) {
        if(!this._encoderInit) {
            setOptionsToEncoder(this);
            this._encoderInit = true;
            let err;
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

        let _chunk = convertToInt32Buffer(chunk, this);
        let samples = _chunk.length / this._channels / 4;
        if(!flac.encoder.process_interleaved(this._enc, _chunk, samples)) {
            let err = flac.encoder.get_state();
            let errStr = flac.encoder.State[err];
            callback(new Error(errStr));
        } else {
            callback();
        }
    }

    _flush(callback) {
        if(this._storedChunk) {
            let chunk = this._storedChunk;
            this._storedChunk = null;
            let _chunk = convertToInt32Buffer(chunk, this);
            let samples = _chunk.length / this._channels / 4;
            flac.encoder.process_interleaved(this._enc, _chunk, samples);
        }

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
        setOptions(this, options);
        this._file = options.file;
        if(!this._file) throw new Error("No file passed as argument");
    }

    _write(chunk, encoding, callback) {
        if(!this._encoderInit) {
            setOptionsToEncoder(this);
            this._encoderInit = true;
            let err;
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

        let _chunk = convertToInt32Buffer(chunk, this);
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

        if(this._storedChunk) {
            let chunk = this._storedChunk;
            this._storedChunk = null;
            let _chunk = convertToInt32Buffer(chunk, this);
            let samples = _chunk.length / this._channels / 4;
            flac.encoder.process_interleaved(this._enc, _chunk, samples);
        }

        flac.encoder.finish(this._enc);
        flac.encoder.delete(this._enc);
        this._enc = null;
    }
}

module.exports = {
    FileEncoder: FlacFileEncoder,
    StreamEncoder: FlacStreamEncoder
};
