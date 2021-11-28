import debug from 'debug'
import { Transform } from 'stream'
import { Encoder } from '../api.js'
import BaseEncoder from './helper.js'

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
      (builder) => builder.buildWithStreamAsync(this._writeFlac.bind(this), null, null, null),
      (builder) => (
        builder.buildWithOggStreamAsync(null, this._writeFlac.bind(this), null, null, null)
      ),
      this._debug,
    )

    this._format = this._format.bind(this)
    this.on('pipe', (src) => {
      this._baseEncoder.setOptionsToEncoder(src)
      src.once('format', this._format)
    })
    this.on('unpipe', (src) => {
      src.removeListener('format', this._format)
    })
  }

  get processedSamples() {
    return this._baseEncoder.processedSamples
  }

  getState() {
    return this._baseEncoder.getState()
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

  _format(format) {
    this._baseEncoder.setOptionsToEncoder(format)
  }
}

export default StreamEncoder
