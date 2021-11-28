import { fns } from '../../lib/api.js'

/**
 * Joins into interleaved
 * @param {Buffer[][]} arrayOfArrayOfBuffers Array of array of buffers
 * @param {number} bytesPerSample Number of bytes per sample (default 4)
 * @returns {[Buffer, number]}
 */
const joinIntoInterleaved = (arrayOfArrayOfBuffers, bytesPerSample = 4) => {
  const joinedBuffers = []
  let totalProcessedSamples = 0
  for (const buffers of arrayOfArrayOfBuffers) {
    const samples = buffers[0].length / bytesPerSample
    const buffer = fns.zipAudio({ buffers, samples })
    joinedBuffers.push(buffer)
    totalProcessedSamples += samples
  }

  return [
    Buffer.concat(joinedBuffers),
    totalProcessedSamples,
  ]
}

export default joinIntoInterleaved
