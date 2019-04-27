/// <reference path="../lib/index.d.ts" />
const { Chain, Iterator, metadata, format } = require('../lib/index').api;
const { assert } = require('chai');
const { promises: fs, ...oldfs } = require('fs');
const path = require('path');
const temp = require('temp');

temp.track();

const pathForFile = (...file) => path.join(__dirname, 'data', 'tags', ...file);

describe('Chain & Iterator', function() {

    describe('read', function() {

        it('throws if the first argument is not a Metadata', function() {
            assert.throws(() => new Chain().read({}));
        });

        it('throws if the first argument is not a Metadata (ogg version)', function() {
            assert.throws(() => new Chain().readOgg(() => 1));
        });

        it('returns false if the file does not exist', async function() {
            const filePath = pathForFile('el.flac');
            const ch = new Chain();

            const ret = ch.read(filePath);

            assert.isFalse(ret);
            assert.equal(ch.status(), Chain.Status.ERROR_OPENING_FILE);
            let hasThrown = false;
            try { await fs.access(filePath); } catch(e) { hasThrown = true; }
            assert.isTrue(hasThrown, 'The file must not exist');
        });

        it('returns true if the file exists', async function() {
            const filePath = pathForFile('no.flac');
            const ch = new Chain();

            const ret = ch.read(filePath);

            assert.isTrue(ret, Chain.StatusString[ch.status()]);
            await fs.access(filePath);
        });

    });

    describe('iterator', function() {

        it('createIterator() returns an iterator and iterates over the blocks correctly', function() {
            const filePath = pathForFile('vc-p.flac');
            const ch = new Chain();

            const ret = ch.read(filePath);

            assert.isTrue(ret, Chain.StatusString[ch.status()]);
            const it = ch.createIterator();
            const i = it[Symbol.iterator]();

            let m = i.next();
            assert.isFalse(m.done);
            assert.equal(m.value.type, format.MetadataType.STREAMINFO);

            m = i.next();
            assert.isFalse(m.done);
            assert.equal(m.value.type, format.MetadataType.APPLICATION);

            m = i.next();
            assert.isFalse(m.done);
            assert.equal(m.value.type, format.MetadataType.PICTURE);

            m = i.next();
            assert.isFalse(m.done);
            assert.equal(m.value.type, format.MetadataType.PADDING);

            m = i.next();
            assert.isFalse(m.done);
            assert.equal(m.value.type, format.MetadataType.VORBIS_COMMENT);

            m = i.next();
            assert.isTrue(m.done);
        });

        it('new Iterator() creates an iterator from the chain and iterates over the blocks correctly', function() {
            const filePath = pathForFile('vc-p.flac');
            const ch = new Chain();
            const it = new Iterator();

            const ret = ch.read(filePath);

            assert.isTrue(ret, Chain.StatusString[ch.status()]);
            it.init(ch);
            const i = it[Symbol.iterator]();

            let m = i.next();
            assert.isFalse(m.done);
            assert.equal(m.value.type, format.MetadataType.STREAMINFO);

            m = i.next();
            assert.isFalse(m.done);
            assert.equal(m.value.type, format.MetadataType.APPLICATION);

            m = i.next();
            assert.isFalse(m.done);
            assert.equal(m.value.type, format.MetadataType.PICTURE);

            m = i.next();
            assert.isFalse(m.done);
            assert.equal(m.value.type, format.MetadataType.PADDING);

            m = i.next();
            assert.isFalse(m.done);
            assert.equal(m.value.type, format.MetadataType.VORBIS_COMMENT);

            m = i.next();
            assert.isTrue(m.done);
        });

    });

    describe('iterator and get*', function() {

        it('should iterate forwards and get info about them correctly', function() {
            const filePath = pathForFile('vc-p.flac');
            const ch = new Chain();

            const initRetValue = ch.read(filePath);
            assert.isTrue(initRetValue, Chain.StatusString[ch.status()]);

            const it = ch.createIterator();
            assert.equal(it.getBlockType(), format.MetadataType.STREAMINFO);
            assert.isTrue(it.getBlock() instanceof metadata.StreamInfoMetadata);

            assert.isTrue(it.next());
            assert.equal(it.getBlockType(), format.MetadataType.APPLICATION);
            assert.isTrue(it.getBlock() instanceof metadata.ApplicationMetadata);

            assert.isTrue(it.next());
            assert.equal(it.getBlockType(), format.MetadataType.PICTURE);
            assert.isTrue(it.getBlock() instanceof metadata.PictureMetadata);

            assert.isTrue(it.next());
            assert.equal(it.getBlockType(), format.MetadataType.PADDING);
            assert.isTrue(it.getBlock() instanceof metadata.PaddingMetadata);

            assert.isTrue(it.next());
            assert.equal(it.getBlockType(), format.MetadataType.VORBIS_COMMENT);
            assert.isTrue(it.getBlock() instanceof metadata.VorbisCommentMetadata);

            assert.isFalse(it.next());
        });

        it('should iterate backwards and get info about them correctly', function() {
            const filePath = pathForFile('vc-p.flac');
            const ch = new Chain();

            const initRetValue = ch.read(filePath);
            assert.isTrue(initRetValue, Chain.StatusString[ch.status()]);

            const it = ch.createIterator();
            while(it.next());

            assert.equal(it.getBlockType(), format.MetadataType.VORBIS_COMMENT);
            assert.isTrue(it.getBlock() instanceof metadata.VorbisCommentMetadata);
            assert.isTrue(it.prev());

            assert.equal(it.getBlockType(), format.MetadataType.PADDING);
            assert.isTrue(it.getBlock() instanceof metadata.PaddingMetadata);
            assert.isTrue(it.prev());

            assert.equal(it.getBlockType(), format.MetadataType.PICTURE);
            assert.isTrue(it.getBlock() instanceof metadata.PictureMetadata);
            assert.isTrue(it.prev());

            assert.equal(it.getBlockType(), format.MetadataType.APPLICATION);
            assert.isTrue(it.getBlock() instanceof metadata.ApplicationMetadata);
            assert.isTrue(it.prev());

            assert.equal(it.getBlockType(), format.MetadataType.STREAMINFO);
            assert.isTrue(it.getBlock() instanceof metadata.StreamInfoMetadata);
            assert.isFalse(it.prev());
        });

    });

    describe('sortPadding', function() {

        it('should move padding to the end', function() {
            const filePath = pathForFile('vc-p.flac');
            const ch = new Chain();

            const initRetValue = ch.read(filePath);
            assert.isTrue(initRetValue, Chain.StatusString[ch.status()]);

            ch.sortPadding();

            assert.deepEqual(
                Array.from(ch.createIterator()).map(i => i.type),
                [
                    format.MetadataType.STREAMINFO,
                    format.MetadataType.APPLICATION,
                    format.MetadataType.PICTURE,
                    format.MetadataType.VORBIS_COMMENT,
                    format.MetadataType.PADDING
                ]
            );
        });

    });

    describe('mergePadding', function() {

        it('should merge adjacent padding blocks', function() {
            const filePath = pathForFile('vc-p.flac');
            const ch = new Chain();

            const initRetValue = ch.read(filePath);
            assert.isTrue(initRetValue, Chain.StatusString[ch.status()]);

            const it = ch.createIterator();
            while(it.next());
            it.insertBlockBefore(new metadata.PaddingMetadata(100));

            assert.deepEqual(
                Array.from(ch.createIterator()).map(i => i.type),
                [
                    format.MetadataType.STREAMINFO,
                    format.MetadataType.APPLICATION,
                    format.MetadataType.PICTURE,
                    format.MetadataType.PADDING,
                    format.MetadataType.PADDING,
                    format.MetadataType.VORBIS_COMMENT,
                ]
            );

            ch.sortPadding();

            assert.deepEqual(
                Array.from(ch.createIterator()).map(i => i.type),
                [
                    format.MetadataType.STREAMINFO,
                    format.MetadataType.APPLICATION,
                    format.MetadataType.PICTURE,
                    format.MetadataType.VORBIS_COMMENT,
                    format.MetadataType.PADDING,
                ]
            );
        });

    });

    describe('modify', function() {

        it('setBlock() throws if the first argument is not a Metadata', function() {
            assert.throws(() => new Iterator().setBlock({}));
        });

        it('insertBlockAfter() throws if the first argument is not a Metadata', function() {
            assert.throws(() => new Iterator().insertBlockAfter({}));
        });

        it('insertBlockBefore() throws if the first argument is not a Metadata', function() {
            assert.throws(() => new Iterator().insertBlockBefore({}));
        });

        it('replace StreamInfo block should not replace it', function() {
            const filePath = pathForFile('vc-cs.flac');
            const ch = new Chain();

            const initRetValue = ch.read(filePath);
            assert.isTrue(initRetValue, Chain.StatusString[ch.status()]);

            assert.isFalse(ch.createIterator().setBlock(new metadata.PaddingMetadata()));
            assert.deepEqual(
                Array.from(ch.createIterator()).map(i => i.type),
                [
                    format.MetadataType.STREAMINFO,
                    format.MetadataType.SEEKTABLE,
                    format.MetadataType.VORBIS_COMMENT,
                    format.MetadataType.CUESHEET,
                ]
            );
        });

        it('replace any other block block should replace it', function() {
            const filePath = pathForFile('vc-cs.flac');
            const ch = new Chain();

            const initRetValue = ch.read(filePath);
            assert.isTrue(initRetValue, Chain.StatusString[ch.status()]);

            const it = ch.createIterator();
            assert.isTrue(it.next());
            assert.isTrue(it.next());
            assert.isTrue(it.setBlock(new metadata.PaddingMetadata()));
            assert.equal(it.getBlockType(), format.MetadataType.PADDING);
            assert.deepEqual(
                Array.from(ch.createIterator()).map(i => i.type),
                [
                    format.MetadataType.STREAMINFO,
                    format.MetadataType.SEEKTABLE,
                    format.MetadataType.PADDING,
                    format.MetadataType.CUESHEET,
                ]
            );
        });

        it('delete StreamInfo block should not delete it', function() {
            const filePath = pathForFile('vc-cs.flac');
            const ch = new Chain();

            const initRetValue = ch.read(filePath);
            assert.isTrue(initRetValue, Chain.StatusString[ch.status()]);

            assert.isFalse(ch.createIterator().deleteBlock());
            assert.deepEqual(
                Array.from(ch.createIterator()).map(i => i.type),
                [
                    format.MetadataType.STREAMINFO,
                    format.MetadataType.SEEKTABLE,
                    format.MetadataType.VORBIS_COMMENT,
                    format.MetadataType.CUESHEET,
                ]
            );
        });

        it('delete any other block block should replace it', function() {
            const filePath = pathForFile('vc-cs.flac');
            const ch = new Chain();

            const initRetValue = ch.read(filePath);
            assert.isTrue(initRetValue, Chain.StatusString[ch.status()]);

            const it = ch.createIterator();
            assert.isTrue(it.next());
            assert.isTrue(it.next());
            assert.isTrue(it.deleteBlock(true));
            assert.equal(it.getBlockType(), format.MetadataType.SEEKTABLE);
            assert.deepEqual(
                Array.from(ch.createIterator()).map(i => i.type),
                [
                    format.MetadataType.STREAMINFO,
                    format.MetadataType.SEEKTABLE,
                    format.MetadataType.PADDING,
                    format.MetadataType.CUESHEET,
                ]
            );
        });

        it('insert before a StreamInfo block should not insert it', function() {
            const filePath = pathForFile('vc-cs.flac');
            const ch = new Chain();

            const initRetValue = ch.read(filePath);
            assert.isTrue(initRetValue, Chain.StatusString[ch.status()]);

            assert.isFalse(ch.createIterator().insertBlockBefore(new metadata.ApplicationMetadata()));
            assert.deepEqual(
                Array.from(ch.createIterator()).map(i => i.type),
                [
                    format.MetadataType.STREAMINFO,
                    format.MetadataType.SEEKTABLE,
                    format.MetadataType.VORBIS_COMMENT,
                    format.MetadataType.CUESHEET,
                ]
            );
        });

        it('insert before any other block block should insert it', function() {
            const filePath = pathForFile('vc-cs.flac');
            const ch = new Chain();

            const initRetValue = ch.read(filePath);
            assert.isTrue(initRetValue, Chain.StatusString[ch.status()]);

            const it = ch.createIterator();
            assert.isTrue(it.next());
            assert.isTrue(it.next());
            assert.isTrue(it.insertBlockBefore(new metadata.ApplicationMetadata()));
            assert.equal(it.getBlockType(), format.MetadataType.APPLICATION);
            assert.deepEqual(
                Array.from(ch.createIterator()).map(i => i.type),
                [
                    format.MetadataType.STREAMINFO,
                    format.MetadataType.SEEKTABLE,
                    format.MetadataType.APPLICATION,
                    format.MetadataType.VORBIS_COMMENT,
                    format.MetadataType.CUESHEET,
                ]
            );
        });

        it('insert after any other block block should insert it', function() {
            const filePath = pathForFile('vc-cs.flac');
            const ch = new Chain();

            const initRetValue = ch.read(filePath);
            assert.isTrue(initRetValue, Chain.StatusString[ch.status()]);

            const it = ch.createIterator();
            assert.isTrue(it.next());
            assert.isTrue(it.next());
            assert.isTrue(it.insertBlockAfter(new metadata.ApplicationMetadata()));
            assert.equal(it.getBlockType(), format.MetadataType.APPLICATION);
            assert.deepEqual(
                Array.from(ch.createIterator()).map(i => i.type),
                [
                    format.MetadataType.STREAMINFO,
                    format.MetadataType.SEEKTABLE,
                    format.MetadataType.VORBIS_COMMENT,
                    format.MetadataType.APPLICATION,
                    format.MetadataType.CUESHEET,
                ]
            );
        });

    });

    describe('write', function() {

        let tmpFile;
        beforeEach('createTemporaryFiles', function() {
            tmpFile = temp.openSync('flac-bindings.metadata2.chain-iterator');
            oldfs.copyFileSync(pathForFile('no.flac'), tmpFile.path);
        });

        afterEach('cleanUpTemporaryFiles', function() {
            temp.cleanupSync();
        });

        it('modify the blocks and write should modify the file correctly', function() {
            const ch = new Chain();
            const initRetValue = ch.read(tmpFile.path);
            assert.isTrue(initRetValue, Chain.StatusString[ch.status()]);
            const it = ch.createIterator();

            const vc = new metadata.VorbisCommentMetadata();
            vc.vendorString = 'flac-bindings 2.0.0';
            assert.isTrue(it.insertBlockAfter(vc));

            assert.isTrue(it.insertBlockAfter(new metadata.PaddingMetadata(50)));
            assert.isTrue(it.insertBlockAfter(new metadata.ApplicationMetadata()));
            assert.isTrue(it.next());

            assert.isTrue(ch.write(false));

            assert.deepEqual(
                Array.from(ch.createIterator()).map(i => i.type),
                [
                    format.MetadataType.STREAMINFO,
                    format.MetadataType.VORBIS_COMMENT,
                    format.MetadataType.PADDING,
                    format.MetadataType.APPLICATION,
                    format.MetadataType.PADDING,
                ]
            );
        });

    });

    describe('gc', function() {
        it('gc should work', function() {
            require('./helper/gc')();
        });
    });

});