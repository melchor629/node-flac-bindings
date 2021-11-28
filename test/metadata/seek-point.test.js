import { metadata } from '../../lib/api.js'
import { gc } from '../helper/index.js'

const { SeekPoint } = metadata

describe('SeekPoint', () => {
  it('create new object should work', () => {
    expect(new SeekPoint()).not.toBeNull()
  })

  it('create with no arguments should set values to 0', () => {
    const sp = new SeekPoint()

    expect(sp.sampleNumber).toBe(0)
    expect(sp.streamOffset).toBe(0)
    expect(sp.frameSamples).toBe(0)
  })

  it('create with one argument should modify the sampleNumber', () => {
    const sp = new SeekPoint(1n)

    expect(sp.sampleNumber).toBe(1)
    expect(sp.streamOffset).toBe(0)
    expect(sp.frameSamples).toBe(0)
  })

  it('create with two arguments should modify the sampleNumber and the streamOffset', () => {
    const sp = new SeekPoint(1n, 22n)

    expect(sp.sampleNumber).toBe(1)
    expect(sp.streamOffset).toBe(22)
    expect(sp.frameSamples).toBe(0)
  })

  it('create with three arguments should modify the three attributes', () => {
    const sp = new SeekPoint(1n, 22n, 333n)

    expect(sp.sampleNumber).toBe(1)
    expect(sp.streamOffset).toBe(22)
    expect(sp.frameSamples).toBe(333)
  })

  it('set sampleNumber should work', () => {
    const sp = new SeekPoint()

    sp.sampleNumber = 1

    expect(sp.sampleNumber).toBe(1)
  })

  it('set sampleNumber should throw if not a number', () => {
    const sp = new SeekPoint()

    expect(() => {
      sp.sampleNumber = null
    }).toThrow()
  })

  it('set streamOffset should work', () => {
    const sp = new SeekPoint()

    sp.streamOffset = 2

    expect(sp.streamOffset).toBe(2)
  })

  it('set streamOffset should throw if not a number', () => {
    const sp = new SeekPoint()

    expect(() => {
      sp.streamOffset = null
    }).toThrow()
  })

  it('set frameSamples should work', () => {
    const sp = new SeekPoint()

    sp.frameSamples = 3

    expect(sp.frameSamples).toBe(3)
  })

  it('set frameSamples should throw if not a number', () => {
    const sp = new SeekPoint()

    expect(() => {
      sp.frameSamples = null
    }).toThrow()
  })

  describe('gc', () => {
    it('gc should work', () => {
      expect(gc).not.toThrow()
    })
  })
})
