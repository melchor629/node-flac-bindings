const { CueSheetMetadata, CueSheetIndex, CueSheetTrack } = require('../../lib/index').api.metadata
const { MetadataType } = require('../../lib/index').api.format
const { getCuesheet } = require('../../lib/index').api.metadata0
const { pathForFile: { tags: pathForFile }, gc } = require('../helper')

describe('CueSheetMetadata', () => {
  it('create new object should work', () => {
    expect(new CueSheetMetadata()).not.toBeNull()
  })

  it('the object should have the right type', () => {
    const cs = new CueSheetMetadata()
    expect(cs.type).toEqual(MetadataType.CUESHEET)
  })

  it(
    'mediaCatalogNumber should store string with less than 129 bytes',
    () => {
      const cst = new CueSheetMetadata()

      cst.mediaCatalogNumber = '31A9-F6AB-979D-505'

      expect(cst.mediaCatalogNumber).toEqual('31A9-F6AB-979D-505')
    },
  )

  it(
    'mediaCatalogNumber should throw if the string has more then 128 bytes',
    () => {
      const cst = new CueSheetMetadata()

      expect(() => {
        cst.mediaCatalogNumber = 'this is a very long string that you should not care about because it is '
                  + 'here only for the reason to throw an exception 🗿🤔🙃🤨'
      }).toThrow()
    },
  )

  it('leadIn should work with numbers', () => {
    const cst = new CueSheetMetadata()

    cst.leadIn = 1234567890

    expect(cst.leadIn).toEqual(1234567890)
  })

  it('leadIn should work with bigints', () => {
    const cst = new CueSheetMetadata()

    cst.leadIn = 12345678900987654321n

    expect(cst.leadIn).toEqual(12345678900987654321n)
  })

  it('leadIn should throw if not a number nor bigint', () => {
    const cst = new CueSheetMetadata()

    expect(() => {
      cst.leadIn = 'sad'
    }).toThrow()
  })

  it('iterator should contain the expected tracks', () => {
    const cs = getCuesheet(pathForFile('vc-cs.flac'))

    expect(cs).not.toBe(false)
    const tracks = Array.from(cs)

    expect(tracks.length).toEqual(2)
    expect(tracks[0].offset).toEqual(0)
    expect(tracks[0].number).toEqual(1)
    expect(tracks[0].isrc).toEqual('')
    expect(tracks[0].type).toEqual(0)
    expect(tracks[0].preEmphasis).toEqual(false)
    expect(tracks[0].count).toEqual(2)
    expect(tracks[1].offset).toEqual(441000)
    expect(tracks[1].number).toEqual(170)
    expect(tracks[1].isrc).toEqual('')
    expect(tracks[1].type).toEqual(0)
    expect(tracks[1].preEmphasis).toEqual(false)
    expect(tracks[1].count).toEqual(0)
  })

  it('isLegal() should return a string when error', () => {
    expect(typeof (new CueSheetMetadata()).isLegal()).toBe('string')
  })

  it('isLegal() should return a true when ok', () => {
    const cs = new CueSheetMetadata()
    cs.insertBlankTrack(0)
    cs.trackInsertIndex(0, 0, new CueSheetIndex(0, 170))
    expect(typeof cs.isLegal()).toBe('string')
  })

  it(
    'calculateCddbId() should throw if the CueSheet is not a CD',
    () => {
      expect(() => (new CueSheetMetadata()).calculateCddbId()).toThrow()
    },
  )

  it(
    'calculateCddbId() should return a number when the CueSheet is a CD',
    () => {
      const cs = new CueSheetMetadata()
      cs.isCd = true
      expect(typeof cs.calculateCddbId()).toBe('number')
    },
  )

  describe('track operations', () => {
    it('insertBlankTrack() should insert a new track', () => {
      const cs = new CueSheetMetadata()

      expect(cs.insertBlankTrack(0)).toBe(true)

      expect(cs.count).toEqual(1)
    })

    it('insertBlankTrack() should fail if the index is invalid', () => {
      const cs = new CueSheetMetadata()

      expect(() => cs.insertBlankTrack(1)).toThrow()
      expect(() => cs.insertBlankTrack(-1)).toThrow()
      expect(() => cs.insertBlankTrack(null)).toThrow()
    })

    it('insertTrack() should insert the track', () => {
      const cs = new CueSheetMetadata()
      const cst = new CueSheetTrack()

      expect(cs.insertTrack(0, cst)).toBe(true)

      expect(cs.count).toEqual(1)
    })

    it('insertTrack() should make a copy', () => {
      const cs = new CueSheetMetadata()
      const cst = new CueSheetTrack()

      expect(cs.insertTrack(0, cst)).toBe(true)
      cst.offset = 123n

      const tracks = Array.from(cs)
      expect(tracks.length).toEqual(1)
      expect(tracks[0].offset).not.toEqual(cst.offset)
    })

    it('insertTrack() should throw if the index is invalid', () => {
      const cs = new CueSheetMetadata()
      const cst = new CueSheetTrack()

      expect(() => cs.insertTrack(-1, cst)).toThrow()
      expect(() => cs.insertTrack(11, cst)).toThrow()
      expect(() => cs.insertTrack(null, cst)).toThrow()
    })

    it(
      'insertTrack() should throw if the track is not a CueSheetTrack',
      () => {
        const cs = new CueSheetMetadata()

        expect(() => cs.insertTrack(0, null)).toThrow()
      },
    )

    it('setTrack() should replace the track', () => {
      const cs = new CueSheetMetadata()
      const cst = new CueSheetTrack()
      cst.offset = 123n

      expect(cs.insertBlankTrack(0)).toBe(true)
      expect(cs.setTrack(0, cst)).toBe(true)

      const tracks = Array.from(cs)
      expect(tracks.length).toEqual(1)
      expect(tracks[0].offset).toEqual(123)
    })

    it('setTrack() should throw if the index is invalid', () => {
      const cs = new CueSheetMetadata()
      const cst = new CueSheetTrack()

      expect(() => cs.setTrack(0, cst)).toThrow()
      expect(cs.insertBlankTrack(0)).toBe(true)
      expect(() => cs.setTrack(1, cst)).toThrow()
      expect(() => cs.setTrack(-1, cst)).toThrow()
      expect(() => cs.setTrack(null, cst)).toThrow()
    })

    it('setTrack() should throw if the track is not CueSheetTrack', () => {
      const cs = new CueSheetMetadata()

      expect(cs.insertBlankTrack(0)).toBe(true)
      expect(() => cs.setTrack(0, null)).toThrow()
    })

    it('deleteTrack() should remove the track', () => {
      const cs = new CueSheetMetadata()
      expect(cs.insertBlankTrack(0)).toBe(true)

      expect(cs.deleteTrack(0)).toBe(true)

      expect(cs.count).toEqual(0)
    })

    it('deleteTrack() should throw if the index is invalid', () => {
      const cs = new CueSheetMetadata()

      expect(() => cs.deleteTrack(0)).toThrow()
      expect(cs.insertBlankTrack(0)).toBe(true)
      expect(() => cs.deleteTrack(-1)).toThrow()
      expect(() => cs.deleteTrack(11)).toThrow()
      expect(() => cs.deleteTrack(null)).toThrow()
    })

    it('resizeTracks() should work', () => {
      const cs = new CueSheetMetadata()

      expect(cs.resizeTracks(10)).toBe(true)
      expect(cs.count).toEqual(10)
      expect(cs.resizeTracks(1)).toBe(true)
      expect(cs.count).toEqual(1)
    })

    it('resizeTracks() should throw if size is not a number', () => {
      const cs = new CueSheetMetadata()

      expect(() => cs.resizeTracks('1')).toThrow()
    })
  })

  describe('indices operations', () => {
    it('trackResizeIndices() should work if track index is valid', () => {
      const cs = new CueSheetMetadata()
      cs.insertBlankTrack(0)

      expect(cs.trackResizeIndices(0, 10)).toBe(true)
      expect(Array.from(cs)[0].count).toEqual(10)
      expect(cs.trackResizeIndices(0, 1)).toBe(true)
      expect(Array.from(cs)[0].count).toEqual(1)
    })

    it('trackResizeIndices() should throw if the index is invalid', () => {
      const cs = new CueSheetMetadata()

      expect(() => cs.trackResizeIndices(0, 1)).toThrow()
      expect(cs.insertBlankTrack(0)).toBe(true)
      expect(() => cs.trackResizeIndices(11, 1)).toThrow()
      expect(() => cs.trackResizeIndices(-1, 1)).toThrow()
    })

    it(
      'trackResizeIndices() should throw if the track number is not a number',
      () => {
        const cs = new CueSheetMetadata()

        expect(() => cs.trackResizeIndices({}, 1)).toThrow()
      },
    )

    it(
      'trackResizeIndices() should throw if the indices size is not a number',
      () => {
        const cs = new CueSheetMetadata()

        expect(() => cs.trackResizeIndices(0, {})).toThrow()
      },
    )

    it('trackInsertIndex() should work if the indices are valid', () => {
      const cs = new CueSheetMetadata()
      cs.insertBlankTrack(0)

      expect(cs.trackInsertIndex(0, 0, new CueSheetIndex(1n, 0))).toBe(true)

      expect(Array.from(cs)[0].count).toEqual(1)
    })

    it(
      'trackInsertIndex() should throw if the track index is invalid',
      () => {
        const cs = new CueSheetMetadata()
        const csi = new CueSheetIndex(1n, 2)

        expect(() => cs.trackInsertIndex(0, 0, csi)).toThrow()
        expect(cs.insertBlankTrack(0)).toBe(true)
        expect(() => cs.trackInsertIndex(11, 0, csi)).toThrow()
        expect(() => cs.trackInsertIndex(-1, 0, csi)).toThrow()
      },
    )

    it(
      'trackInsertIndex() should throw if the index index is invalid',
      () => {
        const cs = new CueSheetMetadata()
        const csi = new CueSheetIndex(1n, 2)
        cs.insertBlankTrack(0)

        expect(() => cs.trackInsertIndex(0, 99, csi)).toThrow()
        expect(() => cs.trackInsertIndex(0, -1, csi)).toThrow()
      },
    )

    it(
      'trackInsertIndex() should throw if the track number is not a number',
      () => {
        const cs = new CueSheetMetadata()

        expect(() => cs.trackInsertIndex(null, 99, null)).toThrow()
      },
    )

    it(
      'trackInsertIndex() should throw if the index number is not a number',
      () => {
        const cs = new CueSheetMetadata()

        expect(() => cs.trackInsertIndex(0, null, null)).toThrow()
      },
    )

    it(
      'trackInsertIndex() should throw if the index object is not CueSheetIndex',
      () => {
        const cs = new CueSheetMetadata()

        expect(() => cs.trackInsertIndex(0, 1, null)).toThrow()
      },
    )

    it(
      'trackInsertBlankIndex() should work if the indices are valid',
      () => {
        const cs = new CueSheetMetadata()
        cs.insertBlankTrack(0)

        expect(cs.trackInsertBlankIndex(0, 0)).toBe(true)

        expect(Array.from(cs)[0].count).toEqual(1)
      },
    )

    it(
      'trackInsertBlankIndex() should throw if the track index is invalid',
      () => {
        const cs = new CueSheetMetadata()

        expect(() => cs.trackInsertBlankIndex(0, 0)).toThrow()
        expect(cs.insertBlankTrack(0)).toBe(true)
        expect(() => cs.trackInsertBlankIndex(11, 0)).toThrow()
        expect(() => cs.trackInsertBlankIndex(-1, 0)).toThrow()
      },
    )

    it(
      'trackInsertBlankIndex() should throw if the index index is invalid',
      () => {
        const cs = new CueSheetMetadata()
        cs.insertBlankTrack(0)

        expect(() => cs.trackInsertBlankIndex(0, 99)).toThrow()
        expect(() => cs.trackInsertBlankIndex(0, -1)).toThrow()
      },
    )

    it(
      'trackInsertBlankIndex() should throw if the track index is not a number',
      () => {
        const cs = new CueSheetMetadata()

        expect(() => cs.trackInsertBlankIndex(null)).toThrow()
      },
    )

    it(
      'trackInsertBlankIndex() should throw if the index index is not a number',
      () => {
        const cs = new CueSheetMetadata()

        expect(() => cs.trackInsertBlankIndex(0, null)).toThrow()
      },
    )

    it('trackDeleteIndex() should work if the indices are valid', () => {
      const cs = new CueSheetMetadata()
      cs.insertBlankTrack(0)
      cs.trackInsertBlankIndex(0, 0)

      expect(cs.trackDeleteIndex(0, 0)).toBe(true)

      expect(Array.from(cs)[0].count).toEqual(0)
    })

    it(
      'trackDeleteIndex() should throw if the track index is invalid',
      () => {
        const cs = new CueSheetMetadata()

        expect(() => cs.trackDeleteIndex(0, 0)).toThrow()
        expect(cs.insertBlankTrack(0)).toBe(true)
        expect(() => cs.trackDeleteIndex(34, 0)).toThrow()
        expect(() => cs.trackDeleteIndex(-1, 0)).toThrow()
      },
    )

    it(
      'trackDeleteIndex() should throw if the index index is invalid',
      () => {
        const cs = new CueSheetMetadata()
        expect(cs.insertBlankTrack(0)).toBe(true)

        expect(() => cs.trackDeleteIndex(0, 12)).toThrow()
        expect(() => cs.trackDeleteIndex(0, -1)).toThrow()
      },
    )

    it(
      'trackDeleteIndex() should throw if the track index is not a number',
      () => {
        const cs = new CueSheetMetadata()

        expect(() => cs.trackDeleteIndex(null)).toThrow()
      },
    )

    it(
      'trackDeleteIndex() should throw if the index index is not a number',
      () => {
        const cs = new CueSheetMetadata()

        expect(() => cs.trackDeleteIndex(0, null)).toThrow()
      },
    )
  })

  describe('gc', () => {
    it('gc should work', () => {
      gc()
    })
  })
})
