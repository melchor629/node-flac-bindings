const { api: { fns } } = require('../lib')

describe('fns', () => {
  describe('convertSampleFormat', () => {
    it('throws if input is not object', () => {
      expect(() => fns.convertSampleFormat(null)).toThrow()
    })

    it('throws if buffer size is smaller', () => {
      expect(() => fns.convertSampleFormat({
        buffer: Buffer.alloc(16),
        inBps: 2,
        samples: 10n,
      })).toThrow(/Buffer has size/)
    })

    it('throws if 0 samples is passed', () => {
      expect(() => fns.convertSampleFormat({
        buffer: Buffer.alloc(16),
        samples: 0n,
        inBps: 2,
      })).toThrow(/Invalid value/)
    })

    it('throws for unsupported input bits per sample', () => {
      expect(() => fns.convertSampleFormat({
        buffer: Buffer.alloc(16),
        samples: 2n,
        inBps: 5,
      })).toThrow(/Unsupported 5 bits per sample/)
    })

    it('throws for unsupported output bits per sample', () => {
      expect(() => fns.convertSampleFormat({
        buffer: Buffer.alloc(16),
        samples: 2n,
        inBps: 2,
        outBps: 5,
      })).toThrow(/Unsupported 5 bits per sample/)
    })

    it('returns same buffer if no conversion is required', () => {
      const buffer = Buffer.alloc(4 * 2, 1)

      const returnedBuffer = fns.convertSampleFormat({
        buffer,
        inBps: 2,
        outBps: 2,
      })

      expect(returnedBuffer).toBe(buffer)
    })

    it('scales down properly', () => {
      const buffer = Buffer.from([
        1, 0, 0,
        2, 0, 0,
        3, 0, 0,
      ])
      const expectedBuffer = Buffer.from([
        1,
        2,
        3,
      ])

      const returnedBuffer = fns.convertSampleFormat({
        buffer,
        inBps: 3,
        outBps: 1,
      })

      expect(returnedBuffer).toEqual(expectedBuffer)
    })

    it('scales up properly', () => {
      const buffer = Buffer.from([
        1, 0,
        2, 0,
        3, 0,
        -1, -1,
      ])
      const expectedBuffer = Buffer.from([
        1, 0, 0, 0,
        2, 0, 0, 0,
        3, 0, 0, 0,
        0xFF, 0xFF, 0xFF, 0xFF,
      ])

      const returnedBuffer = fns.convertSampleFormat({
        buffer,
        inBps: 2,
      })

      expect(returnedBuffer).toEqual(expectedBuffer)
    })

    it('larger buffer with samples provided returns smaller buffer', () => {
      const buffer = Buffer.from([
        1, 0,
        2, 0,
        3, 0,
      ])
      const expectedBuffer = Buffer.from([
        1, 0, 0, 0,
        2, 0, 0, 0,
      ])

      const returnedBuffer = fns.convertSampleFormat({
        buffer,
        inBps: 2,
        samples: 2,
      })

      expect(returnedBuffer).toEqual(expectedBuffer)
    })
  })

  describe('zipAudio', () => {
    it('throws if input is not object', () => {
      expect(() => fns.zipAudio(null)).toThrow()
    })

    it('throws if any buffer is not a buffer', () => {
      expect(() => fns.zipAudio({
        buffers: [null],
      })).toThrow()
    })

    it('throws if any buffer is smaller than expected', () => {
      expect(() => fns.zipAudio({
        buffers: [Buffer.alloc(4 * 2), Buffer.alloc(4 * 2)],
        inBps: 3,
        samples: 3,
      })).toThrow()
    })

    it('throws if samples is 0', () => {
      expect(() => fns.zipAudio({
        buffers: [Buffer.alloc(4 * 2)],
        inBps: 2,
        samples: 0,
      })).toThrow()
    })

    it('throws for unsupported input bits per sample', () => {
      expect(() => fns.zipAudio({
        buffers: [Buffer.alloc(4 * 2)],
        samples: 1n,
        inBps: 5,
      })).toThrow(/Unsupported 5 bits per sample/)
    })

    it('throws for unsupported output bits per sample', () => {
      expect(() => fns.zipAudio({
        buffers: [Buffer.alloc(4 * 2)],
        samples: 2n,
        inBps: 2,
        outBps: 5,
      })).toThrow(/Unsupported 5 bits per sample/)
    })

    it('returns interleaved audio', () => {
      const buffers = [
        Buffer.from([
          1, 0,
          2, 0,
          3, 0,
        ]),
        Buffer.from([
          0, 1,
          0, 2,
          0, 3,
        ]),
      ]
      const expectedBuffer = Buffer.from([
        1, 0,
        0, 1,
        2, 0,
        0, 2,
        3, 0,
        0, 3,
      ])

      const returnedBuffer = fns.zipAudio({
        buffers,
        samples: 3,
        inBps: 2,
        outBps: 2,
      })

      expect(returnedBuffer).toEqual(expectedBuffer)
    })

    it('returns interleaved audio scaled down', () => {
      const buffers = [
        Buffer.from([
          1, 0, 0,
          2, 0, 0,
          3, 0, 0,
        ]),
        Buffer.from([
          0, 1, 0,
          0, 2, 0,
          0, 3, 0,
        ]),
      ]
      const expectedBuffer = Buffer.from([
        1, 0,
        0, 1,
        2, 0,
        0, 2,
        3, 0,
        0, 3,
      ])

      const returnedBuffer = fns.zipAudio({
        buffers,
        samples: 3,
        inBps: 3,
        outBps: 2,
      })

      expect(returnedBuffer).toEqual(expectedBuffer)
    })

    it('returns interleaved audio scaled up', () => {
      const buffers = [
        Buffer.from([
          1, 0,
          2, 0,
          3, 0,
          -1, -1,
        ]),
        Buffer.from([
          0, 1,
          0, 2,
          0, 3,
          -1, -1,
        ]),
      ]
      const expectedBuffer = Buffer.from([
        1, 0, 0,
        0, 1, 0,
        2, 0, 0,
        0, 2, 0,
        3, 0, 0,
        0, 3, 0,
        0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF,
      ])

      const returnedBuffer = fns.zipAudio({
        buffers,
        samples: 4,
        inBps: 2,
        outBps: 3,
      })

      expect(returnedBuffer).toEqual(expectedBuffer)
    })
  })

  describe('unzipAudio', () => {
    it('throws if input is not object', () => {
      expect(() => fns.unzipAudio(null)).toThrow()
    })

    it('throws if any buffer is not a buffer', () => {
      expect(() => fns.unzipAudio({
        buffer: null,
      })).toThrow()
    })

    it('throws if buffer is smaller than expected', () => {
      expect(() => fns.unzipAudio({
        buffer: Buffer.alloc(4 * 2),
        inBps: 3,
        samples: 3,
      })).toThrow()
    })

    it('throws if samples is 0', () => {
      expect(() => fns.unzipAudio({
        buffer: Buffer.alloc(4 * 2),
        inBps: 2,
        samples: 0,
      })).toThrow()
    })

    it('throws for unsupported input bits per sample', () => {
      expect(() => fns.unzipAudio({
        buffer: Buffer.alloc(4 * 2),
        channels: 1,
        samples: 1n,
        inBps: 5,
      })).toThrow(/Unsupported 5 bits per sample/)
    })

    it('throws for unsupported output bits per sample', () => {
      expect(() => fns.unzipAudio({
        buffer: Buffer.alloc(4 * 2),
        channels: 1,
        samples: 2n,
        inBps: 2,
        outBps: 5,
      })).toThrow(/Unsupported 5 bits per sample/)
    })

    it('returns interleaved audio', () => {
      const buffer = Buffer.from([
        1, 0,
        0, 1,
        2, 0,
        0, 2,
        3, 0,
        0, 3,
      ])
      const expectedBuffers = [
        Buffer.from([
          1, 0,
          2, 0,
          3, 0,
        ]),
        Buffer.from([
          0, 1,
          0, 2,
          0, 3,
        ]),
      ]

      const returnedBuffer = fns.unzipAudio({
        buffer,
        samples: 3,
        inBps: 2,
        outBps: 2,
      })

      expect(returnedBuffer).toEqual(expectedBuffers)
    })

    it('returns interleaved audio scaled down', () => {
      const buffer = Buffer.from([
        1, 0, 0,
        0, 1, 0,
        2, 0, 0,
        0, 2, 0,
        3, 0, 0,
        0, 3, 0,
      ])
      const expectedBuffers = [
        Buffer.from([
          1, 0,
          2, 0,
          3, 0,
        ]),
        Buffer.from([
          0, 1,
          0, 2,
          0, 3,
        ]),
      ]

      const returnedBuffer = fns.unzipAudio({
        buffer,
        samples: 3,
        inBps: 3,
        outBps: 2,
      })

      expect(returnedBuffer).toEqual(expectedBuffers)
    })

    it('returns interleaved audio scaled up', () => {
      const buffer = Buffer.from([
        1, 0,
        0, 1,
        2, 0,
        0, 2,
        3, 0,
        0, 3,
        0xFF, 0xFF,
        0xFF, 0xFF,
      ])
      const expectedBuffers = [
        Buffer.from([
          1, 0, 0,
          2, 0, 0,
          3, 0, 0,
          -1, -1, -1,
        ]),
        Buffer.from([
          0, 1, 0,
          0, 2, 0,
          0, 3, 0,
          -1, -1, -1,
        ]),
      ]

      const returnedBuffer = fns.unzipAudio({
        buffer,
        samples: 4,
        inBps: 2,
        outBps: 3,
      })

      expect(returnedBuffer).toEqual(expectedBuffers)
    })
  })
})
