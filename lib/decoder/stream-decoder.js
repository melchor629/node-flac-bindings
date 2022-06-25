/* eslint-disable no-await-in-loop */
import debug from 'debug'
import stream from 'stream'
import * as flac from '../api.js'

class StreamDecoder extends stream.Transform {
  constructor(options = {}) {
    super({
      ...options,
      decodeStrings: false,
      defaultEncoding: undefined,
      encoding: undefined,
    })
    this._debug = debug('flac:decoder:stream')
    this._builder = new flac.DecoderBuilder()
    this._dec = undefined
    this._oggStream = options.isOggStream || false
    this._outputAs32 = options.outputAs32 || false
    this._chunks = []
    this._processedSamples = 0

    this._readLoopPromise = null
    this._dataAvailableCallback = null
    this._chunk = null

    if (this._oggStream && !flac.format.API_SUPPORTS_OGG_FLAC) {
      throw new Error('Ogg FLAC is unsupported')
    }

    if (options.metadata === true) {
      this._debug('Setting decoder to emit all metadata blocks')
      this._builder.setMetadataRespondAll()
    } else if (Array.isArray(options.metadata)) {
      this._debug(`Setting decoder to emit '${options.metadata.join(', ')}' metadata blocks`)
      for (const type of options.metadata) {
        this._builder.setMetadataRespond(type)
      }
    }
  }

  get processedSamples() {
    return this._processedSamples
  }

  getTotalSamples() {
    if (this._totalSamples === undefined) {
      this._totalSamples = this._dec.getTotalSamples()
    }
    return this._totalSamples
  }

  getChannels() {
    if (this._channels === undefined) {
      this._channels = this._dec.getChannels()
    }
    return this._channels
  }

  getChannelAssignment() {
    if (this._channelAssignment === undefined) {
      this._channelAssignment = this._dec.getChannelAssignment()
    }
    return this._channelAssignment
  }

  getBitsPerSample() {
    if (this._bitsPerSample === undefined) {
      this._bitsPerSample = this._dec.getBitsPerSample()
    }
    return this._bitsPerSample
  }

  getOutputBitsPerSample() {
    if (this._outputAs32) {
      return 32
    }

    return this.getBitsPerSample()
  }

  getSampleRate() {
    if (this._sampleRate === undefined) {
      this._sampleRate = this._dec.getSampleRate()
    }
    return this._sampleRate
  }

  getProgress() {
    if (this._dec || (this._totalSamples && this._sampleRate)) {
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
      if (this._dec === undefined) {
        this._debug('Initializing decoder')
        try {
          if (this._oggStream) {
            this._debug('Initializing for Ogg/FLAC')
            this._dec = await this._builder.buildWithOggStreamAsync(
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
            this._dec = await this._builder.buildWithStreamAsync(
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
      }

      this._debug(`Received ${chunk.length} bytes to process`)
      this._chunk = {
        data: this._chunk ? Buffer.concat([this._chunk.data, chunk]) : chunk,
        callback: () => {
          if (this._chunk.data.length) {
            this._debug('Called _transform callback still having data to read')
            this._chunk.callback = () => {}
          } else {
            this._debug('Called _transform callback with no data to read')
            this._chunk = null
          }
          callback()
        },
      }
      this._dataAvailableCallback?.()
      if (!this._readLoopPromise) {
        this._readLoopPromise = this._readLoop()
      }
    } catch (e) {
      callback(e)
    }
  }

  async _flush(callback) {
    try {
      if (!this._dec) {
        this._debug('Decoder did not receive any data and it is being finalized')
        callback(null)
        return
      }

      this._chunk = {
        data: null,
        callback: () => {
          this._debug('Called _flush callback')
          this._chunk = null
          callback()
        },
      }
      this._dataAvailableCallback?.()
      await this._readLoopPromise

      if (this._chunk) {
        this._debug('_flush still has the EOF to read, this is a bug')
        callback(new Error())
      }
    } catch (e) {
      callback(e)
    }
  }

  async _readLoop() {
    try {
      this._debug('Starting _readLoop')
      while (!this.destroyed && this._dec != null) {
        const couldProcess = await this._dec.processSingleAsync()
        const decState = this._dec.getState()
        if (decState === flac.Decoder.State.END_OF_STREAM) {
          this._debug('Decoder reached EOF -> flushing decoder and finishing stream')
          await this._dec.finishAsync()
          this.push(null)
          this._dec = null
        } else if (!couldProcess && !this.destroyed) {
          this._throwDecoderError()
          return
        }
      }

      this._chunk?.callback()
    } catch (e) {
      this.destroy(e)
    } finally {
      this._debug('Ending _readLoop')
      this._readLoopPromise = null
    }
  }

  _writeCbk(frame, buffers) {
    const outBps = this.getOutputBitsPerSample() / 8
    const buff = flac.fns.zipAudio({ samples: frame.header.blocksize, outBps, buffers })

    this._processedSamples += frame.header.blocksize
    this.push(buff)
    this._debug(`Received ${frame.header.blocksize} samples (${buff.length} bytes) of decoded data`)

    return flac.Decoder.WriteStatus.CONTINUE
  }

  async _readCbk(buffer) {
    if (!this._chunk) {
      await new Promise((resolve) => {
        this._dataAvailableCallback = resolve
      })
    }

    if (!this._chunk.data) {
      this._debug('Called read callback without data -> EOF')
      // do not call the callback here, it will be called after finishAsync()
      return { bytes: 0, returnValue: flac.Decoder.ReadStatus.END_OF_STREAM }
    }

    const bytesRead = Math.min(this._chunk.data.length, buffer.length)
    this._chunk.data.copy(buffer, 0, 0, buffer.length)
    this._chunk.data = this._chunk.data.subarray(buffer.length)
    if (this._chunk.data.length === 0) {
      this._debug(`Called read callback with not enough data, ask for more (asked ${buffer.length}B)`)
      this._chunk.callback()
    } else {
      this._debug(`Called read callback with enough data (asked ${buffer.length}B)`)
    }
    return {
      bytes: bytesRead,
      returnValue: flac.Decoder.ReadStatus.CONTINUE,
    }
  }

  _metadataCbk(metadata) {
    if (metadata.type === flac.format.MetadataType.STREAMINFO) {
      this.emit('format', {
        channels: metadata.channels,
        bitDepth: metadata.bitsPerSample,
        bitsPerSample: metadata.bitsPerSample,
        is32bit: this._outputAs32,
        sampleRate: metadata.sampleRate,
        totalSamples: metadata.totalSamples,
      })
      this._totalSamples = metadata.totalSamples
      this._channels = metadata.channels
      this._bitsPerSample = metadata.bitsPerSample
      this._sampleRate = metadata.sampleRate
    }

    this.emit('metadata', metadata)
  }

  _errorCbk(error) {
    const errorObj = new Error(flac.Decoder.ErrorStatusString[error])
    errorObj.code = error
    this._debug(`Decoder called error callback ${error}`)
    throw errorObj
  }

  _throwDecoderError() {
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

export default StreamDecoder
