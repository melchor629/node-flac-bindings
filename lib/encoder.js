//jshint esversion: 6
const stream = require('stream');
const flac = require('bindings')('flac-bindings');
const Buffer = require('buffer').Buffer;

const convertToInt32Buffer = (chunk, self) => {
    if(self._inputAs32) return chunk;

    let inBPS = self._bitsPerSample / 8;
    if(self._storedChunks.length > 0) {
        chunk = Buffer.concat([ ...self._storedChunks, chunk ]);
        self._storedChunks = [];
    }

    let samples = Math.trunc(chunk.length / inBPS);
    let buffer = Buffer.allocUnsafe(samples * 4);
    for(let i = 0; i < samples; i++) {
        buffer.writeInt32LE(chunk.readIntLE(i * inBPS, inBPS), i * 4);
    }

    if(samples !== chunk.length / inBPS) {
        self._storedChunks.push(chunk.slice(samples * inBPS));
    }
    return buffer;
};

const setOptions = (self, options) => {
    self._encoderInit = false;
    self._oggStream = !!options.isOggStream;
    self._channels = Number(options.channels) || 2;
    self._bitsPerSample = Number(options.bitsPerSample) || 16;
    self._inputAs32 = (options.inputAs32 !== undefined && options.inputAs32 !== null) ? !!options.inputAs32 : (self._bitsPerSample === 24);
    self._samplerate = Number(options.samplerate) || 44100;
    self._oggSerialNumber = Number(options.oggSerialNumber) || null;
    self._totalSamplesEstimate = Number(options.totalSamplesEstimate) || null;
    self._compressionLevel = Number(options.compressionLevel) || 5; //NOTE Workaround for #6
    self._doMidSideStereo = options.channels === 2 ? !!options.doMidSideStereo : undefined;
    self._looseMidSideStereo = options.channels === 2 ? !!options.looseMidSideStereo : undefined;
    self._apodization = 'string' === typeof options.apodization ? options.apodization : undefined;
    self._maxLpcOrder = Number(options.maxLpcOrder) || null;
    self._qlpCoeffPrecision = Number(options.qlpCoeffPrecision) || null;
    self._doQlpCoeffPrecSearch = options.doQlpCoeffPrecSearch;
    self._doExhaustiveModelSearch = options.doExhaustiveModelSearch;
    self._minResidualPartitionOrder = Number(options.minResidualPartitionOrder) || null;
    self._maxResidualPartitionOrder = Number(options.maxResidualPartitionOrder) || null;
    self._blocksize = Number(options.blocksize) || null;
    self._metadata = options.metadata;
    self._processedSamples = 0;
    self._storedChunks = [];
};

const setOptionsToEncoder = (self) => {
    self._enc.setChannels(self._channels);
    self._enc.setBitsPerSample(self._bitsPerSample);
    self._enc.setSampleRate(self._samplerate);
    if(self._oggStream && self._oggSerialNumber)
        self._enc.setOggSerialNumber(self._oggSerialNumber);
    if(self._totalSamplesEstimate)
        self._enc.setTotalSamplesEstimate(self._totalSamplesEstimate);
    if(self._compressionLevel)
        self._enc.setCompressionLevel(self._compressionLevel);
    if(self._doMidSideStereo !== undefined)
        self._enc.setDoMidSideStereo(self._doMidSideStereo);
    if(self._looseMidSideStereo !== undefined)
        self._enc.setLooseMidSideStereo(self._looseMidSideStereo);
    if(self._apodization)
        self._enc.setApodization(self._apodization);
    if(self._maxLpcOrder)
        self._enc.setMaxLpcOrder(self._maxLpcOrder);
    if(self._qlpCoeffPrecision)
        self._enc.setQlpCoeffPrecision(self._qlpCoeffPrecision);
    if(self._doQlpCoeffPrecSearch !== undefined)
        self._enc.setDoQlpCoeffPrecSearch(self._doQlpCoeffPrecSearch);
    if(self._doExhaustiveModelSearch !== undefined)
        self._enc.setDoExhaustiveModelSearch(self._doExhaustiveModelSearch);
    if(self._minResidualPartitionOrder)
        self._enc.setMinResidualPartitionOrder(self._minResidualPartitionOrd);
    if(self._maxResidualPartitionOrder)
        self._enc.setMaxResidualPartitionOrder(self._maxResidualPartitionOrd);
    if(self._blocksize)
        self._enc.setBlocksize(self._blocksize);
    if(self._metadata)
        self._enc.setMetadata(self._metadata);
};

