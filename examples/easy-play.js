const Speaker = require('speaker')
const { FileDecoder } = require('flac-bindings')

// this example is so basic that it supposes the flac is 44100 Hz, 16 bit stereo
const speaker = new Speaker({
  channels: 2,
  bitDepth: 16,
  sampleRate: 44100,
})

// creates the decoder using a file
const decoder = new FileDecoder({
 file: 'some.flac',
})

// sends the decoded audio to speaker
decoder.pipe(speaker)

decoder.on('end', () => {
  const { totalSeconds } = decoder.getProgress()
  process.stdout.write(`\r[${(100).toFixed(1)}%] ${totalSeconds.toFixed(1)}s / ${totalSeconds.toFixed(1)}s\n`)
})

setInterval(() => {
  // shows a progress about the song
  // the .getProgress() gets some stats from the current decoding progress, useful for things like this
  const { percentage, totalSeconds, currentSeconds } = decoder.getProgress()
  process.stdout.write(`\r[${(percentage * 100).toFixed(1)}%] ${currentSeconds.toFixed(1)}s / ${totalSeconds.toFixed(1)}s`)
}, 500).unref()

// ** NOTE: on macOS, node crashes when the track ends. It is a known issue in `node-speaker`
