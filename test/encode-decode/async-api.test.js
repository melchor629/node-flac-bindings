/* eslint-disable prefer-arrow-callback */
const { api } = require('../../lib/index');
const { assert } = require('chai');
const temp = require('temp').track();

const {
    pathForFile: { audio: pathForFile },
    createDeferredScope,
    comparePCM,
    getWavAudio,
    generateFlacCallbacks,
    joinIntoInterleaved,
} = require('../helper');

const totalSamples = 992250 / 3 / 2;

const okData = getWavAudio('loop.wav');
const encData = Buffer.allocUnsafe(totalSamples * 4 * 2);
const encDataAlt = [
    Buffer.allocUnsafe(totalSamples * 4),
    Buffer.allocUnsafe(totalSamples * 4),
];
for(let i = 0; i < totalSamples * 2; i++) {
    encData.writeInt32LE(okData.readIntLE(i * 3, 3), i * 4);
}
for(let i = 0; i < totalSamples; i++) {
    encDataAlt[0].writeInt32LE(okData.readIntLE(i * 3 * 2, 3), i * 4);
    encDataAlt[1].writeInt32LE(okData.readIntLE(i * 3 * 2 + 3, 3), i * 4);
}

let tmpFile;
let deferredScope = null;
beforeEach('createTemporaryFiles', function() {
    tmpFile = temp.openSync('flac-bindings.encode-decode.async-api');
    deferredScope = createDeferredScope();
});

afterEach('cleanUpTemporaryFiles', function() {
    temp.cleanupSync();
    return deferredScope.finalize();
});

