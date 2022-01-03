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
    const allBuffers = []
    const dec = new api.DecoderBuilder().buildWithStream(
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
    const allBuffers = []
    const dec = new api.DecoderBuilder().buildWithOggStream(
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
    const allBuffers = []
    const dec = new api.DecoderBuilder().buildWithFile(
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
    const allBuffers = []
    const dec = new api.DecoderBuilder().buildWithOggFile(
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
    const dec = new api.DecoderBuilder().buildWithFile(
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
    const dec = new api.DecoderBuilder().buildWithStream(
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
    expect(dec.getDecodePosition()).toBe(157036)
    expect(dec.processSingle()).toBe(true)
    expect(dec.flush()).toBe(true)
    expect(dec.finish()).toBe(true)
  })

  it('decoder should emit metadata', () => {
    const metadataBlocks = []
    const dec = new api.DecoderBuilder().buildWithFile(
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

    expect(metadataBlocks).toHaveLength(1)
  })

  it('decoder get other properties work', () => {
    const dec = new api.DecoderBuilder()
      .setMd5Checking(true)
      .setOggSerialNumber(0x11223344)
      .setMetadataIgnore(1)
      .buildWithFile(
        pathForFile('loop.flac'),
        () => 0,
        () => 0,
        // eslint-disable-next-line no-console
        (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
      )

    expect(dec.processUntilEndOfMetadata()).toBe(true)
    expect(dec.processSingle()).toBe(true)

    expect(dec.getChannels()).toBe(2)
    expect(dec.getBitsPerSample()).toBe(24)
    expect(dec.getSampleRate()).toBe(44100)
    expect(dec.getChannelAssignment()).toEqual(api.format.ChannelAssignment.MID_SIDE)
    expect(dec.getBlocksize()).toBe(4096)
    expect(dec.getTotalSamples()).toEqual(totalSamples)
    expect(dec.getMd5Checking()).toBe(true)
    expect(dec.getDecodePosition()).toBe(22862)

    expect(dec.finish()).toBe(false)
  })

  it('encode using stream (non-ogg)', () => {
    const callbacks = generateFlacCallbacks.sync(api.Encoder, tmpFile.path, 'w')
    deferredScope.defer(() => callbacks.close())

    const enc = new api.EncoderBuilder()
      .setBitsPerSample(24)
      .setChannels(2)
      .setCompressionLevel(9)
      .setSampleRate(44100)
      .buildWithStream(
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
    const callbacks = generateFlacCallbacks.sync(api.Encoder, tmpFile.path, 'w+')
    deferredScope.defer(() => callbacks.close())

    const enc = new api.EncoderBuilder()
      .setBitsPerSample(24)
      .setChannels(2)
      .setCompressionLevel(9)
      .setSampleRate(44100)
      .buildWithOggStream(
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
    const enc = new api.EncoderBuilder()
      .setBitsPerSample(24)
      .setChannels(2)
      .setCompressionLevel(9)
      .setSampleRate(44100)
      .buildWithFile(
        tmpFile.path,
        (...args) => progressCallbackValues.push(args),
      )

    expect(enc.processInterleaved(encData)).toBe(true)
    expect(enc.finish()).toBe(true)

    comparePCM(okData, tmpFile.path, 24)
    expect(progressCallbackValues).toHaveLength(41)
  })

  it('encode using file (ogg)', () => {
    const progressCallbackValues = []
    const enc = new api.EncoderBuilder()
      .setBitsPerSample(24)
      .setChannels(2)
      .setCompressionLevel(9)
      .setSampleRate(44100)
      .buildWithOggFile(
        tmpFile.path,
        (...args) => progressCallbackValues.push(args),
      )

    expect(enc.processInterleaved(encData)).toBe(true)
    expect(enc.finish()).toBe(true)

    comparePCM(okData, tmpFile.path, 24, true)
    expect(progressCallbackValues).toHaveLength(30)
  })

  it('encode using file with non-interleaved data (non-ogg)', () => {
    const progressCallbackValues = []
    const enc = new api.EncoderBuilder()
      .setBitsPerSample(24)
      .setChannels(2)
      .setCompressionLevel(9)
      .setSampleRate(44100)
      .buildWithFile(
        tmpFile.path,
        (...args) => progressCallbackValues.push(args),
      )

    expect(enc.process(encDataAlt, totalSamples)).toBe(true)
    expect(enc.finish()).toBe(true)

    comparePCM(okData, tmpFile.path, 24)
    expect(progressCallbackValues).toHaveLength(41)
  })

  it('encoder should emit streaminfo metadata block', () => {
    let metadataBlock = null
    const callbacks = generateFlacCallbacks.sync(api.Encoder, tmpFile.path, 'w')
    deferredScope.defer(() => callbacks.close())
    const enc = new api.EncoderBuilder()
      .setBitsPerSample(24)
      .setChannels(2)
      .setCompressionLevel(9)
      .setSampleRate(44100)
      .setMetadata([new api.metadata.VorbisCommentMetadata()])
      .buildWithStream(
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
    expect(metadataBlock.type).toBe(0)
    expect(metadataBlock.totalSamples).toEqual(totalSamples)
  })

  it('encoder process should fail if invalid buffer size is sent', () => {
    const enc = new api.EncoderBuilder()
      .setBitsPerSample(24)
      .setChannels(2)
      .setCompressionLevel(9)
      .setSampleRate(44100)
      .buildWithFile(
        tmpFile.path,
      )

    expect(() => enc.process([encDataAlt[0].slice(4), encDataAlt[1]], totalSamples)).toThrow()
    expect(enc.finish()).toBe(true)
  })

  it('encoder process should fail if invalid number of channels is sent', () => {
    const enc = new api.EncoderBuilder()
      .setBitsPerSample(24)
      .setChannels(2)
      .setCompressionLevel(9)
      .setSampleRate(44100)
      .buildWithFile(
        tmpFile.path,
      )

    expect(() => enc.process([encDataAlt[0]], totalSamples)).toThrow()
    expect(enc.finish()).toBe(true)
  })

  it('encoder processInterleaved should fail if invalid buffer size is sent', () => {
    const enc = new api.EncoderBuilder()
      .setBitsPerSample(24)
      .setChannels(2)
      .setCompressionLevel(9)
      .setSampleRate(44100)
      .buildWithFile(
        tmpFile.path,
      )

    expect(() => enc.processInterleaved(encData.slice(4), totalSamples)).toThrow()
    expect(enc.finish()).toBe(true)
  })

  it('encoder get other properties work', () => {
    const enc = new api.EncoderBuilder()
      .setBitsPerSample(24)
      .setChannels(2)
      .setCompressionLevel(9)
      .setSampleRate(44100)
      .setVerify(true)
      .setTotalSamplesEstimate(totalSamples)
      .buildWithFile(
        tmpFile.path,
      )

    expect(enc.verify).toBe(true)

    expect(enc.channels).toBe(2)
    expect(enc.bitsPerSample).toBe(24)
    expect(enc.sampleRate).toBe(44100)
    expect(enc.blocksize).toBe(4096)
    expect(enc.doMidSideStereo).toBe(true)
    expect(enc.looseMidSideStereo).toBe(false)
    expect(enc.maxLpcOrder).toBe(12)
    expect(enc.qlpCoeffPrecision).toBe(15)
    expect(enc.doQlpCoeffPrecSearch).toBe(false)
    expect(enc.doEscapeCoding).toBe(false)
    expect(enc.doExhaustiveModelSearch).toBe(false)
    expect(enc.minResidualPartitionOrder).toBe(0)
    expect(enc.maxResidualPartitionOrder).toBe(6)
    expect(enc.riceParameterSearchDist).toBe(0)
    expect(enc.totalSamplesEstimate).toEqual(totalSamples)

    expect(enc.finish()).toBe(true)
  })

  it('encoder should throw if built with sync but called async method', async () => {
    const enc = new api.EncoderBuilder()
      .setBitsPerSample(24)
      .setChannels(2)
      .setCompressionLevel(9)
      .setSampleRate(44100)
      .buildWithFile(
        tmpFile.path,
        null,
      )

    expect(() => enc.processInterleavedAsync(encData)).toThrow(/This method cannot be called when Encoder has been created using synchronous method/)

    enc.finish()
  })

  it('decoder should throw if built with sync but called async method', async () => {
    const allBuffers = []
    const dec = new api.DecoderBuilder().buildWithFile(
      pathForFile('loop.flac'),
      (_, buffers) => {
        allBuffers.push(buffers.map((b) => Buffer.from(b)))
        return 0
      },
      null,
      // eslint-disable-next-line no-console
      (errorCode) => console.error(api.Decoder.ErrorStatusString[errorCode], errorCode),
    )

    expect(() => dec.processUntilEndOfMetadataAsync()).toThrow(/This method cannot be called when Decoder has been created using synchronous method/)

    dec.finish()
  })
})
