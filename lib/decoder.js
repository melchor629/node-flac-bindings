const stream = require('stream');
const flac = require('./api');

class FlacStreamDecoder extends stream.Transform {
    constructor(options) {
        options = options || {};
        super({ ...options, decodeStrings: false, defaultEncoding: undefined, encoding: undefined });
        this._dec = new flac.Decoder();
        this._decoderInit = false;
        this._oggStream = options.isOggStream || false;
        this._metadataRespond = options.metadata;
        this._outputAs32 = options.outputAs32;
        this._chunks = [];
        this._processedSamples = 0;
    }

    async _transform(chunk, _, callback) {
        try {
            if(!this._decoderInit) {
                if(this._metadataRespond) {
                    if(this._metadataRespond === true) {
                        this._dec.setMetadataRespondAll();
                    } else {
                        for(const type of this._metadataRespond) {
                            this._dec.setMetadataRespond(type);
                        }
                    }
                }

                let initStatus = flac.Decoder.InitStatus.OK;
                if(this._oggStream) {
                    initStatus = await this._dec.initOggStreamAsync(
                        this._readCbk.bind(this),
                        null,
                        null,
                        null,
                        null,
                        this._writeCbk.bind(this),
                        this._metadataCbk.bind(this),
                        this._errorCbk.bind(this)
                    );
                } else {
                    initStatus = await this._dec.initStreamAsync(
                        this._readCbk.bind(this),
                        null,
                        null,
                        null,
                        null,
                        this._writeCbk.bind(this),
                        this._metadataCbk.bind(this),
                        this._errorCbk.bind(this)
                    );
                }

                if(initStatus !== flac.Decoder.InitStatus.OK) {
                    const initStatusString = flac.Decoder.InitStatusString[initStatus];
                    callback(new Error(`Could not initialize decoder: ${initStatusString}`));
                    return;
                }

                this._decoderInit = true;
                if(!(await this._dec.processUntilEndOfMetadata())) {
                    callback(new Error(`Could not process metadata: ${this._dec.getResolvedStateString()}`));
                    return;
                }
            }

            this._chunks.push(chunk);
            this._streamIsFull = false;
            while(!this._streamIsFull && !this._chunksIsAlmostEmpty() && this._dec !== null) {
                this._streamIsFull = false;
                this._lastBytesDemanded = 0;
                if(!(await this._dec.processSingleAsync())) {
                    this._emitDecoderError(callback);
                    return;
                }
            }
            callback();
        } catch(e) {
            callback(e);
        }
    }

    async _flush(callback) {
        try {
            this._timeToDie = true;
            if(this._chunks.length > 0) {
                if(!(await this._dec.processUntilEndOfStreamAsync())) {
                    this._emitDecoderError(callback);
                    return;
                }
            }
            await this._dec.finishAsync();
            this._dec = null;
            callback();
        } catch(e) {
            callback(e);
        }
    }

    _writeCbk(frame, buffers) {
        const b = this._outputAs32 ? 4 : this.bitsPerSample / 8;
        const buff = Buffer.allocUnsafe(frame.header.blocksize * buffers.length * b);
        for(let sample = 0; sample < frame.header.blocksize; sample++) {
            for(let channel = 0; channel < buffers.length; channel++) {
                const s = buffers[channel].readInt32LE(sample * 4);
                if(this._outputAs32) {
                    buff.writeInt32LE(s, (buffers.length * sample + channel) * 4);
                } else {
                    buff.writeIntLE(s, (buffers.length * sample + channel) * b, b);
                }
            }
        }
        this._processedSamples += frame.header.blocksize;
        this._streamIsFull = !this.push(buff);
        return 0;
    }

    _readCbk(buffer) {
        if(this._chunks.length > 0) {
            const b = this._chunks[0];
            const bytesRead = b.copy(buffer, 0);
            if(bytesRead === b.length) {
                this._chunks = this._chunks.slice(1);
            } else {
                this._chunks[0] = b.slice(bytesRead);
            }
            this._lastBytesDemanded += buffer.length;
            return { bytes: bytesRead, returnValue: flac.Decoder.ReadStatus.CONTINUE };
        }

        if(this._timeToDie) {
            return { bytes: 0, returnValue: flac.Decoder.ReadStatus.END_OF_STREAM };
        }

        console.trace('Decoder stream got empty: that is an issue');
        return { bytes: 0, returnValue: flac.Decoder.ReadStatus.ABORT };
    }

    _metadataCbk(metadata) {
        if(metadata.type === flac.format.MetadataType.STREAMINFO) {
            this.sampleRate = metadata.sampleRate;
            this.channels = metadata.channels;
            this.bitsPerSample = metadata.bitsPerSample;
            this.totalSamples = metadata.totalSamples;
            this._outputAs32 = (this._outputAs32 !== undefined && this._outputAs32 !== null) ?
                Boolean(this._outputAs32) :
                (this._bitsPerSample === 24);
        }

        this.emit('metadata', metadata);
    }

    _errorCbk(error) {
        const errorObj = new Error(flac.Decoder.ErrorStatusString[error]);
        errorObj.code = error;
        this.emit('error', errorObj);
    }

    _emitDecoderError(cbk) {
        const error = this._dec.getState();
        const errorObj = new Error(this._dec.getResolvedStateString());
        errorObj.code = error;
        cbk(errorObj);
    }

    _chunksIsAlmostEmpty() {
        if(this._lastBytesDemanded !== undefined) {
            return this._chunks.reduce((r, v) => r + v.length, 0) < this._lastBytesDemanded * 2;
        } else {
            return this._chunks.length < 2;
        }
    }

