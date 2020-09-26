const debug = require('debug');
const stream = require('stream');
const flac = require('./api');

/**
 * Converts the buffer to int32_t if necessary
 * @param {Buffer} chunk A chunk of data
 * @param {FlacStreamEncoder | FlacFileEncoder} self this
 * @returns {Buffer} converted buffer
 */
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

/**
 * Stores all options in the object
 * @param {FlacStreamEncoder | FlacFileEncoder} self this
 * @param {any} options Object of options
 */
const setOptions = (self, options) => {
    self._encoderInit = false;
    self._oggStream = !!options.isOggStream;
    self._channels = parseInt(options.channels, 10) || 2;
    self._bitsPerSample = parseInt(options.bitsPerSample, 10) || 16;
    self._inputAs32 = (options.inputAs32 !== undefined && options.inputAs32 !== null) ?
        !!options.inputAs32 :
        (self._bitsPerSample === 24);
    self._samplerate = parseInt(options.samplerate, 10) || 44100;
    self._oggSerialNumber = parseInt(options.oggSerialNumber, 10) || null;
    self._totalSamplesEstimate = parseInt(options.totalSamplesEstimate, 10) || null;
    self._compressionLevel = parseInt(options.compressionLevel || '5', 10) || 5; //NOTE Workaround for #6
    self._doMidSideStereo = options.channels === 2 ? !!options.doMidSideStereo : undefined;
    self._looseMidSideStereo = options.channels === 2 ? !!options.looseMidSideStereo : undefined;
    self._apodization = 'string' === typeof options.apodization ? options.apodization : undefined;
    self._maxLpcOrder = parseInt(options.maxLpcOrder, 10) || null;
    self._qlpCoeffPrecision = parseInt(options.qlpCoeffPrecision, 10) || null;
    self._doQlpCoeffPrecSearch = options.doQlpCoeffPrecSearch;
    self._doExhaustiveModelSearch = options.doExhaustiveModelSearch;
    self._minResidualPartitionOrder = parseInt(options.minResidualPartitionOrder, 10) || null;
    self._maxResidualPartitionOrder = parseInt(options.maxResidualPartitionOrder, 10) || null;
    self._blocksize = parseInt(options.blocksize, 10) || null;
    self._metadata = options.metadata;
    self._processedSamples = 0;
    self._storedChunks = [];

    if(self._oggStream && !flac.format.API_SUPPORTS_OGG_FLAC) {
        throw new Error('Ogg FLAC is unsupported');
    }
};

/**
 * Sets the options to the encoder
 * @param {FlacStreamEncoder | FlacFileEncoder} self this
 */
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

/**
 * Process a chunk of data through the encoder
 * @param {FlacStreamEncoder | FlacFileEncoder} self this
 * @param {Buffer} chunk A chunk of data
 * @param {(err?: any) => void} callback Callback to notify end of process
 */
const processChunk = async (self, chunk, callback) => {
    try {
        if(!self._encoderInit) {
            setOptionsToEncoder(self);
            let res = flac.Encoder.InitStatus.OK;
            if(self._oggStream) {
                self._debug('Initializing encoder with Ogg');
                res = await self._initOggEncoder();
            } else {
                self._debug('Initializing encoder');
                res = await self._initEncoder();
            }

            if(res !== flac.Encoder.InitStatus.OK) {
                let initStatusString = flac.Encoder.InitStatusString[res];
                if(res === flac.Encoder.InitStatus.ENCODER_ERROR) {
                    initStatusString = flac.Encoder.StateString[self._enc.getState()];
                }
                self._debug(`Encoder init failed: ${initStatusString} [${res}]`);
                throw new Error(`Could not initialize encoder: ${initStatusString}`);
            }

            self._encoderInit = true;
        }

        const chunkToProcess = convertToInt32Buffer(chunk, self);
        const samples = Math.trunc(chunkToProcess.length / self._channels / 4);
        const extraBytes = chunkToProcess.length - (samples * self._channels * 4);
        self._processedSamples += samples;
        self._debug(`Received ${samples} samples (${chunkToProcess.length} bytes) to be processed`);
        if(extraBytes !== 0) {
            self._storedChunks.push(chunkToProcess.slice(samples * self._channels * 4));
            self._debug(`There are extra bytes which will be stored for the next process (${extraBytes})`);
        }

        if(!(await self._enc.processInterleavedAsync(chunkToProcess, samples))) {
            const err = self._enc.getState();
            const errStr = flac.Encoder.StateString[err];
            self._debug(`Process received samples failed: ${errStr} [${err}]`);
            throw new Error(errStr);
        } else {
            callback(null);
        }
    } catch(e) {
        callback(e);
    }
};

