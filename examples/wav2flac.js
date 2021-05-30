const fs = require('fs')
const wav = require('wav')
const { FileEncoder } = require('flac-bindings')
const args = require('./_args')(__filename)

// first argument is the wav file to encode
// second argument is encoded flac output

const wavReader = new wav.Reader()

fs.createReadStream(args[0] || 'in.wav')
  .pipe(wavReader)
  .pipe(new FileEncoder({
    file: args[1] || 'out.flac',
    compressionLevel: 9,
    // "why don't you set the sample rate and related values?" you may ask...
    // wav reader emits "format" which is captured by the encoder and it configures
    // itself with the values from there. This also works if the source stream of
    // the pipe contains the properties like channels or so (it can read them from
    // there as well, not only from "format" event).
  }))
  .on('error', (error) => {
    console.error(error)
  })
