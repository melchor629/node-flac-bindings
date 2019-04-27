/// <reference path="../lib/index.d.ts" />
const { metadata0, metadata, format } = require('../lib/index').api;
const { assert, use } = require('chai');
const { promises: fs } = require('fs');
const path = require('path');

const pathForFile = (...file) => path.join(__dirname, 'data', 'tags', ...file);

use(require('./helper/async-chai-extensions.js'));

describe('metadata0', function() {

    describe('getTags', function() {

        it('should throw if the argument is not a string', function() {
            assert.throws(() => metadata0.getTags(Symbol.species));
        });

        it('should return false if the file does not exist', async function() {
            const filePath = pathForFile('el.flac');

            const tags = metadata0.getTags(filePath);

            assert.isFalse(tags);
            let hasThrown = false;
            try { await fs.access(filePath); } catch(e) { hasThrown = true; }
            assert.isTrue(hasThrown, 'The file must not exist');
        });

        it('should return false if the file does not have tags', async function() {
            const filePath = pathForFile('no.flac');

            const tags = metadata0.getTags(filePath);

            assert.isFalse(tags);
            await fs.access(filePath);
        });

        it('should return a VorbisComment object if the file contains one of it', function() {
            const filePath = pathForFile('vc-p.flac');

            const tags = metadata0.getTags(filePath);

            assert.isNotFalse(tags);
            assert.isTrue(tags instanceof metadata.VorbisCommentMetadata);
        });

        it('VorbisComment should contain the right tags for the file', function() {
            const filePath = pathForFile('vc-p.flac');

            const tags = metadata0.getTags(filePath);

            assert.equal(tags.get('TITLE'), 'Metadata Test');
            assert.equal(tags.get('ARTIST'), 'melchor629');
            assert.equal(tags.get('ALBUM'), 'flac-bindings');
            assert.equal(tags.get('COMMENT'), 'Nice comment tho');
            assert.equal(tags.get('DATE'), '2019');
            assert.equal(tags.get('TRACKNUMBER'), '1');
        });

    });

    describe('getTagsAsync', function() {

        it('should throw if the argument is not a string', async function() {
            await assert.throwsAsync(() => metadata0.getTagsAsync(Symbol.species));
        });

        it('should throw if the file does not exist', async function() {
            const filePath = pathForFile('el.flac');

            await assert.throwsAsync(() => metadata0.getTagsAsync(filePath));
            await assert.throwsAsync(() => fs.access(filePath));
        });

        it('should throw if the file does not have tags', async function() {
            const filePath = pathForFile('no.flac');

            await assert.throwsAsync(() => metadata0.getTagsAsync(filePath));

            await fs.access(filePath);
        });

        it('should return a VorbisComment object if the file contains one of it', async function() {
            const filePath = pathForFile('vc-p.flac');

            const tags = await metadata0.getTagsAsync(filePath);

            assert.isNotFalse(tags);
            assert.isTrue(tags instanceof metadata.VorbisCommentMetadata);
        });

        it('VorbisComment should contain the right tags for the file', async function() {
            const filePath = pathForFile('vc-p.flac');

            const tags = await metadata0.getTagsAsync(filePath);

            assert.equal(tags.get('TITLE'), 'Metadata Test');
            assert.equal(tags.get('ARTIST'), 'melchor629');
            assert.equal(tags.get('ALBUM'), 'flac-bindings');
            assert.equal(tags.get('COMMENT'), 'Nice comment tho');
            assert.equal(tags.get('DATE'), '2019');
            assert.equal(tags.get('TRACKNUMBER'), '1');
        });

    });

    describe('getPicture', function() {

        it('should throw if the argument is not a string', function() {
            assert.throws(() => metadata0.getPicture(Symbol.iterator));
        });

        it('should return false if the file does not exist', async function() {
            const filePath = pathForFile('el.flac');

            const picture = metadata0.getPicture(filePath);

            assert.isFalse(picture);
            let hasThrown = false;
            try { await fs.access(filePath); } catch(e) { hasThrown = true; }
            assert.isTrue(hasThrown, 'The file must not exist');
        });

        it('should return false if the file does not have a picture', async function() {
            const filePath = pathForFile('no.flac');

            const picture = metadata0.getPicture(filePath);

            assert.isFalse(picture);
            await fs.access(filePath);
        });

        it('should return a Picture object if the file data one of it', function() {
            const filePath = pathForFile('vc-p.flac');

            const picture = metadata0.getPicture(filePath);

            assert.isNotFalse(picture);
            assert.isTrue(picture instanceof metadata.PictureMetadata);
        });

        it('Picture should contain the right tags for the file', function() {
            const filePath = pathForFile('vc-p.flac');

            const picture = metadata0.getPicture(filePath);

            assert.equal(picture.colors, 0);
            assert.equal(picture.depth, 32);
            assert.equal(picture.description, 'o.O.png');
            assert.equal(picture.height, 168);
            assert.equal(picture.mimeType, 'image/png');
            assert.equal(picture.pictureType, format.PictureType.FRONT_COVER);
            assert.equal(picture.width, 150);
            assert.equal(picture.data.length, 17094);
        });

    });

    describe('getPictureAsync', function() {

        it('should throw if the argument is not a string', async function() {
            await assert.throwsAsync(() => metadata0.getPictureAsync(Symbol.iterator));
        });

        it('should throw if the file does not exist', async function() {
            const filePath = pathForFile('el.flac');

            await assert.throwsAsync(() => metadata0.getPictureAsync(filePath));
            await assert.throwsAsync(() => fs.access(filePath));
        });

        it('should throw if the file does not have a picture', async function() {
            const filePath = pathForFile('no.flac');

            await assert.throwsAsync(() => metadata0.getPictureAsync(filePath));
            await fs.access(filePath);
        });

        it('should return a Picture object if the file data one of it', async function() {
            const filePath = pathForFile('vc-p.flac');

            const picture = await metadata0.getPictureAsync(filePath);

            assert.isNotFalse(picture);
            assert.isTrue(picture instanceof metadata.PictureMetadata);
        });

        it('Picture should contain the right tags for the file', async function() {
            const filePath = pathForFile('vc-p.flac');

            const picture = await metadata0.getPictureAsync(filePath);

            assert.equal(picture.colors, 0);
            assert.equal(picture.depth, 32);
            assert.equal(picture.description, 'o.O.png');
            assert.equal(picture.height, 168);
            assert.equal(picture.mimeType, 'image/png');
            assert.equal(picture.pictureType, format.PictureType.FRONT_COVER);
            assert.equal(picture.width, 150);
            assert.equal(picture.data.length, 17094);
        });

    });

    describe('getCuesheet', function() {

        it('should throw if the argument is not a string', function() {
            assert.throws(() => metadata0.getCuesheet(Symbol.asyncIterator));
        });

        it('should return false if the file does not exist', async function() {
            const filePath = pathForFile('el.flac');

            const cueSheet = metadata0.getCuesheet(filePath);

            assert.isFalse(cueSheet);
            let hasThrown = false;
            try { await fs.access(filePath); } catch(e) { hasThrown = true; }
            assert.isTrue(hasThrown, 'The file must not exist');
        });

        it('should return false if the file does not have a cue sheet', async function() {
            const filePath = pathForFile('no.flac');

            const cueSheet = metadata0.getCuesheet(filePath);

            assert.isFalse(cueSheet);
            await fs.access(filePath);
        });

        it('should return a CueSheet object if the file contains one of it', function() {
            const filePath = pathForFile('vc-cs.flac');

            const cueSheet = metadata0.getCuesheet(filePath);

            assert.isNotFalse(cueSheet);
            assert.isTrue(cueSheet instanceof metadata.CueSheetMetadata);
        });

        it('CueSheet should contain the right tracks for the file', function() {
            const filePath = pathForFile('vc-cs.flac');

            const cueSheet = metadata0.getCuesheet(filePath);

            assert.equal(cueSheet.mediaCatalogNumber, 0);
            assert.equal(cueSheet.leadIn, 88200);
            assert.equal(cueSheet.isCd, true);
            assert.equal(cueSheet.tracks.length, 2);
            assert.equal(cueSheet.tracks[0].offset, 0);
            assert.equal(cueSheet.tracks[0].number, 1);
            assert.equal(cueSheet.tracks[0].isrc, '');
            assert.equal(cueSheet.tracks[0].type, 0);
            assert.equal(cueSheet.tracks[0].preEmphasis, false);
            assert.equal(cueSheet.tracks[0].indices.length, 2);
            assert.equal(cueSheet.tracks[0].indices[0].offset, 0);
            assert.equal(cueSheet.tracks[0].indices[0].number, 0);
            assert.equal(cueSheet.tracks[0].indices[1].offset, 18816);
            assert.equal(cueSheet.tracks[0].indices[1].number, 1);
            assert.equal(cueSheet.tracks[1].offset, 441000);
            assert.equal(cueSheet.tracks[1].number, 170);
        });

    });

    describe('getCuesheetAsync', function() {

        it('should throw if the argument is not a string', async function() {
            await assert.throwsAsync(() => metadata0.getCuesheetAsync(Symbol.asyncIterator));
        });

        it('should throw if the file does not exist', async function() {
            const filePath = pathForFile('el.flac');

            await assert.throwsAsync(() => metadata0.getCuesheetAsync(filePath));
            await assert.throwsAsync(() => fs.access(filePath));
        });

        it('should throw if the file does not have a cue sheet', async function() {
            const filePath = pathForFile('no.flac');

            await assert.throwsAsync(() => metadata0.getCuesheetAsync(filePath));
            await fs.access(filePath);
        });

        it('should return a CueSheet object if the file contains one of it', async function() {
            const filePath = pathForFile('vc-cs.flac');

            const cueSheet = await metadata0.getCuesheetAsync(filePath);

            assert.isNotFalse(cueSheet);
            assert.isTrue(cueSheet instanceof metadata.CueSheetMetadata);
        });

        it('CueSheet should contain the right tracks for the file', async function() {
            const filePath = pathForFile('vc-cs.flac');

            const cueSheet = await metadata0.getCuesheetAsync(filePath);

            assert.equal(cueSheet.mediaCatalogNumber, 0);
            assert.equal(cueSheet.leadIn, 88200);
            assert.equal(cueSheet.isCd, true);
            assert.equal(cueSheet.tracks.length, 2);
            assert.equal(cueSheet.tracks[0].offset, 0);
            assert.equal(cueSheet.tracks[0].number, 1);
            assert.equal(cueSheet.tracks[0].isrc, '');
            assert.equal(cueSheet.tracks[0].type, 0);
            assert.equal(cueSheet.tracks[0].preEmphasis, false);
            assert.equal(cueSheet.tracks[0].indices.length, 2);
            assert.equal(cueSheet.tracks[0].indices[0].offset, 0);
            assert.equal(cueSheet.tracks[0].indices[0].number, 0);
            assert.equal(cueSheet.tracks[0].indices[1].offset, 18816);
            assert.equal(cueSheet.tracks[0].indices[1].number, 1);
            assert.equal(cueSheet.tracks[1].offset, 441000);
            assert.equal(cueSheet.tracks[1].number, 170);
        });

    });

    describe('getStreaminfo', function() {

        it('should throw if the argument is not a string', function() {
            assert.throws(() => metadata0.getStreaminfo(Symbol.toPrimitive));
        });

        it('should return false if the file does not exist', async function() {
            const filePath = pathForFile('el.flac');

            const streamInfo = metadata0.getStreaminfo(filePath);

            assert.isFalse(streamInfo);
            let hasThrown = false;
            try { await fs.access(filePath); } catch(e) { hasThrown = true; }
            assert.isTrue(hasThrown, 'The file must not exist');
        });

        it('StreamInfo should contain the right info for the file', function() {
            const filePath = pathForFile('vc-cs.flac');

            const streamInfo = metadata0.getStreaminfo(filePath);

            assert.equal(streamInfo.minBlocksize, 4096);
            assert.equal(streamInfo.maxBlocksize, 4096);
            assert.equal(streamInfo.minFramesize, 1770);
            assert.equal(streamInfo.maxFramesize, 2225);
            assert.equal(streamInfo.bitsPerSample, 16);
            assert.equal(streamInfo.sampleRate, 44100);
            assert.equal(streamInfo.channels, 2);
            assert.equal(streamInfo.totalSamples, 441000);
            assert.isTrue(Buffer.from('c5671d66cdca83ac483dcc302ae169af', 'hex').equals(streamInfo.md5sum), 'MD5Sum does not match');
        });

    });

    describe('getStreaminfoAsync', function() {

        it('should throw if the argument is not a string', function() {
            return assert.throwsAsync(() => metadata0.getStreaminfoAsync(Symbol.toPrimitive));
        });

        it('should throw if the file does not exist', async function() {
            const filePath = pathForFile('el.flac');

            await assert.throwsAsync(() => metadata0.getStreaminfoAsync(filePath));
            await assert.throwsAsync(() => fs.access(filePath));
        });

        it('StreamInfo should contain the right info for the file', async function() {
            const filePath = pathForFile('vc-cs.flac');

            const streamInfo = await metadata0.getStreaminfoAsync(filePath);

            assert.equal(streamInfo.minBlocksize, 4096);
            assert.equal(streamInfo.maxBlocksize, 4096);
            assert.equal(streamInfo.minFramesize, 1770);
            assert.equal(streamInfo.maxFramesize, 2225);
            assert.equal(streamInfo.bitsPerSample, 16);
            assert.equal(streamInfo.sampleRate, 44100);
            assert.equal(streamInfo.channels, 2);
            assert.equal(streamInfo.totalSamples, 441000);
            assert.isTrue(Buffer.from('c5671d66cdca83ac483dcc302ae169af', 'hex').equals(streamInfo.md5sum), 'MD5Sum does not match');
        });

    });

    describe('gc', function() {
        it('gc should work', function() {
            require('./helper/gc')();
        });
    });

});
