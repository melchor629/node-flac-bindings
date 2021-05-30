const { promises: fs, ...oldfs } = require('fs')
const temp = require('temp').track()
const {
  Chain, Iterator, metadata, format,
} = require('../lib/index').api
const {
  pathForFile: { tags: pathForFile },
  generateFlacCallbacks,
  gc,
} = require('./helper')

describe('Chain & Iterator', () => {
  describe('read', () => {
    it('throws if the first argument is not a Metadata', () => {
      expect(() => new Chain().read({})).toThrow()
    })

    it('throws if the first argument is not a Metadata (ogg version)', () => {
      expect(() => new Chain().readOgg(() => 1)).toThrow()
    })

    it('returns false if the file does not exist', async () => {
      const filePath = pathForFile('el.flac')
      const ch = new Chain()

      expect(() => ch.read(filePath)).toThrow()

      await expect(() => fs.access(filePath)).rejects.toThrow(/^ENOENT: no such file or directory/)
    })

    it('returns true if the file exists', async () => {
      const filePath = pathForFile('no.flac')
      const ch = new Chain()

      ch.read(filePath)

      expect(ch.status()).toEqual(Chain.Status.OK)
      await fs.access(filePath)
    })
  })

  describe('readAsync', () => {
    it('throws if the first argument is not a string', () => {
      expect(() => new Chain().readAsync({})).toThrow(/Expected .+? to be string/)
    })

    it('throws if the first argument is not a string (ogg version)', () => {
      expect(() => new Chain().readOggAsync(() => 1)).toThrow(/Expected .+? to be string/)
    })

    it('throws if the file does not exist', async () => {
      const filePath = pathForFile('el.flac')
      const ch = new Chain()

      await expect(() => ch.readAsync(filePath)).rejects.toThrow(/Chain operation failed: ERROR_OPENING_FILE/)

      await expect(() => fs.access(filePath)).rejects.toThrow(/^ENOENT: no such file or directory/)
    })

    it('returns true if the file exists', async () => {
      const filePath = pathForFile('no.flac')
      const ch = new Chain()

      await ch.readAsync(filePath)

      expect(ch.status()).toEqual(Chain.Status.OK)
      await fs.access(filePath)
    })
  })

  describe('readWithCallbacks', () => {
    it('throws if the first argument is not an object', () => {
      expect(() => new Chain().readWithCallbacks(7)).toThrow(/Expected .+? to be object/)
    })

    it('throws if the first argument is not an object (ogg version)', () => {
      expect(() => new Chain().readOggWithCallbacks(7)).toThrow(/Expected .+? to be object/)
    })

    it('throws if the lacks callbacks (flac version)', async () => {
      await expect(() => new Chain().readWithCallbacks({})).rejects.toThrow(/Chain operation failed: INVALID_CALLBACKS/)
    })

    it('throws if the lacks callbacks (ogg version)', async () => {
      await expect(() => new Chain().readOggWithCallbacks({})).rejects.toThrow(/Chain operation failed: INVALID_CALLBACKS/)
    })

    it('returns works if the file can be read', async () => {
      const callbacks = await generateFlacCallbacks.flacio(pathForFile('vc-cs.flac'), 'r')
      const chain = new Chain()
      await expect(
        chain.readWithCallbacks(callbacks)
          .finally(() => callbacks.close()),
      ).resolves.not.toThrow()
    })

    it('throws if the file cannot be read', async () => {
      const chain = new Chain()
      await expect(() => chain.readWithCallbacks({
        read: () => 0,
        seek: () => -1,
        tell: () => BigInt(0),
        close: () => undefined,
      })).rejects.toThrow(/Chain operation failed: SEEK_ERROR/)
    })
  })

  describe('iterator', () => {
    it('createIterator() returns an iterator and iterates over the blocks correctly', () => {
      const filePath = pathForFile('vc-p.flac')
      const ch = new Chain()

      ch.read(filePath)

      const it = ch.createIterator()
      const i = it[Symbol.iterator]()

      let m = i.next()
      expect(m.done).toBe(false)
      expect(m.value.type).toEqual(format.MetadataType.STREAMINFO)

      m = i.next()
      expect(m.done).toBe(false)
      expect(m.value.type).toEqual(format.MetadataType.APPLICATION)

      m = i.next()
      expect(m.done).toBe(false)
      expect(m.value.type).toEqual(format.MetadataType.PICTURE)

      m = i.next()
      expect(m.done).toBe(false)
      expect(m.value.type).toEqual(format.MetadataType.PADDING)

      m = i.next()
      expect(m.done).toBe(false)
      expect(m.value.type).toEqual(format.MetadataType.VORBIS_COMMENT)

      m = i.next()
      expect(m.done).toBe(true)
    })

    it('new Iterator() creates an iterator from the chain and iterates over the blocks correctly', () => {
      const filePath = pathForFile('vc-p.flac')
      const ch = new Chain()
      const it = new Iterator()

      ch.read(filePath)

      it.init(ch)
      const i = it[Symbol.iterator]()

      let m = i.next()
      expect(m.done).toBe(false)
      expect(m.value.type).toEqual(format.MetadataType.STREAMINFO)

      m = i.next()
      expect(m.done).toBe(false)
      expect(m.value.type).toEqual(format.MetadataType.APPLICATION)

      m = i.next()
      expect(m.done).toBe(false)
      expect(m.value.type).toEqual(format.MetadataType.PICTURE)

      m = i.next()
      expect(m.done).toBe(false)
      expect(m.value.type).toEqual(format.MetadataType.PADDING)

      m = i.next()
      expect(m.done).toBe(false)
      expect(m.value.type).toEqual(format.MetadataType.VORBIS_COMMENT)

      m = i.next()
      expect(m.done).toBe(true)
    })
  })

  describe('iterator and get*', () => {
    it('should iterate forwards and get info about them correctly', () => {
      const filePath = pathForFile('vc-p.flac')
      const ch = new Chain()

      ch.read(filePath)

      const it = ch.createIterator()
      expect(it.getBlockType()).toEqual(format.MetadataType.STREAMINFO)
      expect(it.getBlock() instanceof metadata.StreamInfoMetadata).toBe(true)

      expect(it.next()).toBe(true)
      expect(it.getBlockType()).toEqual(format.MetadataType.APPLICATION)
      expect(it.getBlock() instanceof metadata.ApplicationMetadata).toBe(true)

      expect(it.next()).toBe(true)
      expect(it.getBlockType()).toEqual(format.MetadataType.PICTURE)
      expect(it.getBlock() instanceof metadata.PictureMetadata).toBe(true)

      expect(it.next()).toBe(true)
      expect(it.getBlockType()).toEqual(format.MetadataType.PADDING)
      expect(it.getBlock() instanceof metadata.PaddingMetadata).toBe(true)

      expect(it.next()).toBe(true)
      expect(it.getBlockType()).toEqual(format.MetadataType.VORBIS_COMMENT)
      expect(it.getBlock() instanceof metadata.VorbisCommentMetadata).toBe(true)

      expect(it.next()).toBe(false)
    })

    it('should iterate backwards and get info about them correctly', () => {
      const filePath = pathForFile('vc-p.flac')
      const ch = new Chain()

      ch.read(filePath)

      const it = ch.createIterator()
      // eslint-disable-next-line curly
      while (it.next());

      expect(it.getBlockType()).toEqual(format.MetadataType.VORBIS_COMMENT)
      expect(it.getBlock() instanceof metadata.VorbisCommentMetadata).toBe(true)
      expect(it.prev()).toBe(true)

      expect(it.getBlockType()).toEqual(format.MetadataType.PADDING)
      expect(it.getBlock() instanceof metadata.PaddingMetadata).toBe(true)
      expect(it.prev()).toBe(true)

      expect(it.getBlockType()).toEqual(format.MetadataType.PICTURE)
      expect(it.getBlock() instanceof metadata.PictureMetadata).toBe(true)
      expect(it.prev()).toBe(true)

      expect(it.getBlockType()).toEqual(format.MetadataType.APPLICATION)
      expect(it.getBlock() instanceof metadata.ApplicationMetadata).toBe(true)
      expect(it.prev()).toBe(true)

      expect(it.getBlockType()).toEqual(format.MetadataType.STREAMINFO)
      expect(it.getBlock() instanceof metadata.StreamInfoMetadata).toBe(true)
      expect(it.prev()).toBe(false)
    })
  })

  describe('sortPadding', () => {
    it('should move padding to the end', () => {
      const filePath = pathForFile('vc-p.flac')
      const ch = new Chain()

      ch.read(filePath)

      ch.sortPadding()

      expect(Array.from(ch.createIterator()).map((i) => i.type)).toEqual([
        format.MetadataType.STREAMINFO,
        format.MetadataType.APPLICATION,
        format.MetadataType.PICTURE,
        format.MetadataType.VORBIS_COMMENT,
        format.MetadataType.PADDING,
      ])
    })
  })

  describe('mergePadding', () => {
    it('should merge adjacent padding blocks', () => {
      const filePath = pathForFile('vc-p.flac')
      const ch = new Chain()

      ch.read(filePath)

      const it = ch.createIterator()
      // eslint-disable-next-line curly
      while (it.next());
      it.insertBlockBefore(new metadata.PaddingMetadata(100))

      expect(Array.from(ch.createIterator()).map((i) => i.type)).toEqual([
        format.MetadataType.STREAMINFO,
        format.MetadataType.APPLICATION,
        format.MetadataType.PICTURE,
        format.MetadataType.PADDING,
        format.MetadataType.PADDING,
        format.MetadataType.VORBIS_COMMENT,
      ])

      ch.mergePadding()

      expect(Array.from(ch.createIterator()).map((i) => i.type)).toEqual([
        format.MetadataType.STREAMINFO,
        format.MetadataType.APPLICATION,
        format.MetadataType.PICTURE,
        format.MetadataType.PADDING,
        format.MetadataType.VORBIS_COMMENT,
      ])
    })
  })

  describe('modify', () => {
    it('setBlock() throws if the first argument is not a Metadata', () => {
      expect(() => new Iterator().setBlock({})).toThrow()
    })

    it('insertBlockAfter() throws if the first argument is not a Metadata', () => {
      expect(() => new Iterator().insertBlockAfter({})).toThrow()
    })

    it('insertBlockBefore() throws if the first argument is not a Metadata', () => {
      expect(() => new Iterator().insertBlockBefore({})).toThrow()
    })

    it('replace StreamInfo block should not replace it', () => {
      const filePath = pathForFile('vc-cs.flac')
      const ch = new Chain()

      ch.read(filePath)

      expect(ch.createIterator().setBlock(new metadata.PaddingMetadata())).toBe(false)
      expect(Array.from(ch.createIterator()).map((i) => i.type)).toEqual([
        format.MetadataType.STREAMINFO,
        format.MetadataType.SEEKTABLE,
        format.MetadataType.VORBIS_COMMENT,
        format.MetadataType.CUESHEET,
      ])
    })

    it('replace any other block block should replace it', () => {
      const filePath = pathForFile('vc-cs.flac')
      const ch = new Chain()

      ch.read(filePath)

      const it = ch.createIterator()
      expect(it.next()).toBe(true)
      expect(it.next()).toBe(true)
      expect(it.setBlock(new metadata.PaddingMetadata())).toBe(true)
      expect(it.getBlockType()).toEqual(format.MetadataType.PADDING)
      expect(Array.from(ch.createIterator()).map((i) => i.type)).toEqual([
        format.MetadataType.STREAMINFO,
        format.MetadataType.SEEKTABLE,
        format.MetadataType.PADDING,
        format.MetadataType.CUESHEET,
      ])
    })

    it('delete StreamInfo block should not delete it', () => {
      const filePath = pathForFile('vc-cs.flac')
      const ch = new Chain()

      ch.read(filePath)

      expect(ch.createIterator().deleteBlock()).toBe(false)
      expect(Array.from(ch.createIterator()).map((i) => i.type)).toEqual([
        format.MetadataType.STREAMINFO,
        format.MetadataType.SEEKTABLE,
        format.MetadataType.VORBIS_COMMENT,
        format.MetadataType.CUESHEET,
      ])
    })

    it('delete any other block block should replace it', () => {
      const filePath = pathForFile('vc-cs.flac')
      const ch = new Chain()

      ch.read(filePath)

      const it = ch.createIterator()
      expect(it.next()).toBe(true)
      expect(it.next()).toBe(true)
      expect(it.deleteBlock(true)).toBe(true)
      expect(it.getBlockType()).toEqual(format.MetadataType.SEEKTABLE)
      expect(Array.from(ch.createIterator()).map((i) => i.type)).toEqual([
        format.MetadataType.STREAMINFO,
        format.MetadataType.SEEKTABLE,
        format.MetadataType.PADDING,
        format.MetadataType.CUESHEET,
      ])
    })

    it('insert before a StreamInfo block should not insert it', () => {
      const filePath = pathForFile('vc-cs.flac')
      const ch = new Chain()

      ch.read(filePath)

      expect(ch.createIterator().insertBlockBefore(new metadata.ApplicationMetadata())).toBe(false)
      expect(Array.from(ch.createIterator()).map((i) => i.type)).toEqual([
        format.MetadataType.STREAMINFO,
        format.MetadataType.SEEKTABLE,
        format.MetadataType.VORBIS_COMMENT,
        format.MetadataType.CUESHEET,
      ])
    })

    it('insert before any other block block should insert it', () => {
      const filePath = pathForFile('vc-cs.flac')
      const ch = new Chain()

      ch.read(filePath)

      const it = ch.createIterator()
      expect(it.next()).toBe(true)
      expect(it.next()).toBe(true)
      expect(it.insertBlockBefore(new metadata.ApplicationMetadata())).toBe(true)
      expect(it.getBlockType()).toEqual(format.MetadataType.APPLICATION)
      expect(Array.from(ch.createIterator()).map((i) => i.type)).toEqual([
        format.MetadataType.STREAMINFO,
        format.MetadataType.SEEKTABLE,
        format.MetadataType.APPLICATION,
        format.MetadataType.VORBIS_COMMENT,
        format.MetadataType.CUESHEET,
      ])
    })

    it('insert after any other block block should insert it', () => {
      const filePath = pathForFile('vc-cs.flac')
      const ch = new Chain()

      ch.read(filePath)

      const it = ch.createIterator()
      expect(it.next()).toBe(true)
      expect(it.next()).toBe(true)
      expect(it.insertBlockAfter(new metadata.ApplicationMetadata())).toBe(true)
      expect(it.getBlockType()).toEqual(format.MetadataType.APPLICATION)
      expect(Array.from(ch.createIterator()).map((i) => i.type)).toEqual([
        format.MetadataType.STREAMINFO,
        format.MetadataType.SEEKTABLE,
        format.MetadataType.VORBIS_COMMENT,
        format.MetadataType.APPLICATION,
        format.MetadataType.CUESHEET,
      ])
    })
  })

  describe('write', () => {
    let tmpFile
    beforeEach(() => {
      tmpFile = temp.openSync('flac-bindings.metadata2.chain-iterator')
      oldfs.copyFileSync(pathForFile('no.flac'), tmpFile.path)
    })

    afterEach(() => {
      oldfs.closeSync(tmpFile.fd)
      temp.cleanupSync()
    })

    it('modify the blocks and write should modify the file correctly (sync)', () => {
      const ch = new Chain()
      ch.read(tmpFile.path)
      const it = ch.createIterator()

      const vc = new metadata.VorbisCommentMetadata()
      vc.vendorString = 'flac-bindings 2.0.0'
      expect(it.insertBlockAfter(vc)).toBe(true)

      expect(it.insertBlockAfter(new metadata.PaddingMetadata(50))).toBe(true)
      expect(it.insertBlockAfter(new metadata.ApplicationMetadata())).toBe(true)
      expect(it.next()).toBe(true)

      ch.write(false)

      expect(Array.from(ch.createIterator()).map((i) => i.type)).toEqual([
        format.MetadataType.STREAMINFO,
        format.MetadataType.VORBIS_COMMENT,
        format.MetadataType.PADDING,
        format.MetadataType.APPLICATION,
        format.MetadataType.PADDING,
      ])
    })

    it('modify the blocks and write should modify the file correctly (async)', async () => {
      const ch = new Chain()
      await ch.readAsync(tmpFile.path)
      const it = ch.createIterator()

      const vc = new metadata.VorbisCommentMetadata()
      vc.vendorString = 'flac-bindings 2.0.0'
      expect(it.insertBlockAfter(vc)).toBe(true)

      expect(it.insertBlockAfter(new metadata.PaddingMetadata(50))).toBe(true)
      expect(it.insertBlockAfter(new metadata.ApplicationMetadata())).toBe(true)
      expect(it.next()).toBe(true)

      await ch.writeAsync(false)

      expect(Array.from(ch.createIterator()).map((i) => i.type)).toEqual([
        format.MetadataType.STREAMINFO,
        format.MetadataType.VORBIS_COMMENT,
        format.MetadataType.PADDING,
        format.MetadataType.APPLICATION,
        format.MetadataType.PADDING,
      ])
    })

    it('modify the blocks and write should modify the file correctly (callbacks)', async () => {
      const readCallbacks = await generateFlacCallbacks.flacio(tmpFile.path, 'r')
      const ch = new Chain()
      await ch.readWithCallbacks(readCallbacks).finally(() => readCallbacks.close())
      const it = ch.createIterator()

      const vc = new metadata.VorbisCommentMetadata()
      vc.vendorString = 'flac-bindings 2.0.0'
      expect(it.insertBlockAfter(vc)).toBe(true)

      expect(it.insertBlockAfter(new metadata.PaddingMetadata(50))).toBe(true)
      expect(it.insertBlockAfter(new metadata.ApplicationMetadata())).toBe(true)
      expect(it.next()).toBe(true)

      const writeCallbacks = await generateFlacCallbacks.flacio(tmpFile.path, 'r+')
      expect(ch.checkIfTempFileIsNeeded()).toBe(false)
      await ch.writeWithCallbacks(writeCallbacks).finally(() => writeCallbacks.close())

      expect(Array.from(ch.createIterator()).map((i) => i.type)).toEqual([
        format.MetadataType.STREAMINFO,
        format.MetadataType.VORBIS_COMMENT,
        format.MetadataType.PADDING,
        format.MetadataType.APPLICATION,
        format.MetadataType.PADDING,
      ])
    })

    it('modify the blocks and write should modify the file correctly (callbacks + tempfile)', async () => {
      const readCallbacks = await generateFlacCallbacks.flacio(tmpFile.path, 'r')
      const ch = new Chain()
      await ch.readWithCallbacks(readCallbacks).finally(() => readCallbacks.close())
      const it = ch.createIterator()

      const vc = new metadata.VorbisCommentMetadata()
      vc.vendorString = 'flac-bindings 2.0.0'
      expect(it.insertBlockAfter(vc)).toBe(true)

      expect(it.insertBlockAfter(new metadata.PaddingMetadata(50))).toBe(true)
      expect(it.insertBlockAfter(new metadata.ApplicationMetadata())).toBe(true)
      expect(it.next()).toBe(true)

      const tmpFile2 = temp.openSync('flac-bindings.metadata2.chain-iterator')
      const writeCallbacks = await generateFlacCallbacks.flacio(tmpFile.path, 'r+')
      const writeCallbacks2 = await generateFlacCallbacks.flacio(tmpFile2.path, 'r+')
      expect(ch.checkIfTempFileIsNeeded(false)).toBe(true)
      await ch.writeWithCallbacksAndTempFile(false, writeCallbacks, writeCallbacks2)
        .finally(() => writeCallbacks.close().finally(() => writeCallbacks2.close()))

      expect(Array.from(ch.createIterator()).map((i) => i.type)).toEqual([
        format.MetadataType.STREAMINFO,
        format.MetadataType.VORBIS_COMMENT,
        format.MetadataType.PADDING,
        format.MetadataType.APPLICATION,
        format.MetadataType.PADDING,
      ])
    })
  })

  describe('other', () => {
    it('checkIfTempFileIsNeeded() should work', async () => {
      const filePath = pathForFile('vc-cs.flac')
      const ch = new Chain()
      await ch.readAsync(filePath)

      expect(ch.checkIfTempFileIsNeeded()).toBe(false)
    })
  })

  describe('gc', () => {
    it('gc should work', () => {
      expect(gc).not.toThrow()
    })
  })
})
