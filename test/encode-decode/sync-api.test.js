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
    tmpFile = temp.openSync('flac-bindings.encode-decode.sync-api');
    deferredScope = createDeferredScope();
});

afterEach('cleanUpTemporaryFiles', function() {
    temp.cleanupSync();
    deferredScope.finalize();
});

describe('encode & decode: sync api', function() {

    this.slow(250 * 1000);

    it('decode using stream (non-ogg)', function() {
        const callbacks = generateFlacCallbacks.sync(api.Decoder, pathForFile('loop.flac'), 'r');
        deferredScope.defer(() => callbacks.close());
        const dec = new api.Decoder();
        const allBuffers = [];
        assert.equal(dec.initStream(
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

        assert.isTrue(dec.processUntilEndOfMetadata(), dec.getResolvedStateString());
        assert.isTrue(dec.processUntilEndOfStream(), dec.getResolvedStateString());
        assert.isTrue(dec.flush(), dec.getResolvedStateString());
        assert.isTrue(dec.finish(), dec.getResolvedStateString());

        const [ finalBuffer, samples ] = joinIntoInterleaved(allBuffers, totalSamples);
        assert.equal(samples, totalSamples);
        comparePCM(okData, finalBuffer, 32);
    });

    it('decode using stream (ogg)', function() {
        const callbacks = generateFlacCallbacks.sync(api.Decoder, pathForFile('loop.oga'), 'r');
        deferredScope.defer(() => callbacks.close());
        const dec = new api.Decoder();
        const allBuffers = [];
        assert.equal(dec.initOggStream(
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

        assert.isTrue(dec.processUntilEndOfMetadata(), dec.getResolvedStateString());
        assert.isTrue(dec.processUntilEndOfStream(), dec.getResolvedStateString());
        assert.isTrue(dec.flush(), dec.getResolvedStateString());
        assert.isTrue(dec.finish(), dec.getResolvedStateString());

        const [ finalBuffer, samples ] = joinIntoInterleaved(allBuffers, totalSamples);
        assert.equal(samples, totalSamples);
        comparePCM(okData, finalBuffer, 32);
    });

    it('decode using file (non-ogg)', function() {
        const dec = new api.Decoder();
        const allBuffers = [];
        assert.equal(dec.initFile(
            pathForFile('loop.flac'),
            (_, buffers) => {
                allBuffers.push(buffers.map((b) => Buffer.from(b)));
                return 0;
            },
            null,
            // eslint-disable-next-line no-console
            (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
        ), 0, dec.getResolvedStateString());

        assert.isTrue(dec.processUntilEndOfMetadata(), dec.getResolvedStateString());
        assert.isTrue(dec.processUntilEndOfStream(), dec.getResolvedStateString());
        assert.isTrue(dec.flush(), dec.getResolvedStateString());
        assert.isTrue(dec.finish(), dec.getResolvedStateString());

        const [ finalBuffer, samples ] = joinIntoInterleaved(allBuffers, totalSamples);
        assert.equal(samples, totalSamples);
        comparePCM(okData, finalBuffer, 32);
    });

    it('decode using file (ogg)', function() {
        const dec = new api.Decoder();
        const allBuffers = [];
        assert.equal(dec.initOggFile(
            pathForFile('loop.oga'),
            (_, buffers) => {
                allBuffers.push(buffers.map((b) => Buffer.from(b)));
                return 0;
            },
            null,
            // eslint-disable-next-line no-console
            (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
        ), 0, dec.getResolvedStateString());

        assert.isTrue(dec.processUntilEndOfMetadata(), dec.getResolvedStateString());
        assert.isTrue(dec.processUntilEndOfStream(), dec.getResolvedStateString());
        assert.isTrue(dec.flush(), dec.getResolvedStateString());
        assert.isTrue(dec.finish(), dec.getResolvedStateString());

        const [ finalBuffer, samples ] = joinIntoInterleaved(allBuffers, totalSamples);
        assert.equal(samples, totalSamples);
        comparePCM(okData, finalBuffer, 32);
    });

    it('decoder should be able to skip a frame', function() {
        const dec = new api.Decoder();
        assert.equal(dec.initFile(
            pathForFile('loop.flac'),
            () => 0,
            null,
            // eslint-disable-next-line no-console
            (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
        ), 0, dec.getResolvedStateString());

        assert.isTrue(dec.processUntilEndOfMetadata(), dec.getResolvedStateString());
        assert.isTrue(dec.processSingle(), dec.getResolvedStateString());
        assert.isTrue(dec.skipSingleFrame(), dec.getResolvedStateString());
        assert.isTrue(dec.processSingle(), dec.getResolvedStateString());
        assert.isTrue(dec.flush(), dec.getResolvedStateString());
        assert.isTrue(dec.finish(), dec.getResolvedStateString());
    });

    it('decoder should be able to seek to a sample', function() {
        const callbacks = generateFlacCallbacks.sync(api.Decoder, pathForFile('loop.flac'), 'r');
        deferredScope.defer(() => callbacks.close());
        const dec = new api.Decoder();
        assert.equal(dec.initStream(
            callbacks.read,
            callbacks.seek,
            callbacks.tell,
            callbacks.length,
            callbacks.eof,
            () => 0,
            null,
            // eslint-disable-next-line no-console
            (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
        ), 0, dec.getResolvedStateString());

        assert.isTrue(dec.processUntilEndOfMetadata(), dec.getResolvedStateString());
        assert.isTrue(dec.processSingle(), dec.getResolvedStateString());
        assert.isTrue(dec.seekAbsolute(totalSamples / 5), dec.getResolvedStateString());
        assert.equal(dec.getDecodePosition(), 157036);
        assert.isTrue(dec.processSingle(), dec.getResolvedStateString());
        assert.isTrue(dec.flush(), dec.getResolvedStateString());
        assert.isTrue(dec.finish(), dec.getResolvedStateString());
    });

    it('decoder should emit metadata', function() {
        const dec = new api.Decoder();
        const metadataBlocks = [];
        assert.equal(0, dec.initFile(
            pathForFile('loop.flac'),
            () => 0,
            (metadata) => {
                metadataBlocks.push(metadata);
                return 0;
            },
            // eslint-disable-next-line no-console
            (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
        ), dec.getResolvedStateString());

        assert.isTrue(dec.processSingle(), dec.getResolvedStateString());
        assert.isTrue(dec.finish(), dec.getResolvedStateString());

        assert.equal(metadataBlocks.length, 1);
    });

    it('decoder get other properties work', function() {
        const dec = new api.Decoder();
        dec.setMd5Checking(true);
        dec.setOggSerialNumber(0x11223344);
        dec.setMetadataIgnore(1);
        assert.equal(dec.initFile(
            pathForFile('loop.flac'),
            () => 0,
            () => 0,
            // eslint-disable-next-line no-console
            (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
        ), 0, dec.getResolvedStateString());

        assert.isTrue(dec.processUntilEndOfMetadata(), dec.getResolvedStateString());
        assert.isTrue(dec.processSingle(), dec.getResolvedStateString());

        assert.equal(dec.getChannels(), 2);
        assert.equal(dec.getBitsPerSample(), 24);
        assert.equal(dec.getSampleRate(), 44100);
        assert.equal(dec.getChannelAssignment(), api.format.ChannelAssignment.MID_SIDE);
        assert.equal(dec.getBlocksize(), 4096);
        assert.equal(dec.getTotalSamples(), totalSamples);
        assert.isTrue(dec.getMd5Checking());
        assert.equal(dec.getDecodePosition(), 22862);

        assert.isFalse(dec.finish(), dec.getResolvedStateString());
    });

    it('encode using stream (non-ogg)', function() {
        const enc = new api.Encoder();
        const callbacks = generateFlacCallbacks.sync(api.Encoder, tmpFile.path, 'w');
        deferredScope.defer(() => callbacks.close());
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        assert.equal(enc.initStream(
            callbacks.write,
            callbacks.seek,
            callbacks.tell,
            null,
        ), 0, enc.getResolvedStateString());

        assert.isTrue(enc.processInterleaved(encData), enc.getResolvedStateString());
        assert.isTrue(enc.finish(), enc.getResolvedStateString());

        comparePCM(okData, tmpFile.path, 24);
    });

    it('encode using stream (ogg)', function() {
        const enc = new api.Encoder();
        const callbacks = generateFlacCallbacks.sync(api.Encoder, tmpFile.path, 'w+');
        deferredScope.defer(() => callbacks.close());
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        assert.equal(enc.initOggStream(
            callbacks.read,
            callbacks.write,
            callbacks.seek,
            callbacks.tell,
            null,
        ), 0);

        assert.isTrue(enc.processInterleaved(encData), enc.getResolvedStateString());
        assert.isTrue(enc.finish(), enc.getResolvedStateString());

        comparePCM(okData, tmpFile.path, 24, true);
    });

    it('encode using file (non-ogg)', function() {
        const progressCallbackValues = [];
        const enc = new api.Encoder();
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        assert.equal(enc.initFile(
            tmpFile.path,
            (...args) => progressCallbackValues.push(args),
        ), 0, enc.getResolvedStateString());

        assert.isTrue(enc.processInterleaved(encData), enc.getResolvedStateString());
        assert.isTrue(enc.finish(), enc.getResolvedStateString());

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
        assert.equal(enc.initOggFile(
            tmpFile.path,
            (...args) => progressCallbackValues.push(args),
        ), 0, enc.getResolvedStateString());

        assert.isTrue(enc.processInterleaved(encData), enc.getResolvedStateString());
        assert.isTrue(enc.finish(), enc.getResolvedStateString());

        comparePCM(okData, tmpFile.path, 24, true);
        assert.equal(progressCallbackValues.length, 30);
    });

    it('encode using file with non-interleaved data (non-ogg)', function() {
        const progressCallbackValues = [];
        const enc = new api.Encoder();
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        assert.equal(enc.initFile(
            tmpFile.path,
            (...args) => progressCallbackValues.push(args),
        ), 0, enc.getResolvedStateString());

        assert.isTrue(enc.process(encDataAlt, totalSamples), enc.getResolvedStateString());
        assert.isTrue(enc.finish(), enc.getResolvedStateString());

        comparePCM(okData, tmpFile.path, 24);
        assert.equal(progressCallbackValues.length, 41);
    });

    it('encoder should emit streaminfo metadata block', function() {
        let metadataBlock = null;
        const callbacks = generateFlacCallbacks.sync(api.Encoder, tmpFile.path, 'w');
        deferredScope.defer(() => callbacks.close());
        const enc = new api.Encoder();
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        enc.setMetadata([ new api.metadata.VorbisCommentMetadata() ]);
        assert.equal(enc.initStream(
            callbacks.write,
            callbacks.seek,
            callbacks.tell,
            (metadata) => {
                metadataBlock = metadata;
            },
        ), 0, enc.getResolvedStateString());

        assert.isTrue(enc.processInterleaved(encData), enc.getResolvedStateString());
        assert.isTrue(enc.finish(), enc.getResolvedStateString());

        assert.isNotNull(metadataBlock);
        assert.equal(metadataBlock.type, 0);
        assert.equal(metadataBlock.totalSamples, totalSamples);
    });

    it('encoder process should fail if invalid buffer size is sent', function() {
        const enc = new api.Encoder();
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        assert.equal(enc.initFile(
            tmpFile.path,
        ), 0, enc.getResolvedStateString());

        assert.throws(
            () => enc.process([ encDataAlt[0].slice(4), encDataAlt[1] ], totalSamples),
            /^Buffer at position 0 has not enough bytes:/
        );
        assert.isTrue(enc.finish(), enc.getResolvedStateString());
    });

    it('encoder process should fail if invalid number of channels is sent', function() {
        const enc = new api.Encoder();
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        assert.equal(enc.initFile(
            tmpFile.path,
        ), 0, enc.getResolvedStateString());

        assert.throws(
            () => enc.process([ encDataAlt[0] ], totalSamples),
            /^Expected array to have \d+ buffers \(one for each channel\)$/
        );
        assert.isTrue(enc.finish(), enc.getResolvedStateString());
    });

    it('encoder processInterleaved should fail if invalid buffer size is sent', function() {
        const enc = new api.Encoder();
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        assert.equal(enc.initFile(
            tmpFile.path,
        ), 0, enc.getResolvedStateString());

        assert.throws(
            () => enc.processInterleaved(encData.slice(4), totalSamples),
            /^Buffer has not enough bytes:/
        );
        assert.isTrue(enc.finish(), enc.getResolvedStateString());
    });

    it('encoder get other properties work', function() {
        const enc = new api.Encoder();
        enc.bitsPerSample = 24;
        enc.channels = 2;
        enc.setCompressionLevel(9);
        enc.sampleRate = 44100;
        enc.totalSamplesEstimate = totalSamples;
        enc.verify = true;
        assert.equal(enc.initFile(
            tmpFile.path,
        ), 0, enc.getResolvedStateString());

        assert.isTrue(enc.verify);

        assert.equal(enc.channels, 2);
        assert.equal(enc.bitsPerSample, 24);
        assert.equal(enc.sampleRate, 44100);
        assert.equal(enc.blocksize, 4096);
        assert.isTrue(enc.doMidSideStereo);
        assert.isFalse(enc.looseMidSideStereo);
        assert.equal(enc.maxLpcOrder, 12);
        assert.equal(enc.qlpCoeffPrecision, 15);
        assert.isFalse(enc.doQlpCoeffPrecSearch);
        assert.isFalse(enc.doEscapeCoding);
        assert.isFalse(enc.doExhaustiveModelSearch);
        assert.equal(enc.minResidualPartitionOrder, 0);
        assert.equal(enc.maxResidualPartitionOrder, 6);
        assert.equal(enc.riceParameterSearchDist, 0);
        assert.equal(enc.totalSamplesEstimate, totalSamples);

        assert.isTrue(enc.finish(), enc.getResolvedStateString());
    });

});
