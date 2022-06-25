import tempUntracked from 'temp'
import fs from 'fs'
import { api } from '../../lib/index.js'
import {
  pathForFile as fullPathForFile,
  createDeferredScope,
  comparePCM,
  generateFlacCallbacks,
  joinIntoInterleaved,
  loopPcmAudio,
} from '../helper/index.js'

const { audio: pathForFile } = fullPathForFile
const {
  totalSamples,
  encData,
  encDataAlt,
  okData,
} = loopPcmAudio
const temp = tempUntracked.track()

let tmpFile
let deferredScope = null
beforeEach(() => {
  tmpFile = temp.openSync('flac-bindings.encode-decode.async-api')
  deferredScope = createDeferredScope()
  fs.closeSync(tmpFile.fd)
})

afterEach(() => {
  temp.cleanupSync()
  return deferredScope.finalize()
})

describe('encode & decode: async api', () => {
  it('decode using stream (non-ogg)', async () => {
    const callbacks = generateFlacCallbacks.sync(api.Decoder, pathForFile('loop.flac'), 'r')
    deferredScope.defer(() => callbacks.close())
    const allBuffers = []
    const dec = await new api.DecoderBuilder().buildWithStreamAsync(
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
    expect(await dec.finishAsync()).not.toBeNull()

    const [finalBuffer, samples] = joinIntoInterleaved(allBuffers)
    expect(samples).toEqual(totalSamples)
    comparePCM(okData, finalBuffer, 32)
  })

  it('decode using stream (ogg)', async () => {
    const callbacks = generateFlacCallbacks.sync(api.Decoder, pathForFile('loop.oga'), 'r')
    deferredScope.defer(() => callbacks.close())
    const allBuffers = []
    const dec = await new api.DecoderBuilder().buildWithOggStreamAsync(
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
    expect(await dec.finishAsync()).not.toBeNull()

    const [finalBuffer, samples] = joinIntoInterleaved(allBuffers)
    expect(samples).toEqual(totalSamples)
    comparePCM(okData, finalBuffer, 32)
  })

  it('decode with async callbacks using stream (non-ogg)', async () => {
    const callbacks = await generateFlacCallbacks.async(api.Decoder, pathForFile('loop.flac'), 'r')
    deferredScope.defer(() => callbacks.close())
    const allBuffers = []
    const dec = await new api.DecoderBuilder().buildWithStreamAsync(
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
    expect(await dec.finishAsync()).not.toBeNull()

    const [finalBuffer, samples] = joinIntoInterleaved(allBuffers)
    expect(samples).toEqual(totalSamples)
    comparePCM(okData, finalBuffer, 32)
  })

  it('decode with async callbacks using stream (ogg)', async () => {
    const callbacks = await generateFlacCallbacks.async(api.Decoder, pathForFile('loop.oga'), 'r')
    deferredScope.defer(() => callbacks.close())
    const allBuffers = []
    const dec = await new api.DecoderBuilder().buildWithOggStreamAsync(
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
    expect(await dec.finishAsync()).not.toBeNull()

    const [finalBuffer, samples] = joinIntoInterleaved(allBuffers)
    expect(samples).toEqual(totalSamples)
    comparePCM(okData, finalBuffer, 32)
  })

  it('decode using file (non-ogg)', async () => {
    const allBuffers = []
    const dec = await new api.DecoderBuilder().buildWithFileAsync(
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
    expect(await dec.finishAsync()).not.toBeNull()

    const [finalBuffer, samples] = joinIntoInterleaved(allBuffers)
    expect(samples).toEqual(totalSamples)
    comparePCM(okData, finalBuffer, 32)
  })

  it('decode using file (ogg)', async () => {
    const allBuffers = []
    const dec = await new api.DecoderBuilder().buildWithOggFileAsync(
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
    expect(await dec.finishAsync()).not.toBeNull()

    const [finalBuffer, samples] = joinIntoInterleaved(allBuffers)
    expect(samples).toEqual(totalSamples)
    comparePCM(okData, finalBuffer, 32)
  })

  it('decoder should be able to skip a frame', async () => {
    const dec = await new api.DecoderBuilder().buildWithFileAsync(
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
    expect(await dec.finishAsync()).not.toBeNull()
  })

  it('decoder should be able to seek to a sample', async () => {
    const callbacks = await generateFlacCallbacks.async(api.Decoder, pathForFile('loop.flac'), 'r')
    deferredScope.defer(() => callbacks.close())
    const dec = await new api.DecoderBuilder().buildWithStreamAsync(
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
    expect(await dec.getDecodePositionAsync()).toBe(157036)
    expect(await dec.processSingleAsync()).toBe(true)
    expect(await dec.flushAsync()).toBe(true)
    expect(await dec.finishAsync()).not.toBeNull()
  })

  it('decoder should emit metadata', async () => {
    const metadataBlocks = []
    const dec = await new api.DecoderBuilder().buildWithFileAsync(
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
    expect(await dec.finishAsync()).not.toBeNull()

    expect(metadataBlocks).toHaveLength(1)
  })

  it('encode using stream (non-ogg)', async () => {
    const callbacks = generateFlacCallbacks.sync(api.Encoder, tmpFile.path, 'w')
    deferredScope.defer(() => callbacks.close())

    const enc = await new api.EncoderBuilder()
      .setBitsPerSample(24)
      .setChannels(2)
      .setCompressionLevel(9)
      .setSampleRate(44100)
      .buildWithStreamAsync(
        callbacks.write,
        callbacks.seek,
        callbacks.tell,
        null,
      )

    expect(await enc.processInterleavedAsync(encData)).toBe(true)
    expect(await enc.finishAsync()).not.toBeNull()

    comparePCM(okData, tmpFile.path, 24)
  })

  it('encode using stream (ogg)', async () => {
    const callbacks = generateFlacCallbacks.sync(api.Encoder, tmpFile.path, 'w+')
    deferredScope.defer(() => callbacks.close())

    const enc = await new api.EncoderBuilder()
      .setBitsPerSample(24)
      .setChannels(2)
      .setCompressionLevel(9)
      .setSampleRate(44100)
      .buildWithOggStreamAsync(
        callbacks.read,
        callbacks.write,
        callbacks.seek,
        callbacks.tell,
        null,
      )

    expect(await enc.processInterleavedAsync(encData)).toBe(true)
    expect(await enc.finishAsync()).not.toBeNull()

    comparePCM(okData, tmpFile.path, 24, true)
  })

  it('encode with async callbacks using stream (non-ogg)', async () => {
    const callbacks = await generateFlacCallbacks.async(api.Encoder, tmpFile.path, 'w')
    deferredScope.defer(() => callbacks.close())

    const enc = await new api.EncoderBuilder()
      .setBitsPerSample(24)
      .setChannels(2)
      .setCompressionLevel(9)
      .setSampleRate(44100)
      .buildWithStreamAsync(
        callbacks.write,
        callbacks.seek,
        callbacks.tell,
        null,
      )

    expect(await enc.processInterleavedAsync(encData)).toBe(true)
    expect(await enc.finishAsync()).not.toBeNull()

    comparePCM(okData, tmpFile.path, 24)
  })

  it('encode with async callbacks using stream (ogg)', async () => {
    const callbacks = await generateFlacCallbacks.async(api.Encoder, tmpFile.path, 'w+')
    deferredScope.defer(() => callbacks.close())

    const enc = await new api.EncoderBuilder()
      .setBitsPerSample(24)
      .setChannels(2)
      .setCompressionLevel(9)
      .setSampleRate(44100)
      .buildWithOggStreamAsync(
        callbacks.read,
        callbacks.write,
        callbacks.seek,
        callbacks.tell,
        null,
      )

    expect(await enc.processInterleavedAsync(encData)).toBe(true)
    expect(await enc.finishAsync()).not.toBeNull()

    comparePCM(okData, tmpFile.path, 24, true)
  })

  it('encode using file (non-ogg)', async () => {
    const progressCallbackValues = []
    const enc = await new api.EncoderBuilder()
      .setBitsPerSample(24)
      .setChannels(2)
      .setCompressionLevel(9)
      .setSampleRate(44100)
      .buildWithFileAsync(
        tmpFile.path,
        (...args) => progressCallbackValues.push(args),
      )

    expect(await enc.processInterleavedAsync(encData)).toBe(true)
    expect(await enc.finishAsync()).not.toBeNull()

    comparePCM(okData, tmpFile.path, 24)
    expect(progressCallbackValues).toHaveLength(41)
  })

  it('encode using file (ogg)', async () => {
    const progressCallbackValues = []
    const enc = await new api.EncoderBuilder()
      .setBitsPerSample(24)
      .setChannels(2)
      .setCompressionLevel(9)
      .setSampleRate(44100)
      .buildWithOggFileAsync(
        tmpFile.path,
        (...args) => progressCallbackValues.push(args),
      )

    expect(await enc.processInterleavedAsync(encData)).toBe(true)
    expect(await enc.finishAsync()).not.toBeNull()

    comparePCM(okData, tmpFile.path, 24, true)
    expect(progressCallbackValues).toHaveLength(30)
  })

  it('encode using file with non-interleaved data (non-ogg)', async () => {
    const progressCallbackValues = []
    const enc = await new api.EncoderBuilder()
      .setBitsPerSample(24)
      .setChannels(2)
      .setCompressionLevel(9)
      .setSampleRate(44100)
      .buildWithFileAsync(
        tmpFile.path,
        (...args) => progressCallbackValues.push(args),
      )

    expect(await enc.processAsync(encDataAlt, totalSamples)).toBe(true)
    expect(await enc.finishAsync()).not.toBeNull()

    comparePCM(okData, tmpFile.path, 24)
    expect(progressCallbackValues).toHaveLength(41)
  })

  it('encoder should emit streaminfo metadata block', async () => {
    let metadataBlock = null
    const callbacks = generateFlacCallbacks.sync(api.Encoder, tmpFile.path, 'w')
    deferredScope.defer(() => callbacks.close())
    const enc = await new api.EncoderBuilder()
      .setBitsPerSample(24)
      .setChannels(2)
      .setCompressionLevel(9)
      .setSampleRate(44100)
      .buildWithStreamAsync(
        callbacks.write,
        callbacks.seek,
        callbacks.tell,
        (metadata) => {
          metadataBlock = metadata
        },
      )

    expect(await enc.processInterleavedAsync(encData)).toBe(true)
    expect(await enc.finishAsync()).not.toBeNull()

    expect(metadataBlock).not.toBeNull()
    expect(metadataBlock.type).toBe(0)
    expect(metadataBlock.totalSamples).toEqual(totalSamples)
  })

  it('encoder should throw if another async method is running', async () => {
    const enc = await new api.EncoderBuilder()
      .setBitsPerSample(24)
      .setChannels(2)
      .setCompressionLevel(9)
      .setSampleRate(44100)
      .buildWithFileAsync(
        tmpFile.path,
        null,
      )

    const promise = enc.processInterleavedAsync(encData)

    expect(() => enc.processInterleavedAsync(encData)).toThrow(/There is still an operation running/)

    await promise
    await enc.finishAsync()
  })

  it('encoder should throw if built with async but called sync method', async () => {
    const enc = await new api.EncoderBuilder()
      .setBitsPerSample(24)
      .setChannels(2)
      .setCompressionLevel(9)
      .setSampleRate(44100)
      .buildWithFileAsync(
        tmpFile.path,
        null,
      )

    expect(() => enc.processInterleaved(encData)).toThrow(/This method cannot be called when Encoder has been created using asynchronous method/)

    await enc.finishAsync()
  })

  it('decoder should throw if another async method is running', async () => {
    const allBuffers = []
    const dec = await new api.DecoderBuilder().buildWithFileAsync(
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

  it('decoder should throw if built with async but called sync method', async () => {
    const allBuffers = []
    const dec = await new api.DecoderBuilder().buildWithFileAsync(
      pathForFile('loop.flac'),
      (_, buffers) => {
        allBuffers.push(buffers.map((b) => Buffer.from(b)))
        return 0
      },
      null,
      // eslint-disable-next-line no-console
      (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
    )

    expect(() => dec.processUntilEndOfMetadata()).toThrow(/This method cannot be called when Decoder has been created using asynchronous method/)

    await dec.finishAsync()
  })

  it('encoder builder can be reused', async () => {
    const enc = await new api.EncoderBuilder()
      .setBitsPerSample(24)
      .setChannels(2)
      .setCompressionLevel(9)
      .setSampleRate(44100)
      .buildWithFileAsync(tmpFile.path, null)

    const builder = await enc.finishAsync()
    expect(builder).not.toBeNull()

    const enc2 = await builder
      .setBitsPerSample(16)
      .setChannels(1)
      .setCompressionLevel(7)
      .setSampleRate(88200)
      .buildWithFileAsync(tmpFile.path, null)

    expect(await enc2.finishAsync()).not.toBeNull()
  })

  it('decoder builder can be reused', async () => {
    const dec = await new api.DecoderBuilder()
      .buildWithFileAsync(
        pathForFile('loop.flac'),
        () => api.Decoder.WriteStatus.CONTINUE,
        null,
        () => {},
      )

    const builder = await dec.finishAsync()
    expect(builder).not.toBeNull()

    const dec2 = await builder
      .buildWithOggFileAsync(
        pathForFile('loop.oga'),
        () => api.Decoder.WriteStatus.CONTINUE,
        null,
        () => {},
      )

    expect(await dec2.finishAsync()).not.toBeNull()
  })
})