describe('encode & decode: async api', function() {

    this.slow(250 * 1000);

    it('decode using stream (non-ogg)', async function() {
        const callbacks = generateFlacCallbacks.sync(api.Decoder, pathForFile('loop.flac'), 'r');
        deferredScope.defer(() => callbacks.close());
        const dec = new api.Decoder();
        const allBuffers = [];
        await dec.initStreamAsync(
            callbacks.read,
            callbacks.seek,
            callbacks.tell,
            callbacks.length,
            callbacks.eof,
            (_, buffers) => {
                allBuffers.push(buffers.map((b) => Buffer.from(b)));
                return 0;
            },
            null,
            // eslint-disable-next-line no-console
            (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
        );

        assert.isTrue(await dec.processUntilEndOfMetadataAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.processUntilEndOfStreamAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.flushAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.finishAsync(), dec.getResolvedStateString());

        const [ finalBuffer, samples ] = joinIntoInterleaved(allBuffers, totalSamples);
        assert.equal(samples, totalSamples);
        comparePCM(okData, finalBuffer, 32);
    });

    it('decode using stream (ogg)', async function() {
        const callbacks = generateFlacCallbacks.sync(api.Decoder, pathForFile('loop.oga'), 'r');
        deferredScope.defer(() => callbacks.close());
        const dec = new api.Decoder();
        const allBuffers = [];
        await dec.initOggStreamAsync(
            callbacks.read,
            callbacks.seek,
            callbacks.tell,
            callbacks.length,
            callbacks.eof,
            (_, buffers) => {
                allBuffers.push(buffers.map((b) => Buffer.from(b)));
                return 0;
            },
            null,
            // eslint-disable-next-line no-console
            (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
        );

        assert.isTrue(await dec.processUntilEndOfMetadataAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.processUntilEndOfStreamAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.flushAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.finishAsync(), dec.getResolvedStateString());

        const [ finalBuffer, samples ] = joinIntoInterleaved(allBuffers, totalSamples);
        assert.equal(samples, totalSamples);
        comparePCM(okData, finalBuffer, 32);
    });

    it('decode with async callbacks using stream (non-ogg)', async function() {
        const callbacks = await generateFlacCallbacks.async(api.Decoder, pathForFile('loop.flac'), 'r');
        deferredScope.defer(() => callbacks.close());
        const dec = new api.Decoder();
        const allBuffers = [];
        await dec.initStreamAsync(
            callbacks.read,
            callbacks.seek,
            callbacks.tell,
            callbacks.length,
            callbacks.eof,
            (_, buffers) => {
                allBuffers.push(buffers.map((b) => Buffer.from(b)));
                return 0;
            },
            null,
            // eslint-disable-next-line no-console
            (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
        );

        assert.isTrue(await dec.processUntilEndOfMetadataAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.processUntilEndOfStreamAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.flushAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.finishAsync(), dec.getResolvedStateString());

        const [ finalBuffer, samples ] = joinIntoInterleaved(allBuffers, totalSamples);
        assert.equal(samples, totalSamples);
        comparePCM(okData, finalBuffer, 32);
    });

    it('decode with async callbacks using stream (ogg)', async function() {
        const callbacks = await generateFlacCallbacks.async(api.Decoder, pathForFile('loop.oga'), 'r');
        deferredScope.defer(() => callbacks.close());
        const dec = new api.Decoder();
        const allBuffers = [];
        await dec.initOggStreamAsync(
            callbacks.read,
            callbacks.seek,
            callbacks.tell,
            callbacks.length,
            callbacks.eof,
            (_, buffers) => {
                allBuffers.push(buffers.map((b) => Buffer.from(b)));
                return 0;
            },
            null,
            // eslint-disable-next-line no-console
            (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
        );

        assert.isTrue(await dec.processUntilEndOfMetadataAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.processUntilEndOfStreamAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.flushAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.finishAsync(), dec.getResolvedStateString());

        const [ finalBuffer, samples ] = joinIntoInterleaved(allBuffers, totalSamples);
        assert.equal(samples, totalSamples);
        comparePCM(okData, finalBuffer, 32);
    });

    it('decode using file (non-ogg)', async function() {
        const dec = new api.Decoder();
        const allBuffers = [];
        await dec.initFileAsync(
            pathForFile('loop.flac'),
            (_, buffers) => {
                allBuffers.push(buffers.map((b) => Buffer.from(b)));
                return 0;
            },
            null,
            // eslint-disable-next-line no-console
            (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
        );

        assert.isTrue(await dec.processUntilEndOfMetadataAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.processUntilEndOfStreamAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.flushAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.finishAsync(), dec.getResolvedStateString());

        const [ finalBuffer, samples ] = joinIntoInterleaved(allBuffers, totalSamples);
        assert.equal(samples, totalSamples);
        comparePCM(okData, finalBuffer, 32);
    });

    it('decode using file (ogg)', async function() {
        const dec = new api.Decoder();
        const allBuffers = [];
        await dec.initOggFileAsync(
            pathForFile('loop.oga'),
            (_, buffers) => {
                allBuffers.push(buffers.map((b) => Buffer.from(b)));
                return 0;
            },
            null,
            // eslint-disable-next-line no-console
            (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
        );

        assert.isTrue(await dec.processUntilEndOfMetadataAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.processUntilEndOfStreamAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.flushAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.finishAsync(), dec.getResolvedStateString());

        const [ finalBuffer, samples ] = joinIntoInterleaved(allBuffers, totalSamples);
        assert.equal(samples, totalSamples);
        comparePCM(okData, finalBuffer, 32);
    });

    it('decoder should be able to skip a frame', async function() {
        const dec = new api.Decoder();
        await dec.initFileAsync(
            pathForFile('loop.flac'),
            () => 0,
            null,
            // eslint-disable-next-line no-console
            (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
        );

        assert.isTrue(await dec.processUntilEndOfMetadataAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.processSingleAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.skipSingleFrameAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.processSingleAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.flushAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.finishAsync(), dec.getResolvedStateString());
    });

    it('decoder should be able to seek to a sample', async function() {
        const callbacks = await generateFlacCallbacks.async(api.Decoder, pathForFile('loop.flac'), 'r');
        deferredScope.defer(() => callbacks.close());
        const dec = new api.Decoder();
        await dec.initStreamAsync(
            callbacks.read,
            callbacks.seek,
            callbacks.tell,
            callbacks.length,
            callbacks.eof,
            () => 0,
            null,
            // eslint-disable-next-line no-console
            (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
        );

        assert.isTrue(await dec.processUntilEndOfMetadataAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.processSingleAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.seekAbsoluteAsync(totalSamples / 5), dec.getResolvedStateString());
        assert.equal(dec.getDecodePosition(), 157036);
        assert.isTrue(await dec.processSingleAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.flushAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.finishAsync(), dec.getResolvedStateString());
    });

    it('decoder should emit metadata', async function() {
        const dec = new api.Decoder();
        const metadataBlocks = [];
        await dec.initFileAsync(
            pathForFile('loop.flac'),
            () => 0,
            (metadata) => {
                metadataBlocks.push(metadata);
                return 0;
            },
            // eslint-disable-next-line no-console
            (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
        );

        assert.isTrue(await dec.processSingleAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.finishAsync(), dec.getResolvedStateString());

        assert.equal(metadataBlocks.length, 1);
    });

    it('encode using stream (non-ogg)', async function() {
        const enc = new api.Encoder();
        const callbacks = generateFlacCallbacks.sync(api.Encoder, tmpFile.path, 'w');
        deferredScope.defer(() => callbacks.close());
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        await enc.initStreamAsync(
            callbacks.write,
            callbacks.seek,
            callbacks.tell,
            null,
        );

        assert.isTrue(await enc.processInterleavedAsync(encData), enc.getResolvedStateString());
        assert.isTrue(await enc.finishAsync(), enc.getResolvedStateString());

        comparePCM(okData, tmpFile.path, 24);
    });

    it('encode using stream (ogg)', async function() {
        const enc = new api.Encoder();
        const callbacks = generateFlacCallbacks.sync(api.Encoder, tmpFile.path, 'w+');
        deferredScope.defer(() => callbacks.close());
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        await enc.initOggStreamAsync(
            callbacks.read,
            callbacks.write,
            callbacks.seek,
            callbacks.tell,
            null,
        );

        assert.isTrue(await enc.processInterleavedAsync(encData), enc.getResolvedStateString());
        assert.isTrue(await enc.finishAsync(), enc.getResolvedStateString());

        comparePCM(okData, tmpFile.path, 24, true);
    });

    it('encode with async callbacks using stream (non-ogg)', async function() {
        const enc = new api.Encoder();
        const callbacks = await generateFlacCallbacks.async(api.Encoder, tmpFile.path, 'w');
        deferredScope.defer(() => callbacks.close());
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        await enc.initStreamAsync(
            callbacks.write,
            callbacks.seek,
            callbacks.tell,
            null,
        );

        assert.isTrue(await enc.processInterleavedAsync(encData), enc.getResolvedStateString());
        assert.isTrue(await enc.finishAsync(), enc.getResolvedStateString());

        comparePCM(okData, tmpFile.path, 24);
    });

    it('encode with async callbacks using stream (ogg)', async function() {
        const enc = new api.Encoder();
        const callbacks = await generateFlacCallbacks.async(api.Encoder, tmpFile.path, 'w+');
        deferredScope.defer(() => callbacks.close());
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        await enc.initOggStreamAsync(
            callbacks.read,
            callbacks.write,
            callbacks.seek,
            callbacks.tell,
            null,
        );

        assert.isTrue(await enc.processInterleavedAsync(encData), enc.getResolvedStateString());
        assert.isTrue(await enc.finishAsync(), enc.getResolvedStateString());

        comparePCM(okData, tmpFile.path, 24, true);
    });

    it('encode using file (non-ogg)', async function() {
        const progressCallbackValues = [];
        const enc = new api.Encoder();
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        await enc.initFileAsync(
            tmpFile.path,
            (...args) => progressCallbackValues.push(args),
        );

        assert.isTrue(await enc.processInterleavedAsync(encData), enc.getResolvedStateString());
        assert.isTrue(await enc.finishAsync(), enc.getResolvedStateString());

        comparePCM(okData, tmpFile.path, 24);
        assert.equal(progressCallbackValues.length, 41);
    });

    it('encode using file (ogg)', async function() {
        const progressCallbackValues = [];
        const enc = new api.Encoder();
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        await enc.initOggFileAsync(
            tmpFile.path,
            (...args) => progressCallbackValues.push(args),
        );

        assert.isTrue(await enc.processInterleavedAsync(encData), enc.getResolvedStateString());
        assert.isTrue(await enc.finishAsync(), enc.getResolvedStateString());

        comparePCM(okData, tmpFile.path, 24, true);
        assert.equal(progressCallbackValues.length, 30);
    });

    it('encode using file with non-interleaved data (non-ogg)', async function() {
        const progressCallbackValues = [];
        const enc = new api.Encoder();
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        await enc.initFileAsync(
            tmpFile.path,
            (...args) => progressCallbackValues.push(args),
        );

        assert.isTrue(await enc.processAsync(encDataAlt, totalSamples), enc.getResolvedStateString());
        assert.isTrue(await enc.finishAsync(), enc.getResolvedStateString());

        comparePCM(okData, tmpFile.path, 24);
        assert.equal(progressCallbackValues.length, 41);
    });

    it('encoder should emit streaminfo metadata block', async function() {
        let metadataBlock = null;
        const callbacks = generateFlacCallbacks.sync(api.Encoder, tmpFile.path, 'w');
        deferredScope.defer(() => callbacks.close());
        const enc = new api.Encoder();
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        enc.setMetadata([ new api.metadata.VorbisCommentMetadata() ]);
        await enc.initStreamAsync(
            callbacks.write,
            null,
            null,
            (metadata) => {
                metadataBlock = metadata;
            },
        );

        assert.isTrue(await enc.processInterleavedAsync(encData), enc.getResolvedStateString());
        assert.isTrue(await enc.finishAsync(), enc.getResolvedStateString());

        assert.isNotNull(metadataBlock);
        assert.equal(metadataBlock.type, 0);
        assert.equal(metadataBlock.totalSamples, totalSamples);
    });

    it('encoder should throw if another async method is running', async function() {
        const enc = new api.Encoder();
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        await enc.initFileAsync(
            tmpFile.path,
            null,
        );

        const promise = enc.processInterleavedAsync(encData);

        await assert.throwsAsync(() => enc.processInterleavedAsync(encData), /There is still an operation running/);

        await promise;
        await enc.finishAsync();
    });

    it('decoder should throw if another async method is running', async function() {
        const dec = new api.Decoder();
        const allBuffers = [];
        await dec.initFileAsync(
            pathForFile('loop.flac'),
            (_, buffers) => {
                allBuffers.push(buffers.map((b) => Buffer.from(b)));
                return 0;
            },
            null,
            // eslint-disable-next-line no-console
            (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
        );

        const e = await dec.processUntilEndOfMetadataAsync();
        assert.isTrue(e);

        const promise = dec.processSingleAsync();
        await assert.throwsAsync(() => dec.processSingleAsync(), /There is still an operation running/);

        await promise;
        const f = await dec.processUntilEndOfStreamAsync();
        assert.isTrue(f);
        await dec.finishAsync();
    });

});
