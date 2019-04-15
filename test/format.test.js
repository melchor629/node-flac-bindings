/// <reference path="../lib/index.d.ts" />
const { format } = require('../lib/index').api;
const { assert } = require('chai');

describe('format', function() {

    it('sampleRateIsValid() should work', function() {
        assert.isTrue(format.sampleRateIsValid(96000));
        assert.isFalse(format.sampleRateIsValid(112938129312n));
    });

    it('sampleRateIsValid() should return false if the argument is not number', function() {
        assert.isFalse(format.sampleRateIsValid('p'));
        assert.isFalse(format.sampleRateIsValid(Infinity));
        assert.isFalse(format.sampleRateIsValid(NaN));
    });

    it('blocksizeIsSubset() should work', function() {
        assert.isTrue(format.blocksizeIsSubset(4410, 44100));
    });

    it('blocksizeIsSubset() should return false if any of the arguments is not number', function() {
        assert.isFalse(format.blocksizeIsSubset({}, 44100));
        assert.isFalse(format.blocksizeIsSubset(4410, []));
    });

    it('sampleRateIsSubset() should work', function() {
        assert.isTrue(format.sampleRateIsSubset(44100));
        assert.isFalse(format.sampleRateIsSubset(9123812830192n));
    });

    it('sampleRateIsSubset() should return false if the argument is not number', function() {
        assert.isFalse(format.sampleRateIsSubset(Promise));
    });

    it('vorbiscommentEntryIsLegal() should work', function() {
        assert.isTrue(format.vorbiscommentEntryIsLegal('KEY=VALUE'));
        assert.isFalse(format.vorbiscommentEntryIsLegal('KEY VALUE'));
    });

    it('vorbiscommentEntryIsLegal() should return false if the argument is not string', function() {
        assert.isFalse(format.vorbiscommentEntryIsLegal(new Map()));
    });

    it('vorbiscommentEntryNameIsLegal() should work', function() {
        assert.isTrue(format.vorbiscommentEntryNameIsLegal('KEY'));
        assert.isFalse(format.vorbiscommentEntryNameIsLegal('KEY \n'));
    });

    it('vorbiscommentEntryNameIsLegal() should return false if the argument is not string', function() {
        assert.isFalse(format.vorbiscommentEntryNameIsLegal(String));
    });

    it('vorbiscommentEntryValueIsLegal() should work', function() {
        assert.isTrue(format.vorbiscommentEntryValueIsLegal('VALUE IS OK'));
    });

    it('vorbiscommentEntryValueIsLegal() should return false if the argument is not string', function() {
        assert.isFalse(format.vorbiscommentEntryValueIsLegal(Buffer));
    });

});