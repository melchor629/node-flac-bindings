import cp from 'child_process'

/**
 * Decodes a flac file using `flac` cli tool
 * @param {string} file Path to a flac file
 * @param {boolean} ogg Set to `true` if the file is ogg/flac
 * @returns {Buffer} The `stdout` buffer
 */
const readFlacUsingCli = (file, ogg) => {
  const result = cp.spawnSync('flac', ['-d', '-c', file, ogg ? '--ogg' : '--no-ogg'], { encoding: 'buffer' })
  if (result.error) {
    throw result.error
  }

  if (result.status) {
    throw new Error(result.stderr.toString('utf8'))
  }
  return result.stdout
}

/**
 * From given WAV buffer, extracts the PCM data
 * @param {Buffer} buffer WAV buffer
 * @returns {Buffer} The PCM data of the WAV file
 */
export const getPCMData = (buffer) => {
  const pos = buffer.indexOf('data', buffer.indexOf('WAVE')) + 4
  const length = buffer.readUInt32LE(pos)
  return buffer.slice(pos + 4, length !== 0 ? pos + 4 + length : undefined)
}

/**
 * Compare raw PCM data between a flac file and expected audio.
 * @param {Buffer} okData Expected data
 * @param {string | Buffer} flacFile Path to the flac file or raw PCM data
 * @param {number} bitsPerSample Bits per sample of the audio
 * @param {boolean} ogg Set to `true` if the file is ogg/flac
 */
export const comparePCM = (okData, flacFile, bitsPerSample = 16, ogg = false) => {
  const convertedData = Buffer.isBuffer(flacFile)
    ? flacFile
    : getPCMData(readFlacUsingCli(flacFile, ogg))

  const sampleSize = bitsPerSample / 8
  const wavSampleSize = 3
  if (convertedData.length / sampleSize !== okData.length / wavSampleSize) {
    throw new Error(
      `Length is different: ${convertedData.length / sampleSize} vs ${okData.length / wavSampleSize}`,
    )
  }

  for (let i = 0; i < okData.length / wavSampleSize; i += 1) {
    const a = convertedData.readIntLE(i * sampleSize, sampleSize)
    const b = okData.readIntLE(i * wavSampleSize, wavSampleSize)
    if (a !== b) {
      throw new Error(`PCM data is different at sample ${i}: ${a} !== ${b}`)
    }
  }
}
