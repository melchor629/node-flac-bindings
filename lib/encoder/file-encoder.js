import debug from 'debug'
import { Writable } from 'stream'
import BaseEncoder from './helper.js'

class FileEncoder extends Writable {
  constructor(options = {}) {
    super({ ...options, decodeStrings: true })
    this._debug = debug('flac:encoder:file')
    if (!options.file) {
      throw new Error('No file passed as argument')
    }

    this._baseEncoder = new BaseEncoder(
      options,
      (builder) => builder.buildWithFileAsync(options.file, null),
      (builder) => builder.buildWithOggFileAsync(options.file, null),
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

  _write(chunk, _, callback) {
    return this._baseEncoder.processChunk(chunk, callback)
  }

  _final(callback) {
    return this._baseEncoder.finishEncoder(callback)
  }

  _format(format) {
    this._baseEncoder.setOptionsToEncoder(format)
  }
}

export default FileEncoder
