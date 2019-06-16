/* eslint-disable prefer-arrow-callback */
/* global BigInt */
/// <reference path="../lib/index.d.ts" />
const {
    FileDecoder,
    FileEncoder,
    StreamEncoder,
    StreamDecoder,
    api,
} = require('../lib/index');
const { assert } = require('chai');
const fs = require('fs');
const path = require('path');
const cp = require('child_process');
const temp = require('temp');

const totalSamples = 992250 / 3 / 2;

const pathForFile = (...file) => path.join(__dirname, 'data', '*coder', ...file);
const promisifyEvent = (t, event) => new Promise((resolve, reject) => t.on(event, resolve).on('error', reject));
const readFlacUsingCommand = (file) => cp.spawnSync('flac', [ '-d', '-c', file ], { encoding: 'buffer' }).stdout;
const getPCMData = (buffer) => {
    const pos = buffer.indexOf('data', buffer.indexOf('WAVE')) + 4;
    const length = buffer.readUInt32LE(pos);
    return buffer.slice(pos + 4, length !== 0 ? pos + 4 + length : undefined);
};

const okData = getPCMData(fs.readFileSync(pathForFile('loop.wav')));
const comparePCM = (flacFile, bitsPerSample=16) => {
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

const joinIntoInterleaved = (arrayOfArrayOfBuffers) => {
    const finalBuffer = Buffer.allocUnsafe(totalSamples * 4 * 2);
    let sample = 0;
    for(const buffers of arrayOfArrayOfBuffers) {
        for(let i = 0; i < buffers[0].length / 4; i++) {
            finalBuffer.writeInt32LE(buffers[0].readInt32LE(i * 4), sample * 4 * 2);
            finalBuffer.writeInt32LE(buffers[1].readInt32LE(i * 4), sample * 4 * 2 + 4);
            sample++;
        }
    }

    return [ finalBuffer, sample ];
};

let tmpFile;
beforeEach('createTemporaryFiles', function() {
    tmpFile = temp.openSync('flac-bindings.encode-decode');
});

afterEach('cleanUpTemporaryFiles', function() {
    temp.cleanupSync();
});

describe('encode & decode', function() {

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
        await promisifyEvent(output, 'close');

        assert.isTrue((await fs.promises.stat(tmpFile.path)).size > 660 * 1000);
        assert.equal(dec._processedSamples, totalSamples);
        assert.equal(enc._processedSamples, totalSamples);
        comparePCM(tmpFile.path, 24);
    });

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
        await promisifyEvent(enc, 'finish');

        assert.isTrue((await fs.promises.stat(tmpFile.path)).size > 660 * 1000);
        assert.equal(dec._processedSamples, totalSamples);
        assert.equal(enc._processedSamples, totalSamples);
        comparePCM(tmpFile.path, 24);
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
        dec.on('data', (chunk) => chunks.push(chunk));
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
        dec.on('data', (chunk) => chunks.push(chunk));
        await promisifyEvent(dec, 'end');

        const raw = Buffer.concat(chunks);
        assert.equal(raw.length, totalSamples * 4 * 2);
        assert.equal(dec._processedSamples, totalSamples);
        comparePCM(raw, 32);
    });

    it('decode using file and file-bit output', async function() {
        const dec = new FileDecoder({ outputAs32: false, file: pathForFile('loop.flac') });
        const chunks = [];

        dec.on('data', (chunk) => chunks.push(chunk));
        await promisifyEvent(dec, 'end');

        const raw = Buffer.concat(chunks);
        assert.equal(raw.length, totalSamples * 3 * 2);
        assert.equal(dec._processedSamples, totalSamples);
        comparePCM(raw, 24);
    });

    it('decode using file and 32-bit output', async function() {
        const dec = new FileDecoder({ outputAs32: true, file: pathForFile('loop.flac') });
        const chunks = [];

        dec.on('data', (chunk) => chunks.push(chunk));
        await promisifyEvent(dec, 'end');

        const raw = Buffer.concat(chunks);
        assert.equal(raw.length, totalSamples * 4 * 2);
        assert.equal(dec._processedSamples, totalSamples);
        comparePCM(raw, 32);
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
        await promisifyEvent(output, 'close');

        assert.isTrue((await fs.promises.stat(tmpFile.path)).size > 660 * 1000);
        assert.equal(enc._processedSamples, totalSamples);
        comparePCM(tmpFile.path, 24);
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
        await promisifyEvent(output, 'close');

        assert.isTrue((await fs.promises.stat(tmpFile.path)).size > 660 * 1000);
        assert.equal(enc._processedSamples, totalSamples);
        comparePCM(tmpFile.path, 24);
    });

    it('encode using file and file-bit input', async function() {
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
        await promisifyEvent(enc, 'finish');

        assert.isTrue((await fs.promises.stat(tmpFile.path)).size > 660 * 1000);
        assert.equal(enc._processedSamples, totalSamples);
        comparePCM(tmpFile.path, 24);
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
        await promisifyEvent(enc, 'finish');

        assert.isTrue((await fs.promises.stat(tmpFile.path)).size > 660 * 1000);
        assert.equal(enc._processedSamples, totalSamples);
        comparePCM(tmpFile.path, 24);
    });

    it('gc should work', function() {
        require('./helper/gc')();
    });

});

