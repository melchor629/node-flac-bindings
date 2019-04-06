/// <reference path="../lib/index.d.ts" />
const { format } = require('../lib/index').api;
const { assert } = require('chai');

describe('format', function() {

    it('sampleRateIsValid() should work', function() {
        assert.isTrue(format.sampleRateIsValid(96000));
        assert.isFalse(format.sampleRateIsValid(112938129312));
    });

    it('blocksizeIsSubset() should work', function() {
        assert.isTrue(format.blocksizeIsSubset(4410, 44100));
    });

    it('sampleRateIsSubset() should work', function() {
        assert.isTrue(format.sampleRateIsSubset(44100));
        assert.isFalse(format.sampleRateIsSubset(9123812830192));
    });

    it('vorbiscommentEntryNameIsLegal() should work', function() {
        assert.isTrue(format.vorbiscommentEntryIsLegal('KEY=VALUE'));
        assert.isFalse(format.vorbiscommentEntryIsLegal('KEY VALUE'));
    });

    it('vorbiscommentEntryNameIsLegal() should work', function() {
        assert.isTrue(format.vorbiscommentEntryNameIsLegal('KEY'));
        assert.isFalse(format.vorbiscommentEntryNameIsLegal('KEY \n'));
    });

    it('vorbiscommentEntryValueIsLegal() should work', function() {
        assert.isTrue(format.vorbiscommentEntryValueIsLegal('VALUE IS OK'));
    });

});