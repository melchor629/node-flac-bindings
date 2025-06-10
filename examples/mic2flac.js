// requires sox (macOS/Windows) or arecord (Linux)
import mic from 'mic'
import { FileEncoder } from 'flac-bindings'
import createArgs from './_args.js'

const args = createArgs(import.meta.url)

// first argument is the flac where the captured audio will be writen

const micro = mic({
  rate: '44100',
  channels: 1,
})

micro
  .getAudioStream()
  .pipe(new FileEncoder({
    file: args[0] || 'out.flac',
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
