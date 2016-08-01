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
                this._decoderInit = true;
            }
        }

        this._le_chunks.push(chunk);
        if(flac.decoder.process_single(this._dec)) {
            let error = flac.decoder.get_state(this._dec);
            let errorObj = new Error(flac.decoder.get_resolved_state_string(this._dec));
            errorObj.code = error;
            this.emit('error', errorObj);
        }
    }

    _flush(callback) {
        flac.decoder.finish(this._dec);
        flac.decoder.delete(this._dec);
        this._dec = null;
    }

    _writeCbk(frame, buffers) {
        let b = this.bitsPerSample / 8;
        let buff = Buffer.allocUnsafe(frame.header.blocksize * buffers.length * b);
        for(let sample = 0; sample < frame.header.blocksize; sample++) {
            for(let channel = 0; channel < buffers.length; channel++) {
                let s = buffers[channel].readInt32LE(sample * 4);
                buff.writeIntLE(s, (buffers.length * sample + channel) * b, b);
            }
        }
        this.push(buff);
        return 0;
    }

    _read(buffer, bytes) {
        //TODO: I supposed that we never pass the `bytes` value
        this._le_chunks[0].copy(buffer);
        this._le_chunks = this._le_chunks.slice(1);
        return { bytes: this._le_chunk.length, returnValue: flac.decoder.ReadStatus.CONTINUE };
    }

    _metadataCbk(metadata) {
        if(metadata.type === flac.format.MetadataType.STREAMINFO) {
            this.sampleRate = metadata.sample_rate;
            this.channels = metadata.channels;
            this.bitsPerSample = metadata.bits_per_sample;
            this.totalSamples = metadata.total_samples;
        }

        this.emit('metadata', metadata);
    }

    _errorCbk(error) {
        let errorObj = new Error(flac.decoder.ErrorStatusString[error]);
        errorObj.code = error;
        this.emit('error', errorObj);
    }
}

class FlacFileDecoder extends stream.Readable {
    constructor(options) {
        options = options || {};
        super(options);
        this._dec = flac.decoder.new();
        this._decoderInit = false;
        this._oggStream = options.isOggStream || false;
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
                buff.writeIntLE(s, (buffers.length * sample + channel) * b, b);
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
        }

        this.emit('metadata', metadata);
    }

    _errorCbk(error) {
        let errorObj = new Error(flac.decoder.ErrorStatusString[error]);
        errorObj.code = error;
        this.emit('error', errorObj);
    }
}

module.exports = {
    FileDecoder: FlacFileDecoder,
    StreamDecoder: FlacStreamDecoder
};
