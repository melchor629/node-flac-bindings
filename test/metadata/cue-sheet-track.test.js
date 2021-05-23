const { assert } = require('chai')
const { CueSheetTrack, CueSheetIndex } = require('../../lib/index').api.metadata
const { getCuesheet } = require('../../lib/index').api.metadata0
const { pathForFile: { tags: pathForFile }, gc } = require('../helper')

describe('CueSheetTrack', function () {
  it('create a new object should work', function () {
    return new CueSheetTrack()
  })

  it('offset should work with numbers', function () {
    const cst = new CueSheetTrack()

    cst.offset = 9231292

    assert.equal(cst.offset, 9231292)
  })

  it('offset should work with bigints', function () {
    const cst = new CueSheetTrack()

    cst.offset = 8127328197321897923n

    assert.equal(cst.offset, 8127328197321897923n)
  })

  it('number should limit numbers to 255 automatically', function () {
    const cst = new CueSheetTrack()

    cst.number = 256

    assert.equal(cst.number, 0)
  })

  it('isrc should be able to set a string of 12 bytes', function () {
    const cst = new CueSheetTrack()

    cst.isrc = '🗿-5374-12'

    assert.equal(cst.isrc, '🗿-5374-12')
  })

  it('isrc should throw if the string is not 12 bytes', function () {
    assert.throws(() => {
      (new CueSheetTrack()).isrc = 'ç'
    })
  })

  it('type should will only write 0 or 1 as value', function () {
    const cst = new CueSheetTrack()

    cst.type = 2
    assert.equal(cst.type, 0)

    cst.type = 19237
    assert.equal(cst.type, 1)
  })

  it('iterator should get all index items', function () {
    const cs = getCuesheet(pathForFile('vc-cs.flac'))

    assert.isNotFalse(cs, 'The file vc-cs.flac must exist')
    const results = Array.from(Array.from(cs)[0])
    assert.deepEqual(results[0], new CueSheetIndex(0, 0))
    assert.deepEqual(results[1], new CueSheetIndex(18816, 1))
  })

  it('clone should get a different object but equal in contents', function () {
    const cs = getCuesheet(pathForFile('vc-cs.flac'))
    const cst = Array.from(cs)[0]

    const copy = cst.clone()

    assert.deepEqual(copy, cst)
    assert.notEqual(copy, cst)
  })

  describe('gc', function () {
    it('gc should work', function () {
      gc()
    })
  })
})
