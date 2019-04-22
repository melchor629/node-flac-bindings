//jshint esversion: 6
const stream = require('stream');
const flac = require('bindings')('flac-bindings');
const Buffer = require('buffer').Buffer;

class FlacStreamDecoder extends stream.Transform {
    constructor(options) {
        options = options || {};
        super(options);
        this._dec = new flac.Decoder();
        this._decoderInit = false;
        this._oggStream = options.isOggStream || false;
        this._metadataRespond = options.metadata;
        this._outputAs32 = options.outputAs32;
        this._le_chunks = [];
        this._processedSamples = 0;
    }

    _transform(chunk, encoding, callback) {
        if(!this._decoderInit) {
            let initRet;
            if(this._oggStream) {
                initRet = this._dec.initOggStream(
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
                initRet = this._dec.initStream(
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

            if(initRet !== 0) {
                let errorObj = new Error(flac.Decoder.InitStatusString[initRet]);
                errorObj.code = initRet;
                this.emit('error', errorObj);
                return;
            } else {
                if(this._metadataRespond) {
                    if(this._metadataRespond === true) {
                        this._dec.setMetadataRespondAll();
                    } else {
                        for(let type of this._metadataRespond) {
                            this._dec.setMetadataRespond(type);
                        }
                    }
                }
                this._decoderInit = true;
            }
        }

        chunk.startIndex_ = 0;
        this._trCbk = callback;
        this._le_chunks.push(chunk);
        do {
            if(!this._dec.processSingle()) {
                this._emitDecoderError();
            }
        } while(this._le_chunks.length >= 5);
        if(this._le_chunks.length <= 10) this._trCbk();
    }

    _flush(callback) {
        while(this._le_chunks.length > 0) {
            if(!this._dec.processSingle()) {
                this._emitDecoderError();
            }
        }
        this._dec.finish();
        this._dec = null;
        callback();
    }

    _writeCbk(frame, buffers) {
        let b = this._outputAs32 ? 4 : this.bitsPerSample / 8;
        let buff = Buffer.allocUnsafe(frame.header.blocksize * buffers.length * b);
        for(let sample = 0; sample < frame.header.blocksize; sample++) {
            for(let channel = 0; channel < buffers.length; channel++) {
                let s = buffers[channel].readInt32LE(sample * 4);
                if(this._outputAs32) {
                    buff.writeInt32LE(s, (buffers.length * sample + channel) * 4);
                } else {
                    buff.writeIntLE(s, (buffers.length * sample + channel) * b, b);
                }
            }
        }
        this._processedSamples += frame.header.blocksize;
        this.push(buff);
        return 0;
    }

    _readCbk(buffer) {
        let b = this._le_chunks[0];
        let bytesRead = b.copy(buffer, 0, b.startIndex_);
        b.startIndex_ += bytesRead;
        if(b.startIndex_ === b.length) {
            this._le_chunks = this._le_chunks.slice(1);
        }
        return { bytes: bytesRead, returnValue: flac.Decoder.ReadStatus.CONTINUE };
    }

    _metadataCbk(metadata) {
        if(metadata.type === flac.format.MetadataType.STREAMINFO) {
            this.sampleRate = metadata.sampleRate;
            this.channels = metadata.channels;
            this.bitsPerSample = metadata.bitsPerSample;
            this.totalSamples = metadata.totalSamples;
            this._outputAs32 = (this._outputAs32 !== undefined && this._outputAs32 !== null) ? Boolean(this._outputAs32) : (this._bitsPerSample === 24);
        }

        this.emit('metadata', metadata);
    }

    _errorCbk(error) {
        let errorObj = new Error(flac.Decoder.ErrorStatusString[error]);
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
        if(this._decoderInit) {
            return this._dec.getTotalSamples();
        }
    }

    getChannels() {
        if(this._decoderInit) {
            return this._dec.getChannels();
        }
    }

    getChannelAssignment() {
        if(this._decoderInit) {
            return this._dec.getChannelAssignment();
        }
    }

    getBitsPerSample() {
        if(this._decoderInit) {
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
        if(!this._file) throw new Error("No file passed as argument");
        this._processedSamples = 0;
    }

    async _read() {
        do { if(await this._theLock) return; } while(this._theLock);
        let res;
        this._theLock = new Promise((resolve) => res = resolve)
            .then((...args) => { this._theLock = undefined; return Promise.resolve(...args); });

        if(!this._decoderInit) {
            try {
                if(this._oggStream) {
                    await this._dec.initOggFileAsync(
                        this._file,
                        this._writeCbk.bind(this),
                        this._metadataCbk.bind(this),
                        this._errorCbk.bind(this)
                    );
                } else {
                    await this._dec.initFileAsync(
                        this._file,
                        this._writeCbk.bind(this),
                        this._metadataCbk.bind(this),
                        this._errorCbk.bind(this)
                    );
                }
            } catch (err) {
                this.emit('error', err);
                res(true);
            }

            if(this._metadataRespond) {
                if(this._metadataRespond === true) {
                    this._dec.setMetadataRespondAll();
                } else {
                    for(let type of this._metadataRespond) {
                        this._dec.setMetadataRespond(type);
                    }
                }
            }

            await this._dec.processUntilEndOfMetadataAsync();
            this._decoderInit = true;
        }

        this._streamIsFull = false;
        while(!this._streamIsFull && this._dec !== null) {
            this._streamIsFull = false;
            const couldProcess = await this._dec.processSingleAsync();
            const decState = this._dec.getState();
            if(decState == flac.Decoder.State.END_OF_STREAM) {
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
    }

    _writeCbk(frame, buffers) {
        let b = this._outputAs32 ? 4 : this.bitsPerSample / 8;
        let buff = Buffer.allocUnsafe(frame.header.blocksize * buffers.length * b);
        for(let sample = 0; sample < frame.header.blocksize; sample++) {
            for(let channel = 0; channel < buffers.length; channel++) {
                let s = buffers[channel].readInt32LE(sample * 4);
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
            this._outputAs32 = (this._outputAs32 !== undefined && this._outputAs32 !== null) ? Boolean(this._outputAs32) : (this._bitsPerSample === 24);
        }

        this.emit('metadata', metadata);
    }

    _errorCbk(error) {
        let errorObj = new Error(flac.Decoder.ErrorStatusString[error]);
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
        if(this._decoderInit) {
            return this._dec.getTotalSamples();
        }
    }

    getChannels() {
        if(this._decoderInit) {
            return this._dec.getChannels();
        }
    }

    getChannelAssignment() {
        if(this._decoderInit) {
            return this._dec.getChannelAssignment();
        }
    }

    getBitsPerSample() {
        if(this._decoderInit) {
            return this._dec.getBitsPerSample();
        }
    }
}

module.exports = {
    FileDecoder: FlacFileDecoder,
    StreamDecoder: FlacStreamDecoder
};
