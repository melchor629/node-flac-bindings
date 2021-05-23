const fs = require('fs')
const wav = require('wav')
const { FileDecoder } = require('flac-bindings')
const args = require('./_args')(__filename)

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
    // they are usually the same, but if the bps is 24 and outputAs32 is not specified, the output
    // will be 32.
    bitDepth: decoder.getOutputBitsPerSample(),
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

decoder.on('end', () => {
  const { totalSeconds } = decoder.getProgress()
  process.stdout.write(`\r[${(100).toFixed(1)}%] ${totalSeconds.toFixed(1)}s / ${totalSeconds.toFixed(1)}s\n`)
})

setInterval(() => {
  const { percentage, totalSeconds, currentSeconds } = decoder.getProgress()
  process.stdout.write(`\r[${(percentage * 100).toFixed(1)}%] ${currentSeconds.toFixed(1)}s / ${totalSeconds.toFixed(1)}s`)
}, 500).unref()
