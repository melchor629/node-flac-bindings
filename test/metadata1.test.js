/// <reference path="../lib/index.d.ts" />
const { SimpleIterator, metadata, format } = require('../lib/index').api;
const { assert, use } = require('chai');
const { promises: fs, ...oldfs } = require('fs');
const path = require('path');
const temp = require('temp');

temp.track();
use(require('./helper/async-chai-extensions.js'));

const pathForFile = (...file) => path.join(__dirname, 'data', 'tags', ...file);

describe('SimpleIterator', function() {

    describe('init', function() {

        it('throws if the first argument is not a string', function() {
            assert.throws(() => new SimpleIterator().init(8));
        });

        it('returns false if the file does not exist', async function() {
            const filePath = pathForFile('el.flac');
            const it = new SimpleIterator();

            const ret = it.init(filePath);

            assert.isFalse(ret);
            assert.equal(it.status(), SimpleIterator.Status.ERROR_OPENING_FILE);
            await assert.throwsAsync(() => fs.access(filePath));
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

    describe('initAsync', function() {

        it('throws if the first argument is not a string', async function() {
            await assert.throwsAsync(() => new SimpleIterator().initAsync(8));
        });

        it('throws if the file does not exist', async function() {
            const filePath = pathForFile('el.flac');
            const it = new SimpleIterator();

            await assert.throwsAsync(() => it.initAsync(filePath));

            await assert.throwsAsync(() => fs.access(filePath));
        });

        it('returns true if the file exists', async function() {
            const filePath = pathForFile('no.flac');
            const it = new SimpleIterator();

            const ret = await it.initAsync(filePath);

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

    describe('asyncIterator', function() {
        it('should iterate over all blocks', async function() {
            const filePath = pathForFile('vc-cs.flac');
            const it = new SimpleIterator();

            await it.initAsync(filePath);
            const e = it[Symbol.asyncIterator]();

            let m = await e.next();
            assert.isFalse(m.done);
            assert.equal(m.value.type, format.MetadataType.STREAMINFO);

            m = await e.next();
            assert.isFalse(m.done);
            assert.equal(m.value.type, format.MetadataType.SEEKTABLE);

            m = await e.next();
            assert.isFalse(m.done);
            assert.equal(m.value.type, format.MetadataType.VORBIS_COMMENT);

            m = await e.next();
            assert.isFalse(m.done);
            assert.equal(m.value.type, format.MetadataType.CUESHEET);

            m = await e.next();
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
            assert.instanceOf(it.getBlock(), metadata.StreamInfoMetadata);

            assert.isTrue(it.next());
            assert.isFalse(it.isLast());
            assert.equal(it.getBlockOffset(), 42);
            assert.equal(it.getBlockLength(), 25);
            assert.equal(it.getBlockType(), format.MetadataType.APPLICATION);
            assert.instanceOf(it.getBlock(), metadata.ApplicationMetadata);

            assert.isTrue(it.next());
            assert.isFalse(it.isLast());
            assert.equal(it.getBlockOffset(), 71);
            assert.equal(it.getBlockLength(), 17142);
            assert.equal(it.getBlockType(), format.MetadataType.PICTURE);
            assert.instanceOf(it.getBlock(), metadata.PictureMetadata);

            assert.isTrue(it.next());
            assert.isFalse(it.isLast());
            assert.equal(it.getBlockOffset(), 17217);
            assert.equal(it.getBlockLength(), 136);
            assert.equal(it.getBlockType(), format.MetadataType.PADDING);
            assert.instanceOf(it.getBlock(), metadata.PaddingMetadata);

            assert.isTrue(it.next());
            assert.isTrue(it.isLast());
            assert.equal(it.getBlockOffset(), 17357);
            assert.equal(it.getBlockLength(), 165);
            assert.equal(it.getBlockType(), format.MetadataType.VORBIS_COMMENT);
            assert.instanceOf(it.getBlock(), metadata.VorbisCommentMetadata);

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
            assert.instanceOf(it.getBlock(), metadata.VorbisCommentMetadata);
            assert.isTrue(it.prev());

            assert.isFalse(it.isLast());
            assert.equal(it.getBlockOffset(), 17217);
            assert.equal(it.getBlockLength(), 136);
            assert.equal(it.getBlockType(), format.MetadataType.PADDING);
            assert.instanceOf(it.getBlock(), metadata.PaddingMetadata);
            assert.isTrue(it.prev());

            assert.isFalse(it.isLast());
            assert.equal(it.getBlockOffset(), 71);
            assert.equal(it.getBlockLength(), 17142);
            assert.equal(it.getBlockType(), format.MetadataType.PICTURE);
            assert.instanceOf(it.getBlock(), metadata.PictureMetadata);
            assert.isTrue(it.prev());

            assert.isFalse(it.isLast());
            assert.equal(it.getBlockOffset(), 42);
            assert.equal(it.getBlockLength(), 25);
            assert.equal(it.getBlockType(), format.MetadataType.APPLICATION);
            assert.instanceOf(it.getBlock(), metadata.ApplicationMetadata);
            assert.isTrue(it.prev());

            assert.isFalse(it.isLast());
            assert.equal(it.getBlockOffset(), 4);
            assert.equal(it.getBlockLength(), 34);
            assert.equal(it.getBlockType(), format.MetadataType.STREAMINFO);
            assert.instanceOf(it.getBlock(), metadata.StreamInfoMetadata);
            assert.isFalse(it.prev());
        });

    });


    describe('async iterate and get*Async', function() {

        it('should iterate forwards and get info about them correctly', async function() {
            const filePath = pathForFile('vc-p.flac');
            const it = new SimpleIterator();

            await it.initAsync(filePath);

            assert.isFalse(it.isLast());
            assert.equal(it.getBlockOffset(), 4);
            assert.equal(it.getBlockLength(), 34);
            assert.equal(it.getBlockType(), format.MetadataType.STREAMINFO);
            assert.instanceOf(await it.getBlockAsync(), metadata.StreamInfoMetadata);

            assert.isTrue(await it.nextAsync());
            assert.isFalse(it.isLast());
            assert.equal(it.getBlockOffset(), 42);
            assert.equal(it.getBlockLength(), 25);
            assert.equal(it.getBlockType(), format.MetadataType.APPLICATION);
            assert.instanceOf(await it.getBlockAsync(), metadata.ApplicationMetadata);

            assert.isTrue(await it.nextAsync());
            assert.isFalse(it.isLast());
            assert.equal(it.getBlockOffset(), 71);
            assert.equal(it.getBlockLength(), 17142);
            assert.equal(it.getBlockType(), format.MetadataType.PICTURE);
            assert.instanceOf(await it.getBlockAsync(), metadata.PictureMetadata);

            assert.isTrue(await it.nextAsync());
            assert.isFalse(it.isLast());
            assert.equal(it.getBlockOffset(), 17217);
            assert.equal(it.getBlockLength(), 136);
            assert.equal(it.getBlockType(), format.MetadataType.PADDING);
            assert.instanceOf(await it.getBlockAsync(), metadata.PaddingMetadata);

            assert.isTrue(await it.nextAsync());
            assert.isTrue(it.isLast());
            assert.equal(it.getBlockOffset(), 17357);
            assert.equal(it.getBlockLength(), 165);
            assert.equal(it.getBlockType(), format.MetadataType.VORBIS_COMMENT);
            assert.instanceOf(await it.getBlockAsync(), metadata.VorbisCommentMetadata);

            assert.isFalse(await it.nextAsync());
        });

        it('should iterate backwards and get info about them correctly', async function() {
            const filePath = pathForFile('vc-p.flac');
            const it = new SimpleIterator();

            await it.initAsync(filePath);
            while(await it.nextAsync());

            assert.isTrue(it.isLast());
            assert.equal(it.getBlockOffset(), 17357);
            assert.equal(it.getBlockLength(), 165);
            assert.equal(it.getBlockType(), format.MetadataType.VORBIS_COMMENT);
            assert.instanceOf(await it.getBlockAsync(), metadata.VorbisCommentMetadata);
            assert.isTrue(await it.prevAsync());

            assert.isFalse(it.isLast());
            assert.equal(it.getBlockOffset(), 17217);
            assert.equal(it.getBlockLength(), 136);
            assert.equal(it.getBlockType(), format.MetadataType.PADDING);
            assert.instanceOf(await it.getBlockAsync(), metadata.PaddingMetadata);
            assert.isTrue(await it.prevAsync());

            assert.isFalse(it.isLast());
            assert.equal(it.getBlockOffset(), 71);
            assert.equal(it.getBlockLength(), 17142);
            assert.equal(it.getBlockType(), format.MetadataType.PICTURE);
            assert.instanceOf(await it.getBlockAsync(), metadata.PictureMetadata);
            assert.isTrue(await it.prevAsync());

            assert.isFalse(it.isLast());
            assert.equal(it.getBlockOffset(), 42);
            assert.equal(it.getBlockLength(), 25);
            assert.equal(it.getBlockType(), format.MetadataType.APPLICATION);
            assert.instanceOf(await it.getBlockAsync(), metadata.ApplicationMetadata);
            assert.isTrue(await it.prevAsync());

            assert.isFalse(it.isLast());
            assert.equal(it.getBlockOffset(), 4);
            assert.equal(it.getBlockLength(), 34);
            assert.equal(it.getBlockType(), format.MetadataType.STREAMINFO);
            assert.instanceOf(await it.getBlockAsync(), metadata.StreamInfoMetadata);
            assert.isFalse(await it.prevAsync());
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

        it('setBlock() throws if the first argument is not a Metadata', function() {
            assert.throws(() => new SimpleIterator().setBlock({}));
        });

        it('insertBlockAfter() throws if the first argument is not a Metadata', function() {
            assert.throws(() => new SimpleIterator().insertBlockAfter({}));
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
            assert.deepEqual(ot.getApplicationId(), Buffer.from('node'));
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
            assert.deepEqual(ot.getApplicationId(), Buffer.from('node'));
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

    describe('async modify', function() {

        let tmpFile;
        beforeEach('createTemporaryFiles', function() {
            tmpFile = temp.openSync('flac-bindings.metadata1.simpleiterator');
            oldfs.copyFileSync(pathForFile('no.flac'), tmpFile.path);
        });

        afterEach('cleanUpTemporaryFiles', function() {
            temp.cleanupSync();
        });

        it('setBlock() throws if the first argument is not a Metadata', async function() {
            await assert.throwsAsync(() => new SimpleIterator().setBlockAsync({}));
        });

        it('insertBlockAfter() throws if the first argument is not a Metadata', async function() {
            await assert.throwsAsync(() => new SimpleIterator().insertBlockAfterAsync({}));
        });

        it('replace StreamInfo block should not replace it', async function() {
            const it = new SimpleIterator();

            it.initAsync(tmpFile.path);

            await assert.throwsAsync(() => it.setBlockAsync(new metadata.ApplicationMetadata()));
        });

        it('replace any block should effectively replace it', async function() {
            const it = new SimpleIterator();

            await it.initAsync(tmpFile.path);

            assert.isTrue(await it.nextAsync());
            const app = new metadata.ApplicationMetadata();
            app.id = Buffer.from('node');
            app.data = Buffer.from('A Wonderful Adventure');
            assert.isTrue(await it.setBlockAsync(app));

            const ot = new SimpleIterator();
            assert.isTrue(await ot.initAsync(tmpFile.path));
            assert.isTrue(await ot.nextAsync());
            assert.equal(ot.getBlockType(), format.MetadataType.APPLICATION);
            assert.deepEqual(await ot.getApplicationIdAsync(), Buffer.from('node'));
        });

        it('insert any block should effectively insert it', async function() {
            const it = new SimpleIterator();

            await it.initAsync(tmpFile.path);

            assert.isTrue(await it.nextAsync());
            const app = new metadata.ApplicationMetadata();
            app.id = Buffer.from('node');
            app.data = Buffer.from('A Wonderful Adventure');
            assert.isTrue(await it.insertBlockAfterAsync(app));

            const ot = new SimpleIterator();
            assert.isTrue(await ot.initAsync(tmpFile.path));
            assert.isTrue(await ot.nextAsync());
            assert.isTrue(await ot.nextAsync());
            assert.equal(ot.getBlockType(), format.MetadataType.APPLICATION);
            assert.deepEqual(await ot.getApplicationIdAsync(), Buffer.from('node'));
        });

        it('delete StreamInfo block should not delete it', async function() {
            const it = new SimpleIterator();

            await it.initAsync(tmpFile.path);

            await assert.throwsAsync(() => it.deleteBlockAsync());
        });

        it('delete any other block should effectively delete it', async function() {
            const it = new SimpleIterator();

            await it.initAsync(tmpFile.path);

            assert.isTrue(await it.nextAsync());
            assert.isTrue(await it.deleteBlockAsync());
        });

    });

    describe('gc', function() {
        it('gc should work', function() {
            require('./helper/gc')();
        });
    });

});