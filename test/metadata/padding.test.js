const { PaddingMetadata } = require('../../lib/index').api.metadata
const { MetadataType } = require('../../lib/index').api.format
const { gc } = require('../helper')

describe('PaddingMetadata', () => {
  it('create new object should work', () => {
    expect(new PaddingMetadata()).not.toBeNull()
  })

  it('the object should have the right type', () => {
    const p = new PaddingMetadata()

    expect(p.type).toEqual(MetadataType.PADDING)
  })

  it('the object initially has 0 bytes of padding', () => {
    const p = new PaddingMetadata()

    expect(p).toHaveLength(0)
  })

  it('the object created with a size should have that size in bytes of padding', () => {
    const p = new PaddingMetadata(123)

    expect(p).toHaveLength(123)
  })

  describe('gc', () => {
    it('gc should work', () => {
      expect(gc).not.toThrow()
    })
  })
})
