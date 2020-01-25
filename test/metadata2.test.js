/* eslint-disable prefer-arrow-callback */
const { Chain, Iterator, metadata, format } = require('../lib/index').api;
const { assert, use } = require('chai');
const { promises: fs, ...oldfs } = require('fs');
const temp = require('temp').track();
const {
    pathForFile: { tags: pathForFile },
    generateFlacCallbacks,
} = require('./helper');

temp.track();

use(require('./helper').asyncChaiExtensions);

describe('Chain & Iterator', function() {

    describe('read', function() {

        it('throws if the first argument is not a Metadata', function() {
            assert.throws(() => new Chain().read({}), /Expected .+? to be string/);
        });

        it('throws if the first argument is not a Metadata (ogg version)', function() {
            assert.throws(() => new Chain().readOgg(() => 1), /Expected .+? to be string/);
        });

        it('returns false if the file does not exist', async function() {
            const filePath = pathForFile('el.flac');
            const ch = new Chain();

            const ret = ch.read(filePath);

            assert.isFalse(ret);
            assert.equal(ch.status(), Chain.Status.ERROR_OPENING_FILE);
            await assert.throwsAsync(() => fs.access(filePath), /^ENOENT: no such file or directory/);
        });

        it('returns true if the file exists', async function() {
            const filePath = pathForFile('no.flac');
            const ch = new Chain();

            const ret = ch.read(filePath);

            assert.isTrue(ret, Chain.StatusString[ch.status()]);
            await fs.access(filePath);
        });

    });

    describe('readAsync', function() {

        it('throws if the first argument is not a string', async function() {
            await assert.throwsAsync(() => new Chain().readAsync({}), /Expected .+? to be string/);
        });

        it('throws if the first argument is not a string (ogg version)', async function() {
            await assert.throwsAsync(() => new Chain().readOggAsync(() => 1), /Expected .+? to be string/);
        });

        it('throws if the file does not exist', async function() {
            const filePath = pathForFile('el.flac');
            const ch = new Chain();

            assert.isFalse(await ch.readAsync(filePath));

            assert.equal(Chain.StatusString[ch.status()], 'ERROR_OPENING_FILE');
            await assert.throwsAsync(() => fs.access(filePath), /^ENOENT: no such file or directory/);
        });

        it('returns true if the file exists', async function() {
            const filePath = pathForFile('no.flac');
            const ch = new Chain();

            const ret = await ch.readAsync(filePath);

            assert.isTrue(ret, Chain.StatusString[ch.status()]);
            await fs.access(filePath);
        });

    });

    describe('readWithCallbacks', function() {

        it('throws if the first argument is not an object', async function() {
            await assert.throwsAsync(() => new Chain().readWithCallbacks(7), /Expected .+? to be object/);
        });

        it('throws if the first argument is not an object (ogg version)', async function() {
            await assert.throwsAsync(() => new Chain().readOggWithCallbacks(7), /Expected .+? to be object/);
        });

        it('throws if the lacks callbacks (flac version)', async function() {
            assert.isFalse(await new Chain().readWithCallbacks({}));
        });

        it('throws if the lacks callbacks (ogg version)', async function() {
            assert.isFalse(await new Chain().readOggWithCallbacks({}));
        });

        it('returns works if the file can be read', async function() {
            const callbacks = await generateFlacCallbacks.flacio(pathForFile('vc-cs.flac'), 'r');
            const chain = new Chain();
            await chain.readWithCallbacks(callbacks)
                .finally(() => callbacks.close());
        });

        it('it throws if the file cannot be read', async function() {
            const chain = new Chain();
            assert.isFalse(await chain.readWithCallbacks({
                read: () => 0,
                seek: () => -1,
                tell: () => BigInt(0),
                close: () => undefined,
            }));
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
            // eslint-disable-next-line curly
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
                Array.from(ch.createIterator()).map((i) => i.type),
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

    describe('mergePadding', function() {

        it('should merge adjacent padding blocks', function() {
            const filePath = pathForFile('vc-p.flac');
            const ch = new Chain();

            const initRetValue = ch.read(filePath);
            assert.isTrue(initRetValue, Chain.StatusString[ch.status()]);

            const it = ch.createIterator();
            // eslint-disable-next-line curly
            while(it.next());
            it.insertBlockBefore(new metadata.PaddingMetadata(100));

            assert.deepEqual(
                Array.from(ch.createIterator()).map((i) => i.type),
                [
                    format.MetadataType.STREAMINFO,
                    format.MetadataType.APPLICATION,
                    format.MetadataType.PICTURE,
                    format.MetadataType.PADDING,
                    format.MetadataType.PADDING,
                    format.MetadataType.VORBIS_COMMENT,
                ]
            );

            ch.mergePadding();

            assert.deepEqual(
                Array.from(ch.createIterator()).map((i) => i.type),
                [
                    format.MetadataType.STREAMINFO,
                    format.MetadataType.APPLICATION,
                    format.MetadataType.PICTURE,
                    format.MetadataType.PADDING,
                    format.MetadataType.VORBIS_COMMENT,
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
                Array.from(ch.createIterator()).map((i) => i.type),
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
                Array.from(ch.createIterator()).map((i) => i.type),
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
                Array.from(ch.createIterator()).map((i) => i.type),
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
                Array.from(ch.createIterator()).map((i) => i.type),
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
                Array.from(ch.createIterator()).map((i) => i.type),
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
                Array.from(ch.createIterator()).map((i) => i.type),
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
                Array.from(ch.createIterator()).map((i) => i.type),
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

        it('modify the blocks and write should modify the file correctly (sync)', function() {
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
                Array.from(ch.createIterator()).map((i) => i.type),
                [
                    format.MetadataType.STREAMINFO,
                    format.MetadataType.VORBIS_COMMENT,
                    format.MetadataType.PADDING,
                    format.MetadataType.APPLICATION,
                    format.MetadataType.PADDING,
                ]
            );
        });

        it('modify the blocks and write should modify the file correctly (async)', async function() {
            const ch = new Chain();
            await ch.readAsync(tmpFile.path);
            const it = ch.createIterator();

            const vc = new metadata.VorbisCommentMetadata();
            vc.vendorString = 'flac-bindings 2.0.0';
            assert.isTrue(it.insertBlockAfter(vc));

            assert.isTrue(it.insertBlockAfter(new metadata.PaddingMetadata(50)));
            assert.isTrue(it.insertBlockAfter(new metadata.ApplicationMetadata()));
            assert.isTrue(it.next());

            assert.isTrue(await ch.writeAsync(false));

            assert.deepEqual(
                Array.from(ch.createIterator()).map((i) => i.type),
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

    describe('other', function() {
        it('checkIfTempFileIsNeeded() should work', async function() {
            const filePath = pathForFile('vc-cs.flac');
            const ch = new Chain();
            await ch.readAsync(filePath);

            assert.isFalse(ch.checkIfTempFileIsNeeded());
        });
    });

    describe('gc', function() {
        it('gc should work', function() {
            require('./helper/gc')();
        });
    });

});
