const createDeferredScope = require('./deferred')
const gc = require('./gc')
const { comparePCM, getPCMData } = require('./compare-pcm')
const generateFlacCallbacks = require('./generate-flac-callbacks')
const getWavAudio = require('./get-wav-audio')
const joinIntoInterleaved = require('./join-into-interleaved')
const loopPcmAudio = require('./loop-pcm-audio')
const pathForFile = require('./path-for-file')

module.exports = {
  createDeferredScope,
  gc,
  comparePCM,
  getPCMData,
  generateFlacCallbacks,
  getWavAudio,
  joinIntoInterleaved,
  loopPcmAudio,
  pathForFile,
}
