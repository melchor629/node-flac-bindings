// requires sox (macOS/Windows) or arecord (Linux)
const mic = require('mic')
const { FileEncoder } = require('flac-bindings')

const micro = mic({
  rate: '44100',
  channels: 1,
})

micro
  .getAudioStream()
  .pipe(new FileEncoder({
    file: 'out.flac',
    bitsPerSample: 16,
    channels: 1,
    samplerate: 44100,
  }))
  .on('error', (error) => {
    console.error(error)
  })

micro.start()

process.on('SIGINT', () => micro.stop())
process.on('SIGTERM', () => micro.stop())