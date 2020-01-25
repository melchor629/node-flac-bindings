/**
 * Joins into interleaved
 * @param {Buffer[][]} arrayOfArrayOfBuffers Array of array of buffers
 * @param {number} totalSamples Number of total samples expected
 * @returns {[Buffer, number]}
 */
const joinIntoInterleaved = (arrayOfArrayOfBuffers, totalSamples, channels = 2, bitsPerSample = 32) => {
    const bytesPerSample = bitsPerSample / 8;
    const bytesPerFrame = bytesPerSample * channels;
    const finalBuffer = Buffer.allocUnsafe(totalSamples * bytesPerFrame);
    let sample = 0;
    for(const buffers of arrayOfArrayOfBuffers) {
        for(let i = 0; i < buffers[0].length / bytesPerSample; i++) {
            for(let ch = 0; ch < channels; ch++) {
                finalBuffer.writeInt32LE(
                    buffers[ch].readInt32LE(i * bytesPerSample),
                    sample * bytesPerFrame + bytesPerSample * ch,
                );
            }
            sample++;
        }
    }

    return [ finalBuffer, sample ];
};

module.exports = joinIntoInterleaved;
