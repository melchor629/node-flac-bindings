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
  tmpFile = temp.openSync('flac-bindings.encode-decode.async-api')
  deferredScope = createDeferredScope()
})

afterEach(() => {
  fs.closeSync(tmpFile.fd)
  temp.cleanupSync()
  return deferredScope.finalize()
})

describe('encode & decode: async api', () => {
  it('decode using stream (non-ogg)', async () => {
    const callbacks = generateFlacCallbacks.sync(api.Decoder, pathForFile('loop.flac'), 'r')
    deferredScope.defer(() => callbacks.close())
    const dec = new api.Decoder()
    const allBuffers = []
    await dec.initStreamAsync(
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

    expect(await dec.processUntilEndOfMetadataAsync()).toBe(true)
    expect(await dec.processUntilEndOfStreamAsync()).toBe(true)
    expect(await dec.flushAsync()).toBe(true)
    expect(await dec.finishAsync()).toBe(true)

    const [finalBuffer, samples] = joinIntoInterleaved(allBuffers)
    expect(samples).toEqual(totalSamples)
    comparePCM(okData, finalBuffer, 32)
  })

  it('decode using stream (ogg)', async () => {
    const callbacks = generateFlacCallbacks.sync(api.Decoder, pathForFile('loop.oga'), 'r')
    deferredScope.defer(() => callbacks.close())
    const dec = new api.Decoder()
    const allBuffers = []
    await dec.initOggStreamAsync(
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

    expect(await dec.processUntilEndOfMetadataAsync()).toBe(true)
    expect(await dec.processUntilEndOfStreamAsync()).toBe(true)
    expect(await dec.flushAsync()).toBe(true)
    expect(await dec.finishAsync()).toBe(true)

    const [finalBuffer, samples] = joinIntoInterleaved(allBuffers)
    expect(samples).toEqual(totalSamples)
    comparePCM(okData, finalBuffer, 32)
  })

  it('decode with async callbacks using stream (non-ogg)', async () => {
    const callbacks = await generateFlacCallbacks.async(api.Decoder, pathForFile('loop.flac'), 'r')
    deferredScope.defer(() => callbacks.close())
    const dec = new api.Decoder()
    const allBuffers = []
    await dec.initStreamAsync(
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

    expect(await dec.processUntilEndOfMetadataAsync()).toBe(true)
    expect(await dec.processUntilEndOfStreamAsync()).toBe(true)
    expect(await dec.flushAsync()).toBe(true)
    expect(await dec.finishAsync()).toBe(true)

    const [finalBuffer, samples] = joinIntoInterleaved(allBuffers)
    expect(samples).toEqual(totalSamples)
    comparePCM(okData, finalBuffer, 32)
  })

  it('decode with async callbacks using stream (ogg)', async () => {
    const callbacks = await generateFlacCallbacks.async(api.Decoder, pathForFile('loop.oga'), 'r')
    deferredScope.defer(() => callbacks.close())
    const dec = new api.Decoder()
    const allBuffers = []
    await dec.initOggStreamAsync(
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

    expect(await dec.processUntilEndOfMetadataAsync()).toBe(true)
    expect(await dec.processUntilEndOfStreamAsync()).toBe(true)
    expect(await dec.flushAsync()).toBe(true)
    expect(await dec.finishAsync()).toBe(true)

    const [finalBuffer, samples] = joinIntoInterleaved(allBuffers)
    expect(samples).toEqual(totalSamples)
    comparePCM(okData, finalBuffer, 32)
  })

  it('decode using file (non-ogg)', async () => {
    const dec = new api.Decoder()
    const allBuffers = []
    await dec.initFileAsync(
      pathForFile('loop.flac'),
      (_, buffers) => {
        allBuffers.push(buffers.map((b) => Buffer.from(b)))
        return 0
      },
      null,
      // eslint-disable-next-line no-console
      (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
    )

    expect(await dec.processUntilEndOfMetadataAsync()).toBe(true)
    expect(await dec.processUntilEndOfStreamAsync()).toBe(true)
    expect(await dec.flushAsync()).toBe(true)
    expect(await dec.finishAsync()).toBe(true)

    const [finalBuffer, samples] = joinIntoInterleaved(allBuffers)
    expect(samples).toEqual(totalSamples)
    comparePCM(okData, finalBuffer, 32)
  })

  it('decode using file (ogg)', async () => {
    const dec = new api.Decoder()
    const allBuffers = []
    await dec.initOggFileAsync(
      pathForFile('loop.oga'),
      (_, buffers) => {
        allBuffers.push(buffers.map((b) => Buffer.from(b)))
        return 0
      },
      null,
      // eslint-disable-next-line no-console
      (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
    )

    expect(await dec.processUntilEndOfMetadataAsync()).toBe(true)
    expect(await dec.processUntilEndOfStreamAsync()).toBe(true)
    expect(await dec.flushAsync()).toBe(true)
    expect(await dec.finishAsync()).toBe(true)

    const [finalBuffer, samples] = joinIntoInterleaved(allBuffers)
    expect(samples).toEqual(totalSamples)
    comparePCM(okData, finalBuffer, 32)
  })

  it('decoder should be able to skip a frame', async () => {
    const dec = new api.Decoder()
    await dec.initFileAsync(
      pathForFile('loop.flac'),
      () => 0,
      null,
      // eslint-disable-next-line no-console
      (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
    )

    expect(await dec.processUntilEndOfMetadataAsync()).toBe(true)
    expect(await dec.processSingleAsync()).toBe(true)
    expect(await dec.skipSingleFrameAsync()).toBe(true)
    expect(await dec.processSingleAsync()).toBe(true)
    expect(await dec.flushAsync()).toBe(true)
    expect(await dec.finishAsync()).toBe(true)
  })

  it('decoder should be able to seek to a sample', async () => {
    const callbacks = await generateFlacCallbacks.async(api.Decoder, pathForFile('loop.flac'), 'r')
    deferredScope.defer(() => callbacks.close())
    const dec = new api.Decoder()
    await dec.initStreamAsync(
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

    expect(await dec.processUntilEndOfMetadataAsync()).toBe(true)
    expect(await dec.processSingleAsync()).toBe(true)
    expect(await dec.seekAbsoluteAsync(totalSamples / 5)).toBe(true)
    expect(dec.getDecodePosition()).toEqual(157036)
    expect(await dec.processSingleAsync()).toBe(true)
    expect(await dec.flushAsync()).toBe(true)
    expect(await dec.finishAsync()).toBe(true)
  })

  it('decoder should emit metadata', async () => {
    const dec = new api.Decoder()
    const metadataBlocks = []
    await dec.initFileAsync(
      pathForFile('loop.flac'),
      () => 0,
      (metadata) => {
        metadataBlocks.push(metadata)
        return 0
      },
      // eslint-disable-next-line no-console
      (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
    )

    expect(await dec.processSingleAsync()).toBe(true)
    expect(await dec.finishAsync()).toBe(true)

    expect(metadataBlocks).toHaveLength(1)
  })

  it('encode using stream (non-ogg)', async () => {
    const enc = new api.Encoder()
    const callbacks = generateFlacCallbacks.sync(api.Encoder, tmpFile.path, 'w')
    deferredScope.defer(() => callbacks.close())
    enc.bitsPerSample = 24
    enc.channels = 2
    enc.setCompressionLevel(9)
    enc.sampleRate = 44100
    await enc.initStreamAsync(
      callbacks.write,
      callbacks.seek,
      callbacks.tell,
      null,
    )

    expect(await enc.processInterleavedAsync(encData)).toBe(true)
    expect(await enc.finishAsync()).toBe(true)

    comparePCM(okData, tmpFile.path, 24)
  })

  it('encode using stream (ogg)', async () => {
    const enc = new api.Encoder()
    const callbacks = generateFlacCallbacks.sync(api.Encoder, tmpFile.path, 'w+')
    deferredScope.defer(() => callbacks.close())
    enc.bitsPerSample = 24
    enc.channels = 2
    enc.setCompressionLevel(9)
    enc.sampleRate = 44100
    await enc.initOggStreamAsync(
      callbacks.read,
      callbacks.write,
      callbacks.seek,
      callbacks.tell,
      null,
    )

    expect(await enc.processInterleavedAsync(encData)).toBe(true)
    expect(await enc.finishAsync()).toBe(true)

    comparePCM(okData, tmpFile.path, 24, true)
  })

  it('encode with async callbacks using stream (non-ogg)', async () => {
    const enc = new api.Encoder()
    const callbacks = await generateFlacCallbacks.async(api.Encoder, tmpFile.path, 'w')
    deferredScope.defer(() => callbacks.close())
    enc.bitsPerSample = 24
    enc.channels = 2
    enc.setCompressionLevel(9)
    enc.sampleRate = 44100
    await enc.initStreamAsync(
      callbacks.write,
      callbacks.seek,
      callbacks.tell,
      null,
    )

    expect(await enc.processInterleavedAsync(encData)).toBe(true)
    expect(await enc.finishAsync()).toBe(true)

    comparePCM(okData, tmpFile.path, 24)
  })

  it('encode with async callbacks using stream (ogg)', async () => {
    const enc = new api.Encoder()
    const callbacks = await generateFlacCallbacks.async(api.Encoder, tmpFile.path, 'w+')
    deferredScope.defer(() => callbacks.close())
    enc.bitsPerSample = 24
    enc.channels = 2
    enc.setCompressionLevel(9)
    enc.sampleRate = 44100
    await enc.initOggStreamAsync(
      callbacks.read,
      callbacks.write,
      callbacks.seek,
      callbacks.tell,
      null,
    )

    expect(await enc.processInterleavedAsync(encData)).toBe(true)
    expect(await enc.finishAsync()).toBe(true)

    comparePCM(okData, tmpFile.path, 24, true)
  })

  it('encode using file (non-ogg)', async () => {
    const progressCallbackValues = []
    const enc = new api.Encoder()
    enc.bitsPerSample = 24
    enc.channels = 2
    enc.setCompressionLevel(9)
    enc.sampleRate = 44100
    await enc.initFileAsync(
      tmpFile.path,
      (...args) => progressCallbackValues.push(args),
    )

    expect(await enc.processInterleavedAsync(encData)).toBe(true)
    expect(await enc.finishAsync()).toBe(true)

    comparePCM(okData, tmpFile.path, 24)
    expect(progressCallbackValues).toHaveLength(41)
  })

  it('encode using file (ogg)', async () => {
    const progressCallbackValues = []
    const enc = new api.Encoder()
    enc.bitsPerSample = 24
    enc.channels = 2
    enc.setCompressionLevel(9)
    enc.sampleRate = 44100
    await enc.initOggFileAsync(
      tmpFile.path,
      (...args) => progressCallbackValues.push(args),
    )

    expect(await enc.processInterleavedAsync(encData)).toBe(true)
    expect(await enc.finishAsync()).toBe(true)

    comparePCM(okData, tmpFile.path, 24, true)
    expect(progressCallbackValues).toHaveLength(30)
  })

  it('encode using file with non-interleaved data (non-ogg)', async () => {
    const progressCallbackValues = []
    const enc = new api.Encoder()
    enc.bitsPerSample = 24
    enc.channels = 2
    enc.setCompressionLevel(9)
    enc.sampleRate = 44100
    await enc.initFileAsync(
      tmpFile.path,
      (...args) => progressCallbackValues.push(args),
    )

    expect(await enc.processAsync(encDataAlt, totalSamples)).toBe(true)
    expect(await enc.finishAsync()).toBe(true)

    comparePCM(okData, tmpFile.path, 24)
    expect(progressCallbackValues).toHaveLength(41)
  })

  it('encoder should emit streaminfo metadata block', async () => {
    let metadataBlock = null
    const callbacks = generateFlacCallbacks.sync(api.Encoder, tmpFile.path, 'w')
    deferredScope.defer(() => callbacks.close())
    const enc = new api.Encoder()
    enc.bitsPerSample = 24
    enc.channels = 2
    enc.setCompressionLevel(9)
    enc.sampleRate = 44100
    enc.setMetadata([new api.metadata.VorbisCommentMetadata()])
    await enc.initStreamAsync(
      callbacks.write,
      null,
      null,
      (metadata) => {
        metadataBlock = metadata
      },
    )

    expect(await enc.processInterleavedAsync(encData)).toBe(true)
    expect(await enc.finishAsync()).toBe(true)

    expect(metadataBlock).not.toBeNull()
    expect(metadataBlock.type).toEqual(0)
    expect(metadataBlock.totalSamples).toEqual(totalSamples)
  })

  it('encoder should throw if another async method is running', async () => {
    const enc = new api.Encoder()
    enc.bitsPerSample = 24
    enc.channels = 2
    enc.setCompressionLevel(9)
    enc.sampleRate = 44100
    await enc.initFileAsync(
      tmpFile.path,
      null,
    )

    const promise = enc.processInterleavedAsync(encData)

    expect(() => enc.processInterleavedAsync(encData)).toThrow(/There is still an operation running/)

    await promise
    await enc.finishAsync()
  })

  it('decoder should throw if another async method is running', async () => {
    const dec = new api.Decoder()
    const allBuffers = []
    await dec.initFileAsync(
      pathForFile('loop.flac'),
      (_, buffers) => {
        allBuffers.push(buffers.map((b) => Buffer.from(b)))
        return 0
      },
      null,
      // eslint-disable-next-line no-console
      (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
    )

    const e = await dec.processUntilEndOfMetadataAsync()
    expect(e).toBe(true)

    const promise = dec.processSingleAsync()
    expect(() => dec.processSingleAsync()).toThrow(/There is still an operation running/)

    await promise
    const f = await dec.processUntilEndOfStreamAsync()
    expect(f).toBe(true)
    await dec.finishAsync()
  })
})
