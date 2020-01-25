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
    tmpFile = temp.openSync('flac-bindings.encode-decode.sync-api');
});

afterEach('cleanUpTemporaryFiles', function() {
    temp.cleanupSync();
});

describe('encode & decode: sync api', function() {

    it('decode using stream', function() {
        const fd = fs.openSync(pathForFile('loop.flac'), 'r');
        const dec = new api.Decoder();
        const allBuffers = [];
        dec.initStream(
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

        assert.isTrue(dec.processUntilEndOfMetadata());
        assert.isTrue(dec.processUntilEndOfStream());
        assert.isTrue(dec.flush());
        assert.isTrue(dec.finish());

        const [ finalBuffer, samples ] = joinIntoInterleaved(allBuffers, totalSamples);
        assert.equal(samples, totalSamples);
        comparePCM(okData, finalBuffer, 32);
    });

    it('decode using stream (ogg)', function() {
        const fd = fs.openSync(pathForFile('loop.oga'), 'r');
        const dec = new api.Decoder();
        const allBuffers = [];
        assert.equal(dec.initOggStream(
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
        ), 0);

        assert.isTrue(dec.processUntilEndOfMetadata());
        assert.isTrue(dec.processUntilEndOfStream());
        assert.isTrue(dec.flush());
        assert.isTrue(dec.finish());

        const [ finalBuffer, samples ] = joinIntoInterleaved(allBuffers, totalSamples);
        assert.equal(samples, totalSamples);
        comparePCM(okData, finalBuffer, 32);
    });

    it('decode using file', function() {
        const dec = new api.Decoder();
        const allBuffers = [];
        dec.initFile(
            pathForFile('loop.flac'),
            (_, buffers) => {
                allBuffers.push(buffers.map((b) => Buffer.from(b)));
                return 0;
            },
            null,
            // eslint-disable-next-line no-console
            (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
        );

        assert.isTrue(dec.processUntilEndOfMetadata());
        assert.isTrue(dec.processUntilEndOfStream());
        assert.isTrue(dec.flush());
        assert.isTrue(dec.finish());

        const [ finalBuffer, samples ] = joinIntoInterleaved(allBuffers, totalSamples);
        assert.equal(samples, totalSamples);
        comparePCM(okData, finalBuffer, 32);
    });

    it('decode using file (ogg)', function() {
        const dec = new api.Decoder();
        const allBuffers = [];
        dec.initOggFile(
            pathForFile('loop.oga'),
            (_, buffers) => {
                allBuffers.push(buffers.map((b) => Buffer.from(b)));
                return 0;
            },
            null,
            // eslint-disable-next-line no-console
            (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
        );

        assert.isTrue(dec.processUntilEndOfMetadata());
        assert.isTrue(dec.processUntilEndOfStream());
        assert.isTrue(dec.flush());
        assert.isTrue(dec.finish());

        const [ finalBuffer, samples ] = joinIntoInterleaved(allBuffers, totalSamples);
        assert.equal(samples, totalSamples);
        comparePCM(okData, finalBuffer, 32);
    });

    it('decoder should emit metadata', function() {
        const dec = new api.Decoder();
        const metadataBlocks = [];
        dec.initFile(
            pathForFile('loop.flac'),
            () => 0,
            (metadata) => {
                metadataBlocks.push(metadata);
                return 0;
            },
            // eslint-disable-next-line no-console
            (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
        );

        assert.isTrue(dec.processSingle());
        assert.isTrue(dec.finish());

        assert.equal(metadataBlocks.length, 1);
    });

    it('encode using stream', function() {
        const enc = new api.Encoder();
        const fd = fs.openSync(tmpFile.path, 'w');
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        enc.initStream(
            (buffer) => fs.writeSync(fd, buffer, 0, buffer.length, null) === buffer.length ? 0 : 2,
            (offset) => fs.writeSync(fd, Buffer.alloc(1), 0, 0, offset),
            () => ({ offset: BigInt(0), returnValue: api.Encoder.TellStatus.UNSUPPORTED }),
            null,
        );

        const chunkazo = Buffer.allocUnsafe(totalSamples * 4 * 2);
        for(let i = 0; i < totalSamples * 2; i++) {
            chunkazo.writeInt32LE(okData.readIntLE(i * 3, 3), i * 4);
        }
        assert.isTrue(enc.processInterleaved(chunkazo));
        assert.isTrue(enc.finish());

        comparePCM(okData, tmpFile.path, 24);
    });

    it('encode using stream (ogg)', function() {
        const enc = new api.Encoder();
        const fd = fs.openSync(tmpFile.path, 'w+');
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        let newPosition = null;
        enc.initOggStream(
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
        assert.isTrue(enc.processInterleaved(chunkazo));
        assert.isTrue(enc.finish());

        comparePCM(okData, tmpFile.path, 24, true);
    });

    it('encode using file', function() {
        const progressCallbackValues = [];
        const enc = new api.Encoder();
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        enc.initFile(
            tmpFile.path,
            (...args) => progressCallbackValues.push(args),
        );

        const chunkazo = Buffer.allocUnsafe(totalSamples * 4 * 2);
        for(let i = 0; i < totalSamples * 2; i++) {
            chunkazo.writeInt32LE(okData.readIntLE(i * 3, 3), i * 4);
        }
        assert.isTrue(enc.processInterleaved(chunkazo));
        assert.isTrue(enc.finish());

        comparePCM(okData, tmpFile.path, 24);
        assert.equal(progressCallbackValues.length, 41);
    });

    it('encode using file (ogg)', function() {
        const progressCallbackValues = [];
        const enc = new api.Encoder();
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        enc.initOggFile(
            tmpFile.path,
            (...args) => progressCallbackValues.push(args),
        );

        const chunkazo = Buffer.allocUnsafe(totalSamples * 4 * 2);
        for(let i = 0; i < totalSamples * 2; i++) {
            chunkazo.writeInt32LE(okData.readIntLE(i * 3, 3), i * 4);
        }
        assert.isTrue(enc.processInterleaved(chunkazo));
        assert.isTrue(enc.finish());

        comparePCM(okData, tmpFile.path, 24, true);
        assert.equal(progressCallbackValues.length, 30);
    });

    it('encoder should emit streaminfo metadata block', function() {
        let metadataBlock = null;
        const fd = fs.openSync(tmpFile.path, 'w');
        const enc = new api.Encoder();
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        enc.setMetadata([ new api.metadata.VorbisCommentMetadata() ]);
        enc.initStream(
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
        assert.isTrue(enc.processInterleaved(chunkazo));
        assert.isTrue(enc.finish());

        assert.isNotNull(metadataBlock);
        assert.equal(metadataBlock.type, 0);
        assert.equal(metadataBlock.totalSamples, totalSamples);
    });

});
