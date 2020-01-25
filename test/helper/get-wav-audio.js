const fs = require('fs');
const pathForFile = require('./path-for-file');
const { getPCMData } = require('./compare-pcm');

const cache = new Map();

module.exports = (...file) => {
    const key = file.join('/');
    if(cache.has(key)) {
        return cache.get(key);
    }

    const pcm = getPCMData(fs.readFileSync(pathForFile.audio(...file)));
    cache.set(key, pcm);
    return pcm;
};
