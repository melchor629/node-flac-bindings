import { describe, expect, it } from 'vitest'
import { format, metadata } from '../../lib/api.js'
import { gc } from '../helper/index.js'

const { PaddingMetadata } = metadata
const { MetadataType } = format

describe('paddingMetadata', () => {
  it('create new object should work', () => {
    expect(new PaddingMetadata()).not.toBeNull()
  })

  it('the object should have the right type', () => {
    const p = new PaddingMetadata()

    expect(p.type).toStrictEqual(MetadataType.PADDING)
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
