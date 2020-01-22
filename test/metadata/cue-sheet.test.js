/* eslint-disable prefer-arrow-callback */
/// <reference path="../../lib/index.d.ts" />
const { CueSheetMetadata, CueSheetIndex, CueSheetTrack } = require('../../lib/index').api.metadata;
const { MetadataType } = require('../../lib/index').api.format;
const { getCuesheet } = require('../../lib/index').api.metadata0;
const { assert } = require('chai');
const path = require('path');

const pathForFile = (...file) => path.join(__dirname, '..', 'data', 'tags', ...file);

describe('CueSheetMetadata', function() {

    it('create new object should work', function() {
        new CueSheetMetadata();
    });

    it('the object should have the right type', function() {
        const cs = new CueSheetMetadata();
        assert.equal(cs.type, MetadataType.CUESHEET);
    });

    it('mediaCatalogNumber should store string with less than 129 bytes', function() {
        const cst = new CueSheetMetadata();

        cst.mediaCatalogNumber = '31A9-F6AB-979D-505';

        assert.equal(cst.mediaCatalogNumber, '31A9-F6AB-979D-505');
    });

    it('mediaCatalogNumber should throw if the string has more then 128 bytes', function() {
        const cst = new CueSheetMetadata();

        assert.throws(() => {
            cst.mediaCatalogNumber = 'this is a very long string that you should not care about because it is ' +
                'here only for the reason to throw an exception 🗿🤔🙃🤨';
        });
    });

    it('leadIn should work with numbers', function() {
        const cst = new CueSheetMetadata();

        cst.leadIn = 1234567890;

        assert.equal(cst.leadIn, 1234567890);
    });

    it('leadIn should work with bigints', function() {
        const cst = new CueSheetMetadata();

        cst.leadIn = 12345678900987654321n;

        assert.equal(cst.leadIn, 12345678900987654321n);
    });

    it('leadIn should throw if not a number nor bigint', function() {
        const cst = new CueSheetMetadata();

        assert.throw(() => cst.leadIn = 'sad');
    });

    it('iterator should contain the expected tracks', function() {
        const cs = getCuesheet(pathForFile('vc-cs.flac'));

        assert.isNotFalse(cs, 'The file vc-cs.flac must exist');
        const tracks = Array.from(cs);

        assert.equal(tracks.length, 2);
        assert.equal(tracks[0].offset, 0n);
        assert.equal(tracks[0].number, 1);
        assert.equal(tracks[0].isrc, '');
        assert.equal(tracks[0].type, 0);
        assert.equal(tracks[0].preEmphasis, false);
        assert.equal(tracks[0].count, 2);
        assert.equal(tracks[1].offset, 441000n);
        assert.equal(tracks[1].number, 170);
        assert.equal(tracks[1].isrc, '');
        assert.equal(tracks[1].type, 0);
        assert.equal(tracks[1].preEmphasis, false);
        assert.equal(tracks[1].count, 0);
    });

    it('isLegal() should return a string when error', function() {
        assert.isString((new CueSheetMetadata()).isLegal());
    });

    it('isLegal() should return a true when ok', function() {
        const cs = new CueSheetMetadata();
        cs.insertBlankTrack(0);
        cs.trackInsertIndex(0, 0, new CueSheetIndex(0, 170));
        assert.isString(cs.isLegal());
    });

    it('calculateCddbId() should throw if the CueSheet is not a CD', function() {
        assert.throws(() => (new CueSheetMetadata()).calculateCddbId());
    });

    it('calculateCddbId() should return a number when the CueSheet is a CD', function() {
        const cs = new CueSheetMetadata();
        cs.isCd = true;
        assert.isNumber(cs.calculateCddbId());
    });

    describe('track operations', function() {

        it('insertBlankTrack() should insert a new track', function() {
            const cs = new CueSheetMetadata();

            assert.isTrue(cs.insertBlankTrack(0));

            assert.equal(cs.count, 1);
        });

        it('insertBlankTrack() should fail if the index is invalid', function() {
            const cs = new CueSheetMetadata();

            assert.throws(() => cs.insertBlankTrack(1), /Invalid track position/);
            assert.throws(() => cs.insertBlankTrack(-1), /Number -1 is negative/);
            assert.throws(() => cs.insertBlankTrack(null), /Expected null to be number/);
        });

        it('insertTrack() should insert the track', function() {
            const cs = new CueSheetMetadata();
            const cst = new CueSheetTrack();

            assert.isTrue(cs.insertTrack(0, cst));

            assert.equal(cs.count, 1);
        });

        it('insertTrack() should make a copy', function() {
            const cs = new CueSheetMetadata();
            const cst = new CueSheetTrack();

            assert.isTrue(cs.insertTrack(0, cst));
            cst.offset = 123n;

            const tracks = Array.from(cs);
            assert.equal(tracks.length, 1);
            assert.notEqual(tracks[0].offset, cst.offset);
        });

        it('insertTrack() should throw if the index is invalid', function() {
            const cs = new CueSheetMetadata();
            const cst = new CueSheetTrack();

            assert.throws(() => cs.insertTrack(-1, cst), /Number -1 is negative/);
            assert.throws(() => cs.insertTrack(11, cst), /Invalid track position/);
            assert.throws(() => cs.insertTrack(null, cst), /Expected null to be number/);
        });

        it('insertTrack() should throw if the track is not a CueSheetTrack', function() {
            const cs = new CueSheetMetadata();

            assert.throws(() => cs.insertTrack(0, null), /Expected null to be object/);
        });

        it('setTrack() should replace the track', function() {
            const cs = new CueSheetMetadata();
            const cst = new CueSheetTrack();
            cst.offset = 123n;

            assert.isTrue(cs.insertBlankTrack(0));
            assert.isTrue(cs.setTrack(0, cst));

            const tracks = Array.from(cs);
            assert.equal(tracks.length, 1);
            assert.equal(tracks[0].offset, 123n);
        });

        it('setTrack() should throw if the index is invalid', function() {
            const cs = new CueSheetMetadata();
            const cst = new CueSheetTrack();

            assert.throws(() => cs.setTrack(0, cst), /Invalid track position/);
            assert.isTrue(cs.insertBlankTrack(0));
            assert.throws(() => cs.setTrack(1, cst), /Invalid track position/);
            assert.throws(() => cs.setTrack(-1, cst), /Number -1 is negative/);
            assert.throws(() => cs.setTrack(null, cst), /Expected null to be number/);
        });

        it('setTrack() should throw if the track is not CueSheetTrack', function() {
            const cs = new CueSheetMetadata();

            assert.isTrue(cs.insertBlankTrack(0));
            assert.throws(() => cs.setTrack(0, null), /Expected null to be object/);
        });

        it('deleteTrack() should remove the track', function() {
            const cs = new CueSheetMetadata();
            assert.isTrue(cs.insertBlankTrack(0));

            assert.isTrue(cs.deleteTrack(0));

            assert.equal(cs.count, 0);
        });

        it('deleteTrack() should throw if the index is invalid', function() {
            const cs = new CueSheetMetadata();

            assert.throws(() => cs.deleteTrack(0), /Invalid track position/);
            assert.isTrue(cs.insertBlankTrack(0));
            assert.throws(() => cs.deleteTrack(-1), /Number -1 is negative/);
            assert.throws(() => cs.deleteTrack(11), /Invalid track position/);
            assert.throws(() => cs.deleteTrack(null), /Expected null to be number/);
        });

        it('resizeTracks() should work', function() {
            const cs = new CueSheetMetadata();

            assert.isTrue(cs.resizeTracks(10));
            assert.equal(cs.count, 10);
            assert.isTrue(cs.resizeTracks(1));
            assert.equal(cs.count, 1);
        });

        it('resizeTracks() should throw if size is not a number', function() {
            const cs = new CueSheetMetadata();

            assert.throws(() => cs.resizeTracks('1'), /Expected 1 to be number/);
        });

    });

    describe('indices operations', function() {

        it('trackResizeIndices() should work if track index is valid', function() {
            const cs = new CueSheetMetadata();
            cs.insertBlankTrack(0);

            assert.isTrue(cs.trackResizeIndices(0, 10));
            assert.equal(Array.from(cs)[0].count, 10);
            assert.isTrue(cs.trackResizeIndices(0, 1));
            assert.equal(Array.from(cs)[0].count, 1);
        });

        it('trackResizeIndices() should throw if the index is invalid', function() {
            const cs = new CueSheetMetadata();

            assert.throws(() => cs.trackResizeIndices(0, 1), /Invalid track position/);
            assert.isTrue(cs.insertBlankTrack(0));
            assert.throws(() => cs.trackResizeIndices(11, 1), /Invalid track position/);
            assert.throws(() => cs.trackResizeIndices(-1, 1), /Number -1 is negative/);
        });

        it('trackResizeIndices() should throw if the track number is not a number', function() {
            const cs = new CueSheetMetadata();

            assert.throws(() => cs.trackResizeIndices({}, 1), /Expected .+? to be number/);
        });

        it('trackResizeIndices() should throw if the indices size is not a number', function() {
            const cs = new CueSheetMetadata();

            assert.throws(() => cs.trackResizeIndices(0, {}), /Expected .+? to be number/);
        });

        it('trackInsertIndex() should work if the indices are valid', function() {
            const cs = new CueSheetMetadata();
            cs.insertBlankTrack(0);

            assert.isTrue(cs.trackInsertIndex(0, 0, new CueSheetIndex(1n, 0)));

            assert.equal(Array.from(cs)[0].count, 1);
        });

        it('trackInsertIndex() should throw if the track index is invalid', function() {
            const cs = new CueSheetMetadata();
            const csi = new CueSheetIndex(1n, 2);

            assert.throws(() => cs.trackInsertIndex(0, 0, csi), /Invalid track position/);
            assert.isTrue(cs.insertBlankTrack(0));
            assert.throws(() => cs.trackInsertIndex(11, 0, csi), /Invalid track position/);
            assert.throws(() => cs.trackInsertIndex(-1, 0, csi), /Number -1 is negative/);
        });

        it('trackInsertIndex() should throw if the index index is invalid', function() {
            const cs = new CueSheetMetadata();
            const csi = new CueSheetIndex(1n, 2);
            cs.insertBlankTrack(0);

            assert.throws(() => cs.trackInsertIndex(0, 99, csi), /Invalid index position/);
            assert.throws(() => cs.trackInsertIndex(0, -1, csi), /Number -1 is negative/);
        });

        it('trackInsertIndex() should throw if the track number is not a number', function() {
            const cs = new CueSheetMetadata();

            assert.throws(() => cs.trackInsertIndex(null, 99, null));
        });

        it('trackInsertIndex() should throw if the index number is not a number', function() {
            const cs = new CueSheetMetadata();

            assert.throws(() => cs.trackInsertIndex(0, null, null));
        });

        it('trackInsertIndex() should throw if the index object is not CueSheetIndex', function() {
            const cs = new CueSheetMetadata();

            assert.throws(() => cs.trackInsertIndex(0, 1, null));
        });

        it('trackInsertBlankIndex() should work if the indices are valid', function() {
            const cs = new CueSheetMetadata();
            cs.insertBlankTrack(0);

            assert.isTrue(cs.trackInsertBlankIndex(0, 0));

            assert.equal(Array.from(cs)[0].count, 1);
        });

        it('trackInsertBlankIndex() should throw if the track index is invalid', function() {
            const cs = new CueSheetMetadata();

            assert.throws(() => cs.trackInsertBlankIndex(0, 0), /Invalid track position/);
            assert.isTrue(cs.insertBlankTrack(0));
            assert.throws(() => cs.trackInsertBlankIndex(11, 0), /Invalid track position/);
            assert.throws(() => cs.trackInsertBlankIndex(-1, 0), /Number -1 is negative/);
        });

        it('trackInsertBlankIndex() should throw if the index index is invalid', function() {
            const cs = new CueSheetMetadata();
            cs.insertBlankTrack(0);

            assert.throws(() => cs.trackInsertBlankIndex(0, 99), /Invalid index position/);
            assert.throws(() => cs.trackInsertBlankIndex(0, -1), /Number -1 is negative/);
        });

        it('trackInsertBlankIndex() should throw if the track index is not a number', function() {
            const cs = new CueSheetMetadata();

            assert.throws(() => cs.trackInsertBlankIndex(null), /Expected .+? to be number/);
        });

        it('trackInsertBlankIndex() should throw if the index index is not a number', function() {
            const cs = new CueSheetMetadata();

            assert.throws(() => cs.trackInsertBlankIndex(0, null), /Expected .+? to be number/);
        });

        it('trackDeleteIndex() should work if the indices are valid', function() {
            const cs = new CueSheetMetadata();
            cs.insertBlankTrack(0);
            cs.trackInsertBlankIndex(0, 0);

            assert.isTrue(cs.trackDeleteIndex(0, 0));

            assert.equal(Array.from(cs)[0].count, 0);
        });

        it('trackDeleteIndex() should throw if the track index is invalid', function() {
            const cs = new CueSheetMetadata();

            assert.throws(() => cs.trackDeleteIndex(0, 0), /Invalid track position/);
            assert.isTrue(cs.insertBlankTrack(0));
            assert.throws(() => cs.trackDeleteIndex(34, 0), /Invalid track position/);
            assert.throws(() => cs.trackDeleteIndex(-1, 0), /Number -1 is negative/);
        });

        it('trackDeleteIndex() should throw if the index index is invalid', function() {
            const cs = new CueSheetMetadata();
            assert.isTrue(cs.insertBlankTrack(0));

            assert.throws(() => cs.trackDeleteIndex(0, 12), /Invalid index position/);
            assert.throws(() => cs.trackDeleteIndex(0, -1), /Number -1 is negative/);
        });

        it('trackDeleteIndex() should throw if the track index is not a number', function() {
            const cs = new CueSheetMetadata();

            assert.throws(() => cs.trackDeleteIndex(null), /Expected .+? to be number/);
        });

        it('trackDeleteIndex() should throw if the index index is not a number', function() {
            const cs = new CueSheetMetadata();

            assert.throws(() => cs.trackDeleteIndex(0, null), /Expected .+? to be number/);
        });

    });

    describe('gc', function() {
        it('gc should work', function() {
            require('../helper/gc')();
        });
    });
});
