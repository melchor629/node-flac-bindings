import { format, metadata } from '../lib/api.js'

describe('format', () => {
  test('VERSION_STRING is defined', () => {
    expect(format.VERSION_STRING).toBeString()
    expect(format.VERSION_STRING).toMatch(/^\d+\.\d+\.\d+$/)
  })

  test('VENDOR_STRING is defined', () => {
    expect(format.VENDOR_STRING).toBeString()
    expect(format.VENDOR_STRING).toMatch(/^reference libFLAC \d+\.\d+\.\d+ \d+$/)
  })

  test('API_SUPPORTS_OGG_FLAC is defined', () => {
    expect(format.API_SUPPORTS_OGG_FLAC).toBeBoolean()
  })

  test('sampleRateIsValid() should work', () => {
    expect(format.sampleRateIsValid(96000)).toBeTrue()
    expect(format.sampleRateIsValid(BigInt(112938129312))).toBeFalse()
  })

  test('sampleRateIsValid() should throw if the argument is not number', () => {
    expect(() => format.sampleRateIsValid('p')).toThrow(/Expected p to be number/)
    expect(format.sampleRateIsValid(Infinity)).toBeFalse()
    expect(format.sampleRateIsValid(NaN)).toBeFalse()
  })

  test('blocksizeIsSubset() should work', () => {
    expect(format.blocksizeIsSubset(4410, 44100)).toBeTrue()
  })

  test('blocksizeIsSubset() should throw if any of the arguments is not number', () => {
    expect(() => format.blocksizeIsSubset({}, 44100)).toThrow(/Expected .+? to be number/)
    expect(() => format.blocksizeIsSubset(4410, [])).toThrow(/Expected {2}to be number/)
  })

  test('sampleRateIsSubset() should work', () => {
    expect(format.sampleRateIsSubset(44100)).toBeTrue()
    expect(format.sampleRateIsSubset(BigInt(9123812830192))).toBeFalse()
  })

  test('sampleRateIsSubset() should throw if the argument is not number', () => {
    expect(() => format.sampleRateIsSubset(Promise)).toThrow(/Expected .+? to be number/)
  })

  test('vorbiscommentEntryIsLegal() should work', () => {
    expect(format.vorbiscommentEntryIsLegal('KEY=VALUE')).toBeTrue()
    expect(format.vorbiscommentEntryIsLegal('KEY VALUE')).toBeFalse()
  })

  test('vorbiscommentEntryIsLegal() should throw if the argument is not string', () => {
    expect(() => format.vorbiscommentEntryIsLegal(new Map())).toThrow(/Expected .+? to be string/)
  })

  test('vorbiscommentEntryNameIsLegal() should work', () => {
    expect(format.vorbiscommentEntryNameIsLegal('KEY')).toBeTrue()
    expect(format.vorbiscommentEntryNameIsLegal('KEY \n')).toBeFalse()
  })

  test('vorbiscommentEntryNameIsLegal() should throw if the argument is not string', () => {
    expect(() => format.vorbiscommentEntryNameIsLegal(String)).toThrow(/Expected .+? to be string/)
  })

  test('vorbiscommentEntryValueIsLegal() should work', () => {
    expect(format.vorbiscommentEntryValueIsLegal('VALUE IS OK')).toBeTrue()
  })

  test('vorbiscommentEntryValueIsLegal() should throw if the argument is not string', () => {
    expect(() => format.vorbiscommentEntryValueIsLegal(Buffer)).toThrow(/ to be string$/)
  })

  test('seektableIsLegal() should work', () => {
    expect(format.seektableIsLegal(new metadata.SeekTableMetadata())).toBeTrue()
  })

  test('seektableIsLegal() with value not SeekTableMetadata should throw', () => {
    expect(() => format.seektableIsLegal(new metadata.ApplicationMetadata())).toThrow(/is not of type SeekTable/)
  })

  test('seektableSort() should work', () => {
    expect(format.seektableSort(new metadata.SeekTableMetadata())).toBe(0)
  })

  test('seektableSort() with value not SeekTableMetadata should throw', () => {
    expect(() => format.seektableSort(new metadata.ApplicationMetadata())).toThrow(/is not of type SeekTable/)
  })

  test('cuesheetIsLegal() should return reason if wrong', () => {
    const cuesheet = new metadata.CueSheetMetadata()
    expect(format.cuesheetIsLegal(cuesheet)).toBeString()
    expect(format.cuesheetIsLegal(cuesheet)).toBe('cue sheet must have at least one track (the lead-out)')
  })

  test('cuesheetIsLegal() should return null if ok', () => {
    const cuesheet = new metadata.CueSheetMetadata()
    cuesheet.insertBlankTrack(0)
    Array.from(cuesheet)[0].number = 1
    expect(format.cuesheetIsLegal(cuesheet)).toBeNull()
  })

  test('cuesheetIsLegal() with value not CueSheetMetadata should throw', () => {
    expect(() => format.cuesheetIsLegal(new metadata.ApplicationMetadata())).toThrow(/is not of type CueSheet/)
  })

  test('pictureIsLegal() should return null if ok', () => {
    expect(format.pictureIsLegal(new metadata.PictureMetadata())).toBeNull()
  })

  test('pictureIsLegal() should return reason if wrong', () => {
    const picture = new metadata.PictureMetadata()
    picture.mimeType = 'ƒ'
    expect(format.pictureIsLegal(picture)).toBeString()
    expect(format.pictureIsLegal(picture)).toMatch(/^MIME type string must contain only printable ASCII characters/)
  })

  test('pictureIsLegal() with value not PictureMetadata should throw', () => {
    expect(() => format.pictureIsLegal(new metadata.ApplicationMetadata())).toThrow(/is not of type Picture/)
  })

  describe.each([
    'MetadataType',
    'EntropyCodingMethodType',
    'SubframeType',
    'ChannelAssignment',
    'FrameNumberType',
    'PictureType',
  ])('%s', (t) => {
    test('should be an object', () => {
      expect(format[t]).toBeObject()
    })

    test(`${t}String should be an object`, () => {
      expect(format[`${t}String`]).toBeObject()
    })

    test(`${t} and ${t}String should contain the same values`, () => {
      expect(Object.values(format[t]).map((v) => v.toString())).toEqual(Object.keys(format[`${t}String`]))
      expect(Object.values(format[`${t}String`])).toEqual(Object.keys(format[t]))
    })
  })
})
