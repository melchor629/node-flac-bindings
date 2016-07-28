//jshint esversion: 6
const stream = require('stream');
const flac = require('bindings')('flac-bindings');
const Buffer = require('buffer').Buffer;

class FlacStreamDecoder extends stream.Transform {
    constructor(options) {
        super(options);
    }

    _transform(chunk, encoding, callback) {

    }

    _flush(callback) {

    }
}

class FlacFileDecoder extends stream.Readable {
    constructor(options) {
        super(options);
        this._dec = flac.decoder.new();
        this._decoderInit = false;
        this._oggStream = options.isOggStream || false;
        this._file = options.file;
        if(!this._file) throw new Error("No file passed as argument");
    }

    _read(frames) {
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
            }
        }

        for(let i = 0; i < frames || 1; i++) {
            flac.decoder.process_single(this._dec, null);
        }
    }

    _writeCbk(dec, frame, buffers) {
        let buff = Buffer.allocUnsafeSlow(frame.header.blocksize * buffers.length * 4);
        for(let sample = 0; sample < frame.header.blocksize; sample++) {
            for(let channel = 0; channel < buffers.length; channel++) {
                buff.writeInt32LE(buffers[channel].readInt32LE(sample*4), (buffers.length*sample+channel)*4);
            }
        }
        this.push(buff);
    }

    _metadataCbk(dec, metadata) {
        if(metadata.type === flac.format.MetadataType.STREAMINFO) {
            this.sampleRate = metadata.sampleRate;
            this.channels = metadata.channels;
            this.bitsPerSample = metadata.bitsPerSample;
            this.totalSamples = metadata.totalSamples;
        }

        this.emit('metadata', metadata);
    }

    _errorCbk(dec, error) {
        if(error === flac.decoder.State.END_OF_STREAM) {
            this.emit('end');
            flac.decoder.delete(this._dec);
            this._dec = null;
        } else {
            let errorObj = new Error(flac.decoder.ErrorStatusString[error]);
            errorObj.code = error;
            this.emit('error', errorObj);
        }
    }
}

module.exports = {
    FileDecoder: FlacFileDecoder,
    StreamDecoder: FlacStreamDecoder
};
