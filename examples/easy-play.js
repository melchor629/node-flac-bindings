import Speaker from 'speaker'
import { FileDecoder } from 'flac-bindings'
import createArgs from './_args.js'

const args = createArgs(import.meta.url)

// this example is so basic that the configuration is read from the emited format event
// which may not work for you depending on the flac file...
const speaker = new Speaker()

// creates the decoder using a file
const decoder = new FileDecoder({
  file: args[0] || 'some.flac',
})

// sends the decoded audio to speaker
decoder.pipe(speaker)

decoder.on('end', () => {
  const { totalSeconds } = decoder.getProgress()
  process.stdout.write(`\r[${(100).toFixed(1)}%] ${totalSeconds.toFixed(1)}s / ${totalSeconds.toFixed(1)}s\n`)
})

setInterval(() => {
  // shows a progress about the song
  // the .getProgress() gets some stats from the current decoding progress,
  // useful for things like this
  const { percentage, totalSeconds, currentSeconds } = decoder.getProgress()
  process.stdout.write(`\r[${(percentage * 100).toFixed(1)}%] ${currentSeconds.toFixed(1)}s / ${totalSeconds.toFixed(1)}s`)
}, 500).unref()

// ** NOTE: on macOS, node crashes when the track ends. It is a known issue in `node-speaker`
