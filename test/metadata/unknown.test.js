const { UnknownMetadata } = require('../../lib/index').api.metadata
const { MetadataType } = require('../../lib/index').api.format
const { gc } = require('../helper')

describe('UnknownMetadata', () => {
  it('create new object should work', () => {
    expect(new UnknownMetadata()).not.toBeNull()
  })

  it('object has the right type', () => {
    const st = new UnknownMetadata()

    expect(st.type).toEqual(MetadataType.UNDEFINED)
  })

  it('object with custom type has that type', () => {
    const st = new UnknownMetadata(MetadataType.UNDEFINED + 5)

    expect(st.type).toEqual(MetadataType.UNDEFINED)
  })

  it('object with valid metadata type should have UNDEFINED type', () => {
    const st = new UnknownMetadata(MetadataType.CUESHEET)

    expect(st.type).toEqual(MetadataType.UNDEFINED)
  })

  it('object with a really high type should have maximum valid number as type', () => {
    const st = new UnknownMetadata(MetadataType.MAX_METADATA_TYPE * 38)

    expect(st.type).toEqual(MetadataType.UNDEFINED)
  })

  it('data is a buffer', () => {
    const st = new UnknownMetadata()

    expect(Buffer.isBuffer(st.data) || st.data === null).toBe(true)
  })

  describe('gc', () => {
    it('gc should work', () => {
      expect(gc).not.toThrow()
    })
  })
})
