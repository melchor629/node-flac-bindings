/* eslint-disable prefer-arrow-callback */
/// <reference path="../lib/index.d.ts" />
const { format, metadata } = require('../lib/index').api;
const { assert } = require('chai');

describe('format', function() {

    it('FLAC__VERSION_STRING is defined', function() {
        assert.isString(format.FLAC__VERSION_STRING);
    });

    it('FLAC__VENDOR_STRING is defined', function() {
        assert.isString(format.FLAC__VENDOR_STRING);
    });

    it('sampleRateIsValid() should work', function() {
        assert.isTrue(format.sampleRateIsValid(96000));
        assert.isFalse(format.sampleRateIsValid(BigInt(112938129312)));
    });

    it('sampleRateIsValid() should throw if the argument is not number', function() {
        assert.throws(() => format.sampleRateIsValid('p'), /Expected p to be number/);
        assert.isFalse(format.sampleRateIsValid(Infinity));
        assert.isFalse(format.sampleRateIsValid(NaN));
    });

    it('blocksizeIsSubset() should work', function() {
        assert.isTrue(format.blocksizeIsSubset(4410, 44100));
    });

    it('blocksizeIsSubset() should throw if any of the arguments is not number', function() {
        assert.throws(() => format.blocksizeIsSubset({}, 44100), /Expected .+? to be number/);
        assert.throws(() => format.blocksizeIsSubset(4410, []), /Expected {2}to be number/);
    });

    it('sampleRateIsSubset() should work', function() {
        assert.isTrue(format.sampleRateIsSubset(44100));
        assert.isFalse(format.sampleRateIsSubset(BigInt(9123812830192)));
    });

    it('sampleRateIsSubset() should throw if the argument is not number', function() {
        assert.throws(() => format.sampleRateIsSubset(Promise), /Expected .+? to be number/);
    });

    it('vorbiscommentEntryIsLegal() should work', function() {
        assert.isTrue(format.vorbiscommentEntryIsLegal('KEY=VALUE'));
        assert.isFalse(format.vorbiscommentEntryIsLegal('KEY VALUE'));
    });

    it('vorbiscommentEntryIsLegal() should throw if the argument is not string', function() {
        assert.throws(() => format.vorbiscommentEntryIsLegal(new Map()), /Expected .+? to be string/);
    });

    it('vorbiscommentEntryNameIsLegal() should work', function() {
        assert.isTrue(format.vorbiscommentEntryNameIsLegal('KEY'));
        assert.isFalse(format.vorbiscommentEntryNameIsLegal('KEY \n'));
    });

    it('vorbiscommentEntryNameIsLegal() should throw if the argument is not string', function() {
        assert.throws(() => format.vorbiscommentEntryNameIsLegal(String), /Expected .+? to be string/);
    });

    it('vorbiscommentEntryValueIsLegal() should work', function() {
        assert.isTrue(format.vorbiscommentEntryValueIsLegal('VALUE IS OK'));
    });

    it('vorbiscommentEntryValueIsLegal() should throw if the argument is not string', function() {
        assert.throws(() => format.vorbiscommentEntryValueIsLegal(Buffer), / to be string$/);
    });

    it('seektableIsLegal() should work', function() {
        assert.isTrue(format.seektableIsLegal(new metadata.SeekTableMetadata()));
    });

    it('seektableIsLegal() with value not SeekTableMetadata should throw', function() {
        assert.throws(() => format.seektableIsLegal(new metadata.ApplicationMetadata()), /is not of type SeekTable/);
    });

    it('seektableSort() should work', function() {
        assert.equal(format.seektableSort(new metadata.SeekTableMetadata()), 0);
    });

    it('seektableSort() with value not SeekTableMetadata should throw', function() {
        assert.throws(() => format.seektableSort(new metadata.ApplicationMetadata()), /is not of type SeekTable/);
    });

    it('cuesheetIsLegal() should work', function() {
        const cuesheet = new metadata.CueSheetMetadata();
        assert.isString(format.cuesheetIsLegal(cuesheet));
    });

    it('cuesheetIsLegal() with value not CueSheetMetadata should throw', function() {
        assert.throws(() => format.cuesheetIsLegal(new metadata.ApplicationMetadata()), /is not of type CueSheet/);
    });

    it('pictureIsLegal() should work', function() {
        assert.isNull(format.pictureIsLegal(new metadata.PictureMetadata()));
    });

    it('pictureIsLegal() with value not PictureMetadata should throw', function() {
        assert.throws(() => format.pictureIsLegal(new metadata.ApplicationMetadata()), /is not of type Picture/);
    });

    [
        'MetadataType',
        'EntropyCodingMethodType',
        'SubframeType',
        'ChannelAssignment',
        'FrameNumberType',
        'PictureType',
    ].forEach((t) => {
        it(`${t} should be an object`, function() {
            assert.isObject(format[t]);
        });

        it(`${t}String should be an object`, function() {
            assert.isObject(format[`${t}String`]);
        });

        it(`${t} and ${t}String should contain the same values`, function() {
            assert.deepEqual(Object.values(format[t]).map((v) => v.toString()), Object.keys(format[`${t}String`]));
            assert.deepEqual(Object.values(format[`${t}String`]), Object.keys(format[t]));
        });
    });

});
