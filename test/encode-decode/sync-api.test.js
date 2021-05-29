const temp = require('temp').track()
const fs = require('fs')
const { api } = require('../../lib/index')
const {
  pathForFile: { audio: pathForFile },
  createDeferredScope,
  comparePCM,
  generateFlacCallbacks,
  joinIntoInterleaved,
  loopPcmAudio: {
    totalSamples,
    encData,
    encDataAlt,
    okData,
  },
} = require('../helper')

let tmpFile
let deferredScope = null
beforeEach(() => {
  tmpFile = temp.openSync('flac-bindings.encode-decode.sync-api')
  deferredScope = createDeferredScope()
})

afterEach(() => {
  fs.closeSync(tmpFile.fd)
  temp.cleanupSync()
  deferredScope.finalize()
})

describe('encode & decode: sync api', () => {
  it('decode using stream (non-ogg)', () => {
    const callbacks = generateFlacCallbacks.sync(api.Decoder, pathForFile('loop.flac'), 'r')
    deferredScope.defer(() => callbacks.close())
    const dec = new api.Decoder()
    const allBuffers = []
    dec.initStream(
      callbacks.read,
      callbacks.seek,
      callbacks.tell,
      callbacks.length,
      callbacks.eof,
      (_, buffers) => {
        allBuffers.push(buffers.map((b) => Buffer.from(b)))
        return 0
      },
      null,
      // eslint-disable-next-line no-console
      (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
    )

    expect(dec.processUntilEndOfMetadata()).toBe(true)
    expect(dec.processUntilEndOfStream()).toBe(true)
    expect(dec.flush()).toBe(true)
    expect(dec.finish()).toBe(true)

    const [finalBuffer, samples] = joinIntoInterleaved(allBuffers)
    expect(samples).toEqual(totalSamples)
    comparePCM(okData, finalBuffer, 32)
  })

  it('decode using stream (ogg)', () => {
    const callbacks = generateFlacCallbacks.sync(api.Decoder, pathForFile('loop.oga'), 'r')
    deferredScope.defer(() => callbacks.close())
    const dec = new api.Decoder()
    const allBuffers = []
    dec.initOggStream(
      callbacks.read,
      callbacks.seek,
      callbacks.tell,
      callbacks.length,
      callbacks.eof,
      (_, buffers) => {
        allBuffers.push(buffers.map((b) => Buffer.from(b)))
        return 0
      },
      null,
      // eslint-disable-next-line no-console
      (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
    )

    expect(dec.processUntilEndOfMetadata()).toBe(true)
    expect(dec.processUntilEndOfStream()).toBe(true)
    expect(dec.flush()).toBe(true)
    expect(dec.finish()).toBe(true)

    const [finalBuffer, samples] = joinIntoInterleaved(allBuffers)
    expect(samples).toEqual(totalSamples)
    comparePCM(okData, finalBuffer, 32)
  })

  it('decode using file (non-ogg)', () => {
    const dec = new api.Decoder()
    const allBuffers = []
    dec.initFile(
      pathForFile('loop.flac'),
      (_, buffers) => {
        allBuffers.push(buffers.map((b) => Buffer.from(b)))
        return 0
      },
      null,
      // eslint-disable-next-line no-console
      (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
    )

    expect(dec.processUntilEndOfMetadata()).toBe(true)
    expect(dec.processUntilEndOfStream()).toBe(true)
    expect(dec.flush()).toBe(true)
    expect(dec.finish()).toBe(true)

    const [finalBuffer, samples] = joinIntoInterleaved(allBuffers)
    expect(samples).toEqual(totalSamples)
    comparePCM(okData, finalBuffer, 32)
  })

  it('decode using file (ogg)', () => {
    const dec = new api.Decoder()
    const allBuffers = []
    dec.initOggFile(
      pathForFile('loop.oga'),
      (_, buffers) => {
        allBuffers.push(buffers.map((b) => Buffer.from(b)))
        return 0
      },
      null,
      // eslint-disable-next-line no-console
      (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
    )

    expect(dec.processUntilEndOfMetadata()).toBe(true)
    expect(dec.processUntilEndOfStream()).toBe(true)
    expect(dec.flush()).toBe(true)
    expect(dec.finish()).toBe(true)

    const [finalBuffer, samples] = joinIntoInterleaved(allBuffers)
    expect(samples).toEqual(totalSamples)
    comparePCM(okData, finalBuffer, 32)
  })

  it('decoder should be able to skip a frame', () => {
    const dec = new api.Decoder()
    dec.initFile(
      pathForFile('loop.flac'),
      () => 0,
      null,
      // eslint-disable-next-line no-console
      (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
    )

    expect(dec.processUntilEndOfMetadata()).toBe(true)
    expect(dec.processSingle()).toBe(true)
    expect(dec.skipSingleFrame()).toBe(true)
    expect(dec.processSingle()).toBe(true)
    expect(dec.flush()).toBe(true)
    expect(dec.finish()).toBe(true)
  })

  it('decoder should be able to seek to a sample', () => {
    const callbacks = generateFlacCallbacks.sync(api.Decoder, pathForFile('loop.flac'), 'r')
    deferredScope.defer(() => callbacks.close())
    const dec = new api.Decoder()
    dec.initStream(
      callbacks.read,
      callbacks.seek,
      callbacks.tell,
      callbacks.length,
      callbacks.eof,
      () => 0,
      null,
      // eslint-disable-next-line no-console
      (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
    )

    expect(dec.processUntilEndOfMetadata()).toBe(true)
    expect(dec.processSingle()).toBe(true)
    expect(dec.seekAbsolute(totalSamples / 5)).toBe(true)
    expect(dec.getDecodePosition()).toEqual(157036)
    expect(dec.processSingle()).toBe(true)
    expect(dec.flush()).toBe(true)
    expect(dec.finish()).toBe(true)
  })

  it('decoder should emit metadata', () => {
    const dec = new api.Decoder()
    const metadataBlocks = []
    dec.initFile(
      pathForFile('loop.flac'),
      () => 0,
      (metadata) => {
        metadataBlocks.push(metadata)
        return 0
      },
      // eslint-disable-next-line no-console
      (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
    )

    expect(dec.processSingle()).toBe(true)
    expect(dec.finish()).toBe(true)

    expect(metadataBlocks.length).toEqual(1)
  })

  it('decoder get other properties work', () => {
    const dec = new api.Decoder()
    dec.setMd5Checking(true)
    dec.setOggSerialNumber(0x11223344)
    dec.setMetadataIgnore(1)
    dec.initFile(
      pathForFile('loop.flac'),
      () => 0,
      () => 0,
      // eslint-disable-next-line no-console
      (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
    )

    expect(dec.processUntilEndOfMetadata()).toBe(true)
    expect(dec.processSingle()).toBe(true)

    expect(dec.getChannels()).toEqual(2)
    expect(dec.getBitsPerSample()).toEqual(24)
    expect(dec.getSampleRate()).toEqual(44100)
    expect(dec.getChannelAssignment()).toEqual(api.format.ChannelAssignment.MID_SIDE)
    expect(dec.getBlocksize()).toEqual(4096)
    expect(dec.getTotalSamples()).toEqual(totalSamples)
    expect(dec.getMd5Checking()).toBe(true)
    expect(dec.getDecodePosition()).toEqual(22862)

    expect(dec.finish()).toBe(false)
  })

  it('encode using stream (non-ogg)', () => {
    const enc = new api.Encoder()
    const callbacks = generateFlacCallbacks.sync(api.Encoder, tmpFile.path, 'w')
    deferredScope.defer(() => callbacks.close())
    enc.bitsPerSample = 24
    enc.channels = 2
    enc.setCompressionLevel(9)
    enc.sampleRate = 44100
    enc.initStream(
      callbacks.write,
      callbacks.seek,
      callbacks.tell,
      null,
    )

    expect(enc.processInterleaved(encData)).toBe(true)
    expect(enc.finish()).toBe(true)

    comparePCM(okData, tmpFile.path, 24)
  })

  it('encode using stream (ogg)', () => {
    const enc = new api.Encoder()
    const callbacks = generateFlacCallbacks.sync(api.Encoder, tmpFile.path, 'w+')
    deferredScope.defer(() => callbacks.close())
    enc.bitsPerSample = 24
    enc.channels = 2
    enc.setCompressionLevel(9)
    enc.sampleRate = 44100
    enc.initOggStream(
      callbacks.read,
      callbacks.write,
      callbacks.seek,
      callbacks.tell,
      null,
    )

    expect(enc.processInterleaved(encData)).toBe(true)
    expect(enc.finish()).toBe(true)

    comparePCM(okData, tmpFile.path, 24, true)
  })

  it('encode using file (non-ogg)', () => {
    const progressCallbackValues = []
    const enc = new api.Encoder()
    enc.bitsPerSample = 24
    enc.channels = 2
    enc.setCompressionLevel(9)
    enc.sampleRate = 44100
    enc.initFile(
      tmpFile.path,
      (...args) => progressCallbackValues.push(args),
    )

    expect(enc.processInterleaved(encData)).toBe(true)
    expect(enc.finish()).toBe(true)

    comparePCM(okData, tmpFile.path, 24)
    expect(progressCallbackValues.length).toEqual(41)
  })

  it('encode using file (ogg)', () => {
    const progressCallbackValues = []
    const enc = new api.Encoder()
    enc.bitsPerSample = 24
    enc.channels = 2
    enc.setCompressionLevel(9)
    enc.sampleRate = 44100
    enc.initOggFile(
      tmpFile.path,
      (...args) => progressCallbackValues.push(args),
    )

    expect(enc.processInterleaved(encData)).toBe(true)
    expect(enc.finish()).toBe(true)

    comparePCM(okData, tmpFile.path, 24, true)
    expect(progressCallbackValues.length).toEqual(30)
  })

  it('encode using file with non-interleaved data (non-ogg)', () => {
    const progressCallbackValues = []
    const enc = new api.Encoder()
    enc.bitsPerSample = 24
    enc.channels = 2
    enc.setCompressionLevel(9)
    enc.sampleRate = 44100
    enc.initFile(
      tmpFile.path,
      (...args) => progressCallbackValues.push(args),
    )

    expect(enc.process(encDataAlt, totalSamples)).toBe(true)
    expect(enc.finish()).toBe(true)

    comparePCM(okData, tmpFile.path, 24)
    expect(progressCallbackValues.length).toEqual(41)
  })

  it('encoder should emit streaminfo metadata block', () => {
    let metadataBlock = null
    const callbacks = generateFlacCallbacks.sync(api.Encoder, tmpFile.path, 'w')
    deferredScope.defer(() => callbacks.close())
    const enc = new api.Encoder()
    enc.bitsPerSample = 24
    enc.channels = 2
    enc.setCompressionLevel(9)
    enc.sampleRate = 44100
    enc.setMetadata([new api.metadata.VorbisCommentMetadata()])
    enc.initStream(
      callbacks.write,
      callbacks.seek,
      callbacks.tell,
      (metadata) => {
        metadataBlock = metadata
      },
    )

    expect(enc.processInterleaved(encData)).toBe(true)
    expect(enc.finish()).toBe(true)

    expect(metadataBlock).not.toBeNull()
    expect(metadataBlock.type).toEqual(0)
    expect(metadataBlock.totalSamples).toEqual(totalSamples)
  })

  it('encoder process should fail if invalid buffer size is sent', () => {
    const enc = new api.Encoder()
    enc.bitsPerSample = 24
    enc.channels = 2
    enc.setCompressionLevel(9)
    enc.sampleRate = 44100
    enc.initFile(
      tmpFile.path,
    )

    expect(() => enc.process([encDataAlt[0].slice(4), encDataAlt[1]], totalSamples)).toThrow()
    expect(enc.finish()).toBe(true)
  })

  it('encoder process should fail if invalid number of channels is sent', () => {
    const enc = new api.Encoder()
    enc.bitsPerSample = 24
    enc.channels = 2
    enc.setCompressionLevel(9)
    enc.sampleRate = 44100
    enc.initFile(
      tmpFile.path,
    )

    expect(() => enc.process([encDataAlt[0]], totalSamples)).toThrow()
    expect(enc.finish()).toBe(true)
  })

  it('encoder processInterleaved should fail if invalid buffer size is sent', () => {
    const enc = new api.Encoder()
    enc.bitsPerSample = 24
    enc.channels = 2
    enc.setCompressionLevel(9)
    enc.sampleRate = 44100
    enc.initFile(
      tmpFile.path,
    )

    expect(() => enc.processInterleaved(encData.slice(4), totalSamples)).toThrow()
    expect(enc.finish()).toBe(true)
  })

  it('encoder get other properties work', () => {
    const enc = new api.Encoder()
    enc.bitsPerSample = 24
    enc.channels = 2
    enc.setCompressionLevel(9)
    enc.sampleRate = 44100
    enc.totalSamplesEstimate = totalSamples
    enc.verify = true
    enc.initFile(
      tmpFile.path,
    )

    expect(enc.verify).toBe(true)

    expect(enc.channels).toEqual(2)
    expect(enc.bitsPerSample).toEqual(24)
    expect(enc.sampleRate).toEqual(44100)
    expect(enc.blocksize).toEqual(4096)
    expect(enc.doMidSideStereo).toBe(true)
    expect(enc.looseMidSideStereo).toBe(false)
    expect(enc.maxLpcOrder).toEqual(12)
    expect(enc.qlpCoeffPrecision).toEqual(15)
    expect(enc.doQlpCoeffPrecSearch).toBe(false)
    expect(enc.doEscapeCoding).toBe(false)
    expect(enc.doExhaustiveModelSearch).toBe(false)
    expect(enc.minResidualPartitionOrder).toEqual(0)
    expect(enc.maxResidualPartitionOrder).toEqual(6)
    expect(enc.riceParameterSearchDist).toEqual(0)
    expect(enc.totalSamplesEstimate).toEqual(totalSamples)

    expect(enc.finish()).toBe(true)
  })
})
