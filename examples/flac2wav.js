import fs from 'fs'
import wav from 'wav'
import { FileDecoder } from 'flac-bindings'
import createArgs from './_args.js'

const args = createArgs(import.meta.url)

// first argument is the flac file to decode
// second argument is the wav where the decoded flac will be writen

const decoder = new FileDecoder({
  file: args[0] || 'some.flac',
})

// wait until the first data
decoder.once('data', (chunk) => {
  const encoder = new wav.Writer({
    // these methods give you some info about the flac (but they only work after the first
    // 'metadata' or 'data' event).
    channels: decoder.getChannels(),
    // there is a small difference between the flac bits per sample and the output bits per sample.
    // by default both will be the same, but if `outputAs32` is set to true, then the output will
    // always be 32 bit int but with the values from the range of the original bit depth.
    bitDepth: decoder.getBitsPerSample(),
    sampleRate: decoder.getSampleRate(),
  })

  encoder.write(chunk)

  decoder
    .pipe(encoder)
    .pipe(fs.createWriteStream(args[1] || 'out.wav'))
    .on('error', (error) => {
      console.error(error)
    })
})

decoder
  .on('end', () => {
    const { totalSeconds } = decoder.getProgress()
    process.stdout.write(`\r[${(100).toFixed(1)}%] ${totalSeconds.toFixed(1)}s / ${totalSeconds.toFixed(1)}s\n`)
  })
  // FLAC errors on the decoder side will be emitted using this event
  .on('flac-error', (error) => {
    console.error(error)
  })
  // FLAC metadata blocks can be read using this other event
  .on('metadata', (metadata) => {
    console.log(metadata)
  })

setInterval(() => {
  const { percentage, totalSeconds, currentSeconds } = decoder.getProgress()
  process.stdout.write(`\r[${(percentage * 100).toFixed(1)}%] ${currentSeconds.toFixed(1)}s / ${totalSeconds.toFixed(1)}s`)
}, 500).unref()