class FlacStreamEncoder extends stream.Transform {
    constructor(options) {
        options = options || {};
        super(options);
        this._enc = new flac.Encoder();
        setOptions(this, options);
    }

    _transform(chunk, encoding, callback) {
        if(!this._encoderInit) {
            setOptionsToEncoder(this);
            this._encoderInit = true;
            let err;
            if(this._oggStream) {
                err = this._enc.initOggStream(null, this._writeFlac.bind(this), null, null, null);
            } else {
                err = this._enc.initStream(this._writeFlac.bind(this), null, null, null);
            }
            if(err) {
                callback(new Error(flac.Encoder.InitStatusString[err]));
                return;
            }
        }

        let _chunk = convertToInt32Buffer(chunk, this);
        const samples = Math.trunc(_chunk.length / this._channels / 4);
        const extraBytes = _chunk.length - (samples * this._channels * 4);
        this._processedSamples += samples;
        if(extraBytes !== 0) {
            this._storedChunks.push(_chunk.slice(samples * this._channels * 4));
        }
        if(!this._enc.processInterleaved(_chunk, samples)) {
            let err = this._enc.getState();
            let errStr = flac.Encoder.State[err];
            callback(new Error(errStr));
        } else {
            callback();
        }
    }

    _flush(callback) {
        if(this._storedChunks.length > 0) {
            let chunk = Buffer.concat(this._storedChunks);
            this._storedChunks = [];
            let _chunk = convertToInt32Buffer(chunk, this);
            let samples = Math.trunc(_chunk.length / this._channels / 4);
            flac.encoder.process_interleaved(this._enc, _chunk, samples);
        }

        if(!this._enc.finish()) {
            let err = this._enc.getState();
            let errStr = flac.Encoder.State[err];
            callback(new Error(errStr));
        } else {
            callback(null);
        }

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
        this._enc = new flac.Encoder();
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
                err = this._enc.initOggFile(this._file, null);
            } else {
                err = this._enc.initFile(this._file, null);
            }

            if(err) {
                callback(new Error(flac.Encoder.InitStatusString[err]));
                return;
            }
        }

        let _chunk = convertToInt32Buffer(chunk, this);
        const samples = Math.trunc(_chunk.length / this._channels / 4);
        const extraBytes = _chunk.length - (samples * this._channels * 4);
        this._processedSamples += samples;
        if(extraBytes !== 0) {
            this._storedChunks.push(_chunk.slice(samples * this._channels * 4));
        }
        if(!this._enc.processInterleaved(_chunk, samples)) {
            let err = this._enc.getState();
            let errStr = flac.Encoder.State[err];
            callback(new Error(errStr));
        } else {
            callback(null);
        }
    }

    end(chunk, encoding, callback) {
        super.end(chunk, encoding, callback);

        if(this._storedChunks.length > 0) {
            let chunk = Buffer.concat(this._storedChunks);
            this._storedChunks = [];
            let _chunk = convertToInt32Buffer(chunk, this);
            let samples = Math.trunc(_chunk.length / this._channels / 4);
            this._enc.processInterleaved(_chunk, samples);
        }

        this._enc.finish();
        this._enc = null;
    }
}

module.exports = {
    FileEncoder: FlacFileEncoder,
    StreamEncoder: FlacStreamEncoder
};
