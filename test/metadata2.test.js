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
import {
  Chain, Iterator, metadata, format,
} from '../lib/api.js'
import {
  pathForFile as fullPathForFile,
  generateFlacCallbacks,
  gc,
} from './helper/index.js'

const temp = tempUntracked.track()
const { tags: pathForFile } = fullPathForFile

describe('chain & Iterator', () => {
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

      expect(ch.status()).toStrictEqual(Chain.Status.OK)
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

      expect(ch.status()).toStrictEqual(Chain.Status.OK)
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

      const si = ch.createIterator()
      const i = si[Symbol.iterator]()

      let m = i.next()
      expect(m.done).toBeFalsy()
      expect(m.value.type).toStrictEqual(format.MetadataType.STREAMINFO)

      m = i.next()
      expect(m.done).toBeFalsy()
      expect(m.value.type).toStrictEqual(format.MetadataType.APPLICATION)

      m = i.next()
      expect(m.done).toBeFalsy()
      expect(m.value.type).toStrictEqual(format.MetadataType.PICTURE)

      m = i.next()
      expect(m.done).toBeFalsy()
      expect(m.value.type).toStrictEqual(format.MetadataType.PADDING)

      m = i.next()
      expect(m.done).toBeFalsy()
      expect(m.value.type).toStrictEqual(format.MetadataType.VORBIS_COMMENT)

      m = i.next()
      expect(m.done).toBeTruthy()
    })

    it('new Iterator() creates an iterator from the chain and iterates over the blocks correctly', () => {
      const filePath = pathForFile('vc-p.flac')
      const ch = new Chain()
      const si = new Iterator()

      ch.read(filePath)

      si.init(ch)
      const i = si[Symbol.iterator]()

      let m = i.next()
      expect(m.done).toBeFalsy()
      expect(m.value.type).toStrictEqual(format.MetadataType.STREAMINFO)

      m = i.next()
      expect(m.done).toBeFalsy()
      expect(m.value.type).toStrictEqual(format.MetadataType.APPLICATION)

      m = i.next()
      expect(m.done).toBeFalsy()
      expect(m.value.type).toStrictEqual(format.MetadataType.PICTURE)

      m = i.next()
      expect(m.done).toBeFalsy()
      expect(m.value.type).toStrictEqual(format.MetadataType.PADDING)

      m = i.next()
      expect(m.done).toBeFalsy()
      expect(m.value.type).toStrictEqual(format.MetadataType.VORBIS_COMMENT)

      m = i.next()
      expect(m.done).toBeTruthy()
    })
  })

  describe('iterator and get*', () => {
    it('should iterate forwards and get info about them correctly', () => {
      const filePath = pathForFile('vc-p.flac')
      const ch = new Chain()

      ch.read(filePath)

      const si = ch.createIterator()
      expect(si.getBlockType()).toStrictEqual(format.MetadataType.STREAMINFO)
      expect(si.getBlock() instanceof metadata.StreamInfoMetadata).toBeTruthy()

      expect(si.next()).toBeTruthy()
      expect(si.getBlockType()).toStrictEqual(format.MetadataType.APPLICATION)
      expect(si.getBlock() instanceof metadata.ApplicationMetadata).toBeTruthy()

      expect(si.next()).toBeTruthy()
      expect(si.getBlockType()).toStrictEqual(format.MetadataType.PICTURE)
      expect(si.getBlock() instanceof metadata.PictureMetadata).toBeTruthy()

      expect(si.next()).toBeTruthy()
      expect(si.getBlockType()).toStrictEqual(format.MetadataType.PADDING)
      expect(si.getBlock() instanceof metadata.PaddingMetadata).toBeTruthy()

      expect(si.next()).toBeTruthy()
      expect(si.getBlockType()).toStrictEqual(format.MetadataType.VORBIS_COMMENT)
      expect(si.getBlock() instanceof metadata.VorbisCommentMetadata).toBeTruthy()

      expect(si.next()).toBeFalsy()
    })

    it('should iterate backwards and get info about them correctly', () => {
      const filePath = pathForFile('vc-p.flac')
      const ch = new Chain()

      ch.read(filePath)

      const si = ch.createIterator()
      // eslint-disable-next-line curly
      while (si.next());

      expect(si.getBlockType()).toStrictEqual(format.MetadataType.VORBIS_COMMENT)
      expect(si.getBlock() instanceof metadata.VorbisCommentMetadata).toBeTruthy()
      expect(si.prev()).toBeTruthy()

      expect(si.getBlockType()).toStrictEqual(format.MetadataType.PADDING)
      expect(si.getBlock() instanceof metadata.PaddingMetadata).toBeTruthy()
      expect(si.prev()).toBeTruthy()

      expect(si.getBlockType()).toStrictEqual(format.MetadataType.PICTURE)
      expect(si.getBlock() instanceof metadata.PictureMetadata).toBeTruthy()
      expect(si.prev()).toBeTruthy()

      expect(si.getBlockType()).toStrictEqual(format.MetadataType.APPLICATION)
      expect(si.getBlock() instanceof metadata.ApplicationMetadata).toBeTruthy()
      expect(si.prev()).toBeTruthy()

      expect(si.getBlockType()).toStrictEqual(format.MetadataType.STREAMINFO)
      expect(si.getBlock() instanceof metadata.StreamInfoMetadata).toBeTruthy()
      expect(si.prev()).toBeFalsy()
    })
  })

  describe('sortPadding', () => {
    it('should move padding to the end', () => {
      const filePath = pathForFile('vc-p.flac')
      const ch = new Chain()

      ch.read(filePath)

      ch.sortPadding()

      expect(Array.from(ch.createIterator()).map((i) => i.type)).toStrictEqual([
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

      const si = ch.createIterator()
      // eslint-disable-next-line curly
      while (si.next());
      si.insertBlockBefore(new metadata.PaddingMetadata(100))

      expect(Array.from(ch.createIterator()).map((i) => i.type)).toStrictEqual([
        format.MetadataType.STREAMINFO,
        format.MetadataType.APPLICATION,
        format.MetadataType.PICTURE,
        format.MetadataType.PADDING,
        format.MetadataType.PADDING,
        format.MetadataType.VORBIS_COMMENT,
      ])

      ch.mergePadding()

      expect(Array.from(ch.createIterator()).map((i) => i.type)).toStrictEqual([
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

      expect(ch.createIterator().setBlock(new metadata.PaddingMetadata())).toBeFalsy()
      expect(Array.from(ch.createIterator()).map((i) => i.type)).toStrictEqual([
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

      const si = ch.createIterator()
      expect(si.next()).toBeTruthy()
      expect(si.next()).toBeTruthy()
      expect(si.setBlock(new metadata.PaddingMetadata())).toBeTruthy()
      expect(si.getBlockType()).toStrictEqual(format.MetadataType.PADDING)
      expect(Array.from(ch.createIterator()).map((i) => i.type)).toStrictEqual([
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

      expect(ch.createIterator().deleteBlock()).toBeFalsy()
      expect(Array.from(ch.createIterator()).map((i) => i.type)).toStrictEqual([
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

      const si = ch.createIterator()
      expect(si.next()).toBeTruthy()
      expect(si.next()).toBeTruthy()
      expect(si.deleteBlock(true)).toBeTruthy()
      expect(si.getBlockType()).toStrictEqual(format.MetadataType.SEEKTABLE)
      expect(Array.from(ch.createIterator()).map((i) => i.type)).toStrictEqual([
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

      expect(ch.createIterator().insertBlockBefore(new metadata.ApplicationMetadata())).toBeFalsy()
      expect(Array.from(ch.createIterator()).map((i) => i.type)).toStrictEqual([
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

      const si = ch.createIterator()
      expect(si.next()).toBeTruthy()
      expect(si.next()).toBeTruthy()
      expect(si.insertBlockBefore(new metadata.ApplicationMetadata())).toBeTruthy()
      expect(si.getBlockType()).toStrictEqual(format.MetadataType.APPLICATION)
      expect(Array.from(ch.createIterator()).map((i) => i.type)).toStrictEqual([
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

      const si = ch.createIterator()
      expect(si.next()).toBeTruthy()
      expect(si.next()).toBeTruthy()
      expect(si.insertBlockAfter(new metadata.ApplicationMetadata())).toBeTruthy()
      expect(si.getBlockType()).toStrictEqual(format.MetadataType.APPLICATION)
      expect(Array.from(ch.createIterator()).map((i) => i.type)).toStrictEqual([
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
      oldfs.closeSync(tmpFile.fd)
      oldfs.copyFileSync(pathForFile('no.flac'), tmpFile.path)
    })

    afterEach(() => {
      temp.cleanupSync()
    })

    it('modify the blocks and write should modify the file correctly (sync)', () => {
      const ch = new Chain()
      ch.read(tmpFile.path)
      const si = ch.createIterator()

      const vc = new metadata.VorbisCommentMetadata()
      vc.vendorString = 'flac-bindings 2.0.0'
      expect(si.insertBlockAfter(vc)).toBeTruthy()

      expect(si.insertBlockAfter(new metadata.PaddingMetadata(50))).toBeTruthy()
      expect(si.insertBlockAfter(new metadata.ApplicationMetadata())).toBeTruthy()
      expect(si.next()).toBeTruthy()

      ch.write(false)

      expect(Array.from(ch.createIterator()).map((i) => i.type)).toStrictEqual([
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
      const si = ch.createIterator()

      const vc = new metadata.VorbisCommentMetadata()
      vc.vendorString = 'flac-bindings 2.0.0'
      expect(si.insertBlockAfter(vc)).toBeTruthy()

      expect(si.insertBlockAfter(new metadata.PaddingMetadata(50))).toBeTruthy()
      expect(si.insertBlockAfter(new metadata.ApplicationMetadata())).toBeTruthy()
      expect(si.next()).toBeTruthy()

      await ch.writeAsync(false)

      expect(Array.from(ch.createIterator()).map((i) => i.type)).toStrictEqual([
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
      const si = ch.createIterator()

      const vc = new metadata.VorbisCommentMetadata()
      vc.vendorString = 'flac-bindings 2.0.0'
      expect(si.insertBlockAfter(vc)).toBeTruthy()

      expect(si.insertBlockAfter(new metadata.PaddingMetadata(50))).toBeTruthy()
      expect(si.insertBlockAfter(new metadata.ApplicationMetadata())).toBeTruthy()
      expect(si.next()).toBeTruthy()

      const writeCallbacks = await generateFlacCallbacks.flacio(tmpFile.path, 'r+')
      expect(ch.checkIfTempFileIsNeeded()).toBeFalsy()
      await ch.writeWithCallbacks(writeCallbacks).finally(() => writeCallbacks.close())

      expect(Array.from(ch.createIterator()).map((i) => i.type)).toStrictEqual([
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
      const si = ch.createIterator()

      const vc = new metadata.VorbisCommentMetadata()
      vc.vendorString = 'flac-bindings 2.0.0'
      expect(si.insertBlockAfter(vc)).toBeTruthy()

      expect(si.insertBlockAfter(new metadata.PaddingMetadata(50))).toBeTruthy()
      expect(si.insertBlockAfter(new metadata.ApplicationMetadata())).toBeTruthy()
      expect(si.next()).toBeTruthy()

      const tmpFile2 = temp.openSync('flac-bindings.metadata2.chain-iterator')
      const writeCallbacks = await generateFlacCallbacks.flacio(tmpFile.path, 'r+')
      const writeCallbacks2 = await generateFlacCallbacks.flacio(tmpFile2.path, 'r+')
      expect(ch.checkIfTempFileIsNeeded(false)).toBeTruthy()
      await ch.writeWithCallbacksAndTempFile(false, writeCallbacks, writeCallbacks2)
        .finally(() => writeCallbacks.close().finally(() => writeCallbacks2.close()))

      expect(Array.from(ch.createIterator()).map((i) => i.type)).toStrictEqual([
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

      expect(ch.checkIfTempFileIsNeeded()).toBeFalsy()
    })
  })

  describe('gc', () => {
    it('gc should work', () => {
      expect(gc).not.toThrow()
    })
  })
})