/**
 * Finalize the encoding process by flushing the encoder.
 * @param {FlacStreamEncoder | FlacFileEncoder} self this
 * @param {(err?: any) => void} callback Callback to notify end of process
 */
const finishEncoder = async (self, callback) => {
    try {
        if(!self._encoderInit) {
            self._debug('Encoder did not receive any data and it is being finalized');
            callback(null);
            return;
        }

        if(self._storedChunks.length > 0) {
            const chunk = Buffer.concat(self._storedChunks);
            self._storedChunks = [];
            const chunkToProcess = convertToInt32Buffer(chunk, self);
            const samples = Math.trunc(chunkToProcess.length / self._channels / 4);
            self._debug(`Processing final ${samples} samples (${chunkToProcess.length} bytes)`);

            if(!(await self._enc.processInterleavedAsync(chunkToProcess, samples))) {
                const err = self._enc.getState();
                const errStr = flac.Encoder.StateString[err];
                self._debug(`Process final samples failed: ${errStr} [${err}]`);
                throw new Error(errStr);
            }
        }

        self._debug('Flushing encoder');
        if(!(await self._enc.finishAsync())) {
            const err = self._enc.getState();
            const errStr = flac.Encoder.StateString[err];
            self._debug(`Flush encoder failed: ${errStr} [${err}]`);
            throw new Error(errStr);
        }

        callback(null);
        self._enc = null;
    } catch(e) {
        callback(e);
    }
};

class FlacStreamEncoder extends stream.Transform {
    constructor(options) {
        options = options || {};
        super({ ...options, decodeStrings: false, defaultEncoding: undefined, encoding: undefined });
        this._debug = debug('flac:encoder:stream');
        this._enc = new flac.Encoder();
        this._encoderInit = false;
        setOptions(this, options);
    }

    _transform(chunk, _, callback) {
        return processChunk(this, chunk, callback);
    }

    _flush(callback) {
        return finishEncoder(this, callback);
    }

    _initEncoder() {
        return this._enc.initStreamAsync(this._writeFlac.bind(this), null, null, null);
    }

    _initOggEncoder() {
        return this._enc.initOggStreamAsync(null, this._writeFlac.bind(this), null, null, null);
    }

    _writeFlac(buffer) {
        const buffer2 = Buffer.from(buffer);
        this.push(buffer2);
        this._debug(`Received ${buffer2.length} bytes of encoded flac data`);
        return flac.Encoder.WriteStatus.OK;
    }
}

class FlacFileEncoder extends stream.Writable {
    constructor(options) {
        options = options || {};
        super({ ...options, decodeStrings: true });
        this._debug = debug('flac:encoder:file');
        this._enc = new flac.Encoder();
        this._encoderInit = false;
        setOptions(this, options);
        this._file = options.file;
        if(!this._file) {
            throw new Error('No file passed as argument');
        }
    }

    _write(chunk, _, callback) {
        return processChunk(this, chunk, callback);
    }

    _final(callback) {
        return finishEncoder(this, callback);
    }

    _initEncoder() {
        return this._enc.initFileAsync(this._file, null);
    }

    _initOggEncoder() {
        return this._enc.initOggFileAsync(this._file, null);
    }
}

module.exports = {
    FileEncoder: FlacFileEncoder,
    StreamEncoder: FlacStreamEncoder,
};
