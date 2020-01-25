const { join } = require('path');

/**
 * Gets the the absolute path to the requested file or folder
 * @param {string} type Type of test data file
 * @param  {...string} file path splitted in args
 */
const pathForFile = (type, ...file) => join(__dirname, '..', 'data', type, ...file);
pathForFile.audio = (...file) => pathForFile('audio', ...file);
pathForFile.tags = (...file) => pathForFile('tags', ...file);

module.exports = pathForFile;
