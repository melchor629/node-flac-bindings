/* eslint-disable prefer-arrow-callback */
const { api } = require('../../lib/index');
const { assert } = require('chai');
const temp = require('temp').track();

const {
    pathForFile: { audio: pathForFile },
    comparePCM,
    getWavAudio,
    generateFlacCallbacks,
    joinIntoInterleaved,
} = require('../helper');

const totalSamples = 992250 / 3 / 2;

const okData = getWavAudio('loop.wav');
const encData = Buffer.allocUnsafe(totalSamples * 4 * 2);
for(let i = 0; i < totalSamples * 2; i++) {
    encData.writeInt32LE(okData.readIntLE(i * 3, 3), i * 4);
}

let tmpFile;
beforeEach('createTemporaryFiles', function() {
    tmpFile = temp.openSync('flac-bindings.encode-decode.async-api');
});

afterEach('cleanUpTemporaryFiles', function() {
    temp.cleanupSync();
});

describe('encode & decode: async api', function() {

    this.slow(250 * 1000);

    it('decode using stream (non-ogg)', async function() {
        const callbacks = generateFlacCallbacks.sync(api.Decoder, pathForFile('loop.flac'), 'r');
        const dec = new api.Decoder();
        const allBuffers = [];
        assert.equal(await dec.initStreamAsync(
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
        ), 0, dec.getResolvedStateString());

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
        const dec = new api.Decoder();
        const allBuffers = [];
        assert.equal(await dec.initOggStreamAsync(
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
        ), 0, dec.getResolvedStateString());

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
        const dec = new api.Decoder();
        const allBuffers = [];
        assert.equal(await dec.initStreamAsync(
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
        ), 0, dec.getResolvedStateString());

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
        const dec = new api.Decoder();
        const allBuffers = [];
        assert.equal(await dec.initOggStreamAsync(
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
        ), 0, dec.getResolvedStateString());

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
        assert.equal(await dec.initFileAsync(
            pathForFile('loop.flac'),
            (_, buffers) => {
                allBuffers.push(buffers.map((b) => Buffer.from(b)));
                return 0;
            },
            null,
            // eslint-disable-next-line no-console
            (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
        ), 0, dec.getResolvedStateString());

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
        assert.equal(await dec.initOggFileAsync(
            pathForFile('loop.oga'),
            (_, buffers) => {
                allBuffers.push(buffers.map((b) => Buffer.from(b)));
                return 0;
            },
            null,
            // eslint-disable-next-line no-console
            (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
        ), 0, dec.getResolvedStateString());

        assert.isTrue(await dec.processUntilEndOfMetadataAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.processUntilEndOfStreamAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.flushAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.finishAsync(), dec.getResolvedStateString());

        const [ finalBuffer, samples ] = joinIntoInterleaved(allBuffers, totalSamples);
        assert.equal(samples, totalSamples);
        comparePCM(okData, finalBuffer, 32);
    });

    it('decoder should emit metadata', async function() {
        const dec = new api.Decoder();
        const metadataBlocks = [];
        assert.equal(await dec.initFileAsync(
            pathForFile('loop.flac'),
            () => 0,
            (metadata) => {
                metadataBlocks.push(metadata);
                return 0;
            },
            // eslint-disable-next-line no-console
            (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
        ), 0, dec.getResolvedStateString());

        assert.isTrue(await dec.processSingleAsync(), dec.getResolvedStateString());
        assert.isTrue(await dec.finishAsync(), dec.getResolvedStateString());

        assert.equal(metadataBlocks.length, 1);
    });

    it('encode using stream (non-ogg)', async function() {
        const enc = new api.Encoder();
        const callbacks = generateFlacCallbacks.sync(api.Encoder, tmpFile.path, 'w');
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        assert.equal(await enc.initStreamAsync(
            callbacks.write,
            callbacks.seek,
            callbacks.tell,
            null,
        ), 0, enc.getResolvedStateString());

        assert.isTrue(await enc.processInterleavedAsync(encData), enc.getResolvedStateString());
        assert.isTrue(await enc.finishAsync(), enc.getResolvedStateString());

        comparePCM(okData, tmpFile.path, 24);
    });

    it('encode using stream (ogg)', async function() {
        const enc = new api.Encoder();
        const callbacks = generateFlacCallbacks.sync(api.Encoder, tmpFile.path, 'w+');
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        assert.equal(await enc.initOggStreamAsync(
            callbacks.read,
            callbacks.write,
            callbacks.seek,
            callbacks.tell,
            null,
        ), 0, enc.getResolvedStateString());

        assert.isTrue(await enc.processInterleavedAsync(encData), enc.getResolvedStateString());
        assert.isTrue(await enc.finishAsync(), enc.getResolvedStateString());

        comparePCM(okData, tmpFile.path, 24, true);
    });

    it('encode with async callbacks using stream (non-ogg)', async function() {
        const enc = new api.Encoder();
        const callbacks = await generateFlacCallbacks.async(api.Encoder, tmpFile.path, 'w');
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        assert.equal(await enc.initStreamAsync(
            callbacks.write,
            callbacks.seek,
            callbacks.tell,
            null,
        ), 0, enc.getResolvedStateString());

        assert.isTrue(await enc.processInterleavedAsync(encData), enc.getResolvedStateString());
        assert.isTrue(await enc.finishAsync(), enc.getResolvedStateString());

        comparePCM(okData, tmpFile.path, 24);
    });

    it('encode with async callbacks using stream (ogg)', async function() {
        const enc = new api.Encoder();
        const callbacks = await generateFlacCallbacks.async(api.Encoder, tmpFile.path, 'w+');
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        assert.equal(await enc.initOggStreamAsync(
            callbacks.read,
            callbacks.write,
            callbacks.seek,
            callbacks.tell,
            null,
        ), 0, enc.getResolvedStateString());

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
        assert.equal(await enc.initFileAsync(
            tmpFile.path,
            (...args) => progressCallbackValues.push(args),
        ), 0, enc.getResolvedStateString());

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
        assert.equal(await enc.initOggFileAsync(
            tmpFile.path,
            (...args) => progressCallbackValues.push(args),
        ), 0, enc.getResolvedStateString());

        assert.isTrue(await enc.processInterleavedAsync(encData), enc.getResolvedStateString());
        assert.isTrue(await enc.finishAsync(), enc.getResolvedStateString());

        comparePCM(okData, tmpFile.path, 24, true);
        assert.equal(progressCallbackValues.length, 30);
    });

    it('encoder should emit streaminfo metadata block', async function() {
        let metadataBlock = null;
        const callbacks = generateFlacCallbacks.sync(api.Encoder, tmpFile.path, 'w');
        const enc = new api.Encoder();
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        enc.setMetadata([ new api.metadata.VorbisCommentMetadata() ]);
        assert.equal(await enc.initStreamAsync(
            callbacks.write,
            null,
            null,
            (metadata) => {
                metadataBlock = metadata;
            },
        ), 0, enc.getResolvedStateString());

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
