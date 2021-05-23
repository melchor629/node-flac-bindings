const debug = require('debug')
const { Transform } = require('stream')
const { Encoder } = require('../api')
const BaseEncoder = require('./helper')

class StreamEncoder extends Transform {
  constructor(options = {}) {
    super({
      ...options,
      decodeStrings: false,
      defaultEncoding: undefined,
      encoding: undefined,
    })
    this._debug = debug('flac:encoder:stream')
    this._baseEncoder = new BaseEncoder(
      options,
      (enc) => enc.initStreamAsync(this._writeFlac.bind(this), null, null, null),
      (enc) => enc.initOggStreamAsync(null, this._writeFlac.bind(this), null, null, null),
      this._debug,
    )
  }

  get processedSamples() {
    return this._baseEncoder.processedSamples
  }

  getState() {
    return this._baseEncoder.encoder.getState()
  }

  _transform(chunk, _, callback) {
    return this._baseEncoder.processChunk(chunk, callback)
  }

  _flush(callback) {
    return this._baseEncoder.finishEncoder(callback)
  }

  _writeFlac(buffer) {
    const buffer2 = Buffer.from(buffer)
    this.push(buffer2)
    this._debug(`Received ${buffer2.length} bytes of encoded flac data`)
    return Encoder.WriteStatus.OK
  }
}

Object.defineProperty(exports, '__esModule', { value: true })
exports.default = StreamEncoder