    getTotalSamples() {
        if(this._decoderInit && this._dec) {
            return this._dec.getTotalSamples();
        }
    }

    getChannels() {
        if(this._decoderInit && this._dec) {
            return this._dec.getChannels();
        }
    }

    getChannelAssignment() {
        if(this._decoderInit && this._dec) {
            return this._dec.getChannelAssignment();
        }
    }

    getBitsPerSample() {
        if(this._decoderInit && this._dec) {
            return this._dec.getBitsPerSample();
        }
    }
}

class FlacFileDecoder extends stream.Readable {
    constructor(options) {
        options = options || {};
        super(options);
        this._dec = new flac.Decoder();
        this._decoderInit = false;
        this._oggStream = options.isOggStream || false;
        this._metadataRespond = options.metadata;
        this._outputAs32 = options.outputAs32;
        this._file = options.file;
        if(!this._file) {
            throw new Error('No file passed as argument');
        }
        this._processedSamples = 0;
    }

    async _read() {
        do {
            if(await this._theLock) {
                return;
            }
        } while(this._theLock);
        let res;
        this._theLock = new Promise((resolve) => res = resolve)
            .then((...args) => {
                this._theLock = undefined;
                return Promise.resolve(...args);
            });

        try {
            if(!this._decoderInit) {
                if(this._metadataRespond) {
                    if(this._metadataRespond === true) {
                        this._dec.setMetadataRespondAll();
                    } else {
                        for(const type of this._metadataRespond) {
                            this._dec.setMetadataRespond(type);
                        }
                    }
                }

                let initStatus = flac.Decoder.InitStatus.OK;
                if(this._oggStream) {
                    initStatus = await this._dec.initOggFileAsync(
                        this._file,
                        this._writeCbk.bind(this),
                        this._metadataCbk.bind(this),
                        this._errorCbk.bind(this)
                    );
                } else {
                    initStatus = await this._dec.initFileAsync(
                        this._file,
                        this._writeCbk.bind(this),
                        this._metadataCbk.bind(this),
                        this._errorCbk.bind(this)
                    );
                }

                if(initStatus !== flac.Decoder.InitStatus.OK) {
                    const initStatusString = flac.Decoder.InitStatusString[initStatus];
                    this.emit('error', new Error(`Could not initialize decoder: ${initStatusString}`));
                    return;
                }

                this._decoderInit = true;
                if(!(await this._dec.processUntilEndOfMetadata())) {
                    this.emit('error', new Error(`Could not process metadata: ${this._dec.getResolvedStateString()}`));
                    return;
                }
            }

            this._streamIsFull = false;
            while(!this._streamIsFull && this._dec !== null) {
                this._streamIsFull = false;
                const couldProcess = await this._dec.processSingleAsync();
                const decState = this._dec.getState();
                if(decState === flac.Decoder.State.END_OF_STREAM) {
                    this.push(null);
                    await this._dec.finishAsync();
                    this._dec = null;
                    this._streamIsFull = true;
                } else if(!couldProcess) {
                    this._emitDecoderError();
                    res(true);
                    return;
                }
            }

            res(false);
        } catch(e) {
            this.emit('error', e);
            res(true);
            return;
        }
    }

    _writeCbk(frame, buffers) {
        const b = this._outputAs32 ? 4 : this.bitsPerSample / 8;
        const buff = Buffer.allocUnsafe(frame.header.blocksize * buffers.length * b);
        for(let sample = 0; sample < frame.header.blocksize; sample++) {
            for(let channel = 0; channel < buffers.length; channel++) {
                const s = buffers[channel].readInt32LE(sample * 4);
                if(this._outputAs32) {
                    buff.writeInt32LE(s, (buffers.length * sample + channel) * 4);
                } else {
                    buff.writeIntLE(s, (buffers.length * sample + channel) * b, b);
                }
            }
        }
        this._processedSamples += frame.header.blocksize;
        this._streamIsFull = !this.push(buff);
        return 0;
    }

    _metadataCbk(metadata) {
        if(metadata.type === flac.format.MetadataType.STREAMINFO) {
            this.sampleRate = metadata.sampleRate;
            this.channels = metadata.channels;
            this.bitsPerSample = metadata.bitsPerSample;
            this.totalSamples = metadata.totalSamples;
            this._outputAs32 = (this._outputAs32 !== undefined && this._outputAs32 !== null) ?
                Boolean(this._outputAs32) :
                (this._bitsPerSample === 24);
        }

        this.emit('metadata', metadata);
    }

    _errorCbk(error) {
        const errorObj = new Error(flac.Decoder.ErrorStatusString[error]);
        errorObj.code = error;
        this.emit('error', errorObj);
    }

    _emitDecoderError() {
        const error = this._dec.getState();
        const errorObj = new Error(this._dec.getResolvedStateString());
        errorObj.code = error;
        this.emit('error', errorObj);
    }

    getTotalSamples() {
        if(this._decoderInit && this._dec) {
            return this._dec.getTotalSamples();
        }
    }

    getChannels() {
        if(this._decoderInit && this._dec) {
            return this._dec.getChannels();
        }
    }

    getChannelAssignment() {
        if(this._decoderInit && this._dec) {
            return this._dec.getChannelAssignment();
        }
    }

    getBitsPerSample() {
        if(this._decoderInit && this._dec) {
            return this._dec.getBitsPerSample();
        }
    }
}

module.exports = {
    FileDecoder: FlacFileDecoder,
    StreamDecoder: FlacStreamDecoder,
};
