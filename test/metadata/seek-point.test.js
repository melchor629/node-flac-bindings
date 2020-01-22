/* eslint-disable prefer-arrow-callback */
/// <reference path="../../lib/index.d.ts" />
const { SeekPoint } = require('../../lib/index').api.metadata;
const { assert } = require('chai');

describe('SeekPoint', function() {
    it('create new object should work', function() {
        new SeekPoint();
    });

    it('create with no arguments should set values to 0', function() {
        const sp = new SeekPoint();

        assert.equal(sp.sampleNumber, 0);
        assert.equal(sp.streamOffset, 0);
        assert.equal(sp.frameSamples, 0);
    });

    it('create with one argument should modify the sampleNumber', function() {
        const sp = new SeekPoint(1n);

        assert.equal(sp.sampleNumber, 1);
        assert.equal(sp.streamOffset, 0);
        assert.equal(sp.frameSamples, 0);
    });

    it('create with two arguments should modify the sampleNumber and the streamOffset', function() {
        const sp = new SeekPoint(1n, 22n);

        assert.equal(sp.sampleNumber, 1);
        assert.equal(sp.streamOffset, 22);
        assert.equal(sp.frameSamples, 0);
    });

    it('create with three arguments should modify the three attributes', function() {
        const sp = new SeekPoint(1n, 22n, 333n);

        assert.equal(sp.sampleNumber, 1);
        assert.equal(sp.streamOffset, 22);
        assert.equal(sp.frameSamples, 333);
    });

    it('set sampleNumber should work', function() {
        const sp = new SeekPoint();

        sp.sampleNumber = 1;

        assert.equal(sp.sampleNumber, 1);
    });

    it('set sampleNumber should throw if not a number', function() {
        const sp = new SeekPoint();

        assert.throws(() => sp.sampleNumber = null);
    });

    it('set streamOffset should work', function() {
        const sp = new SeekPoint();

        sp.streamOffset = 2;

        assert.equal(sp.streamOffset, 2);
    });

    it('set streamOffset should throw if not a number', function() {
        const sp = new SeekPoint();

        assert.throws(() => sp.streamOffset = null);
    });

    it('set frameSamples should work', function() {
        const sp = new SeekPoint();

        sp.frameSamples = 3;

        assert.equal(sp.frameSamples, 3);
    });

    it('set frameSamples should throw if not a number', function() {
        const sp = new SeekPoint();

        assert.throws(() => sp.frameSamples = null);
    });

    describe('gc', function() {
        it('gc should work', function() {
            require('../helper/gc')();
        });
    });
});
