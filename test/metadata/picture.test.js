import { format, metadata } from '../../lib/api.js'
import { gc } from '../helper/index.js'

const { PictureMetadata } = metadata
const { MetadataType } = format

describe('PictureMetadata', () => {
  it('create new object should work', () => {
    expect(new PictureMetadata()).not.toBeNull()
  })

  it('object has the right type', () => {
    const p = new PictureMetadata()

    expect(p.type).toEqual(MetadataType.PICTURE)
  })

  it('pictureType should change', () => {
    const p = new PictureMetadata()

    p.pictureType = 2

    expect(p.pictureType).toBe(2)
  })

  it('pictureType should throw if not a number', () => {
    const p = new PictureMetadata()

    expect(() => {
      p.pictureType = null
    }).toThrow()
  })

  it('width should change', () => {
    const p = new PictureMetadata()

    p.width = 100

    expect(p.width).toBe(100)
  })

  it('width should throw if not a number', () => {
    const p = new PictureMetadata()

    expect(() => {
      p.width = null
    }).toThrow()
  })

  it('height should change', () => {
    const p = new PictureMetadata()

    p.height = 100

    expect(p.height).toBe(100)
  })

  it('height should throw if not a number', () => {
    const p = new PictureMetadata()

    expect(() => {
      p.height = null
    }).toThrow()
  })

  it('depth should change', () => {
    const p = new PictureMetadata()

    p.depth = 32

    expect(p.depth).toBe(32)
  })

  it('depth should throw if not a number', () => {
    const p = new PictureMetadata()

    expect(() => {
      p.depth = null
    }).toThrow()
  })

  it('colors should change', () => {
    const p = new PictureMetadata()

    p.colors = 1

    expect(p.colors).toBe(1)
  })

  it('colors should throw if not a number', () => {
    const p = new PictureMetadata()

    expect(() => {
      p.colors = null
    }).toThrow()
  })

  it('change mimeType should work', () => {
    const p = new PictureMetadata()

    expect(p.mimeType).toBe('')
    p.mimeType = 'image/jpg'
    expect(p.mimeType).toBe('image/jpg')
  })

  it('change description should work', () => {
    const p = new PictureMetadata()

    expect(p.description).toBe('')
    p.description = 'Big description'
    expect(p.description).toBe('Big description')
  })

  it('change data should work', () => {
    const p = new PictureMetadata()

    expect(p.data).toBeNull()
    p.data = Buffer.from('\x89PNG')
    expect(p.data).toEqual(Buffer.from('\x89PNG'))
  })

  it('change data to empty buffer should work', () => {
    const p = new PictureMetadata()

    expect(p.data).toBeNull()
    p.data = Buffer.alloc(0)
    expect(p.data).toBeNull()
  })

  it('change data to null should work', () => {
    const p = new PictureMetadata()

    expect(p.data).toBeNull()
    p.data = null
    expect(p.data).toBeNull()
  })

  it('isLegal() should work', () => {
    const p = new PictureMetadata()

    expect(p.isLegal()).toBeNull()
  })

  describe('gc', () => {
    it('gc should work', () => {
      expect(gc).not.toThrow()
    })
  })
})
