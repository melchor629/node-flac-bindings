const fs = require('fs')
const pathForFile = require('./path-for-file')
const { getPCMData } = require('./compare-pcm')

const cache = new Map()

/**
 * Reads the file that points the path and returns the PCM data. The function caches the reads so a
 * second call with the same path will just return the cached value.
 * @param {...string} file path parts to a file
 * @returns {Buffer} The PCM data
 */
const getWavAudio = (...file) => {
  const key = file.join('/')
  if (cache.has(key)) {
    return cache.get(key)
  }

  const pcm = getPCMData(fs.readFileSync(pathForFile.audio(...file)))
  cache.set(key, pcm)
  return pcm
}

module.exports = getWavAudio
