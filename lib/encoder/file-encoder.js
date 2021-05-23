const debug = require('debug')
const { Writable } = require('stream')
const BaseEncoder = require('./helper')

class FileEncoder extends Writable {
  constructor(options = {}) {
    super({ ...options, decodeStrings: true })
    this._debug = debug('flac:encoder:file')
    if (!options.file) {
      throw new Error('No file passed as argument')
    }

    this._baseEncoder = new BaseEncoder(
      options,
      (enc) => enc.initFileAsync(options.file, null),
      (enc) => enc.initOggFileAsync(options.file, null),
      this._debug,
    )
  }

  get processedSamples() {
    return this._baseEncoder.processedSamples
  }

  getState() {
    return this._baseEncoder.encoder.getState()
  }

  _write(chunk, _, callback) {
    return this._baseEncoder.processChunk(chunk, callback)
  }

  _final(callback) {
    return this._baseEncoder.finishEncoder(callback)
  }
}

Object.defineProperty(exports, '__esModule', { value: true })
exports.default = FileEncoder
