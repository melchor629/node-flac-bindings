const fs = require('fs')
const wav = require('wav')
const { FileEncoder } = require('flac-bindings')

const wavReader = new wav.Reader()

fs.createReadStream('in.wav')
  .pipe(wavReader)
  .on('format', (format) => {
    const encoder = new FileEncoder({
      file: 'out.flac',
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