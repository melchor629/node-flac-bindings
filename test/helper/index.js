module.exports = {
    asyncChaiExtensions: require('./async-chai-extensions'),
    createDeferredScope: require('./deferred'),
    gc: require('./gc'),
    ...require('./compare-pcm'),
    generateFlacCallbacks: require('./generate-flac-callbacks'),
    getWavAudio: require('./get-wav-audio'),
    joinIntoInterleaved: require('./join-into-interleaved'),
    pathForFile: require('./path-for-file'),
};
