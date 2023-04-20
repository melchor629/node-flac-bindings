import { describe, expect, it } from 'vitest'
import { format, metadata } from '../../lib/api.js'
import { gc } from '../helper/index.js'

const { UnknownMetadata } = metadata
const { MetadataType } = format

describe('unknownMetadata', () => {
  it('create new object should work', () => {
    expect(new UnknownMetadata()).not.toBeNull()
  })

  it('object has the right type', () => {
    const st = new UnknownMetadata()

    expect(st.type).toStrictEqual(MetadataType.UNDEFINED)
  })

  it('object with custom type has that type', () => {
    const st = new UnknownMetadata(MetadataType.UNDEFINED + 5)

    expect(st.type).toStrictEqual(MetadataType.UNDEFINED)
  })

  it('object with valid metadata type should have UNDEFINED type', () => {
    const st = new UnknownMetadata(MetadataType.CUESHEET)

    expect(st.type).toStrictEqual(MetadataType.UNDEFINED)
  })

  it('object with a really high type should have maximum valid number as type', () => {
    const st = new UnknownMetadata(MetadataType.MAX_METADATA_TYPE * 38)

    expect(st.type).toStrictEqual(MetadataType.UNDEFINED)
  })

  it('data is a null', () => {
    const st = new UnknownMetadata()

    expect(st.data).toBeNull()
  })

  describe('gc', () => {
    it('gc should work', () => {
      expect(gc).not.toThrow()
    })
  })
})
