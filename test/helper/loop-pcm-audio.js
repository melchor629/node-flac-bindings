import getWavAudio from './get-wav-audio.js'
import { fns } from '../../lib/api.js'

const totalSamples = 992250 / 3 / 2

/** @type {Buffer | null} */
let okData = null
/** @type {Buffer | null} */
let encData = null
/** @type {Buffer[] | null} */
let encDataAlt = null

const loopPcmAudio = {
  totalSamples,
  // read input data which is stereo 24bit...
  get okData() {
    if (!okData) {
      okData = getWavAudio('loop.wav')
    }

    return okData
  },
  get encData() {
    if (!encData) {
      encData = fns.convertSampleFormat({
        buffer: this.okData,
        inBps: 3,
      })
    }

    return encData
  },
  get encDataAlt() {
    if (!encDataAlt) {
      encDataAlt = fns.unzipAudio({
        buffer: okData,
        inBps: 3,
        channels: 2,
      })
    }

    return encDataAlt
  },
}

export default loopPcmAudio
