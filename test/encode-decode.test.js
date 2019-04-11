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
    const convertedData = getPCMData(readFlacUsingCommand(flacFile));

    if(convertedData.length !== okData.length) {
        assert.fail(convertedData.length, okData.length, `Length is different: ${convertedData.length} vs ${okData.length}`);
    }

    const sampleSize = bitsPerSample / 8;
    for(let i = 0; i < okData.length / sampleSize; i++) {
        const a = convertedData.readUIntLE(i * sampleSize, sampleSize);
        const b = okData.readUIntLE(i * sampleSize, sampleSize);
        if(a !== b) {
            assert.fail(a, b, `PCM data is different at position ${i * 4}: ${a} !== ${b}`);
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

    it('gc should work', function() {
        require('./gc')();
    });

});
