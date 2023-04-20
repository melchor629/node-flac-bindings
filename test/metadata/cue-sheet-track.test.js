import { describe, expect, it } from 'vitest'
import { metadata, metadata0 } from '../../lib/api.js'
import { pathForFile as fullPathForFile, gc } from '../helper/index.js'

const { CueSheetTrack, CueSheetIndex } = metadata
const { getCuesheet } = metadata0
const { tags: pathForFile } = fullPathForFile

describe('cueSheetTrack', () => {
  it('create a new object should work', () => {
    expect(new CueSheetTrack()).toBeTruthy()
  })

  it('offset should work with numbers', () => {
    const cst = new CueSheetTrack()

    cst.offset = 9231292

    expect(cst.offset).toBe(9231292)
  })

  it('offset should work with bigints', () => {
    const cst = new CueSheetTrack()

    cst.offset = 8127328197321897923n

    expect(cst.offset).toBe(8127328197321897923n)
  })

  it('number should limit numbers to 255 automatically', () => {
    const cst = new CueSheetTrack()

    cst.number = 256

    expect(cst.number).toBe(0)
  })

  it('isrc should be able to set a string of 12 bytes', () => {
    const cst = new CueSheetTrack()

    cst.isrc = '🗿-5374-12'

    expect(cst.isrc).toBe('🗿-5374-12')
  })

  it('isrc should throw if the string is not 12 bytes', () => {
    expect(() => {
      (new CueSheetTrack()).isrc = 'ç'
    }).toThrow()
  })

  it('type should will only write 0 or 1 as value', () => {
    const cst = new CueSheetTrack()

    cst.type = 2
    expect(cst.type).toBe(0)

    cst.type = 19237
    expect(cst.type).toBe(1)
  })

  it('iterator should get all index items', () => {
    const cs = getCuesheet(pathForFile('vc-cs.flac'))

    expect(cs).not.toBeFalsy()
    const results = Array.from(Array.from(cs)[0])
    expect(results[0]).toStrictEqual(new CueSheetIndex(0, 0))
    expect(results[1]).toStrictEqual(new CueSheetIndex(18816, 1))
  })

  it('clone should get a different object but equal in contents', () => {
    const cs = getCuesheet(pathForFile('vc-cs.flac'))
    const cst = Array.from(cs)[0]

    const copy = cst.clone()

    expect(copy).toStrictEqual(cst)
    expect(copy).not.toBe(cst)
  })

  describe('gc', () => {
    it('gc should work', () => {
      expect(gc).not.toThrow()
    })
  })
})