describe('encode & decode: manual version', function() {

    it('decode using stream', async function() {
        const fd = fs.openSync(pathForFile('loop.flac'), 'r');
        const dec = new api.Decoder();
        const allBuffers = [];
        await dec.initStreamAsync(
            (buffer) => ({ bytes: fs.readSync(fd, buffer, 0, buffer.length, null), returnValue: 0 }),
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

        const e = await dec.processUntilEndOfMetadataAsync();
        assert.isTrue(e);
        const f = await dec.processUntilEndOfStreamAsync();
        assert.isTrue(f);

        const [ finalBuffer, samples ] = joinIntoInterleaved(allBuffers);
        assert.equal(samples, totalSamples);
        comparePCM(finalBuffer, 32);
    });

    it('decode with async callbacks using stream', async function() {
        const fh = await fs.promises.open(pathForFile('loop.flac'), 'r');
        const dec = new api.Decoder();
        const allBuffers = [];
        await dec.initStreamAsync(
            async (buffer) => ({ bytes: (await fh.read(buffer, 0, buffer.length, null)).bytesRead, returnValue: 0 }),
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

        const e = await dec.processUntilEndOfMetadataAsync();
        assert.isTrue(e);
        const f = await dec.processUntilEndOfStreamAsync();
        assert.isTrue(f);

        const [ finalBuffer, samples ] = joinIntoInterleaved(allBuffers);
        assert.equal(samples, totalSamples);
        comparePCM(finalBuffer, 32);
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

        const e = await dec.processUntilEndOfMetadataAsync();
        assert.isTrue(e);
        const f = await dec.processUntilEndOfStreamAsync();
        assert.isTrue(f);

        const [ finalBuffer, samples ] = joinIntoInterleaved(allBuffers);
        assert.equal(samples, totalSamples);
        comparePCM(finalBuffer, 32);
    });

    it('encode using stream', async function() {
        const enc = new api.Encoder();
        const fd = fs.openSync(tmpFile.path, 'w');
        enc.setBitsPerSample(24);
        enc.setChannels(2);
        enc.setCompressionLevel(9);
        enc.setSampleRate(44100);
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
        await enc.processInterleavedAsync(chunkazo);
        await enc.finishAsync();

        comparePCM(tmpFile.path, 24);
    });

    it('encode with async callbacks using stream', async function() {
        const enc = new api.Encoder();
        const fh = await fs.promises.open(tmpFile.path, 'w');
        enc.setBitsPerSample(24);
        enc.setChannels(2);
        enc.setCompressionLevel(9);
        enc.setSampleRate(44100);
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
        await enc.processInterleavedAsync(chunkazo);
        await enc.finishAsync();

        comparePCM(tmpFile.path, 24);
    });

    it('encode using file', async function() {
        const enc = new api.Encoder();
        enc.setBitsPerSample(24);
        enc.setChannels(2);
        enc.setCompressionLevel(9);
        enc.setSampleRate(44100);
        await enc.initFileAsync(
            tmpFile.path,
            null
        );

        const chunkazo = Buffer.allocUnsafe(totalSamples * 4 * 2);
        for(let i = 0; i < totalSamples * 2; i++) {
            chunkazo.writeInt32LE(okData.readIntLE(i * 3, 3), i * 4);
        }
        await enc.processInterleavedAsync(chunkazo);
        await enc.finishAsync();

        comparePCM(tmpFile.path, 24);
    });

});
