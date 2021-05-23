const fs = require('fs')
const wav = require('wav')
const { FileEncoder } = require('flac-bindings')
const args = require('./_args')(__filename)

// first argument is the wav file to encode
// second argument is encoded flac output

const wavReader = new wav.Reader()

fs.createReadStream(args[0] || 'in.wav')
  .pipe(wavReader)
  .on('format', (format) => {
    const encoder = new FileEncoder({
      file: args[1] || 'out.flac',
      bitsPerSample: format.bitsPerSample,
      channels: format.channels,
      samplerate: format.sampleRate,
    })

    wavReader
      .pipe(encoder)
      .on('error', (error) => {
        console.error(error)
      })
  })
