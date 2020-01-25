/* eslint-disable prefer-arrow-callback */
/// <reference path="../../lib/index.d.ts" />
const { api } = require('../../lib/index');
const { assert } = require('chai');
const fs = require('fs');
const temp = require('temp').track();

const { pathForFile: { audio: pathForFile }, comparePCM, getWavAudio, joinIntoInterleaved } = require('../helper');

const totalSamples = 992250 / 3 / 2;

const okData = getWavAudio('loop.wav');

let tmpFile;
beforeEach('createTemporaryFiles', function() {
    tmpFile = temp.openSync('flac-bindings.encode-decode.async-api');
});

afterEach('cleanUpTemporaryFiles', function() {
    temp.cleanupSync();
});

describe('encode & decode: async api', function() {

    it('decode using stream', async function() {
        const fd = fs.openSync(pathForFile('loop.flac'), 'r');
        const dec = new api.Decoder();
        const allBuffers = [];
        await dec.initStreamAsync(
            (buffer) => {
                const bytes = fs.readSync(fd, buffer, 0, buffer.length, null);
                if(bytes === 0) {
                    return { bytes, returnValue: api.Decoder.ReadStatus.END_OF_STREAM };
                }
                return { bytes, returnValue: api.Decoder.ReadStatus.CONTINUE };
            },
            (offset) => fs.readSync(fd, Buffer.alloc(1), 0, 1, offset - 1) === 1 ? 0 : 2,
            () => ({ returnValue: api.Decoder.TellStatus.UNSUPPORTED, offset: BigInt(0) }),
            () => ({ length: fs.statSync(pathForFile('loop.flac')).size, returnValue: 0 }),
            () => false,
            (_, buffers) => {
                allBuffers.push(buffers.map((b) => Buffer.from(b)));
                return 0;
            },
            null,
            // eslint-disable-next-line no-console
            (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
        );

        assert.isTrue(await dec.processUntilEndOfMetadataAsync());
        assert.isTrue(await dec.processUntilEndOfStreamAsync());
        assert.isTrue(await dec.flushAsync());
        assert.isTrue(await dec.finishAsync());

        const [ finalBuffer, samples ] = joinIntoInterleaved(allBuffers, totalSamples);
        assert.equal(samples, totalSamples);
        comparePCM(okData, finalBuffer, 32);
    });

    it('decode using stream (ogg)', async function() {
        const fd = fs.openSync(pathForFile('loop.oga'), 'r');
        const dec = new api.Decoder();
        const allBuffers = [];
        await dec.initOggStreamAsync(
            (buffer) => {
                const bytes = fs.readSync(fd, buffer, 0, buffer.length, null);
                if(bytes === 0) {
                    return { bytes, returnValue: api.Decoder.ReadStatus.END_OF_STREAM };
                }
                return { bytes, returnValue: api.Decoder.ReadStatus.CONTINUE };
            },
            (offset) => fs.readSync(fd, Buffer.alloc(1), 0, 1, offset - 1) === 1 ? 0 : 2,
            () => ({ returnValue: api.Decoder.TellStatus.UNSUPPORTED, offset: BigInt(0) }),
            () => ({ length: fs.statSync(pathForFile('loop.flac')).size, returnValue: 0 }),
            () => false,
            (_, buffers) => {
                allBuffers.push(buffers.map((b) => Buffer.from(b)));
                return 0;
            },
            null,
            // eslint-disable-next-line no-console
            (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
        );

        assert.isTrue(await dec.processUntilEndOfMetadataAsync());
        assert.isTrue(await dec.processUntilEndOfStreamAsync());
        assert.isTrue(await dec.flushAsync());
        assert.isTrue(await dec.finishAsync());

        const [ finalBuffer, samples ] = joinIntoInterleaved(allBuffers, totalSamples);
        assert.equal(samples, totalSamples);
        comparePCM(okData, finalBuffer, 32);
    });

    it('decode with async callbacks using stream', async function() {
        const fh = await fs.promises.open(pathForFile('loop.flac'), 'r');
        const dec = new api.Decoder();
        const allBuffers = [];
        await dec.initStreamAsync(
            async (buffer) => {
                const { bytesRead } = await fh.read(buffer, 0, buffer.length, null);
                if(bytesRead === 0) {
                    return { bytes: 0, returnValue: api.Decoder.ReadStatus.END_OF_STREAM };
                }
                return { bytes: bytesRead, returnValue: api.Decoder.ReadStatus.CONTINUE };
            },
            async (offset) => (await fh.read(Buffer.alloc(1), 0, 1, offset - 1)).bytesRead === 1 ? 0 : 2,
            () => ({ returnValue: api.Decoder.TellStatus.UNSUPPORTED, offset: BigInt(0) }),
            async () => ({ length: (await fh.stat(pathForFile('loop.flac'))).size, returnValue: 0 }),
            () => false,
            (_, buffers) => {
                allBuffers.push(buffers.map((b) => Buffer.from(b)));
                return 0;
            },
            null,
            // eslint-disable-next-line no-console
            (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
        );

        assert.isTrue(await dec.processUntilEndOfMetadataAsync());
        assert.isTrue(await dec.processUntilEndOfStreamAsync());
        assert.isTrue(await dec.flushAsync());
        assert.isTrue(await dec.finishAsync());

        const [ finalBuffer, samples ] = joinIntoInterleaved(allBuffers, totalSamples);
        assert.equal(samples, totalSamples);
        comparePCM(okData, finalBuffer, 32);
    });

    it('decode with async callbacks using stream (ogg)', async function() {
        const fh = await fs.promises.open(pathForFile('loop.oga'), 'r');
        const dec = new api.Decoder();
        const allBuffers = [];
        await dec.initOggStreamAsync(
            async (buffer) => {
                const { bytesRead } = await fh.read(buffer, 0, buffer.length, null);
                if(bytesRead === 0) {
                    return { bytes: 0, returnValue: api.Decoder.ReadStatus.END_OF_STREAM };
                }
                return { bytes: bytesRead, returnValue: api.Decoder.ReadStatus.CONTINUE };
            },
            async (offset) => (await fh.read(Buffer.alloc(1), 0, 1, offset - 1)).bytesRead === 1 ? 0 : 2,
            () => ({ returnValue: api.Decoder.TellStatus.UNSUPPORTED, offset: BigInt(0) }),
            async () => ({ length: (await fh.stat(pathForFile('loop.flac'))).size, returnValue: 0 }),
            () => false,
            (_, buffers) => {
                allBuffers.push(buffers.map((b) => Buffer.from(b)));
                return 0;
            },
            null,
            // eslint-disable-next-line no-console
            (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
        );

        assert.isTrue(await dec.processUntilEndOfMetadataAsync());
        assert.isTrue(await dec.processUntilEndOfStreamAsync());
        assert.isTrue(await dec.flushAsync());
        assert.isTrue(await dec.finishAsync());

        const [ finalBuffer, samples ] = joinIntoInterleaved(allBuffers, totalSamples);
        assert.equal(samples, totalSamples);
        comparePCM(okData, finalBuffer, 32);
    });

    it('decode using file', async function() {
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

        assert.isTrue(await dec.processUntilEndOfMetadataAsync());
        assert.isTrue(await dec.processUntilEndOfStreamAsync());
        assert.isTrue(await dec.flushAsync());
        assert.isTrue(await dec.finishAsync());

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

        assert.isTrue(await dec.processUntilEndOfMetadataAsync());
        assert.isTrue(await dec.processUntilEndOfStreamAsync());
        assert.isTrue(await dec.flushAsync());
        assert.isTrue(await dec.finishAsync());

        const [ finalBuffer, samples ] = joinIntoInterleaved(allBuffers, totalSamples);
        assert.equal(samples, totalSamples);
        comparePCM(okData, finalBuffer, 32);
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

        assert.isTrue(await dec.processSingleAsync());
        assert.isTrue(await dec.finishAsync());

        assert.equal(metadataBlocks.length, 1);
    });

    it('encode using stream', async function() {
        const enc = new api.Encoder();
        const fd = fs.openSync(tmpFile.path, 'w');
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        await enc.initStreamAsync(
            (buffer) => fs.writeSync(fd, buffer, 0, buffer.length, null) === buffer.length ? 0 : 2,
            (offset) => fs.writeSync(fd, Buffer.alloc(1), 0, 0, offset),
            () => ({ offset: BigInt(0), returnValue: api.Encoder.TellStatus.UNSUPPORTED }),
            null,
        );

        const chunkazo = Buffer.allocUnsafe(totalSamples * 4 * 2);
        for(let i = 0; i < totalSamples * 2; i++) {
            chunkazo.writeInt32LE(okData.readIntLE(i * 3, 3), i * 4);
        }
        assert.isTrue(await enc.processInterleavedAsync(chunkazo));
        assert.isTrue(await enc.finishAsync());

        comparePCM(okData, tmpFile.path, 24);
    });

    it('encode using stream (ogg)', async function() {
        const enc = new api.Encoder();
        const fd = fs.openSync(tmpFile.path, 'w+');
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        let newPosition = null;
        await enc.initOggStreamAsync(
            (buffer) => {
                const bytes = fs.readSync(fd, buffer, 0, buffer.length, newPosition);
                if(newPosition !== null) {
                    newPosition += bytes;
                }
                if(bytes === 0) {
                    return { bytes, returnValue: api.Encoder.ReadStatus.END_OF_STREAM };
                }
                return { bytes, returnValue: api.Encoder.ReadStatus.CONTINUE };
            },
            (buffer) => fs.writeSync(fd, buffer, 0, buffer.length, null) === buffer.length ? 0 : 2,
            (offset) => {
                newPosition = offset;
                return 0;
            },
            () => ({ offset: BigInt(0), returnValue: api.Encoder.TellStatus.UNSUPPORTED }),
            null,
        );

        const chunkazo = Buffer.allocUnsafe(totalSamples * 4 * 2);
        for(let i = 0; i < totalSamples * 2; i++) {
            chunkazo.writeInt32LE(okData.readIntLE(i * 3, 3), i * 4);
        }
        assert.isTrue(await enc.processInterleavedAsync(chunkazo));
        assert.isTrue(await enc.finishAsync());

        comparePCM(okData, tmpFile.path, 24, true);
    });

    it('encode with async callbacks using stream', async function() {
        const enc = new api.Encoder();
        const fh = await fs.promises.open(tmpFile.path, 'w');
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        await enc.initStreamAsync(
            async (buffer) => (await fh.write(buffer, 0, buffer.length, null)).bytesWritten === buffer.length ? 0 : 2,
            async (offset) => (await fh.write(Buffer.alloc(1), 0, 0, offset)).bytesWritten,
            () => ({ offset: BigInt(0), returnValue: api.Encoder.TellStatus.UNSUPPORTED }),
            null,
        );

        const chunkazo = Buffer.allocUnsafe(totalSamples * 4 * 2);
        for(let i = 0; i < totalSamples * 2; i++) {
            chunkazo.writeInt32LE(okData.readIntLE(i * 3, 3), i * 4);
        }
        assert.isTrue(await enc.processInterleavedAsync(chunkazo));
        assert.isTrue(await enc.finishAsync());

        comparePCM(okData, tmpFile.path, 24);
    });

    it('encode with async callbacks using stream (ogg)', async function() {
        const enc = new api.Encoder();
        const fh = await fs.promises.open(tmpFile.path, 'w+');
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        let newPosition = null;
        await enc.initOggStreamAsync(
            async (buffer) => {
                const { bytesRead } = await fh.read(buffer, 0, buffer.length, newPosition);
                if(newPosition !== null) {
                    newPosition += bytesRead;
                }
                if(bytesRead === 0) {
                    return { bytes: 0, returnValue: api.Encoder.ReadStatus.END_OF_STREAM };
                }
                return { bytes: bytesRead, returnValue: api.Encoder.ReadStatus.CONTINUE };
            },
            async (buffer) => (await fh.write(buffer, 0, buffer.length, null)).bytesWritten === buffer.length ? 0 : 2,
            (offset) => {
                newPosition = offset;
                return 0;
            },
            () => ({ offset: BigInt(0), returnValue: api.Encoder.TellStatus.UNSUPPORTED }),
            null,
        );

        const chunkazo = Buffer.allocUnsafe(totalSamples * 4 * 2);
        for(let i = 0; i < totalSamples * 2; i++) {
            chunkazo.writeInt32LE(okData.readIntLE(i * 3, 3), i * 4);
        }
        assert.isTrue(await enc.processInterleavedAsync(chunkazo));
        assert.isTrue(await enc.finishAsync());

        comparePCM(okData, tmpFile.path, 24, true);
    });

    it('encode using file', async function() {
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

        const chunkazo = Buffer.allocUnsafe(totalSamples * 4 * 2);
        for(let i = 0; i < totalSamples * 2; i++) {
            chunkazo.writeInt32LE(okData.readIntLE(i * 3, 3), i * 4);
        }
        assert.isTrue(await enc.processInterleavedAsync(chunkazo));
        assert.isTrue(await enc.finishAsync());

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

        const chunkazo = Buffer.allocUnsafe(totalSamples * 4 * 2);
        for(let i = 0; i < totalSamples * 2; i++) {
            chunkazo.writeInt32LE(okData.readIntLE(i * 3, 3), i * 4);
        }
        assert.isTrue(await enc.processInterleavedAsync(chunkazo));
        assert.isTrue(await enc.finishAsync());

        comparePCM(okData, tmpFile.path, 24, true);
        assert.equal(progressCallbackValues.length, 30);
    });

    it('encoder should emit streaminfo metadata block', async function() {
        let metadataBlock = null;
        const fd = fs.openSync(tmpFile.path, 'w');
        const enc = new api.Encoder();
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        enc.setMetadata([ new api.metadata.VorbisCommentMetadata() ]);
        await enc.initStreamAsync(
            (buffer) => fs.writeSync(fd, buffer, 0, buffer.length, null) === buffer.length ? 0 : 2,
            (offset) => fs.writeSync(fd, Buffer.alloc(1), 0, 0, offset),
            () => ({ offset: BigInt(0), returnValue: api.Encoder.TellStatus.UNSUPPORTED }),
            (metadata) => {
                metadataBlock = metadata;
            },
        );

        const chunkazo = Buffer.allocUnsafe(totalSamples * 4 * 2);
        for(let i = 0; i < totalSamples * 2; i++) {
            chunkazo.writeInt32LE(okData.readIntLE(i * 3, 3), i * 4);
        }
        assert.isTrue(await enc.processInterleavedAsync(chunkazo));
        assert.isTrue(await enc.finishAsync());

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

        const chunkazo = Buffer.allocUnsafe(totalSamples * 4 * 2);
        for(let i = 0; i < totalSamples * 2; i++) {
            chunkazo.writeInt32LE(okData.readIntLE(i * 3, 3), i * 4);
        }
        const promise = enc.processInterleavedAsync(chunkazo);

        await assert.throwsAsync(() => enc.processInterleavedAsync(chunkazo), /There is still an operation running/);

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
