/* eslint-disable prefer-arrow-callback */
/// <reference path="../../lib/index.d.ts" />
const { StreamInfoMetadata } = require('../../lib/index').api.metadata;
const { MetadataType } = require('../../lib/index').api.format;
const { getStreaminfoAsync } = require('../../lib/index').api.metadata0;
const { assert } = require('chai');
const { pathForFile: { tags: pathForFile } } = require('../helper');

describe('StreamInfoMetadata', function() {
    it('create new object should work', function() {
        new StreamInfoMetadata();
    });

    it('object has the right type', function() {
        const st = new StreamInfoMetadata();

        assert.equal(st.type, MetadataType.STREAMINFO);
    });

    it('set minBlocksize should work', function() {
        const st = new StreamInfoMetadata();

        st.minBlocksize = 1024;

        assert.equal(st.minBlocksize, 1024);
    });

    it('set minBlocksize should throw if value is not a number', function() {
        const st = new StreamInfoMetadata();

        assert.throws(() => st.minBlocksize = null);
    });

    it('set maxBlocksize should work', function() {
        const st = new StreamInfoMetadata();

        st.maxBlocksize = 1024;

        assert.equal(st.maxBlocksize, 1024);
    });

    it('set maxBlocksize should throw if value is not a number', function() {
        const st = new StreamInfoMetadata();

        assert.throws(() => st.maxBlocksize = null);
    });

    it('set minFramesize should work', function() {
        const st = new StreamInfoMetadata();

        st.minFramesize = 1024;

        assert.equal(st.minFramesize, 1024);
    });

    it('set minFramesize should throw if value is not a number', function() {
        const st = new StreamInfoMetadata();

        assert.throws(() => st.minFramesize = null);
    });

    it('set maxFramesize should work', function() {
        const st = new StreamInfoMetadata();

        st.maxFramesize = 1024;

        assert.equal(st.maxFramesize, 1024);
    });

    it('set maxFramesize should throw if value is not a number', function() {
        const st = new StreamInfoMetadata();

        assert.throws(() => st.maxFramesize = null);
    });

    it('set channels should work', function() {
        const st = new StreamInfoMetadata();

        st.channels = 2;

        assert.equal(st.channels, 2);
    });

    it('set channels should throw if value is not a number', function() {
        const st = new StreamInfoMetadata();

        assert.throws(() => st.channels = null);
    });

    it('set bitsPerSample should work', function() {
        const st = new StreamInfoMetadata();

        st.bitsPerSample = 16;

        assert.equal(st.bitsPerSample, 16);
    });

    it('set bitsPerSample should throw if value is not a number', function() {
        const st = new StreamInfoMetadata();

        assert.throws(() => st.bitsPerSample = null);
    });

    it('set sampleRate should work', function() {
        const st = new StreamInfoMetadata();

        st.sampleRate = 44100;

        assert.equal(st.sampleRate, 44100);
    });

    it('set sampleRate should throw if value is not a number', function() {
        const st = new StreamInfoMetadata();

        assert.throws(() => st.sampleRate = null);
    });

    it('set totalSamples should work', function() {
        const st = new StreamInfoMetadata();

        st.totalSamples = 441000n;

        assert.equal(st.totalSamples, 441000n);
    });

    it('set totalSamples should throw if value is not a number', function() {
        const st = new StreamInfoMetadata();

        assert.throws(() => st.totalSamples = null);
    });

    it('set md5sum should work', function() {
        const st = new StreamInfoMetadata();

        st.md5sum = Buffer.allocUnsafe(16);

        assert.isTrue(Buffer.isBuffer(st.md5sum));
    });

    it('set md5sum should throw if value is not a buffer', function() {
        const st = new StreamInfoMetadata();

        assert.throws(() => st.md5sum = 123456789);
    });

    it('set md5sum should throw if value buffer is less than 16 bytes', function() {
        const st = new StreamInfoMetadata();

        assert.throws(() => st.md5sum = Buffer.alloc(10));
    });

    it('is correct from mapping', async function() {
        const st = await getStreaminfoAsync(pathForFile('no.flac'));

        assert.equal(st.bitsPerSample, 16);
        assert.equal(st.channels, 2);
        assert.equal(st.maxBlocksize, 4096);
        assert.equal(st.minBlocksize, 4096);
        assert.deepEqual(st.md5sum, Buffer.from('7ÓË©ó»PLÀ8H_pG', 'ascii'));
        assert.equal(st.maxFramesize, 4214);
        assert.equal(st.minFramesize, 2565);
        assert.equal(st.sampleRate, 44100);
        assert.equal(st.totalSamples, 10651);
    });

    describe('gc', function() {
        it('gc should work', function() {
            require('../helper/gc')();
        });
    });
});
