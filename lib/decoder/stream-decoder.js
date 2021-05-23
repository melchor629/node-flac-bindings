const debug = require('debug')
const stream = require('stream')
const flac = require('../api')

class StreamDecoder extends stream.Transform {
  constructor(options = {}) {
    super({
      ...options,
      decodeStrings: false,
      defaultEncoding: undefined,
      encoding: undefined,
    })
    this._debug = debug('flac:decoder:stream')
    this._dec = new flac.Decoder()
    this._decoderInit = false
    this._oggStream = options.isOggStream || false
    this._metadataRespond = options.metadata
    this._outputAs32 = options.outputAs32
    this._chunks = []
    this._processedSamples = 0

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

  async _transform(chunk, _, callback) {
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
            await this._dec.initOggStreamAsync(
              this._readCbk.bind(this),
              null,
              null,
              null,
              null,
              this._writeCbk.bind(this),
              this._metadataCbk.bind(this),
              this._errorCbk.bind(this),
            )
          } else {
            this._debug('Initializing for FLAC')
            await this._dec.initStreamAsync(
              this._readCbk.bind(this),
              null,
              null,
              null,
              null,
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

      this._debug(`Received ${chunk.length} bytes to process`)
      this._chunks.push(chunk)
      this._streamIsFull = false
      while (!this._streamIsFull && !this._chunksIsAlmostEmpty() && this._dec !== null) {
        this._streamIsFull = false
        // a processSingle needs data, and have received enough, so it is time to unblock
        if (this._fillUpPause) {
          this._debug('Another processSingle call is blocked and there is enough data -> unblocking')
          this._fillUpPause()
          callback()
          return
        }
        // if the processSingle blocked did not finished yet, just don't do anything
        if (this._readCallback) {
          this._debug('Another processSingle call did not finished yet, doing nothing with this')
          callback()
          return
        }

        // flag to know if the next processSingle call has been blocked
        this._hasBeenBlocked = false
        this._lastBytesDemanded = 0
        // store the current callback just in case it gets blocked
        this._readCallback = callback
        this._debug('Processing data')
        if (!(await this._dec.processSingleAsync())) {
          return this._emitDecoderError()
        }
        this._readCallback = null

        // if the call has been blocked, then it does not need to do anything else
        if (this._hasBeenBlocked) {
          this._debug('processSingle was blocked, stopping loop for this processing')
          return
        }

        // avoid values of 0 bytes, by using a default value
        this._lastBytesDemanded = this._lastBytesDemanded || this._dec.getBlocksize() || 1024 * 32
      }
      callback()
    } catch (e) {
      callback(e)
    }
  }

  async _flush(callback) {
    try {
      if (!this._decoderInit) {
        this._debug('Decoder did not receive any data and it is being finalized')
        callback(null)
        return
      }

      this._timeToDie = true
      if (this._chunks.length > 0) {
        this._debug('Processing final chunks of data')
        if (!(await this._dec.processUntilEndOfStreamAsync())) {
          this._emitDecoderError()
          return
        }
      }

      this._debug('Flushing decoder')
      if (!(await this._dec.finishAsync())) {
        this._emitDecoderError()
      }

      callback(null)
    } catch (e) {
      callback(e)
    }
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

  _readCbk(buffer) {
    if (this._chunks.length > 0) {
      const b = this._chunks[0]
      const bytesRead = b.copy(buffer, 0)
      if (bytesRead === b.length) {
        this._chunks = this._chunks.slice(1)
      } else {
        this._chunks[0] = b.slice(bytesRead)
      }
      this._lastBytesDemanded += buffer.length
      this._debug(`Read ${bytesRead} bytes from stored chunks to be decoded`)
      return { bytes: bytesRead, returnValue: flac.Decoder.ReadStatus.CONTINUE }
    }

    if (this._timeToDie) {
      this._debug('Wanted to read, but the stream is being finished -> returning EOF')
      return { bytes: 0, returnValue: flac.Decoder.ReadStatus.END_OF_STREAM }
    }

    this._debug(`There is no chunks of data to be read (wanted ${buffer.length}) -> blocking processSingle`)
    // there is not enough data: block the processSingle
    //   1. store the required bytes
    this._lastBytesDemanded = buffer.length
    //   2. copy the callback
    const cbk = this._readCallback
    //   3. notify that processSingle has been blocked
    this._hasBeenBlocked = true
    //   4. block
    return new Promise((resolve) => {
      this._fillUpPause = () => {
        resolve({ bytes: 0, returnValue: flac.Decoder.ReadStatus.CONTINUE })
        this._fillUpPause = undefined
      }

      if (cbk) {
        // call the readCallback here
        cbk()
      }
    })
  }

  _metadataCbk(metadata) {
    if (metadata.type === flac.format.MetadataType.STREAMINFO) {
      this._outputAs32 = (this._outputAs32 != null)
        ? !!this._outputAs32
        : (this._dec.getBitsPerSample() === 24)
      this.emit('format')
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
    throw errorObj
  }

  _chunksIsAlmostEmpty() {
    if (this._lastBytesDemanded !== undefined) {
      return this._chunks.reduce((r, v) => r + v.length, 0) < this._lastBytesDemanded * 2
    }
    return this._chunks.length < 2
  }
}

Object.defineProperty(exports, '__esModule', { value: true })
exports.default = StreamDecoder
