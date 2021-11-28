import {
  EncoderBuilder, Encoder, format, fns,
} from '../api.js'

class BaseEncoder {
  /**
   * Stores all options in the object
   * @param {import('./interfaces').EncoderOptions} options Object of options
   * @param {(enc: EncoderBuilder) => Promise<Encoder>} initEncoder
   * @param {(enc: EncoderBuilder) => Promise<Encoder>} initOggEncoder
   * @param {import('debug').Debugger} debug
   */
  constructor(options, initEncoder, initOggEncoder, debug) {
    this._processedSamples = 0
    this._storedChunks = []

    // default values
    this._channels = 2
    this._bitsPerSample = 16
    this._builder = new EncoderBuilder()
      .setChannels(2)
      .setBitsPerSample(16)
      .setSampleRate(44100)

    this._initEncoder = initEncoder
    this._initOggEncoder = initOggEncoder
    this._debug = debug

    this.setOptionsToEncoder(options)
  }

  get processedSamples() {
    return this._processedSamples
  }

  getState() {
    return (this._enc || this._builder).getState()
  }

  /**
   * Converts the buffer to int32_t if necessary
   * @param {Buffer} inputChunk A chunk of data
   * @returns {Buffer} converted buffer
   */
  convertToInt32Buffer(inputChunk) {
    if (this._inputAs32) {
      return inputChunk
    }

    const inBps = this._bitsPerSample / 8
    let chunk = inputChunk
    if (this._storedChunks.length > 0) {
      chunk = Buffer.concat([...this._storedChunks, inputChunk])
      this._storedChunks = []
    }

    const samples = Math.trunc(chunk.length / inBps / this._channels)
    const buffer = fns.convertSampleFormat({
      inBps,
      buffer: chunk,
      samples: samples * this._channels,
    })

    if (samples !== chunk.length / inBps / this._channels) {
      const extraBytes = chunk.length - (samples * inBps * this._channels)
      const remainingBuffer = Buffer.allocUnsafe(extraBytes)
      chunk.copy(remainingBuffer, 0, samples * inBps * this._channels)
      this._storedChunks.push(remainingBuffer)
      this._debug(`There are extra bytes which will be stored for the next process (${remainingBuffer.length})`)
    }

    return buffer
  }

  /**
   * Sets the options to the encoder
   * @param {import('./interfaces').EncoderOptions} options Object of options
   */
  setOptionsToEncoder(options) {
    this._debug('setOptionsToEncoder()')

    this._inputAs32 = options.inputAs32 || options.is32bit || false

    if (options.isOggStream != null) {
      this._oggStream = !!options.isOggStream
      if (this._oggStream && !format.API_SUPPORTS_OGG_FLAC) {
        throw new Error('Ogg FLAC is unsupported')
      }
    }

    if (this._oggStream && options.oggSerialNumber) {
      this._builder.setOggSerialNumber(options.oggSerialNumber)
    }

    if (options.channels) {
      this._builder.setChannels(options.channels)
      this._channels = options.channels
    }

    const bps = options.bitsPerSample || options.bitDepth
    if (bps) {
      this._builder.setBitsPerSample(bps)
      this._bitsPerSample = this._inputAs32 ? 32 : bps
    }

    const sampleRate = options.sampleRate || options.samplerate
    if (sampleRate) {
      this._builder.setSampleRate(sampleRate)
    }

    if (options.totalSamplesEstimate != null) {
      this._builder.setTotalSamplesEstimate(options.totalSamplesEstimate)
    }

    if (options.compressionLevel != null) {
      this._builder.setCompressionLevel(options.compressionLevel)
    } else {
      if (options.doMidSideStereo !== null) {
        this._builder.setDoMidSideStereo(options.doMidSideStereo)
      }
      if (options.looseMidSideStereo !== null) {
        this._builder.setLooseMidSideStereo(options.looseMidSideStereo)
      }
      if (options.apodization) {
        this._builder.setApodization(options.apodization)
      }
      if (options.maxLpcOrder != null) {
        this._builder.setMaxLpcOrder(options.maxLpcOrder)
      }
      if (options.qlpCoeffPrecision != null) {
        this._builder.setQlpCoeffPrecision(options.qlpCoeffPrecision)
      }
      if (options.doQlpCoeffPrecSearch != null) {
        this._builder.setDoQlpCoeffPrecSearch(options.doQlpCoeffPrecSearch)
      }
      if (options.doExhaustiveModelSearch != null) {
        this._builder.setDoExhaustiveModelSearch(options.doExhaustiveModelSearch)
      }
      if (options.minResidualPartitionOrder != null) {
        this._builder.setMinResidualPartitionOrder(options.minResidualPartitionOrder)
      }
      if (options.maxResidualPartitionOrder != null) {
        this._builder.setMaxResidualPartitionOrder(options.maxResidualPartitionOrder)
      }
    }

    if (options.blocksize != null) {
      this._builder.setBlocksize(options.blocksize)
    }

    if (Array.isArray(options.metadata)) {
      this._builder.setMetadata(options.metadata)
    }
  }

  /**
   * Process a chunk of data through the encoder
   * @param {Buffer} chunk A chunk of data
   * @param {(err?: any) => void} callback Callback to notify end of process
   */
  async processChunk(chunk, callback) {
    try {
      if (!this._enc) {
        try {
          if (this._oggStream) {
            this._debug('Initializing encoder with Ogg')
            this._enc = await this._initOggEncoder(this._builder)
          } else {
            this._debug('Initializing encoder')
            this._enc = await this._initEncoder(this._builder)
          }
        } catch (error) {
          const initStatus = error.status || -1
          const initStatusString = error.statusString || 'Unknown'
          this._debug(`Failed initializing encoder: ${initStatus} ${initStatusString}`)
          throw error
        }
      }

      const chunkToProcess = this.convertToInt32Buffer(chunk)
      const samples = Math.trunc(chunkToProcess.length / this._channels / 4)
      this._processedSamples += samples
      this._debug(`Received ${samples} samples (${chunkToProcess.length} bytes) to be processed`)

      if (!(await this._enc.processInterleavedAsync(chunkToProcess, samples))) {
        const err = this._enc.getState()
        const errStr = Encoder.StateString[err]
        this._debug(`Process received samples failed: ${errStr} [${err}]`)
        throw new Error(errStr)
      } else {
        callback(null)
      }
    } catch (e) {
      callback(e)
    }
  }

  /**
   * Finalize the encoding process by flushing the encoder.
   * @param {(err?: any) => void} callback Callback to notify end of process
   */
  async finishEncoder(callback) {
    try {
      if (!this._enc) {
        this._debug('Encoder did not receive any data and it is being finalized')
        callback(null)
        return
      }

      if (this._storedChunks.length > 0) {
        const chunk = Buffer.concat(this._storedChunks)
        this._storedChunks = []
        this._debug('Processing final samples')
        await this.processChunk(chunk, (err) => {
          if (err) {
            throw err
          }
        })
      }

      this._debug('Flushing encoder')
      if (!(await this._enc.finishAsync())) {
        const err = this._enc.getState()
        const errStr = Encoder.StateString[err]
        this._debug(`Flush encoder failed: ${errStr} [${err}]`)
        throw new Error(errStr)
      }

      callback(null)
    } catch (e) {
      callback(e)
    }
  }
}

export default BaseEncoder
