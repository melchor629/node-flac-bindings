const stream = require('stream');
const flac = require('./api');

const convertToInt32Buffer = (chunk, self) => {
    if(self._inputAs32) {
        return chunk;
    }

    const inBPS = self._bitsPerSample / 8;
    if(self._storedChunks.length > 0) {
        chunk = Buffer.concat([ ...self._storedChunks, chunk ]);
        self._storedChunks = [];
    }

    const samples = Math.trunc(chunk.length / inBPS);
    const buffer = Buffer.allocUnsafe(samples * 4);
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
    self._inputAs32 = (options.inputAs32 !== undefined && options.inputAs32 !== null) ?
        !!options.inputAs32 :
        (self._bitsPerSample === 24);
    self._samplerate = Number(options.samplerate) || 44100;
    self._oggSerialNumber = Number(options.oggSerialNumber) || null;
    self._totalSamplesEstimate = Number(options.totalSamplesEstimate) || null;
    self._compressionLevel = Number(options.compressionLevel || '5') || 5; //NOTE Workaround for #6
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

    if(self._oggStream && !flac.format.API_SUPPORTS_OGG_FLAC) {
        throw new Error('Ogg FLAC is unsupported');
    }
};

const setOptionsToEncoder = (self) => {
    self._enc.channels = self._channels;
    self._enc.bitsPerSample = self._bitsPerSample;
    self._enc.sampleRate = self._samplerate;
    if(self._oggStream && self._oggSerialNumber) {
        self._enc.setOggSerialNumber(self._oggSerialNumber);
    }
    if(typeof self._totalSamplesEstimate === 'bigint' || typeof self._totalSamplesEstimate === 'number') {
        self._enc.totalSamplesEstimate = self._totalSamplesEstimate;
    }
    if(typeof self._compressionLevel === 'number') {
        self._enc.setCompressionLevel(self._compressionLevel);
    }
    if(self._doMidSideStereo !== undefined) {
        self._enc.doMidSideStereo = self._doMidSideStereo;
    }
    if(self._looseMidSideStereo !== undefined) {
        self._enc.looseMidSideStereo = self._looseMidSideStereo;
    }
    if(self._apodization) {
        self._enc.setApodization(self._apodization);
    }
    if(typeof self._maxLpcOrder === 'number') {
        self._enc.maxLpcOrder = self._maxLpcOrder;
    }
    if(typeof self._qlpCoeffPrecision === 'number') {
        self._enc.qlpCoeffPrecision = self._qlpCoeffPrecision;
    }
    if(self._doQlpCoeffPrecSearch !== undefined) {
        self._enc.doQlpCoeffPrecSearch = self._doQlpCoeffPrecSearch;
    }
    if(self._doExhaustiveModelSearch !== undefined) {
        self._enc.doExhaustiveModelSearch = self._doExhaustiveModelSearch;
    }
    if(typeof self._minResidualPartitionOrder === 'number') {
        self._enc.minResidualPartitionOrder = self._minResidualPartitionOrder;
    }
    if(typeof self._maxResidualPartitionOrder === 'number') {
        self._enc.maxResidualPartitionOrder = self._maxResidualPartitionOrder;
    }
    if(typeof self._blocksize === 'number') {
        self._enc.blocksize = self._blocksize;
    }
    if(Array.isArray(self._metadata)) {
        self._enc.setMetadata(self._metadata);
    }
};

class FlacStreamEncoder extends stream.Transform {
    constructor(options) {
        options = options || {};
        super({ ...options, decodeStrings: false, defaultEncoding: undefined, encoding: undefined });
        this._enc = new flac.Encoder();
        setOptions(this, options);
    }

