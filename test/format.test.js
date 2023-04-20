import { describe, expect, it } from 'vitest'
import { format, metadata } from '../lib/api.js'

describe('format', () => {
  it('vERSION_STRING is defined', () => {
    expect(format.VERSION_STRING).toBeString()
    expect(format.VERSION_STRING).toMatch(/^\d+\.\d+\.\d+$/)
  })

  it('vENDOR_STRING is defined', () => {
    expect(format.VENDOR_STRING).toBeString()
    expect(format.VENDOR_STRING).toMatch(/^reference libFLAC \d+\.\d+\.\d+ \d+$/)
  })

  it('aPI_SUPPORTS_OGG_FLAC is defined', () => {
    expect(format.API_SUPPORTS_OGG_FLAC).toBeBoolean()
  })

  it('sampleRateIsValid() should work', () => {
    expect(format.sampleRateIsValid(96000)).toBeTrue()
    expect(format.sampleRateIsValid(BigInt(112938129312))).toBeFalse()
  })

  it('sampleRateIsValid() should throw if the argument is not number', () => {
    expect(() => format.sampleRateIsValid('p')).toThrow(/Expected p to be number/)
    expect(format.sampleRateIsValid(Infinity)).toBeFalse()
    expect(format.sampleRateIsValid(NaN)).toBeFalse()
  })

  it('blocksizeIsSubset() should work', () => {
    expect(format.blocksizeIsSubset(4410, 44100)).toBeTrue()
  })

  it('blocksizeIsSubset() should throw if any of the arguments is not number', () => {
    expect(() => format.blocksizeIsSubset({}, 44100)).toThrow(/Expected .+? to be number/)
    expect(() => format.blocksizeIsSubset(4410, [])).toThrow(/Expected {2}to be number/)
  })

  it('sampleRateIsSubset() should work', () => {
    expect(format.sampleRateIsSubset(44100)).toBeTrue()
    expect(format.sampleRateIsSubset(BigInt(9123812830192))).toBeFalse()
  })

  it('sampleRateIsSubset() should throw if the argument is not number', () => {
    expect(() => format.sampleRateIsSubset(Promise)).toThrow(/Expected .+? to be number/)
  })

  it('vorbiscommentEntryIsLegal() should work', () => {
    expect(format.vorbiscommentEntryIsLegal('KEY=VALUE')).toBeTrue()
    expect(format.vorbiscommentEntryIsLegal('KEY VALUE')).toBeFalse()
  })

  it('vorbiscommentEntryIsLegal() should throw if the argument is not string', () => {
    expect(() => format.vorbiscommentEntryIsLegal(new Map())).toThrow(/Expected .+? to be string/)
  })

  it('vorbiscommentEntryNameIsLegal() should work', () => {
    expect(format.vorbiscommentEntryNameIsLegal('KEY')).toBeTrue()
    expect(format.vorbiscommentEntryNameIsLegal('KEY \n')).toBeFalse()
  })

  it('vorbiscommentEntryNameIsLegal() should throw if the argument is not string', () => {
    expect(() => format.vorbiscommentEntryNameIsLegal(String)).toThrow(/Expected .+? to be string/)
  })

  it('vorbiscommentEntryValueIsLegal() should work', () => {
    expect(format.vorbiscommentEntryValueIsLegal('VALUE IS OK')).toBeTrue()
  })

  it('vorbiscommentEntryValueIsLegal() should throw if the argument is not string', () => {
    expect(() => format.vorbiscommentEntryValueIsLegal(Buffer)).toThrow(/ to be string$/)
  })

  it('seektableIsLegal() should work', () => {
    expect(format.seektableIsLegal(new metadata.SeekTableMetadata())).toBeTrue()
  })

  it('seektableIsLegal() with value not SeekTableMetadata should throw', () => {
    expect(() => format.seektableIsLegal(new metadata.ApplicationMetadata()))
      .toThrow(/is not of type SeekTable/)
  })

  it('seektableSort() should work', () => {
    expect(format.seektableSort(new metadata.SeekTableMetadata())).toBe(0)
  })

  it('seektableSort() with value not SeekTableMetadata should throw', () => {
    expect(() => format.seektableSort(new metadata.ApplicationMetadata()))
      .toThrow(/is not of type SeekTable/)
  })

  it('cuesheetIsLegal() should return reason if wrong', () => {
    const cuesheet = new metadata.CueSheetMetadata()
    expect(format.cuesheetIsLegal(cuesheet)).toBeString()
    expect(format.cuesheetIsLegal(cuesheet))
      .toBe('cue sheet must have at least one track (the lead-out)')
  })

  it('cuesheetIsLegal() should return null if ok', () => {
    const cuesheet = new metadata.CueSheetMetadata()
    cuesheet.insertBlankTrack(0)
    Array.from(cuesheet)[0].number = 1
    expect(format.cuesheetIsLegal(cuesheet)).toBeNull()
  })

  it('cuesheetIsLegal() with value not CueSheetMetadata should throw', () => {
    expect(() => format.cuesheetIsLegal(new metadata.ApplicationMetadata()))
      .toThrow(/is not of type CueSheet/)
  })

  it('pictureIsLegal() should return null if ok', () => {
    expect(format.pictureIsLegal(new metadata.PictureMetadata())).toBeNull()
  })

  it('pictureIsLegal() should return reason if wrong', () => {
    const picture = new metadata.PictureMetadata()
    picture.mimeType = 'ƒ'
    expect(format.pictureIsLegal(picture)).toBeString()
    expect(format.pictureIsLegal(picture))
      .toMatch(/^MIME type string must contain only printable ASCII characters/)
  })

  it('pictureIsLegal() with value not PictureMetadata should throw', () => {
    expect(() => format.pictureIsLegal(new metadata.ApplicationMetadata()))
      .toThrow(/is not of type Picture/)
  })

  describe.each([
    'MetadataType',
    'EntropyCodingMethodType',
    'SubframeType',
    'ChannelAssignment',
    'FrameNumberType',
    'PictureType',
  ])('%s', (t) => {
    it('should be an object', () => {
      expect(format[t]).toBeObject()
    })

    it(`${t}String should be an object`, () => {
      expect(format[`${t}String`]).toBeObject()
    })

    it(`${t} and ${t}String should contain the same values`, () => {
      expect(Object.values(format[t]).map((v) => v.toString()))
        .toStrictEqual(Object.keys(format[`${t}String`]))
      expect(Object.values(format[`${t}String`])).toStrictEqual(Object.keys(format[t]))
    })
  })
})
