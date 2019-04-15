/// <reference path="../lib/index.d.ts" />
const {
    FileDecoder,
    FileEncoder,
    StreamEncoder,
    StreamDecoder,
} = require('../lib/index');
const { assert } = require('chai');
const fs = require('fs');
const path = require('path');
const cp = require('child_process');
const temp = require('temp');

const pathForFile = (...file) => path.join(__dirname, 'data', '*coder', ...file);
const promisifyEvent = (t, event) => new Promise((resolve, reject) => t.on(event, resolve).on('error', reject));
const readFlacUsingCommand = (file) => cp.spawnSync('flac', [ '-d', '-c', file ], { encoding: 'buffer' }).stdout;
const getPCMData = (buffer) => {
    const pos = buffer.indexOf('data', buffer.indexOf('WAVE')) + 4;
    const length = buffer.readUInt32LE(pos);
    return buffer.slice(pos + 4, length !== 0 ? pos + 4 + length : undefined);
};

const comparePCM = (flacFile, bitsPerSample=16) => {
    const okData = getPCMData(fs.readFileSync(pathForFile('loop.wav')));
    const convertedData = Buffer.isBuffer(flacFile) ? flacFile : getPCMData(readFlacUsingCommand(flacFile));

    const sampleSize = bitsPerSample / 8;
    const wavSampleSize = 3;
    if(convertedData.length / sampleSize !== okData.length / wavSampleSize) {
        assert.fail(
            convertedData.length / sampleSize,
            okData.length / wavSampleSize,
            `Length is different: ${convertedData.length / sampleSize} vs ${okData.length / wavSampleSize}`
        );
    }

    for(let i = 0; i < okData.length / wavSampleSize; i++) {
        const a = convertedData.readIntLE(i * sampleSize, sampleSize);
        const b = okData.readIntLE(i * wavSampleSize, wavSampleSize);
        if(a !== b) {
            assert.fail(a, b, `PCM data is different at sample ${i}: ${a} !== ${b}`);
        }
    }
};

