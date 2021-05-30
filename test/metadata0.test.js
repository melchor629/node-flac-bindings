const { promises: fs } = require('fs')
const { metadata0, metadata, format } = require('../lib/index').api
const { pathForFile: { tags: pathForFile }, gc } = require('./helper')

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
      expect(tags instanceof metadata.VorbisCommentMetadata).toBe(true)
    })

    it('VorbisComment should contain the right tags for the file', () => {
      const filePath = pathForFile('vc-p.flac')

      const tags = metadata0.getTags(filePath)

      expect(tags.get('TITLE')).toEqual('Metadata Test')
      expect(tags.get('ARTIST')).toEqual('melchor629')
      expect(tags.get('ALBUM')).toEqual('flac-bindings')
      expect(tags.get('COMMENT')).toEqual('Nice comment tho')
      expect(tags.get('DATE')).toEqual('2019')
      expect(tags.get('TRACKNUMBER')).toEqual('1')
    })
  })

  describe('getTagsAsync', () => {
    it('should throw if the argument is not a string', () => {
      expect(() => metadata0.getTagsAsync(Symbol.species)).toThrow()
    })

    it('should return null if the file does not exist', async () => {
      const filePath = pathForFile('el.flac')

      expect(await metadata0.getTagsAsync(filePath)).toBeNull()
      await expect(() => fs.access(filePath)).rejects.toThrow()
    })

    it('should return null if the file does not have tags', async () => {
      const filePath = pathForFile('no.flac')

      expect(await metadata0.getTagsAsync(filePath)).toBeNull()

      await fs.access(filePath)
    })

    it('should return a VorbisComment object if the file contains one of it', async () => {
      const filePath = pathForFile('vc-p.flac')

      const tags = await metadata0.getTagsAsync(filePath)

      expect(tags).not.toBeNull()
      expect(tags instanceof metadata.VorbisCommentMetadata).toBe(true)
    })

    it('VorbisComment should contain the right tags for the file', async () => {
      const filePath = pathForFile('vc-p.flac')

      const tags = await metadata0.getTagsAsync(filePath)

      expect(tags.get('TITLE')).toEqual('Metadata Test')
      expect(tags.get('ARTIST')).toEqual('melchor629')
      expect(tags.get('ALBUM')).toEqual('flac-bindings')
      expect(tags.get('COMMENT')).toEqual('Nice comment tho')
      expect(tags.get('DATE')).toEqual('2019')
      expect(tags.get('TRACKNUMBER')).toEqual('1')
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
      expect(picture instanceof metadata.PictureMetadata).toBe(true)
    })

    it('Picture should contain the right tags for the file', () => {
      const filePath = pathForFile('vc-p.flac')

      const picture = metadata0.getPicture(filePath)

      expect(picture.colors).toEqual(0)
      expect(picture.depth).toEqual(32)
      expect(picture.description).toEqual('o.O.png')
      expect(picture.height).toEqual(168)
      expect(picture.mimeType).toEqual('image/png')
      expect(picture.pictureType).toEqual(format.PictureType.FRONT_COVER)
      expect(picture.width).toEqual(150)
      expect(picture.data).toHaveLength(17094)
    })
  })

  describe('getPictureAsync', () => {
    it('should throw if the argument is not a string', () => {
      expect(() => metadata0.getPictureAsync(Symbol.iterator)).toThrow()
    })

    it('should return null if the file does not exist', async () => {
      const filePath = pathForFile('el.flac')

      expect(await metadata0.getPictureAsync(filePath)).toBeNull()
      await expect(() => fs.access(filePath)).rejects.toThrow()
    })

    it('should return null if the file does not have a picture', async () => {
      const filePath = pathForFile('no.flac')

      expect(await metadata0.getPictureAsync(filePath)).toBeNull()
      await fs.access(filePath)
    })

    it('should return a Picture object if the file data one of it', async () => {
      const filePath = pathForFile('vc-p.flac')

      const picture = await metadata0.getPictureAsync(filePath)

      expect(picture).not.toBeNull()
      expect(picture instanceof metadata.PictureMetadata).toBe(true)
    })

    it('Picture should contain the right tags for the file', async () => {
      const filePath = pathForFile('vc-p.flac')

      const picture = await metadata0.getPictureAsync(filePath)

      expect(picture.colors).toEqual(0)
      expect(picture.depth).toEqual(32)
      expect(picture.description).toEqual('o.O.png')
      expect(picture.height).toEqual(168)
      expect(picture.mimeType).toEqual('image/png')
      expect(picture.pictureType).toEqual(format.PictureType.FRONT_COVER)
      expect(picture.width).toEqual(150)
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
      expect(cueSheet instanceof metadata.CueSheetMetadata).toBe(true)
    })

    it('CueSheet should contain the right tracks for the file', () => {
      const filePath = pathForFile('vc-cs.flac')

      const cueSheet = metadata0.getCuesheet(filePath)

      const tracks = Array.from(cueSheet)
      const indices0 = Array.from(tracks[0])
      expect(cueSheet.mediaCatalogNumber).toEqual('')
      expect(cueSheet.leadIn).toEqual(88200)
      expect(cueSheet.isCd).toEqual(true)
      expect(tracks).toHaveLength(2)
      expect(tracks[0].offset).toEqual(0)
      expect(tracks[0].number).toEqual(1)
      expect(tracks[0].isrc).toEqual('')
      expect(tracks[0].type).toEqual(0)
      expect(tracks[0].preEmphasis).toEqual(false)
      expect(indices0).toHaveLength(2)
      expect(indices0[0].offset).toEqual(0)
      expect(indices0[0].number).toEqual(0)
      expect(indices0[1].offset).toEqual(18816)
      expect(indices0[1].number).toEqual(1)
      expect(tracks[1].offset).toEqual(441000)
      expect(tracks[1].number).toEqual(170)
    })
  })

  describe('getCuesheetAsync', () => {
    it('should throw if the argument is not a string', () => {
      expect(() => metadata0.getCuesheetAsync(Symbol.asyncIterator)).toThrow()
    })

    it('should return null if the file does not exist', async () => {
      const filePath = pathForFile('el.flac')

      expect(await metadata0.getCuesheetAsync(filePath)).toBeNull()
      await expect(() => fs.access(filePath)).rejects.toThrow()
    })

    it('should return null if the file does not have a cue sheet', async () => {
      const filePath = pathForFile('no.flac')

      expect(await metadata0.getCuesheetAsync(filePath)).toBeNull()
      await fs.access(filePath)
    })

    it('should return a CueSheet object if the file contains one of it', async () => {
      const filePath = pathForFile('vc-cs.flac')

      const cueSheet = await metadata0.getCuesheetAsync(filePath)

      expect(cueSheet).not.toBeNull()
      expect(cueSheet instanceof metadata.CueSheetMetadata).toBe(true)
    })

    it('CueSheet should contain the right tracks for the file', async () => {
      const filePath = pathForFile('vc-cs.flac')

      const cueSheet = await metadata0.getCuesheetAsync(filePath)

      const tracks = Array.from(cueSheet)
      const indices0 = Array.from(tracks[0])
      expect(cueSheet.mediaCatalogNumber).toEqual('')
      expect(cueSheet.leadIn).toEqual(88200)
      expect(cueSheet.isCd).toEqual(true)
      expect(tracks).toHaveLength(2)
      expect(tracks[0].offset).toEqual(0)
      expect(tracks[0].number).toEqual(1)
      expect(tracks[0].isrc).toEqual('')
      expect(tracks[0].type).toEqual(0)
      expect(tracks[0].preEmphasis).toEqual(false)
      expect(indices0).toHaveLength(2)
      expect(indices0[0].offset).toEqual(0)
      expect(indices0[0].number).toEqual(0)
      expect(indices0[1].offset).toEqual(18816)
      expect(indices0[1].number).toEqual(1)
      expect(tracks[1].offset).toEqual(441000)
      expect(tracks[1].number).toEqual(170)
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

    it('StreamInfo should contain the right info for the file', () => {
      const filePath = pathForFile('vc-cs.flac')

      const streamInfo = metadata0.getStreaminfo(filePath)

      expect(streamInfo.minBlocksize).toEqual(4096)
      expect(streamInfo.maxBlocksize).toEqual(4096)
      expect(streamInfo.minFramesize).toEqual(1770)
      expect(streamInfo.maxFramesize).toEqual(2225)
      expect(streamInfo.bitsPerSample).toEqual(16)
      expect(streamInfo.sampleRate).toEqual(44100)
      expect(streamInfo.channels).toEqual(2)
      expect(streamInfo.totalSamples).toEqual(441000)
      expect(
        Buffer.from('c5671d66cdca83ac483dcc302ae169af', 'hex').equals(streamInfo.md5sum),
      ).toBe(true)
    })
  })

  describe('getStreaminfoAsync', () => {
    it('should throw if the argument is not a string', () => {
      expect(() => metadata0.getStreaminfoAsync(Symbol.toPrimitive)).toThrow()
    })

    it('should return null if the file does not exist', async () => {
      const filePath = pathForFile('el.flac')

      expect(await metadata0.getStreaminfoAsync(filePath)).toBeNull()
      await expect(() => fs.access(filePath)).rejects.toThrow()
    })

    it('StreamInfo should contain the right info for the file', async () => {
      const filePath = pathForFile('vc-cs.flac')

      const streamInfo = await metadata0.getStreaminfoAsync(filePath)

      expect(streamInfo.minBlocksize).toEqual(4096)
      expect(streamInfo.maxBlocksize).toEqual(4096)
      expect(streamInfo.minFramesize).toEqual(1770)
      expect(streamInfo.maxFramesize).toEqual(2225)
      expect(streamInfo.bitsPerSample).toEqual(16)
      expect(streamInfo.sampleRate).toEqual(44100)
      expect(streamInfo.channels).toEqual(2)
      expect(streamInfo.totalSamples).toEqual(441000)
      expect(
        Buffer.from('c5671d66cdca83ac483dcc302ae169af', 'hex').equals(streamInfo.md5sum),
      ).toBe(true)
    })
  })

  describe('gc', () => {
    it('gc should work', () => {
      expect(gc).not.toThrow()
    })
  })
})
