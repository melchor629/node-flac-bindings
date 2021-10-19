const { StreamInfoMetadata } = require('../../lib/index').api.metadata
const { MetadataType } = require('../../lib/index').api.format
const { getStreaminfoAsync } = require('../../lib/index').api.metadata0
const { pathForFile: { tags: pathForFile }, gc } = require('../helper')

describe('StreamInfoMetadata', () => {
  it('create new object should work', () => {
    expect(new StreamInfoMetadata()).not.toBeNull()
  })

  it('object has the right type', () => {
    const st = new StreamInfoMetadata()

    expect(st.type).toEqual(MetadataType.STREAMINFO)
  })

  it('set minBlocksize should work', () => {
    const st = new StreamInfoMetadata()

    st.minBlocksize = 1024

    expect(st.minBlocksize).toBe(1024)
  })

  it('set minBlocksize should throw if value is not a number', () => {
    const st = new StreamInfoMetadata()

    expect(() => {
      st.minBlocksize = null
    }).toThrow()
  })

  it('set maxBlocksize should work', () => {
    const st = new StreamInfoMetadata()

    st.maxBlocksize = 1024

    expect(st.maxBlocksize).toBe(1024)
  })

  it('set maxBlocksize should throw if value is not a number', () => {
    const st = new StreamInfoMetadata()

    expect(() => {
      st.maxBlocksize = null
    }).toThrow()
  })

  it('set minFramesize should work', () => {
    const st = new StreamInfoMetadata()

    st.minFramesize = 1024

    expect(st.minFramesize).toBe(1024)
  })

  it('set minFramesize should throw if value is not a number', () => {
    const st = new StreamInfoMetadata()

    expect(() => {
      st.minFramesize = null
    }).toThrow()
  })

  it('set maxFramesize should work', () => {
    const st = new StreamInfoMetadata()

    st.maxFramesize = 1024

    expect(st.maxFramesize).toBe(1024)
  })

  it('set maxFramesize should throw if value is not a number', () => {
    const st = new StreamInfoMetadata()

    expect(() => {
      st.maxFramesize = null
    }).toThrow()
  })

  it('set channels should work', () => {
    const st = new StreamInfoMetadata()

    st.channels = 2

    expect(st.channels).toBe(2)
  })

  it('set channels should throw if value is not a number', () => {
    const st = new StreamInfoMetadata()

    expect(() => {
      st.channels = null
    }).toThrow()
  })

  it('set bitsPerSample should work', () => {
    const st = new StreamInfoMetadata()

    st.bitsPerSample = 16

    expect(st.bitsPerSample).toBe(16)
  })

  it('set bitsPerSample should throw if value is not a number', () => {
    const st = new StreamInfoMetadata()

    expect(() => {
      st.bitsPerSample = null
    }).toThrow()
  })

  it('set sampleRate should work', () => {
    const st = new StreamInfoMetadata()

    st.sampleRate = 44100

    expect(st.sampleRate).toBe(44100)
  })

  it('set sampleRate should throw if value is not a number', () => {
    const st = new StreamInfoMetadata()

    expect(() => {
      st.sampleRate = null
    }).toThrow()
  })

  it('set totalSamples should work', () => {
    const st = new StreamInfoMetadata()

    st.totalSamples = 441000n

    expect(st.totalSamples).toBe(441000)
  })

  it('set totalSamples should throw if value is not a number', () => {
    const st = new StreamInfoMetadata()

    expect(() => {
      st.totalSamples = null
    }).toThrow()
  })

  it('set md5sum should work', () => {
    const st = new StreamInfoMetadata()

    st.md5sum = Buffer.allocUnsafe(16)

    expect(Buffer.isBuffer(st.md5sum)).toBe(true)
  })

  it('set md5sum should throw if value is not a buffer', () => {
    const st = new StreamInfoMetadata()

    expect(() => {
      st.md5sum = 123456789
    }).toThrow()
  })

  it('set md5sum should throw if value buffer is less than 16 bytes', () => {
    const st = new StreamInfoMetadata()

    expect(() => {
      st.md5sum = Buffer.alloc(10)
    }).toThrow()
  })

  it('is correct from mapping', async () => {
    const st = await getStreaminfoAsync(pathForFile('no.flac'))

    expect(st.bitsPerSample).toBe(16)
    expect(st.channels).toBe(2)
    expect(st.maxBlocksize).toBe(4096)
    expect(st.minBlocksize).toBe(4096)
    expect(st.md5sum).toEqual(Buffer.from('7ÓË©ó»PLÀ8H_pG', 'ascii'))
    expect(st.maxFramesize).toBe(4214)
    expect(st.minFramesize).toBe(2565)
    expect(st.sampleRate).toBe(44100)
    expect(st.totalSamples).toBe(10651)
  })

  describe('gc', () => {
    it('gc should work', () => {
      expect(gc).not.toThrow()
    })
  })
})
