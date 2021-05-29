const { promises: fs, ...oldfs } = require('fs')
const temp = require('temp').track()
const { SimpleIterator, metadata, format } = require('../lib/index').api
const { pathForFile: { tags: pathForFile }, gc } = require('./helper')

describe('SimpleIterator', () => {
  describe('init', () => {
    it('throws if the first argument is not a string', () => {
      expect(() => new SimpleIterator().init(8)).toThrow()
    })

    it('returns false if the file does not exist', async () => {
      const filePath = pathForFile('el.flac')
      const it = new SimpleIterator()

      expect(() => it.init(filePath)).toThrow()

      await expect(() => fs.access(filePath)).rejects.toThrow()
    })

    it('returns true if the file exists', async () => {
      const filePath = pathForFile('no.flac')
      const it = new SimpleIterator()

      it.init(filePath)

      expect(it.status()).toEqual(SimpleIterator.Status.OK)
      expect(it.isWritable()).toBe(true)
      await fs.access(filePath)
    })
  })

  describe('initAsync', () => {
    it('throws if the first argument is not a string', () => {
      expect(() => new SimpleIterator().initAsync(8)).toThrow()
    })

    it('throws if the file does not exist', async () => {
      const filePath = pathForFile('el.flac')
      const it = new SimpleIterator()

      await expect(
        () => it.initAsync(filePath),
      ).rejects.toThrow(/SimpleIterator initialization failed: ERROR_OPENING_FILE/)

      await expect(() => fs.access(filePath)).rejects.toThrow()
    })

    it('returns true if the file exists', async () => {
      const filePath = pathForFile('no.flac')
      const it = new SimpleIterator()

      await it.initAsync(filePath)

      expect(it.status()).toEqual(SimpleIterator.Status.OK)
      expect(it.isWritable()).toBe(true)
      await fs.access(filePath)
    })
  })

  describe('iterator', () => {
    it('should iterate over all blocks', () => {
      const filePath = pathForFile('vc-cs.flac')
      const it = new SimpleIterator()

      it.init(filePath)
      const e = it[Symbol.iterator]()

      let m = e.next()
      expect(m.done).toBe(false)
      expect(m.value.type).toEqual(format.MetadataType.STREAMINFO)

      m = e.next()
      expect(m.done).toBe(false)
      expect(m.value.type).toEqual(format.MetadataType.SEEKTABLE)

      m = e.next()
      expect(m.done).toBe(false)
      expect(m.value.type).toEqual(format.MetadataType.VORBIS_COMMENT)

      m = e.next()
      expect(m.done).toBe(false)
      expect(m.value.type).toEqual(format.MetadataType.CUESHEET)

      m = e.next()
      expect(m.done).toBe(true)
      expect(m.value).not.toBeDefined()
    })
  })

  describe('asyncIterator', () => {
    it('should iterate over all blocks', async () => {
      const filePath = pathForFile('vc-cs.flac')
      const it = new SimpleIterator()

      await it.initAsync(filePath)
      const e = it[Symbol.asyncIterator]()

      let m = await e.next()
      expect(m.done).toBe(false)
      expect(m.value.type).toEqual(format.MetadataType.STREAMINFO)

      m = await e.next()
      expect(m.done).toBe(false)
      expect(m.value.type).toEqual(format.MetadataType.SEEKTABLE)

      m = await e.next()
      expect(m.done).toBe(false)
      expect(m.value.type).toEqual(format.MetadataType.VORBIS_COMMENT)

      m = await e.next()
      expect(m.done).toBe(false)
      expect(m.value.type).toEqual(format.MetadataType.CUESHEET)

      m = await e.next()
      expect(m.done).toBe(true)
      expect(m.value).not.toBeDefined()
    })
  })

  describe('iterate and get*', () => {
    it('should iterate forwards and get info about them correctly', () => {
      const filePath = pathForFile('vc-p.flac')
      const it = new SimpleIterator()

      it.init(filePath)

      expect(it.isLast()).toBe(false)
      expect(it.getBlockOffset()).toEqual(4)
      expect(it.getBlockLength()).toEqual(34)
      expect(it.getBlockType()).toEqual(format.MetadataType.STREAMINFO)
      expect(it.getBlock()).toBeInstanceOf(metadata.StreamInfoMetadata)

      expect(it.next()).toBe(true)
      expect(it.isLast()).toBe(false)
      expect(it.getBlockOffset()).toEqual(42)
      expect(it.getBlockLength()).toEqual(25)
      expect(it.getBlockType()).toEqual(format.MetadataType.APPLICATION)
      expect(it.getBlock()).toBeInstanceOf(metadata.ApplicationMetadata)

      expect(it.next()).toBe(true)
      expect(it.isLast()).toBe(false)
      expect(it.getBlockOffset()).toEqual(71)
      expect(it.getBlockLength()).toEqual(17142)
      expect(it.getBlockType()).toEqual(format.MetadataType.PICTURE)
      expect(it.getBlock()).toBeInstanceOf(metadata.PictureMetadata)

      expect(it.next()).toBe(true)
      expect(it.isLast()).toBe(false)
      expect(it.getBlockOffset()).toEqual(17217)
      expect(it.getBlockLength()).toEqual(136)
      expect(it.getBlockType()).toEqual(format.MetadataType.PADDING)
      expect(it.getBlock()).toBeInstanceOf(metadata.PaddingMetadata)

      expect(it.next()).toBe(true)
      expect(it.isLast()).toBe(true)
      expect(it.getBlockOffset()).toEqual(17357)
      expect(it.getBlockLength()).toEqual(165)
      expect(it.getBlockType()).toEqual(format.MetadataType.VORBIS_COMMENT)
      expect(it.getBlock()).toBeInstanceOf(metadata.VorbisCommentMetadata)

      expect(it.next()).toBe(false)
    })

    it('should iterate backwards and get info about them correctly', () => {
      const filePath = pathForFile('vc-p.flac')
      const it = new SimpleIterator()

      it.init(filePath)
      // eslint-disable-next-line curly
      while (it.next());

      expect(it.isLast()).toBe(true)
      expect(it.getBlockOffset()).toEqual(17357)
      expect(it.getBlockLength()).toEqual(165)
      expect(it.getBlockType()).toEqual(format.MetadataType.VORBIS_COMMENT)
      expect(it.getBlock()).toBeInstanceOf(metadata.VorbisCommentMetadata)
      expect(it.prev()).toBe(true)

      expect(it.isLast()).toBe(false)
      expect(it.getBlockOffset()).toEqual(17217)
      expect(it.getBlockLength()).toEqual(136)
      expect(it.getBlockType()).toEqual(format.MetadataType.PADDING)
      expect(it.getBlock()).toBeInstanceOf(metadata.PaddingMetadata)
      expect(it.prev()).toBe(true)

      expect(it.isLast()).toBe(false)
      expect(it.getBlockOffset()).toEqual(71)
      expect(it.getBlockLength()).toEqual(17142)
      expect(it.getBlockType()).toEqual(format.MetadataType.PICTURE)
      expect(it.getBlock()).toBeInstanceOf(metadata.PictureMetadata)
      expect(it.prev()).toBe(true)

      expect(it.isLast()).toBe(false)
      expect(it.getBlockOffset()).toEqual(42)
      expect(it.getBlockLength()).toEqual(25)
      expect(it.getBlockType()).toEqual(format.MetadataType.APPLICATION)
      expect(it.getBlock()).toBeInstanceOf(metadata.ApplicationMetadata)
      expect(it.prev()).toBe(true)

      expect(it.isLast()).toBe(false)
      expect(it.getBlockOffset()).toEqual(4)
      expect(it.getBlockLength()).toEqual(34)
      expect(it.getBlockType()).toEqual(format.MetadataType.STREAMINFO)
      expect(it.getBlock()).toBeInstanceOf(metadata.StreamInfoMetadata)
      expect(it.prev()).toBe(false)
    })
  })

  describe('async iterate and get*Async', () => {
    it('should iterate forwards and get info about them correctly', async () => {
      const filePath = pathForFile('vc-p.flac')
      const it = new SimpleIterator()

      await it.initAsync(filePath)

      expect(it.isLast()).toBe(false)
      expect(it.getBlockOffset()).toEqual(4)
      expect(it.getBlockLength()).toEqual(34)
      expect(it.getBlockType()).toEqual(format.MetadataType.STREAMINFO)
      expect(await it.getBlockAsync()).toBeInstanceOf(metadata.StreamInfoMetadata)

      expect(await it.nextAsync()).toBe(true)
      expect(it.isLast()).toBe(false)
      expect(it.getBlockOffset()).toEqual(42)
      expect(it.getBlockLength()).toEqual(25)
      expect(it.getBlockType()).toEqual(format.MetadataType.APPLICATION)
      expect(await it.getBlockAsync()).toBeInstanceOf(metadata.ApplicationMetadata)

      expect(await it.nextAsync()).toBe(true)
      expect(it.isLast()).toBe(false)
      expect(it.getBlockOffset()).toEqual(71)
      expect(it.getBlockLength()).toEqual(17142)
      expect(it.getBlockType()).toEqual(format.MetadataType.PICTURE)
      expect(await it.getBlockAsync()).toBeInstanceOf(metadata.PictureMetadata)

      expect(await it.nextAsync()).toBe(true)
      expect(it.isLast()).toBe(false)
      expect(it.getBlockOffset()).toEqual(17217)
      expect(it.getBlockLength()).toEqual(136)
      expect(it.getBlockType()).toEqual(format.MetadataType.PADDING)
      expect(await it.getBlockAsync()).toBeInstanceOf(metadata.PaddingMetadata)

      expect(await it.nextAsync()).toBe(true)
      expect(it.isLast()).toBe(true)
      expect(it.getBlockOffset()).toEqual(17357)
      expect(it.getBlockLength()).toEqual(165)
      expect(it.getBlockType()).toEqual(format.MetadataType.VORBIS_COMMENT)
      expect(await it.getBlockAsync()).toBeInstanceOf(metadata.VorbisCommentMetadata)

      expect(await it.nextAsync()).toBe(false)
    })

    it('should iterate backwards and get info about them correctly', async () => {
      const filePath = pathForFile('vc-p.flac')
      const it = new SimpleIterator()

      await it.initAsync(filePath)
      // eslint-disable-next-line no-await-in-loop
      while (await it.nextAsync());

      expect(it.isLast()).toBe(true)
      expect(it.getBlockOffset()).toEqual(17357)
      expect(it.getBlockLength()).toEqual(165)
      expect(it.getBlockType()).toEqual(format.MetadataType.VORBIS_COMMENT)
      expect(await it.getBlockAsync()).toBeInstanceOf(metadata.VorbisCommentMetadata)
      expect(await it.prevAsync()).toBe(true)

      expect(it.isLast()).toBe(false)
      expect(it.getBlockOffset()).toEqual(17217)
      expect(it.getBlockLength()).toEqual(136)
      expect(it.getBlockType()).toEqual(format.MetadataType.PADDING)
      expect(await it.getBlockAsync()).toBeInstanceOf(metadata.PaddingMetadata)
      expect(await it.prevAsync()).toBe(true)

      expect(it.isLast()).toBe(false)
      expect(it.getBlockOffset()).toEqual(71)
      expect(it.getBlockLength()).toEqual(17142)
      expect(it.getBlockType()).toEqual(format.MetadataType.PICTURE)
      expect(await it.getBlockAsync()).toBeInstanceOf(metadata.PictureMetadata)
      expect(await it.prevAsync()).toBe(true)

      expect(it.isLast()).toBe(false)
      expect(it.getBlockOffset()).toEqual(42)
      expect(it.getBlockLength()).toEqual(25)
      expect(it.getBlockType()).toEqual(format.MetadataType.APPLICATION)
      expect(await it.getBlockAsync()).toBeInstanceOf(metadata.ApplicationMetadata)
      expect(await it.prevAsync()).toBe(true)

      expect(it.isLast()).toBe(false)
      expect(it.getBlockOffset()).toEqual(4)
      expect(it.getBlockLength()).toEqual(34)
      expect(it.getBlockType()).toEqual(format.MetadataType.STREAMINFO)
      expect(await it.getBlockAsync()).toBeInstanceOf(metadata.StreamInfoMetadata)
      expect(await it.prevAsync()).toBe(false)
    })
  })

  describe('modify', () => {
    let tmpFile
    beforeEach(() => {
      tmpFile = temp.openSync('flac-bindings.metadata1.simpleiterator')
      oldfs.copyFileSync(pathForFile('no.flac'), tmpFile.path)
    })

    afterEach(() => {
      oldfs.closeSync(tmpFile.fd)
      temp.cleanupSync()
    })

    it('setBlock() throws if the first argument is not a Metadata', () => {
      expect(() => new SimpleIterator().setBlock({})).toThrow()
    })

    it('insertBlockAfter() throws if the first argument is not a Metadata', () => {
      expect(() => new SimpleIterator().insertBlockAfter({})).toThrow()
    })

    it('replace StreamInfo block should not replace it', () => {
      const it = new SimpleIterator()

      it.init(tmpFile.path)

      expect(it.setBlock(new metadata.ApplicationMetadata())).toBe(false)
    })

    it('replace any block should effectively replace it', () => {
      const it = new SimpleIterator()

      it.init(tmpFile.path)

      expect(it.next()).toBe(true)
      const app = new metadata.ApplicationMetadata()
      app.id = Buffer.from('node')
      app.data = Buffer.from('A Wonderful Adventure')
      expect(it.setBlock(app)).toBe(true)

      const ot = new SimpleIterator()
      ot.init(tmpFile.path)
      expect(ot.next()).toBe(true)
      expect(ot.getBlockType()).toEqual(format.MetadataType.APPLICATION)
      expect(ot.getApplicationId()).toEqual(Buffer.from('node'))
    })

    it('insert any block should effectively insert it', () => {
      const it = new SimpleIterator()

      it.init(tmpFile.path)

      expect(it.next()).toBe(true)
      const app = new metadata.ApplicationMetadata()
      app.id = Buffer.from('node')
      app.data = Buffer.from('A Wonderful Adventure')
      expect(it.insertBlockAfter(app)).toBe(true)

      const ot = new SimpleIterator()
      ot.init(tmpFile.path)
      expect(ot.next()).toBe(true)
      expect(ot.next()).toBe(true)
      expect(ot.getBlockType()).toEqual(format.MetadataType.APPLICATION)
      expect(ot.getApplicationId()).toEqual(Buffer.from('node'))
    })

    it('delete StreamInfo block should not delete it', () => {
      const it = new SimpleIterator()

      it.init(tmpFile.path)

      expect(it.deleteBlock()).toBe(false)
    })

    it('delete any other block should effectively delete it', () => {
      const it = new SimpleIterator()

      it.init(tmpFile.path)

      expect(it.next()).toBe(true)
      expect(it.deleteBlock()).toBe(true)
    })
  })

  describe('async modify', () => {
    let tmpFile
    beforeEach(() => {
      tmpFile = temp.openSync('flac-bindings.metadata1.simpleiterator')
      oldfs.copyFileSync(pathForFile('no.flac'), tmpFile.path)
    })

    afterEach(() => {
      temp.cleanupSync()
    })

    it('setBlock() throws if the first argument is not a Metadata', () => {
      expect(() => new SimpleIterator().setBlockAsync({})).toThrow()
    })

    it('insertBlockAfter() throws if the first argument is not a Metadata', () => {
      expect(() => new SimpleIterator().insertBlockAfterAsync({})).toThrow()
    })

    it('replace StreamInfo block should not replace it', async () => {
      const it = new SimpleIterator()

      it.initAsync(tmpFile.path)

      expect(await it.setBlockAsync(new metadata.ApplicationMetadata())).toBe(false)
    })

    it('replace any block should effectively replace it', async () => {
      const it = new SimpleIterator()

      await it.initAsync(tmpFile.path)

      expect(await it.nextAsync()).toBe(true)
      const app = new metadata.ApplicationMetadata()
      app.id = Buffer.from('node')
      app.data = Buffer.from('A Wonderful Adventure')
      expect(await it.setBlockAsync(app)).toBe(true)

      const ot = new SimpleIterator()
      await ot.initAsync(tmpFile.path)
      expect(await ot.nextAsync()).toBe(true)
      expect(ot.getBlockType()).toEqual(format.MetadataType.APPLICATION)
      expect(await ot.getApplicationIdAsync()).toEqual(Buffer.from('node'))
    })

    it('insert any block should effectively insert it', async () => {
      const it = new SimpleIterator()

      await it.initAsync(tmpFile.path)

      expect(await it.nextAsync()).toBe(true)
      const app = new metadata.ApplicationMetadata()
      app.id = Buffer.from('node')
      app.data = Buffer.from('A Wonderful Adventure')
      expect(await it.insertBlockAfterAsync(app)).toBe(true)

      const ot = new SimpleIterator()
      await ot.initAsync(tmpFile.path)
      expect(await ot.nextAsync()).toBe(true)
      expect(await ot.nextAsync()).toBe(true)
      expect(ot.getBlockType()).toEqual(format.MetadataType.APPLICATION)
      expect(await ot.getApplicationIdAsync()).toEqual(Buffer.from('node'))
    })

    it('delete StreamInfo block should not delete it', async () => {
      const it = new SimpleIterator()

      await it.initAsync(tmpFile.path)

      expect(await it.deleteBlockAsync()).toBe(false)
    })

    it('delete any other block should effectively delete it', async () => {
      const it = new SimpleIterator()

      await it.initAsync(tmpFile.path)

      expect(await it.nextAsync()).toBe(true)
      expect(await it.deleteBlockAsync()).toBe(true)
    })
  })

  describe('gc', () => {
    it('gc should work', () => {
      gc()
    })
  })
})
