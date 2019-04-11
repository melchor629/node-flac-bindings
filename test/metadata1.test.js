/// <reference path="../lib/index.d.ts" />
const { SimpleIterator, metadata, format } = require('../lib/index').api;
const { assert } = require('chai');
const { promises: fs, ...oldfs } = require('fs');
const path = require('path');
const temp = require('temp');

temp.track();

const pathForFile = (...file) => path.join(__dirname, 'data', 'tags', ...file);

describe('SimpleIterator', function() {

    describe('init', function() {

        it('returns false if the file does not exist', async function() {
            const filePath = pathForFile('el.flac');
            const it = new SimpleIterator();

            const ret = it.init(filePath);

            assert.isFalse(ret);
            assert.equal(it.status(), SimpleIterator.Status.ERROR_OPENING_FILE);
            let hasThrown = false;
            try { await fs.access(filePath); } catch(e) { hasThrown = true; }
            assert.isTrue(hasThrown, 'The file must not exist');
        });

        it('returns true if the file exists', async function() {
            const filePath = pathForFile('no.flac');
            const it = new SimpleIterator();

            const ret = it.init(filePath);

            assert.isTrue(ret, SimpleIterator.StatusString[it.status()]);
            assert.isTrue(it.isWritable());
            await fs.access(filePath);
        });

    });

    describe('iterator', function() {
        it('should iterate over all blocks', function() {
            const filePath = pathForFile('vc-cs.flac');
            const it = new SimpleIterator();

            const initRetValue = it.init(filePath);
            assert.isTrue(initRetValue, SimpleIterator.StatusString[it.status()]);
            const e = it[Symbol.iterator]();

            let m = e.next();
            assert.isFalse(m.done);
            assert.equal(m.value.type, format.MetadataType.STREAMINFO);

            m = e.next();
            assert.isFalse(m.done);
            assert.equal(m.value.type, format.MetadataType.SEEKTABLE);

            m = e.next();
            assert.isFalse(m.done);
            assert.equal(m.value.type, format.MetadataType.VORBIS_COMMENT);

            m = e.next();
            assert.isFalse(m.done);
            assert.equal(m.value.type, format.MetadataType.CUESHEET);

            m = e.next();
            assert.isTrue(m.done);
            assert.isUndefined(m.value);
        });
    });

    describe('iterate and get*', function() {

        it('should iterate forwards and get info about them correctly', function() {
            const filePath = pathForFile('vc-p.flac');
            const it = new SimpleIterator();

            const initRetValue = it.init(filePath);
            assert.isTrue(initRetValue, SimpleIterator.StatusString[it.status()]);

            assert.isFalse(it.isLast());
            assert.equal(it.getBlockOffset(), 4);
            assert.equal(it.getBlockLength(), 34);
            assert.equal(it.getBlockType(), format.MetadataType.STREAMINFO);
            assert.isTrue(it.getBlock() instanceof metadata.StreamInfoMetadata);

            assert.isTrue(it.next());
            assert.isFalse(it.isLast());
            assert.equal(it.getBlockOffset(), 42);
            assert.equal(it.getBlockLength(), 25);
            assert.equal(it.getBlockType(), format.MetadataType.APPLICATION);
            assert.isTrue(it.getBlock() instanceof metadata.ApplicationMetadata);

            assert.isTrue(it.next());
            assert.isFalse(it.isLast());
            assert.equal(it.getBlockOffset(), 71);
            assert.equal(it.getBlockLength(), 17142);
            assert.equal(it.getBlockType(), format.MetadataType.PICTURE);
            assert.isTrue(it.getBlock() instanceof metadata.PictureMetadata);

            assert.isTrue(it.next());
            assert.isFalse(it.isLast());
            assert.equal(it.getBlockOffset(), 17217);
            assert.equal(it.getBlockLength(), 136);
            assert.equal(it.getBlockType(), format.MetadataType.PADDING);
            assert.isTrue(it.getBlock() instanceof metadata.PaddingMetadata);

            assert.isTrue(it.next());
            assert.isTrue(it.isLast());
            assert.equal(it.getBlockOffset(), 17357);
            assert.equal(it.getBlockLength(), 165);
            assert.equal(it.getBlockType(), format.MetadataType.VORBIS_COMMENT);
            assert.isTrue(it.getBlock() instanceof metadata.VorbisCommentMetadata);

            assert.isFalse(it.next());
        });

        it('should iterate backwards and get info about them correctly', function() {
            const filePath = pathForFile('vc-p.flac');
            const it = new SimpleIterator();

            const initRetValue = it.init(filePath);
            assert.isTrue(initRetValue, SimpleIterator.StatusString[it.status()]);
            while(it.next());

            assert.isTrue(it.isLast());
            assert.equal(it.getBlockOffset(), 17357);
            assert.equal(it.getBlockLength(), 165);
            assert.equal(it.getBlockType(), format.MetadataType.VORBIS_COMMENT);
            assert.isTrue(it.getBlock() instanceof metadata.VorbisCommentMetadata);
            assert.isTrue(it.prev());

            assert.isFalse(it.isLast());
            assert.equal(it.getBlockOffset(), 17217);
            assert.equal(it.getBlockLength(), 136);
            assert.equal(it.getBlockType(), format.MetadataType.PADDING);
            assert.isTrue(it.getBlock() instanceof metadata.PaddingMetadata);
            assert.isTrue(it.prev());

            assert.isFalse(it.isLast());
            assert.equal(it.getBlockOffset(), 71);
            assert.equal(it.getBlockLength(), 17142);
            assert.equal(it.getBlockType(), format.MetadataType.PICTURE);
            assert.isTrue(it.getBlock() instanceof metadata.PictureMetadata);
            assert.isTrue(it.prev());

            assert.isFalse(it.isLast());
            assert.equal(it.getBlockOffset(), 42);
            assert.equal(it.getBlockLength(), 25);
            assert.equal(it.getBlockType(), format.MetadataType.APPLICATION);
            assert.isTrue(it.getBlock() instanceof metadata.ApplicationMetadata);
            assert.isTrue(it.prev());

            assert.isFalse(it.isLast());
            assert.equal(it.getBlockOffset(), 4);
            assert.equal(it.getBlockLength(), 34);
            assert.equal(it.getBlockType(), format.MetadataType.STREAMINFO);
            assert.isTrue(it.getBlock() instanceof metadata.StreamInfoMetadata);
            assert.isFalse(it.prev());
        });

    });

    describe('modify', function() {

        let tmpFile;
        beforeEach('createTemporaryFiles', function() {
            tmpFile = temp.openSync('flac-bindings.metadata1.simpleiterator');
            oldfs.copyFileSync(pathForFile('no.flac'), tmpFile.path);
        });

        afterEach('cleanUpTemporaryFiles', function() {
            temp.cleanupSync();
        });

        it('replace StreamInfo block should not replace it', function() {
            const it = new SimpleIterator();

            const initRetValue = it.init(tmpFile.path);
            assert.isTrue(initRetValue, SimpleIterator.StatusString[it.status()]);

            assert.isFalse(it.setBlock(new metadata.ApplicationMetadata()));
        });

        it('replace any block should effectively replace it', function() {
            const it = new SimpleIterator();

            const initRetValue = it.init(tmpFile.path);
            assert.isTrue(initRetValue, SimpleIterator.StatusString[it.status()]);

            assert.isTrue(it.next());
            const app = new metadata.ApplicationMetadata();
            app.id = Buffer.from('node');
            app.data = Buffer.from('A Wonderful Adventure');
            assert.isTrue(it.setBlock(app));

            const ot = new SimpleIterator();
            assert.isTrue(ot.init(tmpFile.path));
            assert.isTrue(ot.next());
            assert.equal(ot.getBlockType(), format.MetadataType.APPLICATION);
            assert.deepEqual(ot.getApplicationId(), Array.from(Buffer.from('node')));
        });

        it('insert any block should effectively insert it', function() {
            const it = new SimpleIterator();

            const initRetValue = it.init(tmpFile.path);
            assert.isTrue(initRetValue, SimpleIterator.StatusString[it.status()]);

            assert.isTrue(it.next());
            const app = new metadata.ApplicationMetadata();
            app.id = Buffer.from('node');
            app.data = Buffer.from('A Wonderful Adventure');
            assert.isTrue(it.insertBlockAfter(app));

            const ot = new SimpleIterator();
            assert.isTrue(ot.init(tmpFile.path));
            assert.isTrue(ot.next());
            assert.isTrue(ot.next());
            assert.equal(ot.getBlockType(), format.MetadataType.APPLICATION);
            assert.deepEqual(ot.getApplicationId(), Array.from(Buffer.from('node')));
        });

        it('delete StreamInfo block should not delete it', function() {
            const it = new SimpleIterator();

            const initRetValue = it.init(tmpFile.path);
            assert.isTrue(initRetValue, SimpleIterator.StatusString[it.status()]);

            assert.isFalse(it.deleteBlock());
        });

        it('delete any other block should effectively delete it', function() {
            const it = new SimpleIterator();

            const initRetValue = it.init(tmpFile.path);
            assert.isTrue(initRetValue, SimpleIterator.StatusString[it.status()]);

            assert.isTrue(it.next());
            assert.isTrue(it.deleteBlock());
        });

    });

    describe('gc', function() {
        it('gc should work', function() {
            require('./gc')();
        });
    });

});