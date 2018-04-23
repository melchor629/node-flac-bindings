//jshint esversion: 6
const stream = require('stream');
const flac = require('bindings')('flac-bindings');
const Buffer = require('buffer').Buffer;

class FlacStreamDecoder extends stream.Transform {
    constructor(options) {
        options = options || {};
        super(options);
        this._dec = flac.decoder.new();
        this._decoderInit = false;
        this._oggStream = options.isOggStream || false;
        this._metadataRespond = options.metadata;
        this._outputAs32 = options.outputAs32;
        this._le_chunks = [];
    }

    _transform(chunk, encoding, callback) {
        if(!this._decoderInit) {
            let initRet;
            if(this._oggStream) {
                initRet = flac.decoder.init_ogg_stream(this._dec,
                    this._readCbk.bind(this),
                    null,
                    null,
                    null,
                    null,
                    this._writeCbk.bind(this),
                    this._metadataCbk.bind(this),
                    this._errorCbk.bind(this));
            } else {
                initRet = flac.decoder.init_stream(this._dec,
                    this._readCbk.bind(this),
                    null,
                    null,
                    null,
                    null,
                    this._writeCbk.bind(this),
                    this._metadataCbk.bind(this),
                    this._errorCbk.bind(this));
            }
            if(initRet !== 0) {
                let errorObj = new Error(flac.decoder.InitStatusString[initRet]);
                errorObj.code = initRet;
                this.emit('error', errorObj);
                return;
            } else {
                if(this._metadataRespond) {
                    if(this._metadataRespond === true) {
                        flac.decoder.set_metadata_respond_all(this._dec);
                    } else {
                        for(let type in this._metadataRespond) {
                            flac.decoder.set_metadata_respond(this._dec, type);
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
            if(!flac.decoder.process_single(this._dec)) {
                let error = flac.decoder.get_state(this._dec);
                let errorObj = new Error(flac.decoder.get_resolved_state_string(this._dec));
                errorObj.code = error;
                this.emit('error', errorObj);
            }
        } while(this._le_chunks.length >= 5);
        if(this._le_chunks.length <= 10) this._trCbk();
    }

    _flush(callback) {
        while(this._le_chunks.length > 0) {
            if(!flac.decoder.process_single(this._dec)) {
                let error = flac.decoder.get_state(this._dec);
                let errorObj = new Error(flac.decoder.get_resolved_state_string(this._dec));
                errorObj.code = error;
                this.emit('error', errorObj);
            }
        }
        flac.decoder.finish(this._dec);
        flac.decoder.delete(this._dec);
        this._dec = null;
        callback();
    }

    _writeCbk(frame, buffers) {
        let b = this.bitsPerSample / 8;
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
        return { bytes: bytesRead, returnValue: flac.decoder.ReadStatus.CONTINUE };
    }

    _metadataCbk(metadata) {
        if(metadata.type === flac.format.MetadataType.STREAMINFO) {
            this.sampleRate = metadata.sample_rate;
            this.channels = metadata.channels;
            this.bitsPerSample = metadata.bits_per_sample;
            this.totalSamples = metadata.total_samples;
            this._outputAs32 = (this._outputAs32 !== undefined && this._outputAs32 !== null) ? Boolean(this._outputAs32) : (this._bitsPerSample === 24);
        }

        this.emit('metadata', metadata);
    }

    _errorCbk(error) {
        let errorObj = new Error(flac.decoder.ErrorStatusString[error]);
        errorObj.code = error;
        this.emit('error', errorObj);
    }

    getTotalSamples() {
        if(this._decoderInit) {
            return flac.decoder.get_total_samples(this._dec);
        }
    }

    getChannels() {
        if(this._decoderInit) {
            return flac.decoder.get_channels(this._dec);
        }
    }

    getChannelAssignment() {
        if(this._decoderInit) {
            return flac.decoder.get_channel_assignment(this._dec);
        }
    }

    getBitsPerSample() {
        if(this._decoderInit) {
            return flac.decoder.get_bits_per_sample(this._dec);
        }
    }
}

class FlacFileDecoder extends stream.Readable {
    constructor(options) {
        options = options || {};
        super(options);
        this._dec = flac.decoder.new();
        this._decoderInit = false;
        this._oggStream = options.isOggStream || false;
        this._metadataRespond = options.metadata;
        this._outputAs32 = options.outputAs32;
        this._file = options.file;
        if(!this._file) throw new Error("No file passed as argument");
    }

    _read() {
        if(!this._decoderInit) {
            let initRet;
            if(this._oggStream) {
                initRet = flac.decoder.init_ogg_file(this._dec, this._file,
                    this._writeCbk.bind(this),
                    this._metadataCbk.bind(this),
                    this._errorCbk.bind(this));
            } else {
                initRet = flac.decoder.init_file(this._dec, this._file,
                    this._writeCbk.bind(this),
                    this._metadataCbk.bind(this),
                    this._errorCbk.bind(this));
            }
            if(initRet !== 0) {
                let errorObj = new Error(flac.decoder.InitStatusString[initRet]);
                errorObj.code = initRet;
                this.emit('error', errorObj);
                return;
            } else {
                if(this._metadataRespond) {
                    if(this._metadataRespond === true) {
                        flac.decoder.set_metadata_respond_all(this._dec);
                    } else {
                        for(let type in this._metadataRespond) {
                            flac.decoder.set_metadata_respond(this._dec, type);
                        }
                    }
                }
                flac.decoder.process_until_end_of_metadata(this._dec);
                this._decoderInit = true;
            }
        }

        let d = flac.decoder.process_single(this._dec, null);
        if(!d || flac.decoder.get_state(this._dec) === flac.decoder.State.END_OF_STREAM) {
            let error = flac.decoder.get_state(this._dec);
            if(error == flac.decoder.State.END_OF_STREAM) {
                this.push(null);
                flac.decoder.finish(this._dec);
                flac.decoder.delete(this._dec);
                this._dec = null;
            } else {
                let errorObj = new Error(flac.decoder.get_resolved_state_string(this._dec));
                errorObj.code = error;
                this.emit('error', errorObj);
            }
        }
    }

    _writeCbk(frame, buffers) {
        let b = this.bitsPerSample / 8;
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
        this.push(buff);
        return 0;
    }

    _metadataCbk(metadata) {
        if(metadata.type === flac.format.MetadataType.STREAMINFO) {
            this.sampleRate = metadata.sample_rate;
            this.channels = metadata.channels;
            this.bitsPerSample = metadata.bits_per_sample;
            this.totalSamples = metadata.total_samples;
            this._outputAs32 = (this._outputAs32 !== undefined && this._outputAs32 !== null) ? Boolean(this._outputAs32) : (this._bitsPerSample === 24);
        }

        this.emit('metadata', metadata);
    }

    _errorCbk(error) {
        let errorObj = new Error(flac.decoder.ErrorStatusString[error]);
        errorObj.code = error;
        this.emit('error', errorObj);
    }

    getTotalSamples() {
        if(this._decoderInit) {
            return flac.decoder.get_total_samples(this._dec);
        }
    }

    getChannels() {
        if(this._decoderInit) {
            return flac.decoder.get_channels(this._dec);
        }
    }

    getChannelAssignment() {
        if(this._decoderInit) {
            return flac.decoder.get_channel_assignment(this._dec);
        }
    }

    getBitsPerSample() {
        if(this._decoderInit) {
            return flac.decoder.get_bits_per_sample(this._dec);
        }
    }
}

module.exports = {
    FileDecoder: FlacFileDecoder,
    StreamDecoder: FlacStreamDecoder
};
