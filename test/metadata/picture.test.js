const { PictureMetadata } = require('../../lib/index').api.metadata
const { MetadataType } = require('../../lib/index').api.format
const { gc } = require('../helper')

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

    expect(p.pictureType).toEqual(2)
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

    expect(p.width).toEqual(100)
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

    expect(p.height).toEqual(100)
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

    expect(p.depth).toEqual(32)
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

    expect(p.colors).toEqual(1)
  })

  it('colors should throw if not a number', () => {
    const p = new PictureMetadata()

    expect(() => {
      p.colors = null
    }).toThrow()
  })

  it('change mimeType should work', () => {
    const p = new PictureMetadata()

    expect(p.mimeType).toEqual('')
    p.mimeType = 'image/jpg'
    expect(p.mimeType).toEqual('image/jpg')
  })

  it('change description should work', () => {
    const p = new PictureMetadata()

    expect(p.description).toEqual('')
    p.description = 'Big description'
    expect(p.description).toEqual('Big description')
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
