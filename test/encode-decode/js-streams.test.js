/* eslint-disable prefer-arrow-callback */
/// <reference path="../../lib/index.d.ts" />
const {
    FileDecoder,
    FileEncoder,
    StreamEncoder,
    StreamDecoder,
} = require('../../lib/index');
const { assert } = require('chai');
const events = require('events');
const fs = require('fs');
const temp = require('temp').track();

const { pathForFile: { audio: pathForFile }, comparePCM, getWavAudio, gc } = require('../helper');

const totalSamples = 992250 / 3 / 2;
const okData = getWavAudio('loop.wav');

let tmpFile;
beforeEach('createTemporaryFiles', function() {
    tmpFile = temp.openSync('flac-bindings.encode-decode.js-streams');
});

afterEach('cleanUpTemporaryFiles', function() {
    temp.cleanupSync();
});

describe('encode & decode: js streams', function() {

    describe('Stream', function() {

        it('encode/decode using stream', async function() {
            const dec = new StreamDecoder({ outputAs32: false });
            const enc = new StreamEncoder({
                samplerate: 44100,
                channels: 2,
                bitsPerSample: 24,
                compressionLevel: 9,
                inputAs32: false,
            });
            const input = fs.createReadStream(pathForFile('loop.flac'));
            const output = fs.createWriteStream(tmpFile.path);

            input.pipe(dec);
            dec.pipe(enc);
            enc.pipe(output);
            await events.once(output, 'close');

            assert.isTrue((await fs.promises.stat(tmpFile.path)).size > 660 * 1000);
            assert.equal(dec._processedSamples, totalSamples);
            assert.equal(enc._processedSamples, totalSamples);
            comparePCM(okData, tmpFile.path, 24);
        });

        /*it('encode/decode using stream (ogg)', async function() {
            const dec = new StreamDecoder({ outputAs32: false, isOggStream: true });
            const enc = new StreamEncoder({
                samplerate: 44100,
                channels: 2,
                bitsPerSample: 24,
                compressionLevel: 9,
                inputAs32: false,
                isOggStream: true,
            });
            const input = fs.createReadStream(pathForFile('loop.oga'));
            const output = fs.createWriteStream(tmpFile.path);

            input.pipe(dec);
            dec.pipe(enc);
            enc.pipe(output);
            await events.once(output, 'close');

            assert.isTrue((await fs.promises.stat(tmpFile.path)).size > 660 * 1000);
            assert.equal(dec._processedSamples, totalSamples);
            assert.equal(enc._processedSamples, totalSamples);
            comparePCM(okData, tmpFile.path, 24, true);
        });*/

        it('decode using stream and file-bit output', async function() {
            const input = fs.createReadStream(pathForFile('loop.flac'));
            const dec = new StreamDecoder({ outputAs32: false });
            const chunks = [];

            input.pipe(dec);
            dec.on('data', (chunk) => chunks.push(chunk));
            await events.once(dec, 'end');

            const raw = Buffer.concat(chunks);
            assert.equal(raw.length, totalSamples * 3 * 2);
            assert.equal(dec._processedSamples, totalSamples);
            comparePCM(okData, raw, 24);
        });

        it('decode using stream and 32-bit output', async function() {
            const input = fs.createReadStream(pathForFile('loop.flac'));
            const dec = new StreamDecoder({ outputAs32: true });
            const chunks = [];

            input.pipe(dec);
            dec.on('data', (chunk) => chunks.push(chunk));
            await events.once(dec, 'end');

            const raw = Buffer.concat(chunks);
            assert.equal(raw.length, totalSamples * 4 * 2);
            assert.equal(dec._processedSamples, totalSamples);
            comparePCM(okData, raw, 32);
        });

        it('encode using stream and file-bit input', async function() {
            const output = fs.createWriteStream(tmpFile.path);
            const enc = new StreamEncoder({
                samplerate: 44100,
                channels: 2,
                bitsPerSample: 24,
                compressionLevel: 9,
                inputAs32: false,
            });
            const raw = okData;

            enc.pipe(output);
            enc.end(raw);
            await events.once(output, 'close');

            assert.isTrue((await fs.promises.stat(tmpFile.path)).size > 660 * 1000);
            assert.equal(enc._processedSamples, totalSamples);
            comparePCM(okData, tmpFile.path, 24);
        });

        it('encode using stream and 32-bit input', async function() {
            const output = fs.createWriteStream(tmpFile.path);
            const enc = new StreamEncoder({
                samplerate: 44100,
                channels: 2,
                bitsPerSample: 24,
                compressionLevel: 9,
                inputAs32: true,
            });
            const raw = okData;
            const chunkazo = Buffer.allocUnsafe(totalSamples * 4 * 2);
            for(let i = 0; i < totalSamples * 2; i++) {
                chunkazo.writeInt32LE(raw.readIntLE(i * 3, 3), i * 4);
            }

            enc.pipe(output);
            enc.end(chunkazo);
            await events.once(output, 'close');

            assert.isTrue((await fs.promises.stat(tmpFile.path)).size > 660 * 1000);
            assert.equal(enc._processedSamples, totalSamples);
            comparePCM(okData, tmpFile.path, 24);
        });

        it('stream decoder should read properties', async function() {
            const input = fs.createReadStream(pathForFile('loop.flac'));
            const dec = new StreamDecoder({ outputAs32: true });

            input.pipe(dec);
            await events.once(dec, 'data');

            assert.equal(dec.getBitsPerSample(), 24);
            assert.equal(dec.getChannels(), 2);
            assert.equal(dec.getChannelAssignment(), 3);
            assert.equal(dec.getTotalSamples(), totalSamples);

            dec.on('data', () => undefined);
            await events.once(dec, 'end');
        });

        it('stream decoder should emit metadata when required', async function() {
            const input = fs.createReadStream(pathForFile('loop.flac'));
            const dec = new StreamDecoder({ outputAs32: true, metadata: true });
            const metadataBlocks = [];

            dec.on('metadata', (metadata) => metadataBlocks.push(metadata));
            input.pipe(dec);
            dec.on('data', () => undefined);
            await events.once(dec, 'end');

            assert.isNotEmpty(metadataBlocks);
            assert.equal(metadataBlocks.length, 4);
        });

        it('encode using ogg (stream)', async function() {
            const dec = new StreamDecoder({ outputAs32: false });
            const enc = new StreamEncoder({
                isOggStream: true,
                oggSerialNumber: 0x123456,
                samplerate: 44100,
                channels: 2,
                bitsPerSample: 24,
                compressionLevel: 9,
                inputAs32: false,
            });
            const input = fs.createReadStream(pathForFile('loop.flac'));
            const output = fs.createWriteStream(tmpFile.path);

            input.pipe(dec);
            dec.pipe(enc);
            enc.pipe(output);
            await events.once(output, 'close');

            assert.isTrue((await fs.promises.stat(tmpFile.path)).size > 660 * 1000);
            assert.equal(dec._processedSamples, totalSamples);
            assert.equal(enc._processedSamples, totalSamples);
            comparePCM(okData, tmpFile.path, 24, true);
        });

        it('encoder options should not throw exception', async function() {
            const enc = new StreamEncoder({
                bitsPerSample: 16,
                channels: 2,
                samplerate: 48000,
                apodization: 'tukey(0.5);partial_tukey(2);punchout_tukey(3)',
                blocksize: 1024,
                doExhaustiveModelSearch: false,
                doMidSideStereo: true,
                doQlpCoeffPrecSearch: false,
                looseMidSideStereo: false,
                maxLpcOrder: 12,
                maxResidualPartitionOrder: 6,
                minResidualPartitionOrder: 0,
                qlpCoeffPrecision: 0,
                totalSamplesEstimate: 48000,
                metadata: [],
            });

            enc.write(Buffer.alloc(1000 * 2 * 2));
            enc.end();
            enc.on('data', () => undefined);
            await events.once(enc, 'end');
        });

    });

    describe('File', function() {

        it('encode/decode using file', async function() {
            const dec = new FileDecoder({ file: pathForFile('loop.flac'), outputAs32: false });
            const enc = new FileEncoder({
                file: tmpFile.path,
                samplerate: 44100,
                channels: 2,
                bitsPerSample: 24,
                compressionLevel: 9,
                inputAs32: false,
            });

            dec.pipe(enc);
            await events.once(enc, 'finish');

            assert.isTrue((await fs.promises.stat(tmpFile.path)).size > 660 * 1000);
            assert.equal(dec._processedSamples, totalSamples);
            assert.equal(enc._processedSamples, totalSamples);
            comparePCM(okData, tmpFile.path, 24);
        });

        it('encode/decode using 32 bit integers', async function() {
            const dec = new FileDecoder({ file: pathForFile('loop.flac'), outputAs32: true });
            const enc = new FileEncoder({
                file: tmpFile.path,
                samplerate: 44100,
                channels: 2,
                bitsPerSample: 24,
                compressionLevel: 9,
                inputAs32: true,
            });

            dec.pipe(enc);
            await events.once(enc, 'finish');

            assert.isTrue((await fs.promises.stat(tmpFile.path)).size > 660 * 1000);
            assert.equal(dec._processedSamples, totalSamples);
            assert.equal(enc._processedSamples, totalSamples);
            comparePCM(okData, tmpFile.path, 24);
        });

        it('decode using file and file-bit output', async function() {
            const dec = new FileDecoder({ outputAs32: false, file: pathForFile('loop.flac') });
            const chunks = [];

            dec.on('data', (chunk) => chunks.push(chunk));
            await events.once(dec, 'end');

            const raw = Buffer.concat(chunks);
            assert.equal(raw.length, totalSamples * 3 * 2);
            assert.equal(dec._processedSamples, totalSamples);
            comparePCM(okData, raw, 24);
        });

        it('decode using file and 32-bit output', async function() {
            const dec = new FileDecoder({ outputAs32: true, file: pathForFile('loop.flac') });
            const chunks = [];

            dec.on('data', (chunk) => chunks.push(chunk));
            await events.once(dec, 'end');

            const raw = Buffer.concat(chunks);
            assert.equal(raw.length, totalSamples * 4 * 2);
            assert.equal(dec._processedSamples, totalSamples);
            comparePCM(okData, raw, 32);
        });

        it('encode using file and 24-bit input', async function() {
            const file = tmpFile.path;
            const enc = new FileEncoder({
                samplerate: 44100,
                channels: 2,
                bitsPerSample: 24,
                compressionLevel: 9,
                inputAs32: false,
                file,
            });
            const raw = okData;

            enc.end(raw);
            await events.once(enc, 'finish');

            assert.isTrue((await fs.promises.stat(tmpFile.path)).size > 660 * 1000);
            assert.equal(enc._processedSamples, totalSamples);
            comparePCM(okData, tmpFile.path, 24);
        });

        it('encode using file and 32-bit input', async function() {
            const file = tmpFile.path;
            const enc = new FileEncoder({
                samplerate: 44100,
                channels: 2,
                bitsPerSample: 24,
                compressionLevel: 9,
                inputAs32: true,
                file,
            });
            const raw = okData;
            const chunkazo = Buffer.allocUnsafe(totalSamples * 4 * 2);
            for(let i = 0; i < totalSamples * 2; i++) {
                chunkazo.writeInt32LE(raw.readIntLE(i * 3, 3), i * 4);
            }

            enc.end(chunkazo);
            await events.once(enc, 'finish');

            assert.isTrue((await fs.promises.stat(tmpFile.path)).size > 660 * 1000);
            assert.equal(enc._processedSamples, totalSamples);
            comparePCM(okData, tmpFile.path, 24);
        });

        it('file decoder should read properties', async function() {
            const dec = new FileDecoder({ outputAs32: true, file: pathForFile('loop.flac') });

            await events.once(dec, 'data');

            assert.equal(dec.getBitsPerSample(), 24);
            assert.equal(dec.getChannels(), 2);
            assert.equal(dec.getChannelAssignment(), 3);
            assert.equal(dec.getTotalSamples(), totalSamples);

            dec.on('data', () => undefined);
            await events.once(dec, 'end');
        });

        it('file decoder should emit metadata when required', async function() {
            const dec = new FileDecoder({ outputAs32: true, metadata: true, file: pathForFile('loop.flac') });
            const metadataBlocks = [];

            dec.on('metadata', (metadata) => metadataBlocks.push(metadata));
            dec.on('data', () => undefined);
            await events.once(dec, 'end');

            assert.isNotEmpty(metadataBlocks);
            assert.equal(metadataBlocks.length, 4);
        });

        it('file decoder should fail if file does not exist', async function() {
            const dec = new FileDecoder({ file: pathForFile('does not exist.flac') });

            dec.on('data', () => undefined);
            await assert.throwsAsync(
                () => events.once(dec, 'data'),
                'Could not initialize decoder: ERROR_OPENING_FILE',
            );
        });

        it('file encoder should fail if file cannot write', async function() {
            const enc = new FileEncoder({
                file: pathForFile('does/not/exist.flac'),
                channels: 1,
                bitsPerSample: 16,
                samplerate: 44100,
                totalSamplesEstimate: 44100,
            });

            enc.write(Buffer.alloc(1000 * 2));
            await assert.throwsAsync(() => events.once(enc, 'data'), 'Could not initialize encoder: IO_ERROR');
        });

        it('encode using ogg (file)', async function() {
            const dec = new FileDecoder({ file: pathForFile('loop.flac'), outputAs32: false });
            const enc = new FileEncoder({
                file: tmpFile.path,
                isOggStream: true,
                oggSerialNumber: 0x6543321,
                samplerate: 44100,
                channels: 2,
                bitsPerSample: 24,
                compressionLevel: 9,
                inputAs32: false,
            });

            dec.pipe(enc);
            await events.once(enc, 'finish');

            assert.isTrue((await fs.promises.stat(tmpFile.path)).size > 660 * 1000);
            assert.equal(dec._processedSamples, totalSamples);
            assert.equal(enc._processedSamples, totalSamples);
            comparePCM(okData, tmpFile.path, 24, true);
        });

    });

    it('gc should work', function() {
        gc();
    });

});
