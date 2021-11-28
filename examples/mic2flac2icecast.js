import http from 'http'
// requires sox (macOS/Windows) or arecord (Linux)
import mic from 'mic'
import { StreamEncoder } from 'flac-bindings'

// here there are a lot of stuff to configure, change the script with the right value :)

const micro = mic({
  rate: '44100',
  channels: 1,
})

// this tells the icecast server that a new streaming is going to happen
const res = http.request('http://localhost:8000/stream', {
  auth: 'USER:PASSWORD',
  headers: {
    'User-Agent': `node.js ${process.version}`,
    Accept: '*/*',
    Expect: '100-continue',
    Connection: 'close',
    Host: 'http://localhost:8000',
    'Content-Type': 'audio/flac',
    'Icy-MetaData': '0',
    'Icy-Name': 'Sadly unknown radio stream',
    'Icy-Description': 'Very nice radio',
    'Icy-Pub': '0',
    'Icy-Br': '',
    'Ice-Public': '0',
    'Transfer-Encoding': 'identity',
  },
  method: 'PUT',
})

micro
  .getAudioStream()
  .pipe(new StreamEncoder({
    bitsPerSample: 16,
    channels: 1,
    samplerate: 44100,
    isOggStream: true,
  }))
  .pipe(res)
  .on('error', (error) => {
    console.error(error)
  })

// icecast sends a response with a 100 CONTINUE to indicate that we can stream
res.on('information', () => micro.start())
// ways to stop the streaming: Ctrl+C or failure
res.on('error', () => micro.stop())
res.on('close', () => micro.stop())
process.on('SIGINT', () => micro.stop())
process.on('SIGTERM', () => micro.stop())

// ** NOTE: This example is an over-simplification about how to stream to an icecast, but it works
