const { CueSheetTrack, CueSheetIndex } = require('../../lib/index').api.metadata
const { getCuesheet } = require('../../lib/index').api.metadata0
const { pathForFile: { tags: pathForFile }, gc } = require('../helper')

describe('CueSheetTrack', () => {
  it('create a new object should work', () => {
    expect(new CueSheetTrack()).toBeTruthy()
  })

  it('offset should work with numbers', () => {
    const cst = new CueSheetTrack()

    cst.offset = 9231292

    expect(cst.offset).toEqual(9231292)
  })

  it('offset should work with bigints', () => {
    const cst = new CueSheetTrack()

    cst.offset = 8127328197321897923n

    expect(cst.offset).toEqual(8127328197321897923n)
  })

  it('number should limit numbers to 255 automatically', () => {
    const cst = new CueSheetTrack()

    cst.number = 256

    expect(cst.number).toEqual(0)
  })

  it('isrc should be able to set a string of 12 bytes', () => {
    const cst = new CueSheetTrack()

    cst.isrc = '🗿-5374-12'

    expect(cst.isrc).toEqual('🗿-5374-12')
  })

  it('isrc should throw if the string is not 12 bytes', () => {
    expect(() => {
      (new CueSheetTrack()).isrc = 'ç'
    }).toThrow()
  })

  it('type should will only write 0 or 1 as value', () => {
    const cst = new CueSheetTrack()

    cst.type = 2
    expect(cst.type).toEqual(0)

    cst.type = 19237
    expect(cst.type).toEqual(1)
  })

  it('iterator should get all index items', () => {
    const cs = getCuesheet(pathForFile('vc-cs.flac'))

    expect(cs).not.toBe(false)
    const results = Array.from(Array.from(cs)[0])
    expect(results[0]).toEqual(new CueSheetIndex(0, 0))
    expect(results[1]).toEqual(new CueSheetIndex(18816, 1))
  })

  it('clone should get a different object but equal in contents', () => {
    const cs = getCuesheet(pathForFile('vc-cs.flac'))
    const cst = Array.from(cs)[0]

    const copy = cst.clone()

    expect(copy).toEqual(cst)
    expect(copy).not.toBe(cst)
  })

  describe('gc', () => {
    it('gc should work', () => {
      expect(gc).not.toThrow()
    })
  })
})
