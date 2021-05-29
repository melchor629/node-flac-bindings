const {
  ApplicationMetadata,
  Metadata,
} = require('../../lib/index').api.metadata
const { gc } = require('../helper')

describe('Metadata', () => {
  it('create metadata object should throw', () => {
    expect(() => new Metadata(1)).toThrow()
  })

  it('get isLast should work', () => {
    const m = new ApplicationMetadata()

    expect(m.isLast).toBe(false)
  })

  it('isEqual() returns true if the objects are similar', () => {
    const am1 = new ApplicationMetadata()
    const am2 = new ApplicationMetadata()
    am1.data = Buffer.from('no')
    am1.id = Buffer.from('node')
    am2.data = am1.data
    am2.id = am1.id

    expect(am1.isEqual(am2)).toBe(true)
    expect(am2.isEqual(am1)).toBe(true)
  })

  it('isEqual() returns false if the objects are different', () => {
    const am1 = new ApplicationMetadata()
    const am2 = new ApplicationMetadata()
    am1.data = Buffer.from('nodo')
    am1.id = Buffer.from('node')
    am2.id = am1.data
    am2.data = am1.id

    expect(am1.isEqual(am2)).toBe(false)
    expect(am2.isEqual(am1)).toBe(false)
  })

  it('isEqual() throws if first argument is not provided', () => {
    expect(() => new ApplicationMetadata().isEqual()).toThrow()
  })

  it('isEqual() throws if first argument is not Metadata object', () => {
    expect(() => new ApplicationMetadata().isEqual({})).toThrow()
  })

  it('clone() should create a different object but equal', () => {
    const am1 = new ApplicationMetadata()
    am1.data = Buffer.from('nodo')
    am1.id = Buffer.from('node')

    const am2 = am1.clone()

    expect(am1.isEqual(am2)).toBe(true)
    expect(am2.isEqual(am1)).toBe(true)
    expect(Object.is(am1, am2)).toBe(false)
  })

  describe('gc', () => {
    it('gc should work', () => {
      gc()
    })
  })
})
