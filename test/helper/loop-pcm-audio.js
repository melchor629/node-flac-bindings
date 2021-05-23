const getWavAudio = require('./get-wav-audio')

const totalSamples = 992250 / 3 / 2

// read input data and convert into int32[] arrays...
const okData = getWavAudio('loop.wav')
// ...interleaved
const encData = Buffer.allocUnsafe(totalSamples * 4 * 2)
// ...non-interleaved
const encDataAlt = [
  Buffer.allocUnsafe(totalSamples * 4),
  Buffer.allocUnsafe(totalSamples * 4),
]
for (let i = 0; i < totalSamples * 2; i += 1) {
  encData.writeInt32LE(okData.readIntLE(i * 3, 3), i * 4)
}
for (let i = 0; i < totalSamples; i += 1) {
  encDataAlt[0].writeInt32LE(okData.readIntLE(i * 3 * 2, 3), i * 4)
  encDataAlt[1].writeInt32LE(okData.readIntLE(i * 3 * 2 + 3, 3), i * 4)
}

module.exports = {
  totalSamples,
  okData,
  encData,
  encDataAlt,
}