describe('encode & decode', function() {

    const totalSamples = 992250 / 3 / 2;
    let tmpFile;
    beforeEach('createTemporaryFiles', function() {
        tmpFile = temp.openSync('flac-bindings.encode-decode');
    });

    afterEach('cleanUpTemporaryFiles', function() {
        temp.cleanupSync();
    });

    it('encode/decode using stream', async function() {
        const dec = new StreamDecoder({ outputAs32: false });
        const enc = new StreamEncoder({ samplerate: 44100, channels: 2, bitsPerSample: 24, compressionLevel: 9, inputAs32: false });
        const input = fs.createReadStream(pathForFile('loop.flac'));
        const output = fs.createWriteStream(tmpFile.path);

        input.pipe(dec);
        dec.pipe(enc);
        enc.pipe(output);
        await promisifyEvent(output, 'close');

        assert.isTrue((await fs.promises.stat(tmpFile.path)).size > 660 * 1000);
        assert.equal(dec._processedSamples, totalSamples);
        assert.equal(enc._processedSamples, totalSamples);
        comparePCM(tmpFile.path, 24);
    });

    it('encode/decode using file', async function() {
        const dec = new FileDecoder({ file: pathForFile('loop.flac'), outputAs32: false });
        const enc = new FileEncoder({ file: tmpFile.path, samplerate: 44100, channels: 2, bitsPerSample: 24, compressionLevel: 9, inputAs32: false });

        dec.pipe(enc);
        await promisifyEvent(enc, 'finish');

        assert.isTrue((await fs.promises.stat(tmpFile.path)).size > 660 * 1000);
        assert.equal(dec._processedSamples, totalSamples);
        assert.equal(enc._processedSamples, totalSamples);
        comparePCM(tmpFile.path, 24);
    });

    it('encode/decode using 32 bit integers', async function() {
        const dec = new FileDecoder({ file: pathForFile('loop.flac'), outputAs32: true });
        const enc = new FileEncoder({ file: tmpFile.path, samplerate: 44100, channels: 2, bitsPerSample: 24, compressionLevel: 9, inputAs32: true });

        dec.pipe(enc);
        await promisifyEvent(enc, 'finish');

        assert.isTrue((await fs.promises.stat(tmpFile.path)).size > 660 * 1000);
        assert.equal(dec._processedSamples, totalSamples);
        assert.equal(enc._processedSamples, totalSamples);
        comparePCM(tmpFile.path, 24);
    });

    it('decode using stream and file-bit output', async function() {
        const input = fs.createReadStream(pathForFile('loop.flac'));
        const dec = new StreamDecoder({ outputAs32: false });
        const chunks = [];

        input.pipe(dec);
        dec.on('data', chunk => chunks.push(chunk));
        await promisifyEvent(dec, 'end');

        const raw = Buffer.concat(chunks);
        assert.equal(raw.length, totalSamples * 3 * 2);
        assert.equal(dec._processedSamples, totalSamples);
        comparePCM(raw, 24);
    });

    it('decode using stream and 32-bit output', async function() {
        const input = fs.createReadStream(pathForFile('loop.flac'));
        const dec = new StreamDecoder({ outputAs32: true });
        const chunks = [];

        input.pipe(dec);
        dec.on('data', chunk => chunks.push(chunk));
        await promisifyEvent(dec, 'end');

        const raw = Buffer.concat(chunks);
        assert.equal(raw.length, totalSamples * 4 * 2);
        assert.equal(dec._processedSamples, totalSamples);
        comparePCM(raw, 32);
    });

    it('decode using file and file-bit output', async function() {
        const dec = new FileDecoder({ outputAs32: false, file: pathForFile('loop.flac') });
        const chunks = [];

        dec.on('data', chunk => chunks.push(chunk));
        await promisifyEvent(dec, 'end');

        const raw = Buffer.concat(chunks);
        assert.equal(raw.length, totalSamples * 3 * 2);
        assert.equal(dec._processedSamples, totalSamples);
        comparePCM(raw, 24);
    });

    it('decode using file and 32-bit output', async function() {
        const dec = new FileDecoder({ outputAs32: true, file: pathForFile('loop.flac') });
        const chunks = [];

        dec.on('data', chunk => chunks.push(chunk));
        await promisifyEvent(dec, 'end');

        const raw = Buffer.concat(chunks);
        assert.equal(raw.length, totalSamples * 4 * 2);
        assert.equal(dec._processedSamples, totalSamples);
        comparePCM(raw, 32);
    });

    it('encode using stream and file-bit input', async function() {
        const output = fs.createWriteStream(tmpFile.path);
        const enc = new StreamEncoder({ samplerate: 44100, channels: 2, bitsPerSample: 24, compressionLevel: 9, inputAs32: false });
        const raw = getPCMData(fs.readFileSync(pathForFile('loop.wav')));

        enc.pipe(output);
        enc.end(raw);
        await promisifyEvent(output, 'close');

        assert.isTrue((await fs.promises.stat(tmpFile.path)).size > 660 * 1000);
        assert.equal(enc._processedSamples, totalSamples);
        comparePCM(tmpFile.path, 24);
    });

    it('encode using stream and 32-bit input', async function() {
        const output = fs.createWriteStream(tmpFile.path);
        const enc = new StreamEncoder({ samplerate: 44100, channels: 2, bitsPerSample: 24, compressionLevel: 9, inputAs32: true });
        const raw = getPCMData(fs.readFileSync(pathForFile('loop.wav')));
        const chunkazo = Buffer.allocUnsafe(totalSamples * 4 * 2);
        for(let i = 0; i < totalSamples * 2; i++) chunkazo.writeInt32LE(raw.readIntLE(i * 3, 3), i * 4);

        enc.pipe(output);
        enc.end(chunkazo);
        await promisifyEvent(output, 'close');

        assert.isTrue((await fs.promises.stat(tmpFile.path)).size > 660 * 1000);
        assert.equal(enc._processedSamples, totalSamples);
        comparePCM(tmpFile.path, 24);
    });

    it('encode using file and file-bit input', async function() {
        const file = tmpFile.path;
        const enc = new FileEncoder({ samplerate: 44100, channels: 2, bitsPerSample: 24, compressionLevel: 9, inputAs32: false, file });
        const raw = getPCMData(fs.readFileSync(pathForFile('loop.wav')));

        enc.end(raw);
        await promisifyEvent(enc, 'finish');

        assert.isTrue((await fs.promises.stat(tmpFile.path)).size > 660 * 1000);
        assert.equal(enc._processedSamples, totalSamples);
        comparePCM(tmpFile.path, 24);
    });

    it('encode using file and 32-bit input', async function() {
        const file = tmpFile.path;
        const enc = new FileEncoder({ samplerate: 44100, channels: 2, bitsPerSample: 24, compressionLevel: 9, inputAs32: true, file });
        const raw = getPCMData(fs.readFileSync(pathForFile('loop.wav')));
        const chunkazo = Buffer.allocUnsafe(totalSamples * 4 * 2);
        for(let i = 0; i < totalSamples * 2; i++) chunkazo.writeInt32LE(raw.readIntLE(i * 3, 3), i * 4);

        enc.end(chunkazo);
        await promisifyEvent(enc, 'finish');

        assert.isTrue((await fs.promises.stat(tmpFile.path)).size > 660 * 1000);
        assert.equal(enc._processedSamples, totalSamples);
        comparePCM(tmpFile.path, 24);
    });

    it('gc should work', function() {
        require('./gc')();
    });

});
