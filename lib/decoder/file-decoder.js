/* eslint-disable no-await-in-loop */
const debug = require('debug')
const { Readable } = require('stream')
const flac = require('../api')

class FileDecoder extends Readable {
  constructor(options = {}) {
    super(options)
    this._debug = debug('flac:decoder:file')
    this._dec = new flac.Decoder()
    this._decoderInit = false
    this._oggStream = options.isOggStream || false
    this._metadataRespond = options.metadata
    this._outputAs32 = options.outputAs32
    this._file = options.file
    this._processedSamples = 0
    this._failed = false

    if (!this._file) {
      throw new Error('No file passed as argument')
    }

    if (this._oggStream && !flac.format.API_SUPPORTS_OGG_FLAC) {
      throw new Error('Ogg FLAC is unsupported')
    }
  }

  get processedSamples() {
    return this._processedSamples
  }

  getTotalSamples() {
    if (this._decoderInit) {
      if (this._totalSamples === undefined) {
        this._totalSamples = this._dec.getTotalSamples()
      }
      return this._totalSamples
    }

    return undefined
  }

  getChannels() {
    if (this._decoderInit) {
      if (this._channels === undefined) {
        this._channels = this._dec.getChannels()
      }
      return this._channels
    }

    return undefined
  }

  getChannelAssignment() {
    if (this._decoderInit) {
      if (this._channelAssignment === undefined) {
        this._channelAssignment = this._dec.getChannelAssignment()
      }
      return this._channelAssignment
    }

    return undefined
  }

  getBitsPerSample() {
    if (this._decoderInit) {
      if (this._bitsPerSample === undefined) {
        this._bitsPerSample = this._dec.getBitsPerSample()
      }
      return this._bitsPerSample
    }

    return undefined
  }

  getOutputBitsPerSample() {
    if (this._decoderInit) {
      if (this._outputAs32) {
        return 32
      }

      return this.getBitsPerSample()
    }

    return undefined
  }

  getSampleRate() {
    if (this._decoderInit) {
      if (this._sampleRate === undefined) {
        this._sampleRate = this._dec.getSampleRate()
      }
      return this._sampleRate
    }

    return undefined
  }

  getProgress() {
    if (this._decoderInit) {
      const position = this._processedSamples
      const totalSamples = this.getTotalSamples()
      const percentage = totalSamples ? position / totalSamples : NaN
      const totalSeconds = totalSamples ? totalSamples / this.getSampleRate() : NaN
      const currentSeconds = position / this.getSampleRate()
      return {
        position,
        totalSamples,
        percentage,
        totalSeconds,
        currentSeconds,
      }
    }

    return undefined
  }

  _read() {
    if (!this._readLoopPromise) {
      this._readLoopPromise = this._readLoop().then(() => {
        if (this.readableLength === 0 && !this._failed) {
          this._read()
        }
      })
    }
  }

  async _readLoop() {
    try {
      if (!this._decoderInit) {
        this._debug('Initializing decoder')
        if (this._metadataRespond) {
          if (this._metadataRespond === true) {
            this._debug('Setting decoder to emit all metadata blocks')
            this._dec.setMetadataRespondAll()
          } else {
            this._debug(`Setting decoder to emit '${this._metadataRespond.join(', ')}' metadata blocks`)
            for (const type of this._metadataRespond) {
              this._dec.setMetadataRespond(type)
            }
          }
        }

        try {
          if (this._oggStream) {
            this._debug('Initializing for Ogg/FLAC')
            await this._dec.initOggFileAsync(
              this._file,
              this._writeCbk.bind(this),
              this._metadataCbk.bind(this),
              this._errorCbk.bind(this),
            )
          } else {
            this._debug('Initializing for FLAC')
            await this._dec.initFileAsync(
              this._file,
              this._writeCbk.bind(this),
              this._metadataCbk.bind(this),
              this._errorCbk.bind(this),
            )
          }
        } catch (error) {
          const initStatus = error.status || -1
          const initStatusString = error.statusString || 'Unknown'
          this._debug(`Failed initializing decoder: ${initStatus} ${initStatusString}`)
          throw error
        }

        this._decoderInit = true
      }

      this._debug('Wants data from decoder')
      this._streamIsFull = false
      while (!this._streamIsFull && this._dec !== null) {
        this._streamIsFull = false
        const couldProcess = await this._dec.processSingleAsync()
        const decState = this._dec.getState()
        if (decState === flac.Decoder.State.END_OF_STREAM) {
          this._debug('Decoder reached EOF -> flushing decoder and finishing stream')
          await this._dec.finishAsync()
          this.push(null)
          this._dec = null
          this._streamIsFull = true
        } else if (!couldProcess) {
          this._emitDecoderError()
          return
        }
      }
    } catch (e) {
      this._failed = true
      this.emit('error', e)
    }

    this._readLoopPromise = null
  }

  _writeCbk(frame, buffers) {
    const outBps = this.getOutputBitsPerSample() / 8
    const buff = flac.fns.zipAudio({ samples: frame.header.blocksize, outBps, buffers })

    this._processedSamples += frame.header.blocksize
    this._streamIsFull = !this.push(buff)
    this._debug(`Received ${frame.header.blocksize} samples (${buff.length} bytes) of decoded data`)
    if (this._streamIsFull) {
      this._debug('Stream is now full, pausing stream')
    }

    return flac.Decoder.WriteStatus.CONTINUE
  }

  _metadataCbk(metadata) {
    if (metadata.type === flac.format.MetadataType.STREAMINFO) {
      this._outputAs32 = (this._outputAs32 != null)
        ? !!this._outputAs32
        : (metadata.bitsPerSample === 24)
      this.emit('format', {
        channels: metadata.channels,
        bitDepth: metadata.bitsPerSample,
        bitsPerSample: metadata.bitsPerSample,
        is32bit: this._outputAs32,
        sampleRate: metadata.sampleRate,
        totalSamples: metadata.totalSamples,
      })
    }

    this.emit('metadata', metadata)
  }

  _errorCbk(error) {
    const errorObj = new Error(flac.Decoder.ErrorStatusString[error])
    errorObj.code = error
    this.emit('error', errorObj)
  }

  _emitDecoderError() {
    const error = this._dec.getState()
    const errorObj = new Error(this._dec.getResolvedStateString())
    this._debug(`Decoder call failed: ${this._dec.getResolvedStateString()} [${error}]`)
    errorObj.code = error
    this.emit('error', errorObj)
  }
}

Object.defineProperty(exports, '__esModule', { value: true })
exports.default = FileDecoder
