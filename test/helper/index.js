module.exports = {
    asyncChaiExtensions: require('./async-chai-extensions'),
    gc: require('./gc'),
    ...require('./compare-pcm'),
    getWavAudio: require('./get-wav-audio'),
    joinIntoInterleaved: require('./join-into-interleaved'),
    pathForFile: require('./path-for-file'),
};
