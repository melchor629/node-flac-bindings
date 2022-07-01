/* eslint-disable no-await-in-loop */
import debug from 'debug'
import { Readable } from 'stream'
import * as flac from '../api.js'

class FileDecoder extends Readable {
  constructor(options = {}) {
    super(options)
    this._debug = debug('flac:decoder:file')
    this._builder = new flac.DecoderBuilder()
    this._dec = undefined
    this._oggStream = options.isOggStream || false
    this._outputAs32 = options.outputAs32 || false
    this._file = options.file
    this._processedSamples = 0
    this._failed = false

    if (!this._file) {
      throw new Error('No file passed as argument')
    }

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

  _read() {
    if (!this._readLoopPromise) {
      this._readLoopPromise = this._readLoop().then(() => {
        if (this.readableLength === 0 && !this.destroyed) {
          this._read()
        }
      })
    }
  }

  async _readLoop() {
    try {
      if (this._dec === undefined) {
        this._debug('Initializing decoder')
        try {
          if (this._oggStream) {
            this._debug('Initializing for Ogg/FLAC')
            this._dec = await this._builder.buildWithOggFileAsync(
              this._file,
              this._writeCbk.bind(this),
              this._metadataCbk.bind(this),
              this._errorCbk.bind(this),
            )
          } else {
            this._debug('Initializing for FLAC')
            this._dec = await this._builder.buildWithFileAsync(
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
      }

      this._debug('Wants data from decoder')
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
    } catch (e) {
      this.destroy(e)
    }

    this._readLoopPromise = null
  }

  _writeCbk(frame, buffers) {
    const outBps = this.getOutputBitsPerSample() / 8
    const buff = flac.fns.zipAudio({ samples: frame.header.blocksize, outBps, buffers })

    this._processedSamples += frame.header.blocksize
    this.push(buff)
    this._debug(`Received ${frame.header.blocksize} samples (${buff.length} bytes) of decoded data`)

    return flac.Decoder.WriteStatus.CONTINUE
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

  _errorCbk(code) {
    const message = flac.Decoder.ErrorStatusString[code]
    this._debug(`Decoder called error callback ${code} (${message})`)
    this.emit('flac-error', { code, message })
  }

  _throwDecoderError() {
    const error = this._dec.getState()
    const errorObj = new Error(this._dec.getResolvedStateString())
    this._debug(`Decoder call failed: ${this._dec.getResolvedStateString()} [${error}]`)
    errorObj.code = error
    throw errorObj
  }
}

export default FileDecoder
