const { Encoder, format, fns } = require('../api')

class BaseEncoder {
  /**
   * Stores all options in the object
   * @param {import('./helper').EncoderOptions} options Object of options
   * @param {(enc: Encoder) => Promise<void>} initEncoder
   * @param {(enc: Encoder) => Promise<void>} initOggEncoder
   * @param {import('debug').Debugger} debug
   */
  constructor(options, initEncoder, initOggEncoder, debug) {
    this._encoderInit = false
    this._oggStream = !!options.isOggStream
    this._channels = parseInt(options.channels, 10) || 2
    this._bitsPerSample = parseInt(options.bitsPerSample, 10) || 16
    this._inputAs32 = (options.inputAs32 != null)
      ? !!options.inputAs32
      : (this._bitsPerSample === 24)
    this._samplerate = parseInt(options.samplerate, 10) || 44100
    this._oggSerialNumber = parseInt(options.oggSerialNumber, 10) || null
    this._totalSamplesEstimate = parseInt(options.totalSamplesEstimate, 10) || null
    this._compressionLevel = parseInt(options.compressionLevel || '5', 10) || 5 // NOTE Workaround for #6
    this._doMidSideStereo = options.channels === 2 ? !!options.doMidSideStereo : undefined
    this._looseMidSideStereo = options.channels === 2 ? !!options.looseMidSideStereo : undefined
    this._apodization = typeof options.apodization === 'string' ? options.apodization : undefined
    this._maxLpcOrder = parseInt(options.maxLpcOrder, 10) || null
    this._qlpCoeffPrecision = parseInt(options.qlpCoeffPrecision, 10) || null
    this._doQlpCoeffPrecSearch = options.doQlpCoeffPrecSearch
    this._doExhaustiveModelSearch = options.doExhaustiveModelSearch
    this._minResidualPartitionOrder = parseInt(options.minResidualPartitionOrder, 10) || null
    this._maxResidualPartitionOrder = parseInt(options.maxResidualPartitionOrder, 10) || null
    this._blocksize = parseInt(options.blocksize, 10) || null
    this._metadata = options.metadata
    this._processedSamples = 0
    this._storedChunks = []
    this._enc = new Encoder()

    this._initEncoder = initEncoder
    this._initOggEncoder = initOggEncoder
    this._debug = debug

    if (this._oggStream && !format.API_SUPPORTS_OGG_FLAC) {
      throw new Error('Ogg FLAC is unsupported')
    }
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
   */
  setOptionsToEncoder() {
    this._enc.channels = this._channels
    this._enc.bitsPerSample = this._bitsPerSample
    this._enc.sampleRate = this._samplerate
    if (this._oggStream && this._oggSerialNumber) {
      this._enc.setOggSerialNumber(this._oggSerialNumber)
    }
    if (typeof this._totalSamplesEstimate === 'bigint' || typeof this._totalSamplesEstimate === 'number') {
      this._enc.totalSamplesEstimate = this._totalSamplesEstimate
    }
    if (typeof this._compressionLevel === 'number') {
      this._enc.setCompressionLevel(this._compressionLevel)
    }
    if (this._doMidSideStereo !== undefined) {
      this._enc.doMidSideStereo = this._doMidSideStereo
    }
    if (this._looseMidSideStereo !== undefined) {
      this._enc.looseMidSideStereo = this._looseMidSideStereo
    }
    if (this._apodization) {
      this._enc.setApodization(this._apodization)
    }
    if (typeof this._maxLpcOrder === 'number') {
      this._enc.maxLpcOrder = this._maxLpcOrder
    }
    if (typeof this._qlpCoeffPrecision === 'number') {
      this._enc.qlpCoeffPrecision = this._qlpCoeffPrecision
    }
    if (this._doQlpCoeffPrecSearch !== undefined) {
      this._enc.doQlpCoeffPrecSearch = this._doQlpCoeffPrecSearch
    }
    if (this._doExhaustiveModelSearch !== undefined) {
      this._enc.doExhaustiveModelSearch = this._doExhaustiveModelSearch
    }
    if (typeof this._minResidualPartitionOrder === 'number') {
      this._enc.minResidualPartitionOrder = this._minResidualPartitionOrder
    }
    if (typeof this._maxResidualPartitionOrder === 'number') {
      this._enc.maxResidualPartitionOrder = this._maxResidualPartitionOrder
    }
    if (typeof this._blocksize === 'number') {
      this._enc.blocksize = this._blocksize
    }
    if (Array.isArray(this._metadata)) {
      this._enc.setMetadata(this._metadata)
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
        this.setOptionsToEncoder()
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
      this._enc = null
    } catch (e) {
      callback(e)
    }
  }
}

module.exports = BaseEncoder
