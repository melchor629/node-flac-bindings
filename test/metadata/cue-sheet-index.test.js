import { metadata } from '../../lib/api.js'
import { gc } from '../helper/index.js'

const { CueSheetIndex } = metadata

describe('CueSheetIndex', () => {
  it('create a new object with parameters should work', () => {
    const csi = new CueSheetIndex(10, 0)

    expect(csi.offset).toBe(10)
    expect(csi.number).toBe(0)
  })

  it('create a new object with offset not being a number should throw', () => {
    expect(() => new CueSheetIndex(null, 0)).toThrow()
  })

  it('create a new object with number not being a number should throw', () => {
    expect(() => new CueSheetIndex(1, null)).toThrow()
  })

  it('use of bigint in offset should work', () => {
    const csi = new CueSheetIndex()

    csi.offset = 9007199254740993n

    expect(csi.offset).toBe(9007199254740993n)
  })

  it('set offset is not a number should throw', () => {
    const csi = new CueSheetIndex()

    expect(() => {
      csi.offset = null
    }).toThrow()
  })

  it('set number should work', () => {
    const csi = new CueSheetIndex()

    csi.number = 10

    expect(csi.number).toBe(10)
  })

  it('set number is not a number should throw', () => {
    const csi = new CueSheetIndex()

    expect(() => {
      csi.number = null
    }).toThrow()
  })

  describe('gc', () => {
    it('gc should work', () => {
      expect(gc).not.toThrow()
    })
  })
})
