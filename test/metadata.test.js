/* eslint-disable prefer-arrow-callback */
/// <reference path="../lib/index.d.ts" />
const {
    ApplicationMetadata,
    CueSheetIndex,
    CueSheetMetadata,
    CueSheetTrack,
    Metadata,
    PaddingMetadata,
    PictureMetadata,
    SeekPoint,
    SeekTableMetadata,
    StreamInfoMetadata,
    UnknownMetadata,
    VorbisCommentMetadata,
} = require('../lib/index').api.metadata;
const { MetadataType } = require('../lib/index').api.format;
const { getCuesheet, getTags } = require('../lib/index').api.metadata0;
const { SimpleIterator } = require('../lib/index').api;
const { assert } = require('chai');
const path = require('path');

const pathForFile = (...file) => path.join(__dirname, 'data', 'tags', ...file);

describe('metadata', function() {

    describe('ApplicationMetadata', function() {

        it('new object has the right type', function() {
            const am = new ApplicationMetadata();

            assert.equal(am.type, MetadataType.APPLICATION);
        });

        it('set id of object should work', function() {
            const am = new ApplicationMetadata();

            am.id = Buffer.from('1234');
            assert.deepEqual(am.id, Buffer.from('1234'));
        });

        it('set id of object having less than 4 bytes should throw', function() {
            const am = new ApplicationMetadata();

            assert.throws(() => am.id = Buffer.from('si'));
        });

        it('set id of object having more than 4 bytes should work but discard the rest', function() {
            const am = new ApplicationMetadata();

            am.id = Buffer.from('1234===-><-');
            assert.deepEqual(am.id, Buffer.from('1234'));
        });

        it('set data of object should work', function() {
            const am = new ApplicationMetadata();

            am.data = Buffer.from('this is a wonderful data that is useless per se, pero bueno');
            assert.deepEqual(am.data, Buffer.from('this is a wonderful data that is useless per se, pero bueno'));
        });

    });

    describe('CueSheetIndex', function() {

        it('create a new object with parameters should work', function() {
            const csi = new CueSheetIndex(10, 0);

            assert.equal(csi.offset, 10);
            assert.equal(csi.number, 0);
        });

        it('create a new object with offset not being a number should throw', function() {
            assert.throws(() => new CueSheetIndex(null, 0), /First argument/);
        });

        it('create a new object with number not being a number should throw', function() {
            assert.throws(() => new CueSheetIndex(1, null), /Second argument/);
        });

        it('use of bigint in offset should work', function() {
            const csi = new CueSheetIndex();

            csi.offset = 9007199254740993n;

            assert.equal(csi.offset, 9007199254740993n);
        });

        it('set offset is not a number should throw', function() {
            const csi = new CueSheetIndex();

            assert.throws(() => csi.offset = null);
        });

        it('set number should work', function() {
            const csi = new CueSheetIndex();

            csi.number = 10;

            assert.equal(csi.number, 10);
        });

        it('set number is not a number should throw', function() {
            const csi = new CueSheetIndex();

            assert.throws(() => csi.number = null);
        });

    });

    describe('CueSheetTrack', function() {

        it('create a new object should work', function() {
            new CueSheetTrack();
        });

        it('offset should work with numbers', function() {
            const cst = new CueSheetTrack();

            cst.offset = 9231292;

            assert.equal(cst.offset, 9231292);
        });

        it('offset should work with bigints', function() {
            const cst = new CueSheetTrack();

            cst.offset = 8127328197321897923n;

            assert.equal(cst.offset, 8127328197321897923n);
        });

        it('number should limit numbers to 255 automatically', function() {
            const cst = new CueSheetTrack();

            cst.number = 256;

            assert.equal(cst.number, 0);
        });

        it('isrc should be able to set a string of 12 bytes', function() {
            const cst = new CueSheetTrack();

            cst.isrc = '🗿-5374-12';

            assert.equal(cst.isrc, '🗿-5374-12');
        });

        it('isrc should throw if the string is not 12 bytes', function() {
            assert.throws(() => (new CueSheetTrack()).isrc = 'ç');
        });

        it('type should will only write 0 or 1 as value', function() {
            const cst = new CueSheetTrack();

            cst.type = 2;
            assert.equal(cst.type, 0);

            cst.type = -3;
            assert.equal(cst.type, 1);
        });

        it('indices should get an array of them', function() {
            const cs = getCuesheet(pathForFile('vc-cs.flac'));

            assert.isNotFalse(cs, 'The file vc-cs.flac must exist');
            assert.equal(cs.tracks[0].indices.length, 2);
            assert.deepEqual(cs.tracks[0].indices[0], new CueSheetIndex(0, 0));
            assert.deepEqual(cs.tracks[0].indices[1], new CueSheetIndex(18816, 1));
        });

        it('iterator should get all index items', function() {
            const cs = getCuesheet(pathForFile('vc-cs.flac'));

            assert.isNotFalse(cs, 'The file vc-cs.flac must exist');
            const results = Array.from(cs.tracks[0]);
            assert.deepEqual(results[0], new CueSheetIndex(0, 0));
            assert.deepEqual(results[1], new CueSheetIndex(18816, 1));
        });

        it('clone should get a different object but equal in contents', function() {
            const cs = getCuesheet(pathForFile('vc-cs.flac'));
            const cst = cs.tracks[0];

            const copy = cst.clone();

            assert.deepEqual(copy, cst);
            assert.notEqual(copy, cst);
        });

    });

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

        it('tracks should contain the expected tracks', function() {
            const cs = getCuesheet(pathForFile('vc-cs.flac'));

            assert.isNotFalse(cs, 'The file vc-cs.flac must exist');
            assert.equal(cs.tracks.length, 2);
            assert.equal(cs.tracks[0].offset, 0n);
            assert.equal(cs.tracks[0].number, 1);
            assert.equal(cs.tracks[0].isrc, '');
            assert.equal(cs.tracks[0].type, 0);
            assert.equal(cs.tracks[0].preEmphasis, false);
            assert.equal(cs.tracks[0].indices.length, 2);
            assert.equal(cs.tracks[1].offset, 441000n);
            assert.equal(cs.tracks[1].number, 170);
            assert.equal(cs.tracks[1].isrc, '');
            assert.equal(cs.tracks[1].type, 0);
            assert.equal(cs.tracks[1].preEmphasis, false);
            assert.equal(cs.tracks[1].indices.length, 0);
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
            assert.equal(tracks[0].indices.length, 2);
            assert.equal(tracks[1].offset, 441000n);
            assert.equal(tracks[1].number, 170);
            assert.equal(tracks[1].isrc, '');
            assert.equal(tracks[1].type, 0);
            assert.equal(tracks[1].preEmphasis, false);
            assert.equal(tracks[1].indices.length, 0);
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

                assert.equal(cs.tracks.length, 1);
            });

            it('insertBlankTrack() should fail if the index is invalid', function() {
                const cs = new CueSheetMetadata();

                assert.throws(() => cs.insertBlankTrack(1), /Invalid index/);
                assert.throws(() => cs.insertBlankTrack(-1), /Invalid index/);
                assert.throws(() => cs.insertBlankTrack(null), /Expected first/);
            });

            it('insertTrack() should insert the track', function() {
                const cs = new CueSheetMetadata();
                const cst = new CueSheetTrack();

                assert.isTrue(cs.insertTrack(0, cst));

                assert.equal(cs.tracks.length, 1);
            });

            it('insertTrack() should make a copy', function() {
                const cs = new CueSheetMetadata();
                const cst = new CueSheetTrack();

                assert.isTrue(cs.insertTrack(0, cst));
                cst.offset = 123n;

                assert.equal(cs.tracks.length, 1);
                assert.notEqual(cs.tracks[0].offset, cst.offset);
            });

            it('insertTrack() should throw if the index is invalid', function() {
                const cs = new CueSheetMetadata();
                const cst = new CueSheetTrack();

                assert.throws(() => cs.insertTrack(-1, cst), /Invalid index/);
                assert.throws(() => cs.insertTrack(11, cst), /Invalid index/);
                assert.throws(() => cs.insertTrack(null, cst), /Expected first/);
            });

            it('insertTrack() should throw if the track is not a CueSheetTrack', function() {
                const cs = new CueSheetMetadata();

                assert.throws(() => cs.insertTrack(-1, null), /Expected second/);
            });

            it('setTrack() should replace the track', function() {
                const cs = new CueSheetMetadata();
                const cst = new CueSheetTrack();
                cst.offset = 123n;

                assert.isTrue(cs.insertBlankTrack(0));
                assert.isTrue(cs.setTrack(0, cst));

                assert.equal(cs.tracks.length, 1);
                assert.equal(cs.tracks[0].offset, 123n);
            });

            it('setTrack() should throw if the index is invalid', function() {
                const cs = new CueSheetMetadata();
                const cst = new CueSheetTrack();

                assert.throws(() => cs.setTrack(0, cst), /Invalid index/);
                assert.isTrue(cs.insertBlankTrack(0));
                assert.throws(() => cs.setTrack(1, cst), /Invalid index/);
                assert.throws(() => cs.setTrack(-1, cst), /Invalid index/);
                assert.throws(() => cs.setTrack(null, cst), /Expected first/);
            });

            it('setTrack() should throw if the track is not CueSheetTrack', function() {
                const cs = new CueSheetMetadata();

                assert.throws(() => cs.setTrack(0, null), /Expected second/);
            });

            it('deleteTrack() should remove the track', function() {
                const cs = new CueSheetMetadata();
                assert.isTrue(cs.insertBlankTrack(0));

                assert.isTrue(cs.deleteTrack(0));

                assert.equal(cs.tracks.length, 0);
            });

            it('deleteTrack() should throw if the index is invalid', function() {
                const cs = new CueSheetMetadata();

                assert.throws(() => cs.deleteTrack(0), /Invalid index/);
                assert.isTrue(cs.insertBlankTrack(0));
                assert.throws(() => cs.deleteTrack(-1), /Invalid index/);
                assert.throws(() => cs.deleteTrack(11), /Invalid index/);
                assert.throws(() => cs.deleteTrack(null), /Expected first/);
            });

            it('resizeTracks() should work', function() {
                const cs = new CueSheetMetadata();

                assert.isTrue(cs.resizeTracks(10));
                assert.equal(cs.tracks.length, 10);
                assert.isTrue(cs.resizeTracks(1));
                assert.equal(cs.tracks.length, 1);
            });

            it('resizeTracks() should throw if size is not a number', function() {
                const cs = new CueSheetMetadata();

                assert.throws(() => cs.resizeTracks('1'), /Expected first/);
            });

        });

        describe('indices operations', function() {

            it('trackResizeIndices() should work if track index is valid', function() {
                const cs = new CueSheetMetadata();
                cs.insertBlankTrack(0);

                assert.isTrue(cs.trackResizeIndices(0, 10));
                assert.equal(cs.tracks[0].indices.length, 10);
                assert.isTrue(cs.trackResizeIndices(0, 1));
                assert.equal(cs.tracks[0].indices.length, 1);
            });

            it('trackResizeIndices() should throw if the index is invalid', function() {
                const cs = new CueSheetMetadata();

                assert.throws(() => cs.trackResizeIndices(0, 1), /Invalid track position/);
                assert.isTrue(cs.insertBlankTrack(0));
                assert.throws(() => cs.trackResizeIndices(11, 1), /Invalid track position/);
                assert.throws(() => cs.trackResizeIndices(-1, 1), /Invalid track position/);
            });

            it('trackResizeIndices() should throw if the track number is not a number', function() {
                const cs = new CueSheetMetadata();

                assert.throws(() => cs.trackResizeIndices({}, 1), /Expected first/);
            });

            it('trackResizeIndices() should throw if the indices size is not a number', function() {
                const cs = new CueSheetMetadata();

                assert.throws(() => cs.trackResizeIndices(0, {}), /Expected second/);
            });

            it('trackInsertIndex() should work if the indices are valid', function() {
                const cs = new CueSheetMetadata();
                cs.insertBlankTrack(0);

                assert.isTrue(cs.trackInsertIndex(0, 0, new CueSheetIndex(1n, 0)));

                assert.equal(cs.tracks[0].indices.length, 1);
            });

            it('trackInsertIndex() should throw if the track index is invalid', function() {
                const cs = new CueSheetMetadata();
                const csi = new CueSheetIndex(1n, 2);

                assert.throws(() => cs.trackInsertIndex(0, 0, csi), /Invalid track position/);
                assert.isTrue(cs.insertBlankTrack(0));
                assert.throws(() => cs.trackInsertIndex(11, 0, csi), /Invalid track position/);
                assert.throws(() => cs.trackInsertIndex(-1, 0, csi), /Invalid track position/);
            });

            it('trackInsertIndex() should throw if the index index is invalid', function() {
                const cs = new CueSheetMetadata();
                const csi = new CueSheetIndex(1n, 2);
                cs.insertBlankTrack(0);

                assert.throws(() => cs.trackInsertIndex(0, 99, csi), /Invalid index position/);
                assert.throws(() => cs.trackInsertIndex(0, -1, csi), /Invalid index position/);
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

                assert.equal(cs.tracks[0].indices.length, 1);
            });

            it('trackInsertBlankIndex() should throw if the track index is invalid', function() {
                const cs = new CueSheetMetadata();

                assert.throws(() => cs.trackInsertBlankIndex(0, 0), /Invalid track position/);
                assert.isTrue(cs.insertBlankTrack(0));
                assert.throws(() => cs.trackInsertBlankIndex(11, 0), /Invalid track position/);
                assert.throws(() => cs.trackInsertBlankIndex(-1, 0), /Invalid track position/);
            });

            it('trackInsertBlankIndex() should throw if the index index is invalid', function() {
                const cs = new CueSheetMetadata();
                cs.insertBlankTrack(0);

                assert.throws(() => cs.trackInsertBlankIndex(0, 99), /Invalid index position/);
                assert.throws(() => cs.trackInsertBlankIndex(0, -1), /Invalid index position/);
            });

            it('trackInsertBlankIndex() should throw if the track index is not a number', function() {
                const cs = new CueSheetMetadata();

                assert.throws(() => cs.trackInsertBlankIndex(null), /Expected first/);
            });

            it('trackInsertBlankIndex() should throw if the index index is not a number', function() {
                const cs = new CueSheetMetadata();

                assert.throws(() => cs.trackInsertBlankIndex(0, null), /Expected second/);
            });

            it('trackDeleteIndex() should work if the indices are valid', function() {
                const cs = new CueSheetMetadata();
                cs.insertBlankTrack(0);
                cs.trackInsertBlankIndex(0, 0);

                assert.isTrue(cs.trackDeleteIndex(0, 0));

                assert.equal(cs.tracks[0].indices.length, 0);
            });

            it('trackDeleteIndex() should throw if the track index is invalid', function() {
                const cs = new CueSheetMetadata();

                assert.throws(() => cs.trackDeleteIndex(0, 0), /Invalid track position/);
                assert.isTrue(cs.insertBlankTrack(0));
                assert.throws(() => cs.trackDeleteIndex(34, 0), /Invalid track position/);
                assert.throws(() => cs.trackDeleteIndex(-1, 0), /Invalid track position/);
            });

            it('trackDeleteIndex() should throw if the index index is invalid', function() {
                const cs = new CueSheetMetadata();
                assert.isTrue(cs.insertBlankTrack(0));

                assert.throws(() => cs.trackDeleteIndex(0, 12), /Invalid index position/);
                assert.throws(() => cs.trackDeleteIndex(0, -1), /Invalid index position/);
            });

            it('trackDeleteIndex() should throw if the track index is not a number', function() {
                const cs = new CueSheetMetadata();

                assert.throws(() => cs.trackDeleteIndex(null), /Expected first/);
            });

            it('trackDeleteIndex() should throw if the index index is not a number', function() {
                const cs = new CueSheetMetadata();

                assert.throws(() => cs.trackDeleteIndex(0, null), /Expected second/);
            });

        });

    });

    describe('PaddingMetadata', function() {

        it('create new object should work', function() {
            new PaddingMetadata();
        });

        it('the object should have the right type', function() {
            const p = new PaddingMetadata();

            assert.equal(p.type, MetadataType.PADDING);
        });

        it('the object initially has 0 bytes of padding', function() {
            const p = new PaddingMetadata();

            assert.equal(p.length, 0);
        });

        it('the object created with a size should have that size in bytes of padding', function() {
            const p = new PaddingMetadata(123);

            assert.equal(p.length, 123);
        });

    });

    describe('PictureMetadata', function() {

        it('create new object should work', function() {
            new PictureMetadata();
        });

        it('object has the right type', function() {
            const p = new PictureMetadata();

            assert.equal(p.type, MetadataType.PICTURE);
        });

        it('pictureType should change', function() {
            const p = new PictureMetadata();

            p.pictureType = 2;

            assert.equal(p.pictureType, 2);
        });

        it('pictureType should throw if not a number', function() {
            const p = new PictureMetadata();

            assert.throws(() => p.pictureType = null);
        });

        it('width should change', function() {
            const p = new PictureMetadata();

            p.width = 100;

            assert.equal(p.width, 100);
        });

        it('width should throw if not a number', function() {
            const p = new PictureMetadata();

            assert.throws(() => p.width = null);
        });

        it('height should change', function() {
            const p = new PictureMetadata();

            p.height = 100;

            assert.equal(p.height, 100);
        });

        it('height should throw if not a number', function() {
            const p = new PictureMetadata();

            assert.throws(() => p.height = null);
        });

        it('depth should change', function() {
            const p = new PictureMetadata();

            p.depth = 32;

            assert.equal(p.depth, 32);
        });

        it('depth should throw if not a number', function() {
            const p = new PictureMetadata();

            assert.throws(() => p.depth = null);
        });

        it('colors should change', function() {
            const p = new PictureMetadata();

            p.colors = 1;

            assert.equal(p.colors, 1);
        });

        it('colors should throw if not a number', function() {
            const p = new PictureMetadata();

            assert.throws(() => p.colors = null);
        });

        it('change mimeType should work', function() {
            const p = new PictureMetadata();

            assert.equal(p.mimeType, '');
            p.mimeType = 'image/jpg';
            assert.equal(p.mimeType, 'image/jpg');
        });

        it('change description should work', function() {
            const p = new PictureMetadata();

            assert.equal(p.description, '');
            p.description = 'Big description';
            assert.equal(p.description, 'Big description');
        });

        it('change data should work', function() {
            const p = new PictureMetadata();

            assert.deepEqual(p.data, Buffer.alloc(0));
            p.data = Buffer.from('\x89PNG');
            assert.deepEqual(p.data, Buffer.from('\x89PNG'));
        });

        it('change data to empty buffer should work', function() {
            const p = new PictureMetadata();

            assert.deepEqual(p.data, Buffer.alloc(0));
            p.data = Buffer.alloc(0);
            assert.deepEqual(p.data, Buffer.alloc(0));
        });

        it('isLegal() should work', function() {
            const p = new PictureMetadata();

            assert.isTrue(p.isLegal());
        });

    });

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

    });

    describe('SeekTableMetadata', function() {

        it('create new object should work', function() {
            new SeekTableMetadata();
        });

        it('object has the right type', function() {
            const st = new SeekTableMetadata();

            assert.equal(st.type, MetadataType.SEEKTABLE);
        });

        it('.points gets all points in the object', function() {
            const it = new SimpleIterator();
            it.init(pathForFile('vc-cs.flac'), true);
            it.next();
            const { points } = it.getBlock();

            assert.equal(points.length, 2);
            assert.equal(points[0].sampleNumber, 0n);
            assert.equal(points[0].streamOffset, 0n);
            assert.equal(points[0].frameSamples, 4096n);
            assert.equal(points[1].sampleNumber, 16384n);
            assert.equal(points[1].streamOffset, 8780n);
            assert.equal(points[1].frameSamples, 4096n);
        });

        it('iterator should iterate over all points in the object', function() {
            const it = new SimpleIterator();
            it.init(pathForFile('vc-cs.flac'), true);
            it.next();
            const st = it.getBlock();
            const i = st[Symbol.iterator]();

            let v = i.next();
            assert.isFalse(v.done);
            assert.equal(v.value.sampleNumber, 0n);
            assert.equal(v.value.streamOffset, 0n);
            assert.equal(v.value.frameSamples, 4096n);

            v = i.next();
            assert.isFalse(v.done);
            assert.equal(v.value.sampleNumber, 16384n);
            assert.equal(v.value.streamOffset, 8780n);
            assert.equal(v.value.frameSamples, 4096n);

            v = i.next();
            assert.isTrue(v.done);
        });

        it('resizePoints() should insert and remove points', function() {
            const st = new SeekTableMetadata();

            assert.equal(st.points.length, 0);
            assert.isTrue(st.resizePoints(10));
            assert.equal(st.points.length, 10);
            assert.isTrue(st.resizePoints(1));
            assert.equal(st.points.length, 1);
        });

        it('resizePoints() should throw if size is not a number', function() {
            const st = new SeekTableMetadata();

            assert.throws(() => st.resizePoints(null), /Expected first argument/);
        });

        it('insertPoint() should insert a point if the position is valid', function() {
            const st = new SeekTableMetadata();

            assert.isTrue(st.insertPoint(0, new SeekPoint()));

            assert.equal(st.points.length, 1);
        });

        it('insertPoint() should throw if the position is invalid', function() {
            const st = new SeekTableMetadata();

            assert.throws(() => st.insertPoint(11, new SeekPoint()), /Point position is invalid/);
            assert.throws(() => st.insertPoint(-1, new SeekPoint()), /Point position is invalid/);

            assert.equal(st.points.length, 0);
        });

        it('insertPoint() should throw if the position is not a number', function() {
            const st = new SeekTableMetadata();

            assert.throws(() => st.insertPoint(null, new SeekPoint()), /Expected first/);
        });

        it('insertPoint() should throw if the point is not a SeekPoint', function() {
            const st = new SeekTableMetadata();

            assert.throws(() => st.insertPoint(1, null), /Expected second/);
        });

        it('setPoint() should replace a point if the position is valid', function() {
            const st = new SeekTableMetadata();
            st.insertPoint(0, new SeekPoint());

            st.setPoint(0, new SeekPoint(3n, 2n, 1n));

            assert.equal(st.points[0].sampleNumber, 3n);
            assert.equal(st.points[0].streamOffset, 2n);
            assert.equal(st.points[0].frameSamples, 1n);
        });

        it('setPoint() should throw if the position is invalid', function() {
            const st = new SeekTableMetadata();

            assert.throws(() => st.setPoint(0, new SeekPoint()), /Point position is invalid/);
            st.insertPoint(0, new SeekPoint());
            assert.throws(() => st.setPoint(91, new SeekPoint()), /Point position is invalid/);
            assert.throws(() => st.setPoint(-1, new SeekPoint()), /Point position is invalid/);
        });

        it('setPoint() should throw if the position is not a number', function() {
            const st = new SeekTableMetadata();

            assert.throws(() => st.setPoint(null, new SeekPoint()), /Expected first/);
        });

        it('setPoint() should throw if the point is not a SeekPoint', function() {
            const st = new SeekTableMetadata();

            assert.throws(() => st.setPoint(1, null), /Expected second/);
        });

        it('deletePoint() should delete a point if the position is valid', function() {
            const st = new SeekTableMetadata();
            st.insertPoint(0, new SeekPoint());

            assert.isTrue(st.deletePoint(0));

            assert.equal(st.points.length, 0);
        });

        it('deletePoint() should throw if the position is invalid', function() {
            const st = new SeekTableMetadata();

            assert.throws(() => st.deletePoint(0), /Point position is invalid/);
            st.insertPoint(0, new SeekPoint());
            assert.throws(() => st.deletePoint(91), /Point position is invalid/);
            assert.throws(() => st.deletePoint(-1), /Point position is invalid/);
        });

        it('deletePoint() should throw if the position is not a number', function() {
            const st = new SeekTableMetadata();

            assert.throws(() => st.deletePoint(null), /Expected first/);
        });

        it('templateAppendPlaceholders() should insert points at the end', function() {
            const st = new SeekTableMetadata();
            st.insertPoint(0, new SeekPoint(998877665544332211n));

            assert.isTrue(st.templateAppendPlaceholders(10));

            assert.equal(st.points.length, 11);
            assert.equal(st.points[0].sampleNumber, 998877665544332211n);
        });

        it('templateAppendPlaceholders() should throw if count is not a number', function() {
            const st = new SeekTableMetadata();

            assert.throws(() => st.templateAppendPlaceholders(null), /Expected first/);
        });

        it('templateAppendPoint() should append a new point to the end', function() {
            const st = new SeekTableMetadata();
            st.insertPoint(0, new SeekPoint(998877665544332211n));

            assert.isTrue(st.templateAppendPoint(675n));

            assert.equal(st.points.length, 2);
            assert.equal(st.points[0].sampleNumber, 998877665544332211n);
            assert.equal(st.points[1].sampleNumber, 675n);
        });

        it('templateAppendPoint() should throw if sampleNumber is not a number', function() {
            const st = new SeekTableMetadata();

            assert.throws(() => st.templateAppendPoint(null), /Expected first/);
        });

        it('templateAppendPoints() should append some new points to the end', function() {
            const st = new SeekTableMetadata();
            st.insertPoint(0, new SeekPoint(998877665544332211n));

            assert.isTrue(st.templateAppendPoints([ 675n, 879n, 213n ]));

            assert.equal(st.points.length, 4);
            assert.equal(st.points[0].sampleNumber, 998877665544332211n);
            assert.equal(st.points[1].sampleNumber, 675n);
            assert.equal(st.points[2].sampleNumber, 879n);
            assert.equal(st.points[3].sampleNumber, 213n);
        });

        it('templateAppendPoints() should throw if sampleNumbers is not an array', function() {
            const st = new SeekTableMetadata();

            assert.throws(() => st.templateAppendPoints(null), /Expected first/);
        });

        it('templateAppendPoints() should throw if sampleNumbers array contains a non-number value', function() {
            const st = new SeekTableMetadata();

            assert.throws(() => st.templateAppendPoints([ 1, false, 3 ]), /Element at position \d+ is not a number/);
        });

        it('templateAppendSpacedPoints() should append some new points to the end', function() {
            const st = new SeekTableMetadata();
            st.insertPoint(0, new SeekPoint(123n));

            assert.isTrue(st.templateAppendSpacedPoints(5, 25n));

            assert.equal(st.points.length, 6);
            assert.equal(st.points[0].sampleNumber, 123n);
            assert.equal(st.points[1].sampleNumber, 5n * 0n);
            assert.equal(st.points[2].sampleNumber, 5n * 1n);
            assert.equal(st.points[3].sampleNumber, 5n * 2n);
            assert.equal(st.points[4].sampleNumber, 5n * 3n);
            assert.equal(st.points[5].sampleNumber, 5n * 4n);
        });

        it('templateAppendSpacedPoints() should throw if totalSamples is 0', function() {
            assert.throws(() => new SeekTableMetadata().templateAppendSpacedPoints(1, 0n), /Total samples is 0/);
        });

        it('templateAppendSpacedPoints() should throw if num is not a number', function() {
            assert.throws(() => new SeekTableMetadata().templateAppendSpacedPoints(null, 0n), /Expected first/);
        });

        it('templateAppendSpacedPoints() should throw if totalSamples is not a number', function() {
            assert.throws(() => new SeekTableMetadata().templateAppendSpacedPoints(0, null), /Expected second/);
        });

        it('templateAppendSpacedPointsBySamples() should append some new points to the end', function() {
            const st = new SeekTableMetadata();
            st.insertPoint(0, new SeekPoint(123n));

            assert.isTrue(st.templateAppendSpacedPointsBySamples(5, 25));

            assert.equal(st.points.length, 6);
            assert.equal(st.points[0].sampleNumber, 123n);
            assert.equal(st.points[1].sampleNumber, 5n * 0n);
            assert.equal(st.points[2].sampleNumber, 5n * 1n);
            assert.equal(st.points[3].sampleNumber, 5n * 2n);
            assert.equal(st.points[4].sampleNumber, 5n * 3n);
            assert.equal(st.points[5].sampleNumber, 5n * 4n);
        });

        it('templateAppendSpacedPointsBySamples() should throw if samples is 0', function() {
            assert.throws(() => new SeekTableMetadata().templateAppendSpacedPointsBySamples(0, 25n), /samples is 0/);
        });

        it('templateAppendSpacedPointsBySamples() should throw if totalSamples is 0', function() {
            assert.throws(() => new SeekTableMetadata().templateAppendSpacedPointsBySamples(5, 0n), /totalSamples is 0/);
        });

        it('templateAppendSpacedPointsBySamples() should throw if num is not a number', function() {
            assert.throws(() => new SeekTableMetadata().templateAppendSpacedPointsBySamples(null, 0n), /Expected first/);
        });

        it('templateAppendSpacedPointsBySamples() should throw if totalSamples is not a number', function() {
            assert.throws(() => new SeekTableMetadata().templateAppendSpacedPointsBySamples(0, null), /Expected second/);
        });

        it('templateSort() sorts the points', function() {
            const st = new SeekTableMetadata();
            st.insertPoint(0, new SeekPoint(123n));
            st.templateAppendSpacedPointsBySamples(5, 25);
            st.insertPoint(4, new SeekPoint(500n));
            st.insertPoint(7, new SeekPoint(5n, 5n));

            assert.isTrue(st.templateSort());

            assert.equal(st.points.length, 8);
            assert.equal(st.points[0].sampleNumber, 5n * 0n);
            assert.equal(st.points[1].sampleNumber, 5n * 1n);
            assert.equal(st.points[2].sampleNumber, 5n * 2n);
            assert.equal(st.points[3].sampleNumber, 5n * 3n);
            assert.equal(st.points[4].sampleNumber, 5n * 4n);
            assert.equal(st.points[5].sampleNumber, 123n);
            assert.equal(st.points[6].sampleNumber, 500n);
            assert.equal(st.points[7].sampleNumber, 18446744073709551615n);
        });

        it('templateSort(true) sorts the points and removes duplicates', function() {
            const st = new SeekTableMetadata();
            st.insertPoint(0, new SeekPoint(123n));
            st.templateAppendSpacedPointsBySamples(5, 25);
            st.insertPoint(4, new SeekPoint(500n));
            st.insertPoint(7, new SeekPoint(5n, 5n));

            assert.isTrue(st.templateSort(true));

            assert.equal(st.points.length, 7);
            assert.equal(st.points[0].sampleNumber, 5n * 0n);
            assert.equal(st.points[1].sampleNumber, 5n * 1n);
            assert.equal(st.points[2].sampleNumber, 5n * 2n);
            assert.equal(st.points[3].sampleNumber, 5n * 3n);
            assert.equal(st.points[4].sampleNumber, 5n * 4n);
            assert.equal(st.points[5].sampleNumber, 123n);
            assert.equal(st.points[6].sampleNumber, 500n);
        });

        it('isLegal() should work', function() {
            const st = new SeekTableMetadata();
            assert.isTrue(st.isLegal());
        });

    });

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

    });

    describe('UnknownMetadata', function() {

        it('create new object should work', function() {
            new UnknownMetadata();
        });

        it('object has the right type', function() {
            const st = new UnknownMetadata();

            assert.equal(st.type, MetadataType.UNDEFINED);
        });

        it('object with custom type has that type', function() {
            const st = new UnknownMetadata(MetadataType.UNDEFINED + 5);

            assert.equal(st.type, MetadataType.UNDEFINED + 5);
        });

        it('object with valid metadata type should have UNDEFINED type', function() {
            const st = new UnknownMetadata(MetadataType.CUESHEET);

            assert.equal(st.type, MetadataType.UNDEFINED);
        });

        it('object with a really high type should have maximum valid number as type', function() {
            const st = new UnknownMetadata(MetadataType.MAX_METADATA_TYPE * 38);

            assert.equal(st.type, MetadataType.MAX_METADATA_TYPE);
        });

        it('data is a buffer', function() {
            const st = new UnknownMetadata();

            assert.isTrue(Buffer.isBuffer(st.data));
        });

    });

    describe('VorbisCommentMetadata', function() {

        it('create new object should work', function() {
            new VorbisCommentMetadata();
        });

        it('object has the right type', function() {
            const vc = new VorbisCommentMetadata();

            assert.equal(vc.type, MetadataType.VORBIS_COMMENT);
        });

        it('vendor string can be modified', function() {
            const vc = new VorbisCommentMetadata();

            vc.vendorString = 'mee';

            assert.equal(vc.vendorString, 'mee');
        });

        it('.comments should contain all the expected comments', function() {
            const { comments } = getTags(pathForFile('vc-p.flac'));

            assert.equal(comments.length, 6);
            assert.equal(comments[0], 'TITLE=Metadata Test');
            assert.equal(comments[1], 'ARTIST=melchor629');
            assert.equal(comments[2], 'ALBUM=flac-bindings');
            assert.equal(comments[3], 'COMMENT=Nice comment tho');
            assert.equal(comments[4], 'DATE=2019');
            assert.equal(comments[5], 'TRACKNUMBER=1');
        });

        it('iterator should iterate over all the expected comments', function() {
            const it = getTags(pathForFile('vc-p.flac'))[Symbol.iterator]();
            let m;

            m = it.next();
            assert.isFalse(m.done);
            assert.equal(m.value, 'TITLE=Metadata Test');

            m = it.next();
            assert.isFalse(m.done);
            assert.equal(m.value, 'ARTIST=melchor629');

            m = it.next();
            assert.isFalse(m.done);
            assert.equal(m.value, 'ALBUM=flac-bindings');

            m = it.next();
            assert.isFalse(m.done);
            assert.equal(m.value, 'COMMENT=Nice comment tho');

            m = it.next();
            assert.isFalse(m.done);
            assert.equal(m.value, 'DATE=2019');

            m = it.next();
            assert.isFalse(m.done);
            assert.equal(m.value, 'TRACKNUMBER=1');

            m = it.next();
            assert.isTrue(m.done);
        });

        it('get() should get the value of an existing entry', function() {
            const vc = getTags(pathForFile('vc-p.flac'));

            const entryValue = vc.get('COMMENT');

            assert.isNotNull(entryValue);
            assert.equal(entryValue, 'Nice comment tho');
        });

        it('get() should return null for a non existing entry', function() {
            const vc = getTags(pathForFile('vc-p.flac'));

            const entryValue = vc.get('TIME');

            assert.isNull(entryValue);
        });

        it('get() should throw if the first argument is not a string', function() {
            assert.throws(() => new VorbisCommentMetadata().get(function() {}));
        });

        it('resizeComments() should add and remove comment entries', function() {
            const vc = new VorbisCommentMetadata();

            assert.isTrue(vc.resizeComments(10));
            assert.equal(vc.comments.length, 10);
            assert.isTrue(vc.resizeComments(1));
            assert.equal(vc.comments.length, 1);
        });

        it('resizeComments() should throw if size is not a number', function() {
            const vc = new VorbisCommentMetadata();

            assert.throws(() => vc.resizeComments(null), /Expected first/);
        });

        it('setComment() should replace the comment entry if the position is valid', function() {
            const vc = new VorbisCommentMetadata();
            vc.resizeComments(1);

            assert.isTrue(vc.setComment(0, 'ARTIST=Mario'));

            assert.equal(vc.comments.length, 1);
            assert.equal(vc.comments[0], 'ARTIST=Mario');
        });

        it('setComment() should throw if the position is invalid', function() {
            const vc = new VorbisCommentMetadata();

            assert.throws(() => vc.setComment(0, ''));
            vc.resizeComments(1);
            assert.throws(() => vc.setComment(38, ''));
            assert.throws(() => vc.setComment(-9, ''));
        });

        it('setComment() should throw if the first argument is not a number', function() {
            assert.throws(() => new VorbisCommentMetadata().setComment(null));
        });

        it('setComment() should throw if the second argument is not a string', function() {
            assert.throws(() => new VorbisCommentMetadata().setComment(0, NaN));
        });

        it('insertComment() should add a new comment entry if the position is valid', function() {
            const vc = new VorbisCommentMetadata();

            assert.isTrue(vc.insertComment(0, 'TITLE=tests'));

            assert.equal(vc.comments.length, 1);
            assert.equal(vc.comments[0], 'TITLE=tests');
        });

        it('insertComment() should throw if the position is invalid', function() {
            const vc = new VorbisCommentMetadata();

            assert.throws(() => vc.insertComment(38, ''));
            assert.throws(() => vc.insertComment(-9, ''));
        });

        it('insertComment() should throw if the first argument is not a number', function() {
            assert.throws(() => new VorbisCommentMetadata().insertComment(null));
        });

        it('insertComment() should throw if the second argument is not a string', function() {
            assert.throws(() => new VorbisCommentMetadata().insertComment(0, NaN));
        });

        it('appendComment() should add a new comment entry', function() {
            const vc = new VorbisCommentMetadata();

            assert.isTrue(vc.appendComment('TITLE=tests'));

            assert.equal(vc.comments.length, 1);
            assert.equal(vc.comments[0], 'TITLE=tests');
        });

        it('appendComment() should throw if the first argument is not a string', function() {
            assert.throws(() => new VorbisCommentMetadata().appendComment({ '1': '2' }));
        });

        it('replaceComment() should replace comments', function() {
            const vc = new VorbisCommentMetadata();
            vc.appendComment('TITLE=E');

            assert.isTrue(vc.replaceComment('TITLE=tests', true));

            assert.equal(vc.comments.length, 1);
            assert.equal(vc.comments[0], 'TITLE=tests');
        });

        it('deleteComment() should remove the comment entry if the position is valid', function() {
            const vc = new VorbisCommentMetadata();
            vc.resizeComments(1);

            assert.isTrue(vc.deleteComment(0, 'TITLE=tests'));

            assert.equal(vc.comments.length, 0);
        });

        it('deleteComment() should throw if the position is invalid', function() {
            const vc = new VorbisCommentMetadata();

            assert.throws(() => vc.deleteComment(38, ''));
            assert.throws(() => vc.deleteComment(-9, ''));
        });

        it('deleteComment() should throw if the first argument is not a number', function() {
            assert.throws(() => new VorbisCommentMetadata().deleteComment(null));
        });

        it('findEntryFrom() should return the position of the entry', function() {
            const vc = new VorbisCommentMetadata();
            vc.insertComment(0, 'TITLE=me');

            const pos = vc.findEntryFrom(0, 'title');

            assert.equal(pos, 0);
        });

        it('findEntryFrom() should return the -1 if the entry cannot be found', function() {
            const vc = new VorbisCommentMetadata();
            vc.insertComment(0, 'TITLE=me');

            const pos = vc.findEntryFrom(0, 'ARTIST');

            assert.equal(pos, -1);
        });

        it('findEntryFrom() should throw if the first argument is not a number', function() {
            assert.throws(() => new VorbisCommentMetadata().findEntryFrom(null), /Expected first/);
        });

        it('findEntryFrom() should throw if the second argument is not a string', function() {
            assert.throws(() => new VorbisCommentMetadata().findEntryFrom(0, undefined), /Expected second/);
        });

        it('removeEntryMatching() should return 0 if no entries has been removed', function() {
            const vc = new VorbisCommentMetadata();
            vc.insertComment(0, 'TITLE=me');

            const ret = vc.removeEntryMatching('ARTIST');

            assert.equal(ret, 0);
            assert.equal(vc.comments.length, 1);
        });

        it('removeEntryMatching() should return 1 if at least one entry has been removed', function() {
            const vc = new VorbisCommentMetadata();
            vc.insertComment(0, 'TITLE=me');
            vc.appendComment('TITLE=mo');

            const ret = vc.removeEntryMatching('TITLE');

            assert.equal(ret, 1);
            assert.equal(vc.comments.length, 1);
        });

        it('removeEntryMatching() should throw if the first argument is not a string', function() {
            assert.throws(() => new VorbisCommentMetadata().removeEntryMatching(NaN));
        });

        it('removeEntriesMatching() should return 0 if no entries has been removed', function() {
            const vc = new VorbisCommentMetadata();
            vc.insertComment(0, 'TITLE=me');

            const ret = vc.removeEntriesMatching('ARTIST');

            assert.equal(ret, 0);
            assert.equal(vc.comments.length, 1);
        });

        it('removeEntriesMatching() should return 2 if entries has been removed', function() {
            const vc = new VorbisCommentMetadata();
            vc.insertComment(0, 'TITLE=me');
            vc.appendComment('TITLE=mo');

            const ret = vc.removeEntriesMatching('TITLE');

            assert.equal(ret, 2);
            assert.equal(vc.comments.length, 0);
        });

        it('removeEntriesMatching() should throw if the first argument is not a string', function() {
            assert.throws(() => new VorbisCommentMetadata().removeEntriesMatching(NaN));
        });

    });

    describe('Metadata', function() {

        it('create metadata object should throw', function() {
            assert.throws(() => new Metadata(1), /The Metadata class is abstract/);
        });

        it('get isLast should work', function() {
            const m = new ApplicationMetadata();

            assert.isFalse(m.isLast);
        });

        it('isEqual() returns true if the objects are similar', function() {
            const am1 = new ApplicationMetadata();
            const am2 = new ApplicationMetadata();
            am1.data = am2.data = Buffer.from('no');
            am1.id = am2.id = Buffer.from('node');

            assert.isTrue(am1.isEqual(am2));
            assert.isTrue(am2.isEqual(am1));
        });

        it('isEqual() returns false if the objects are different', function() {
            const am1 = new ApplicationMetadata();
            const am2 = new ApplicationMetadata();
            am1.data = am2.id = Buffer.from('nodo');
            am1.id = am2.data = Buffer.from('node');

            assert.isFalse(am1.isEqual(am2));
            assert.isFalse(am2.isEqual(am1));
        });

        it('isEqual() throws if first argument is not provided', function() {
            assert.throws(() => new ApplicationMetadata().isEqual(), /Expected one argument/);
        });

        it('isEqual() throws if first argument is not Metadata object', function() {
            assert.throws(() => new ApplicationMetadata().isEqual({}), /Object does not seem to be valid/);
        });

        it('clone() should create a different object but equal', function() {
            const am1 = new ApplicationMetadata();
            am1.data = Buffer.from('nodo');
            am1.id = Buffer.from('node');

            const am2 = am1.clone();

            assert.isTrue(am1.isEqual(am2));
            assert.isTrue(am2.isEqual(am1));
            assert.isFalse(Object.is(am1, am2));
        });

    });

    describe('gc', function() {
        it('gc should work', function() {
            require('./helper/gc')();
        });
    });

});
