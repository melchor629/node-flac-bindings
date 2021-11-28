import { dirname, join } from 'path'
import { fileURLToPath } from 'url'

const dataPath = join(dirname(fileURLToPath(new URL(import.meta.url))), '..', 'data')

/**
 * Gets the the absolute path to the requested file or folder
 * @param {string} type Type of test data file
 * @param  {...string} file path splitted in args
 */
const pathForFile = (type, ...file) => join(dataPath, type, ...file)

/**
 * Gets the absolute path to the request path from the audio data folder
 * @param {...string} file path to the file
 */
pathForFile.audio = (...file) => pathForFile('audio', ...file)

/**
 * Gets the absolute path to the request path from the tags data folder
 * @param {...string} file path to the file
 */
pathForFile.tags = (...file) => pathForFile('tags', ...file)

export default pathForFile
