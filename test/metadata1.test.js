import oldfs from 'node:fs'
import fs from 'node:fs/promises'
import tempUntracked from 'temp'
import {
  afterEach,
  beforeEach,
  describe,
  expect,
  it,
} from 'vitest'
import { SimpleIterator, metadata, format } from '../lib/api.js'
import { pathForFile as fullPathForFile, gc } from './helper/index.js'

const temp = tempUntracked.track()
const { tags: pathForFile } = fullPathForFile

describe('simpleIterator', () => {
  describe('init', () => {
    it('throws if the first argument is not a string', () => {
      expect(() => new SimpleIterator().init(8)).toThrow()
    })

    it('returns false if the file does not exist', async () => {
      const filePath = pathForFile('el.flac')
      const si = new SimpleIterator()

      expect(() => si.init(filePath)).toThrow()

      await expect(() => fs.access(filePath)).rejects.toThrow()
    })

    it('returns true if the file exists', async () => {
      const filePath = pathForFile('no.flac')
      const si = new SimpleIterator()

      si.init(filePath)

      expect(si.status()).toStrictEqual(SimpleIterator.Status.OK)
      expect(si.isWritable()).toBeTruthy()
      await fs.access(filePath)
    })
  })

  describe('initAsync', () => {
    it('throws if the first argument is not a string', () => {
      expect(() => new SimpleIterator().initAsync(8)).toThrow()
    })

    it('throws if the file does not exist', async () => {
      const filePath = pathForFile('el.flac')
      const si = new SimpleIterator()

      await expect(
        () => si.initAsync(filePath),
      ).rejects.toThrow(/SimpleIterator initialization failed: ERROR_OPENING_FILE/)

      await expect(() => fs.access(filePath)).rejects.toThrow()
    })

    it('returns true if the file exists', async () => {
      const filePath = pathForFile('no.flac')
      const si = new SimpleIterator()

      await si.initAsync(filePath)

      expect(si.status()).toStrictEqual(SimpleIterator.Status.OK)
      expect(si.isWritable()).toBeTruthy()
      await fs.access(filePath)
    })
  })

  describe('iterator', () => {
    it('should iterate over all blocks', () => {
      const filePath = pathForFile('vc-cs.flac')
      const si = new SimpleIterator()

      si.init(filePath)
      const e = si[Symbol.iterator]()

      let m = e.next()
      expect(m.done).toBeFalsy()
      expect(m.value.type).toStrictEqual(format.MetadataType.STREAMINFO)

      m = e.next()
      expect(m.done).toBeFalsy()
      expect(m.value.type).toStrictEqual(format.MetadataType.SEEKTABLE)

      m = e.next()
      expect(m.done).toBeFalsy()
      expect(m.value.type).toStrictEqual(format.MetadataType.VORBIS_COMMENT)

      m = e.next()
      expect(m.done).toBeFalsy()
      expect(m.value.type).toStrictEqual(format.MetadataType.CUESHEET)

      m = e.next()
      expect(m.done).toBeTruthy()
      expect(m.value).toBeUndefined()
    })
  })

  describe('asyncIterator', () => {
    it('should iterate over all blocks', async () => {
      const filePath = pathForFile('vc-cs.flac')
      const si = new SimpleIterator()

      await si.initAsync(filePath)
      const e = si[Symbol.asyncIterator]()

      let m = await e.next()
      expect(m.done).toBeFalsy()
      expect(m.value.type).toStrictEqual(format.MetadataType.STREAMINFO)

      m = await e.next()
      expect(m.done).toBeFalsy()
      expect(m.value.type).toStrictEqual(format.MetadataType.SEEKTABLE)

      m = await e.next()
      expect(m.done).toBeFalsy()
      expect(m.value.type).toStrictEqual(format.MetadataType.VORBIS_COMMENT)

      m = await e.next()
      expect(m.done).toBeFalsy()
      expect(m.value.type).toStrictEqual(format.MetadataType.CUESHEET)

      m = await e.next()
      expect(m.done).toBeTruthy()
      expect(m.value).toBeUndefined()
    })
  })

  describe('iterate and get*', () => {
    it('should iterate forwards and get info about them correctly', () => {
      const filePath = pathForFile('vc-p.flac')
      const si = new SimpleIterator()

      si.init(filePath)

      expect(si.isLast()).toBeFalsy()
      expect(si.getBlockOffset()).toBe(4)
      expect(si.getBlockLength()).toBe(34)
      expect(si.getBlockType()).toStrictEqual(format.MetadataType.STREAMINFO)
      expect(si.getBlock()).toBeInstanceOf(metadata.StreamInfoMetadata)

      expect(si.next()).toBeTruthy()
      expect(si.isLast()).toBeFalsy()
      expect(si.getBlockOffset()).toBe(42)
      expect(si.getBlockLength()).toBe(25)
      expect(si.getBlockType()).toStrictEqual(format.MetadataType.APPLICATION)
      expect(si.getBlock()).toBeInstanceOf(metadata.ApplicationMetadata)

      expect(si.next()).toBeTruthy()
      expect(si.isLast()).toBeFalsy()
      expect(si.getBlockOffset()).toBe(71)
      expect(si.getBlockLength()).toBe(17142)
      expect(si.getBlockType()).toStrictEqual(format.MetadataType.PICTURE)
      expect(si.getBlock()).toBeInstanceOf(metadata.PictureMetadata)

      expect(si.next()).toBeTruthy()
      expect(si.isLast()).toBeFalsy()
      expect(si.getBlockOffset()).toBe(17217)
      expect(si.getBlockLength()).toBe(136)
      expect(si.getBlockType()).toStrictEqual(format.MetadataType.PADDING)
      expect(si.getBlock()).toBeInstanceOf(metadata.PaddingMetadata)

      expect(si.next()).toBeTruthy()
      expect(si.isLast()).toBeTruthy()
      expect(si.getBlockOffset()).toBe(17357)
      expect(si.getBlockLength()).toBe(165)
      expect(si.getBlockType()).toStrictEqual(format.MetadataType.VORBIS_COMMENT)
      expect(si.getBlock()).toBeInstanceOf(metadata.VorbisCommentMetadata)

      expect(si.next()).toBeFalsy()
    })

    it('should iterate backwards and get info about them correctly', () => {
      const filePath = pathForFile('vc-p.flac')
      const si = new SimpleIterator()

      si.init(filePath)
      // eslint-disable-next-line curly
      while (si.next());

      expect(si.isLast()).toBeTruthy()
      expect(si.getBlockOffset()).toBe(17357)
      expect(si.getBlockLength()).toBe(165)
      expect(si.getBlockType()).toStrictEqual(format.MetadataType.VORBIS_COMMENT)
      expect(si.getBlock()).toBeInstanceOf(metadata.VorbisCommentMetadata)
      expect(si.prev()).toBeTruthy()

      expect(si.isLast()).toBeFalsy()
      expect(si.getBlockOffset()).toBe(17217)
      expect(si.getBlockLength()).toBe(136)
      expect(si.getBlockType()).toStrictEqual(format.MetadataType.PADDING)
      expect(si.getBlock()).toBeInstanceOf(metadata.PaddingMetadata)
      expect(si.prev()).toBeTruthy()

      expect(si.isLast()).toBeFalsy()
      expect(si.getBlockOffset()).toBe(71)
      expect(si.getBlockLength()).toBe(17142)
      expect(si.getBlockType()).toStrictEqual(format.MetadataType.PICTURE)
      expect(si.getBlock()).toBeInstanceOf(metadata.PictureMetadata)
      expect(si.prev()).toBeTruthy()

      expect(si.isLast()).toBeFalsy()
      expect(si.getBlockOffset()).toBe(42)
      expect(si.getBlockLength()).toBe(25)
      expect(si.getBlockType()).toStrictEqual(format.MetadataType.APPLICATION)
      expect(si.getBlock()).toBeInstanceOf(metadata.ApplicationMetadata)
      expect(si.prev()).toBeTruthy()

      expect(si.isLast()).toBeFalsy()
      expect(si.getBlockOffset()).toBe(4)
      expect(si.getBlockLength()).toBe(34)
      expect(si.getBlockType()).toStrictEqual(format.MetadataType.STREAMINFO)
      expect(si.getBlock()).toBeInstanceOf(metadata.StreamInfoMetadata)
      expect(si.prev()).toBeFalsy()
    })
  })

  describe('async iterate and get*Async', () => {
    it('should iterate forwards and get info about them correctly', async () => {
      const filePath = pathForFile('vc-p.flac')
      const si = new SimpleIterator()

      await si.initAsync(filePath)

      expect(si.isLast()).toBeFalsy()
      expect(si.getBlockOffset()).toBe(4)
      expect(si.getBlockLength()).toBe(34)
      expect(si.getBlockType()).toStrictEqual(format.MetadataType.STREAMINFO)
      await expect(si.getBlockAsync()).resolves.toBeInstanceOf(metadata.StreamInfoMetadata)

      await expect(si.nextAsync()).resolves.toBeTruthy()
      expect(si.isLast()).toBeFalsy()
      expect(si.getBlockOffset()).toBe(42)
      expect(si.getBlockLength()).toBe(25)
      expect(si.getBlockType()).toStrictEqual(format.MetadataType.APPLICATION)
      await expect(si.getBlockAsync()).resolves.toBeInstanceOf(metadata.ApplicationMetadata)

      await expect(si.nextAsync()).resolves.toBeTruthy()
      expect(si.isLast()).toBeFalsy()
      expect(si.getBlockOffset()).toBe(71)
      expect(si.getBlockLength()).toBe(17142)
      expect(si.getBlockType()).toStrictEqual(format.MetadataType.PICTURE)
      await expect(si.getBlockAsync()).resolves.toBeInstanceOf(metadata.PictureMetadata)

      await expect(si.nextAsync()).resolves.toBeTruthy()
      expect(si.isLast()).toBeFalsy()
      expect(si.getBlockOffset()).toBe(17217)
      expect(si.getBlockLength()).toBe(136)
      expect(si.getBlockType()).toStrictEqual(format.MetadataType.PADDING)
      await expect(si.getBlockAsync()).resolves.toBeInstanceOf(metadata.PaddingMetadata)

      await expect(si.nextAsync()).resolves.toBeTruthy()
      expect(si.isLast()).toBeTruthy()
      expect(si.getBlockOffset()).toBe(17357)
      expect(si.getBlockLength()).toBe(165)
      expect(si.getBlockType()).toStrictEqual(format.MetadataType.VORBIS_COMMENT)
      await expect(si.getBlockAsync()).resolves.toBeInstanceOf(metadata.VorbisCommentMetadata)

      await expect(si.nextAsync()).resolves.toBeFalsy()
    })

    it('should iterate backwards and get info about them correctly', async () => {
      const filePath = pathForFile('vc-p.flac')
      const si = new SimpleIterator()

      await si.initAsync(filePath)
      // eslint-disable-next-line no-await-in-loop
      while (await si.nextAsync());

      expect(si.isLast()).toBeTruthy()
      expect(si.getBlockOffset()).toBe(17357)
      expect(si.getBlockLength()).toBe(165)
      expect(si.getBlockType()).toStrictEqual(format.MetadataType.VORBIS_COMMENT)
      await expect(si.getBlockAsync()).resolves.toBeInstanceOf(metadata.VorbisCommentMetadata)
      await expect(si.prevAsync()).resolves.toBeTruthy()

      expect(si.isLast()).toBeFalsy()
      expect(si.getBlockOffset()).toBe(17217)
      expect(si.getBlockLength()).toBe(136)
      expect(si.getBlockType()).toStrictEqual(format.MetadataType.PADDING)
      await expect(si.getBlockAsync()).resolves.toBeInstanceOf(metadata.PaddingMetadata)
      await expect(si.prevAsync()).resolves.toBeTruthy()

      expect(si.isLast()).toBeFalsy()
      expect(si.getBlockOffset()).toBe(71)
      expect(si.getBlockLength()).toBe(17142)
      expect(si.getBlockType()).toStrictEqual(format.MetadataType.PICTURE)
      await expect(si.getBlockAsync()).resolves.toBeInstanceOf(metadata.PictureMetadata)
      await expect(si.prevAsync()).resolves.toBeTruthy()

      expect(si.isLast()).toBeFalsy()
      expect(si.getBlockOffset()).toBe(42)
      expect(si.getBlockLength()).toBe(25)
      expect(si.getBlockType()).toStrictEqual(format.MetadataType.APPLICATION)
      await expect(si.getBlockAsync()).resolves.toBeInstanceOf(metadata.ApplicationMetadata)
      await expect(si.prevAsync()).resolves.toBeTruthy()

      expect(si.isLast()).toBeFalsy()
      expect(si.getBlockOffset()).toBe(4)
      expect(si.getBlockLength()).toBe(34)
      expect(si.getBlockType()).toStrictEqual(format.MetadataType.STREAMINFO)
      await expect(si.getBlockAsync()).resolves.toBeInstanceOf(metadata.StreamInfoMetadata)
      await expect(si.prevAsync()).resolves.toBeFalsy()
    })
  })

  describe('modify', () => {
    let tmpFile
    beforeEach(() => {
      tmpFile = temp.openSync('flac-bindings.metadata1.simpleiterator')
      oldfs.closeSync(tmpFile.fd)
      oldfs.copyFileSync(pathForFile('no.flac'), tmpFile.path)
    })

    afterEach(() => {
      temp.cleanupSync()
    })

    it('setBlock() throws if the first argument is not a Metadata', () => {
      expect(() => new SimpleIterator().setBlock({})).toThrow()
    })

    it('insertBlockAfter() throws if the first argument is not a Metadata', () => {
      expect(() => new SimpleIterator().insertBlockAfter({})).toThrow()
    })

    it('replace StreamInfo block should not replace it', () => {
      const si = new SimpleIterator()

      si.init(tmpFile.path)

      expect(si.setBlock(new metadata.ApplicationMetadata())).toBeFalsy()
    })

    it('replace any block should effectively replace it', () => {
      const si = new SimpleIterator()

      si.init(tmpFile.path)

      expect(si.next()).toBeTruthy()
      const app = new metadata.ApplicationMetadata()
      app.id = Buffer.from('node')
      app.data = Buffer.from('A Wonderful Adventure')
      expect(si.setBlock(app)).toBeTruthy()

      const ot = new SimpleIterator()
      ot.init(tmpFile.path)
      expect(ot.next()).toBeTruthy()
      expect(ot.getBlockType()).toStrictEqual(format.MetadataType.APPLICATION)
      expect(ot.getApplicationId()).toStrictEqual(Buffer.from('node'))
    })

    it('insert any block should effectively insert it', () => {
      const si = new SimpleIterator()

      si.init(tmpFile.path)

      expect(si.next()).toBeTruthy()
      const app = new metadata.ApplicationMetadata()
      app.id = Buffer.from('node')
      app.data = Buffer.from('A Wonderful Adventure')
      expect(si.insertBlockAfter(app)).toBeTruthy()

      const ot = new SimpleIterator()
      ot.init(tmpFile.path)
      expect(ot.next()).toBeTruthy()
      expect(ot.next()).toBeTruthy()
      expect(ot.getBlockType()).toStrictEqual(format.MetadataType.APPLICATION)
      expect(ot.getApplicationId()).toStrictEqual(Buffer.from('node'))
    })

    it('delete StreamInfo block should not delete it', () => {
      const si = new SimpleIterator()

      si.init(tmpFile.path)

      expect(si.deleteBlock()).toBeFalsy()
    })

    it('delete any other block should effectively delete it', () => {
      const si = new SimpleIterator()

      si.init(tmpFile.path)

      expect(si.next()).toBeTruthy()
      expect(si.deleteBlock()).toBeTruthy()
    })
  })

  describe('async modify', () => {
    let tmpFile
    beforeEach(() => {
      tmpFile = temp.openSync('flac-bindings.metadata1.simpleiterator')
      oldfs.closeSync(tmpFile.fd)
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
      const si = new SimpleIterator()

      si.initAsync(tmpFile.path)

      await expect(si.setBlockAsync(new metadata.ApplicationMetadata())).resolves.toBeFalsy()
    })

    it('replace any block should effectively replace it', async () => {
      const si = new SimpleIterator()

      await si.initAsync(tmpFile.path)

      await expect(si.nextAsync()).resolves.toBeTruthy()
      const app = new metadata.ApplicationMetadata()
      app.id = Buffer.from('node')
      app.data = Buffer.from('A Wonderful Adventure')
      await expect(si.setBlockAsync(app)).resolves.toBeTruthy()

      const ot = new SimpleIterator()
      await ot.initAsync(tmpFile.path)
      await expect(ot.nextAsync()).resolves.toBeTruthy()
      expect(ot.getBlockType()).toStrictEqual(format.MetadataType.APPLICATION)
      await expect(ot.getApplicationIdAsync()).resolves.toStrictEqual(Buffer.from('node'))
    })

    it('insert any block should effectively insert it', async () => {
      const si = new SimpleIterator()

      await si.initAsync(tmpFile.path)

      await expect(si.nextAsync()).resolves.toBeTruthy()
      const app = new metadata.ApplicationMetadata()
      app.id = Buffer.from('node')
      app.data = Buffer.from('A Wonderful Adventure')
      await expect(si.insertBlockAfterAsync(app)).resolves.toBeTruthy()

      const ot = new SimpleIterator()
      await ot.initAsync(tmpFile.path)
      await expect(ot.nextAsync()).resolves.toBeTruthy()
      await expect(ot.nextAsync()).resolves.toBeTruthy()
      expect(ot.getBlockType()).toStrictEqual(format.MetadataType.APPLICATION)
      await expect(ot.getApplicationIdAsync()).resolves.toStrictEqual(Buffer.from('node'))
    })

    it('delete StreamInfo block should not delete it', async () => {
      const si = new SimpleIterator()

      await si.initAsync(tmpFile.path)

      await expect(si.deleteBlockAsync()).resolves.toBeFalsy()
    })

    it('delete any other block should effectively delete it', async () => {
      const si = new SimpleIterator()

      await si.initAsync(tmpFile.path)

      await expect(si.nextAsync()).resolves.toBeTruthy()
      await expect(si.deleteBlockAsync()).resolves.toBeTruthy()
    })
  })

  describe('gc', () => {
    it('gc should work', () => {
      expect(gc).not.toThrow()
    })
  })
})
