import fs from 'node:fs/promises'
import { describe, expect, it } from 'vitest'
import { metadata0, metadata, format } from '../lib/api.js'
import { pathForFile as fullPathForFile, gc } from './helper/index.js'

const { tags: pathForFile } = fullPathForFile

describe('metadata0', () => {
  describe('getTags', () => {
    it('should throw if the argument is not a string', () => {
      expect(() => metadata0.getTags(Symbol.species)).toThrow()
    })

    it('should return null if the file does not exist', async () => {
      const filePath = pathForFile('el.flac')

      const tags = metadata0.getTags(filePath)

      expect(tags).toBeNull()
      await expect(() => fs.access(filePath)).rejects.toThrow()
    })

    it('should return null if the file does not have tags', async () => {
      const filePath = pathForFile('no.flac')

      const tags = metadata0.getTags(filePath)

      expect(tags).toBeNull()
      await fs.access(filePath)
    })

    it('should return a VorbisComment object if the file contains one of it', () => {
      const filePath = pathForFile('vc-p.flac')

      const tags = metadata0.getTags(filePath)

      expect(tags).not.toBeNull()
      expect(tags instanceof metadata.VorbisCommentMetadata).toBeTruthy()
    })

    it('vorbisComment should contain the right tags for the file', () => {
      const filePath = pathForFile('vc-p.flac')

      const tags = metadata0.getTags(filePath)

      expect(tags.get('TITLE')).toBe('Metadata Test')
      expect(tags.get('ARTIST')).toBe('melchor629')
      expect(tags.get('ALBUM')).toBe('flac-bindings')
      expect(tags.get('COMMENT')).toBe('Nice comment tho')
      expect(tags.get('DATE')).toBe('2019')
      expect(tags.get('TRACKNUMBER')).toBe('1')
    })
  })

  describe('getTagsAsync', () => {
    it('should throw if the argument is not a string', () => {
      expect(() => metadata0.getTagsAsync(Symbol.species)).toThrow()
    })

    it('should return null if the file does not exist', async () => {
      const filePath = pathForFile('el.flac')

      await expect(metadata0.getTagsAsync(filePath)).resolves.toBeNull()
      await expect(() => fs.access(filePath)).rejects.toThrow()
    })

    it('should return null if the file does not have tags', async () => {
      const filePath = pathForFile('no.flac')

      await expect(metadata0.getTagsAsync(filePath)).resolves.toBeNull()

      await fs.access(filePath)
    })

    it('should return a VorbisComment object if the file contains one of it', async () => {
      const filePath = pathForFile('vc-p.flac')

      const tags = await metadata0.getTagsAsync(filePath)

      expect(tags).not.toBeNull()
      expect(tags instanceof metadata.VorbisCommentMetadata).toBeTruthy()
    })

    it('vorbisComment should contain the right tags for the file', async () => {
      const filePath = pathForFile('vc-p.flac')

      const tags = await metadata0.getTagsAsync(filePath)

      expect(tags.get('TITLE')).toBe('Metadata Test')
      expect(tags.get('ARTIST')).toBe('melchor629')
      expect(tags.get('ALBUM')).toBe('flac-bindings')
      expect(tags.get('COMMENT')).toBe('Nice comment tho')
      expect(tags.get('DATE')).toBe('2019')
      expect(tags.get('TRACKNUMBER')).toBe('1')
    })
  })

  describe('getPicture', () => {
    it('should throw if the argument is not a string', () => {
      expect(() => metadata0.getPicture(Symbol.iterator)).toThrow()
    })

    it('should return false if the file does not exist', async () => {
      const filePath = pathForFile('el.flac')

      const picture = metadata0.getPicture(filePath)

      expect(picture).toBeNull()
      await expect(() => fs.access(filePath)).rejects.toThrow()
    })

    it('should return false if the file does not have a picture', async () => {
      const filePath = pathForFile('no.flac')

      const picture = metadata0.getPicture(filePath)

      expect(picture).toBeNull()
      await fs.access(filePath)
    })

    it('should return a Picture object if the file data one of it', () => {
      const filePath = pathForFile('vc-p.flac')

      const picture = metadata0.getPicture(filePath)

      expect(picture).not.toBeNull()
      expect(picture instanceof metadata.PictureMetadata).toBeTruthy()
    })

    it('picture should contain the right tags for the file', () => {
      const filePath = pathForFile('vc-p.flac')

      const picture = metadata0.getPicture(filePath)

      expect(picture.colors).toBe(0)
      expect(picture.depth).toBe(32)
      expect(picture.description).toBe('o.O.png')
      expect(picture.height).toBe(168)
      expect(picture.mimeType).toBe('image/png')
      expect(picture.pictureType).toStrictEqual(format.PictureType.FRONT_COVER)
      expect(picture.width).toBe(150)
      expect(picture.data).toHaveLength(17094)
    })
  })

  describe('getPictureAsync', () => {
    it('should throw if the argument is not a string', () => {
      expect(() => metadata0.getPictureAsync(Symbol.iterator)).toThrow()
    })

    it('should return null if the file does not exist', async () => {
      const filePath = pathForFile('el.flac')

      await expect(metadata0.getPictureAsync(filePath)).resolves.toBeNull()
      await expect(() => fs.access(filePath)).rejects.toThrow()
    })

    it('should return null if the file does not have a picture', async () => {
      const filePath = pathForFile('no.flac')

      await expect(metadata0.getPictureAsync(filePath)).resolves.toBeNull()
      await fs.access(filePath)
    })

    it('should return a Picture object if the file data one of it', async () => {
      const filePath = pathForFile('vc-p.flac')

      const picture = await metadata0.getPictureAsync(filePath)

      expect(picture).not.toBeNull()
      expect(picture instanceof metadata.PictureMetadata).toBeTruthy()
    })

    it('picture should contain the right tags for the file', async () => {
      const filePath = pathForFile('vc-p.flac')

      const picture = await metadata0.getPictureAsync(filePath)

      expect(picture.colors).toBe(0)
      expect(picture.depth).toBe(32)
      expect(picture.description).toBe('o.O.png')
      expect(picture.height).toBe(168)
      expect(picture.mimeType).toBe('image/png')
      expect(picture.pictureType).toStrictEqual(format.PictureType.FRONT_COVER)
      expect(picture.width).toBe(150)
      expect(picture.data).toHaveLength(17094)
    })
  })

  describe('getCuesheet', () => {
    it('should throw if the argument is not a string', () => {
      expect(() => metadata0.getCuesheet(Symbol.asyncIterator)).toThrow()
    })

    it('should return null if the file does not exist', async () => {
      const filePath = pathForFile('el.flac')

      const cueSheet = metadata0.getCuesheet(filePath)

      expect(cueSheet).toBeNull()
      await expect(() => fs.access(filePath)).rejects.toThrow()
    })

    it('should return null if the file does not have a cue sheet', async () => {
      const filePath = pathForFile('no.flac')

      const cueSheet = metadata0.getCuesheet(filePath)

      expect(cueSheet).toBeNull()
      await fs.access(filePath)
    })

    it('should return a CueSheet object if the file contains one of it', () => {
      const filePath = pathForFile('vc-cs.flac')

      const cueSheet = metadata0.getCuesheet(filePath)

      expect(cueSheet).not.toBeNull()
      expect(cueSheet instanceof metadata.CueSheetMetadata).toBeTruthy()
    })

    it('cueSheet should contain the right tracks for the file', () => {
      const filePath = pathForFile('vc-cs.flac')

      const cueSheet = metadata0.getCuesheet(filePath)

      const tracks = Array.from(cueSheet)
      const indices0 = Array.from(tracks[0])
      expect(cueSheet.mediaCatalogNumber).toBe('')
      expect(cueSheet.leadIn).toBe(88200)
      expect(cueSheet.isCd).toBeTruthy()
      expect(tracks).toHaveLength(2)
      expect(tracks[0].offset).toBe(0)
      expect(tracks[0].number).toBe(1)
      expect(tracks[0].isrc).toBe('')
      expect(tracks[0].type).toBe(0)
      expect(tracks[0].preEmphasis).toBeFalsy()
      expect(indices0).toHaveLength(2)
      expect(indices0[0].offset).toBe(0)
      expect(indices0[0].number).toBe(0)
      expect(indices0[1].offset).toBe(18816)
      expect(indices0[1].number).toBe(1)
      expect(tracks[1].offset).toBe(441000)
      expect(tracks[1].number).toBe(170)
    })
  })

  describe('getCuesheetAsync', () => {
    it('should throw if the argument is not a string', () => {
      expect(() => metadata0.getCuesheetAsync(Symbol.asyncIterator)).toThrow()
    })

    it('should return null if the file does not exist', async () => {
      const filePath = pathForFile('el.flac')

      await expect(metadata0.getCuesheetAsync(filePath)).resolves.toBeNull()
      await expect(() => fs.access(filePath)).rejects.toThrow()
    })

    it('should return null if the file does not have a cue sheet', async () => {
      const filePath = pathForFile('no.flac')

      await expect(metadata0.getCuesheetAsync(filePath)).resolves.toBeNull()
      await fs.access(filePath)
    })

    it('should return a CueSheet object if the file contains one of it', async () => {
      const filePath = pathForFile('vc-cs.flac')

      const cueSheet = await metadata0.getCuesheetAsync(filePath)

      expect(cueSheet).not.toBeNull()
      expect(cueSheet instanceof metadata.CueSheetMetadata).toBeTruthy()
    })

    it('cueSheet should contain the right tracks for the file', async () => {
      const filePath = pathForFile('vc-cs.flac')

      const cueSheet = await metadata0.getCuesheetAsync(filePath)

      const tracks = Array.from(cueSheet)
      const indices0 = Array.from(tracks[0])
      expect(cueSheet.mediaCatalogNumber).toBe('')
      expect(cueSheet.leadIn).toBe(88200)
      expect(cueSheet.isCd).toBeTruthy()
      expect(tracks).toHaveLength(2)
      expect(tracks[0].offset).toBe(0)
      expect(tracks[0].number).toBe(1)
      expect(tracks[0].isrc).toBe('')
      expect(tracks[0].type).toBe(0)
      expect(tracks[0].preEmphasis).toBeFalsy()
      expect(indices0).toHaveLength(2)
      expect(indices0[0].offset).toBe(0)
      expect(indices0[0].number).toBe(0)
      expect(indices0[1].offset).toBe(18816)
      expect(indices0[1].number).toBe(1)
      expect(tracks[1].offset).toBe(441000)
      expect(tracks[1].number).toBe(170)
    })
  })

  describe('getStreaminfo', () => {
    it('should throw if the argument is not a string', () => {
      expect(() => metadata0.getStreaminfo(Symbol.toPrimitive)).toThrow()
    })

    it('should return null if the file does not exist', async () => {
      const filePath = pathForFile('el.flac')

      const streamInfo = metadata0.getStreaminfo(filePath)

      expect(streamInfo).toBeNull()
      await expect(() => fs.access(filePath)).rejects.toThrow()
    })

    it('streamInfo should contain the right info for the file', () => {
      const filePath = pathForFile('vc-cs.flac')

      const streamInfo = metadata0.getStreaminfo(filePath)

      expect(streamInfo.minBlocksize).toBe(4096)
      expect(streamInfo.maxBlocksize).toBe(4096)
      expect(streamInfo.minFramesize).toBe(1770)
      expect(streamInfo.maxFramesize).toBe(2225)
      expect(streamInfo.bitsPerSample).toBe(16)
      expect(streamInfo.sampleRate).toBe(44100)
      expect(streamInfo.channels).toBe(2)
      expect(streamInfo.totalSamples).toBe(441000)
      expect(
        Buffer.from('c5671d66cdca83ac483dcc302ae169af', 'hex').equals(streamInfo.md5sum),
      ).toBeTruthy()
    })
  })

  describe('getStreaminfoAsync', () => {
    it('should throw if the argument is not a string', () => {
      expect(() => metadata0.getStreaminfoAsync(Symbol.toPrimitive)).toThrow()
    })

    it('should return null if the file does not exist', async () => {
      const filePath = pathForFile('el.flac')

      await expect(metadata0.getStreaminfoAsync(filePath)).resolves.toBeNull()
      await expect(() => fs.access(filePath)).rejects.toThrow()
    })

    it('streamInfo should contain the right info for the file', async () => {
      const filePath = pathForFile('vc-cs.flac')

      const streamInfo = await metadata0.getStreaminfoAsync(filePath)

      expect(streamInfo.minBlocksize).toBe(4096)
      expect(streamInfo.maxBlocksize).toBe(4096)
      expect(streamInfo.minFramesize).toBe(1770)
      expect(streamInfo.maxFramesize).toBe(2225)
      expect(streamInfo.bitsPerSample).toBe(16)
      expect(streamInfo.sampleRate).toBe(44100)
      expect(streamInfo.channels).toBe(2)
      expect(streamInfo.totalSamples).toBe(441000)
      expect(
        Buffer.from('c5671d66cdca83ac483dcc302ae169af', 'hex').equals(streamInfo.md5sum),
      ).toBeTruthy()
    })
  })

  describe('gc', () => {
    it('gc should work', () => {
      expect(gc).not.toThrow()
    })
  })
})