    async _transform(chunk, _, callback) {
        try {
            if(!this._encoderInit) {
                setOptionsToEncoder(this);
                let res = flac.Encoder.InitStatus.OK;
                if(this._oggStream) {
                    res = await this._enc.initOggStreamAsync(null, this._writeFlac.bind(this), null, null, null);
                } else {
                    res = await this._enc.initStreamAsync(this._writeFlac.bind(this), null, null, null);
                }

                if(res !== flac.Encoder.InitStatus.OK) {
                    let initStatusString = flac.Encoder.InitStatusString[res];
                    if(res === flac.Encoder.InitStatus.ENCODER_ERROR) {
                        initStatusString = flac.Encoder.StateString[this._enc.getState()];
                    }
                    throw new Error(`Could not initialize encoder: ${initStatusString}`);
                }

                this._encoderInit = true;
            }

            const _chunk = convertToInt32Buffer(chunk, this);
            const samples = Math.trunc(_chunk.length / this._channels / 4);
            const extraBytes = _chunk.length - (samples * this._channels * 4);
            this._processedSamples += samples;
            if(extraBytes !== 0) {
                this._storedChunks.push(_chunk.slice(samples * this._channels * 4));
            }
            if(!(await this._enc.processInterleavedAsync(_chunk, samples))) {
                const err = this._enc.getState();
                const errStr = flac.Encoder.State[err];
                callback(new Error(errStr));
            } else {
                callback(null);
            }
        } catch(e) {
            callback(e);
        }
    }

    async _flush(callback) {
        try {
            if(this._storedChunks.length > 0) {
                const chunk = Buffer.concat(this._storedChunks);
                this._storedChunks = [];
                const _chunk = convertToInt32Buffer(chunk, this);
                const samples = Math.trunc(_chunk.length / this._channels / 4);
                await this._enc.processInterleavedAsync(_chunk, samples);
            }

            if(!(await this._enc.finishAsync())) {
                const err = this._enc.getState();
                const errStr = flac.Encoder.State[err];
                callback(new Error(errStr));
            } else {
                callback(null);
            }

            this._enc = null;
        } catch(e) {
            callback(e);
        }
    }

    _writeFlac(buffer) {
        const buffer2 = Buffer.from(buffer);
        this.push(buffer2);
        return 0;
    }
}

class FlacFileEncoder extends stream.Writable {
    constructor(options) {
        options = options || {};
        super({ ...options, decodeStrings: true });
        this._enc = new flac.Encoder();
        setOptions(this, options);
        this._file = options.file;
        if(!this._file) {
            throw new Error('No file passed as argument');
        }
    }

    async _write(chunk, _, callback) {
        try {
            if(!this._encoderInit) {
                setOptionsToEncoder(this);
                let res = flac.Encoder.InitStatus.OK;
                if(this._oggStream) {
                    res = await this._enc.initOggFileAsync(this._file, null);
                } else {
                    res = await this._enc.initFileAsync(this._file, null);
                }

                if(res !== flac.Encoder.InitStatus.OK) {
                    let initStatusString = flac.Encoder.InitStatusString[res];
                    if(res === flac.Encoder.InitStatus.ENCODER_ERROR) {
                        initStatusString = flac.Encoder.StateString[this._enc.getState()];
                    }
                    throw new Error(`Could not initialize encoder: ${initStatusString}`);
                }

                this._encoderInit = true;
            }

            const _chunk = convertToInt32Buffer(chunk, this);
            const samples = Math.trunc(_chunk.length / this._channels / 4);
            const extraBytes = _chunk.length - (samples * this._channels * 4);
            this._processedSamples += samples;
            if(extraBytes !== 0) {
                this._storedChunks.push(_chunk.slice(samples * this._channels * 4));
            }
            if(!(await this._enc.processInterleavedAsync(_chunk, samples))) {
                const err = this._enc.getState();
                const errStr = flac.Encoder.State[err];
                callback(new Error(errStr));
            } else {
                callback(null);
            }
        } catch(e) {
            callback(e);
        }
    }

    async _final(callback) {
        try {
            if(this._storedChunks.length > 0) {
                const chunk = Buffer.concat(this._storedChunks);
                this._storedChunks = [];
                const _chunk = convertToInt32Buffer(chunk, this);
                const samples = Math.trunc(_chunk.length / this._channels / 4);
                await this._enc.processInterleavedAsync(_chunk, samples);
            }

            await this._enc.finishAsync();
            this._enc = null;
            callback(null);
        } catch(e) {
            callback(e);
        }
    }
}

module.exports = {
    FileEncoder: FlacFileEncoder,
    StreamEncoder: FlacStreamEncoder,
};
