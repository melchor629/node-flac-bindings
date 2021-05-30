const { Encoder, format, fns } = require('../api')

class BaseEncoder {
  /**
   * Stores all options in the object
   * @param {import('./interfaces').EncoderOptions} options Object of options
   * @param {(enc: Encoder) => Promise<void>} initEncoder
   * @param {(enc: Encoder) => Promise<void>} initOggEncoder
   * @param {import('debug').Debugger} debug
   */
  constructor(options, initEncoder, initOggEncoder, debug) {
    this._encoderInit = false
    this._processedSamples = 0
    this._storedChunks = []
    this._enc = new Encoder()

    // default values
    this._enc.channels = 2
    this._channels = 2
    this._enc.bitsPerSample = 16
    this._bitsPerSample = 16
    this._enc.sampleRate = 44100

    this._initEncoder = initEncoder
    this._initOggEncoder = initOggEncoder
    this._debug = debug

    this.setOptionsToEncoder(options)
  }

  get processedSamples() {
    return this._processedSamples
  }

  get encoder() {
    return this._enc
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

    const inBPS = this._bitsPerSample / 8
    let chunk = inputChunk
    if (this._storedChunks.length > 0) {
      chunk = Buffer.concat([...this._storedChunks, inputChunk])
      this._storedChunks = []
    }

    const samples = Math.trunc(chunk.length / inBPS)
    const buffer = fns.convertSampleFormat({ inBps: inBPS, buffer: chunk, samples })

    if (samples !== chunk.length / inBPS) {
      this._storedChunks.push(chunk.slice(samples * inBPS))
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
      this._enc.setOggSerialNumber(options.oggSerialNumber)
    }

    if (options.channels) {
      this._enc.channels = options.channels
      this._channels = options.channels
    }

    const bps = options.bitsPerSample || options.bitDepth
    if (bps) {
      this._enc.bitsPerSample = bps
      this._bitsPerSample = this._inputAs32 ? 32 : bps
    }

    const sampleRate = options.sampleRate || options.samplerate
    if (sampleRate) {
      this._enc.sampleRate = sampleRate
    }

    if (options.totalSamplesEstimate != null) {
      this._enc.totalSamplesEstimate = options.totalSamplesEstimate
    }

    if (options.compressionLevel != null) {
      this._enc.setCompressionLevel(options.compressionLevel)
    } else {
      if (options.doMidSideStereo !== null) {
        this._enc.doMidSideStereo = options.doMidSideStereo
    }
      if (options.looseMidSideStereo !== null) {
        this._enc.looseMidSideStereo = options.looseMidSideStereo
    }
      if (options.apodization) {
        this._enc.setApodization(options.apodization)
    }
      if (options.maxLpcOrder != null) {
        this._enc.maxLpcOrder = options.maxLpcOrder
    }
      if (options.qlpCoeffPrecision != null) {
        this._enc.qlpCoeffPrecision = options.qlpCoeffPrecision
    }
      if (options.doQlpCoeffPrecSearch != null) {
        this._enc.doQlpCoeffPrecSearch = options.doQlpCoeffPrecSearch
    }
      if (options.doExhaustiveModelSearch != null) {
        this._enc.doExhaustiveModelSearch = options.doExhaustiveModelSearch
    }
      if (options.minResidualPartitionOrder != null) {
        this._enc.minResidualPartitionOrder = options.minResidualPartitionOrder
    }
      if (options.maxResidualPartitionOrder != null) {
        this._enc.maxResidualPartitionOrder = options.maxResidualPartitionOrder
    }
    }

    if (options.blocksize != null) {
      this._enc.blocksize = options.blocksize
    }

    if (Array.isArray(options.metadata)) {
      this._enc.setMetadata(options.metadata)
    }
  }

  /**
   * Process a chunk of data through the encoder
   * @param {Buffer} chunk A chunk of data
   * @param {(err?: any) => void} callback Callback to notify end of process
   */
  async processChunk(chunk, callback) {
    try {
      if (!this._encoderInit) {
        try {
          if (this._oggStream) {
            this._debug('Initializing encoder with Ogg')
            await this._initOggEncoder(this._enc)
          } else {
            this._debug('Initializing encoder')
            await this._initEncoder(this._enc)
          }
        } catch (error) {
          const initStatus = error.status || -1
          const initStatusString = error.statusString || 'Unknown'
          this._debug(`Failed initializing encoder: ${initStatus} ${initStatusString}`)
          throw error
        }

        this._encoderInit = true
      }

      const chunkToProcess = this.convertToInt32Buffer(chunk)
      const samples = Math.trunc(chunkToProcess.length / this._channels / 4)
      const extraBytes = chunkToProcess.length - (samples * this._channels * 4)
      this._processedSamples += samples
      this._debug(`Received ${samples} samples (${chunkToProcess.length} bytes) to be processed`)
      if (extraBytes !== 0) {
        this._storedChunks.push(chunkToProcess.slice(samples * this._channels * 4))
        this._debug(`There are extra bytes which will be stored for the next process (${extraBytes})`)
      }

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
      if (!this._encoderInit) {
        this._debug('Encoder did not receive any data and it is being finalized')
        callback(null)
        return
      }

      if (this._storedChunks.length > 0) {
        const chunk = Buffer.concat(this._storedChunks)
        this._storedChunks = []
        const chunkToProcess = this.convertToInt32Buffer(chunk)
        const samples = Math.trunc(chunkToProcess.length / this._channels / 4)
        this._debug(`Processing final ${samples} samples (${chunkToProcess.length} bytes)`)

        if (!(await this._enc.processInterleavedAsync(chunkToProcess, samples))) {
          const err = this._enc.getState()
          const errStr = Encoder.StateString[err]
          this._debug(`Process final samples failed: ${errStr} [${err}]`)
          throw new Error(errStr)
        }
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

module.exports = BaseEncoder
