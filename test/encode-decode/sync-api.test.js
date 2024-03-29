import fs from 'node:fs'
import tempUntracked from 'temp'
import {
  afterEach,
  beforeEach,
  describe,
  expect,
  it,
} from 'vitest'
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

describe('encode & decode: sync api', () => {
  beforeEach(() => {
    tmpFile = temp.openSync('flac-bindings.encode-decode.sync-api')
    deferredScope = createDeferredScope()
    fs.closeSync(tmpFile.fd)
  })

  afterEach(() => {
    temp.cleanupSync()
    deferredScope.finalize()
  })

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

    expect(dec.processUntilEndOfMetadata()).toBeTruthy()
    expect(dec.processUntilEndOfStream()).toBeTruthy()
    expect(dec.flush()).toBeTruthy()
    expect(dec.finish()).not.toBeNull()

    const [finalBuffer, samples] = joinIntoInterleaved(allBuffers)
    expect(samples).toStrictEqual(totalSamples)
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

    expect(dec.processUntilEndOfMetadata()).toBeTruthy()
    expect(dec.processUntilEndOfStream()).toBeTruthy()
    expect(dec.flush()).toBeTruthy()
    expect(dec.finish()).not.toBeNull()

    const [finalBuffer, samples] = joinIntoInterleaved(allBuffers)
    expect(samples).toStrictEqual(totalSamples)
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

    expect(dec.processUntilEndOfMetadata()).toBeTruthy()
    expect(dec.processUntilEndOfStream()).toBeTruthy()
    expect(dec.flush()).toBeTruthy()
    expect(dec.finish()).not.toBeNull()

    const [finalBuffer, samples] = joinIntoInterleaved(allBuffers)
    expect(samples).toStrictEqual(totalSamples)
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

    expect(dec.processUntilEndOfMetadata()).toBeTruthy()
    expect(dec.processUntilEndOfStream()).toBeTruthy()
    expect(dec.flush()).toBeTruthy()
    expect(dec.finish()).not.toBeNull()

    const [finalBuffer, samples] = joinIntoInterleaved(allBuffers)
    expect(samples).toStrictEqual(totalSamples)
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

    expect(dec.processUntilEndOfMetadata()).toBeTruthy()
    expect(dec.processSingle()).toBeTruthy()
    expect(dec.skipSingleFrame()).toBeTruthy()
    expect(dec.processSingle()).toBeTruthy()
    expect(dec.flush()).toBeTruthy()
    expect(dec.finish()).not.toBeNull()
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

    expect(dec.processUntilEndOfMetadata()).toBeTruthy()
    expect(dec.processSingle()).toBeTruthy()
    expect(dec.seekAbsolute(totalSamples / 5)).toBeTruthy()
    expect(dec.getDecodePosition()).toBe(157036)
    expect(dec.processSingle()).toBeTruthy()
    expect(dec.flush()).toBeTruthy()
    expect(dec.finish()).not.toBeNull()
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

    expect(dec.processSingle()).toBeTruthy()
    expect(dec.finish()).not.toBeNull()

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

    expect(dec.processUntilEndOfMetadata()).toBeTruthy()
    expect(dec.processSingle()).toBeTruthy()

    expect(dec.getChannels()).toBe(2)
    expect(dec.getBitsPerSample()).toBe(24)
    expect(dec.getSampleRate()).toBe(44100)
    expect(dec.getChannelAssignment()).toStrictEqual(api.format.ChannelAssignment.MID_SIDE)
    expect(dec.getBlocksize()).toBe(4096)
    expect(dec.getTotalSamples()).toStrictEqual(totalSamples)
    expect(dec.getMd5Checking()).toBeTruthy()
    expect(dec.getDecodePosition()).toBe(22862)

    expect(dec.finish()).toBeNull()
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

    expect(enc.processInterleaved(encData)).toBeTruthy()
    expect(enc.finish()).not.toBeNull()

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

    expect(enc.processInterleaved(encData)).toBeTruthy()
    expect(enc.finish()).not.toBeNull()

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

    expect(enc.processInterleaved(encData)).toBeTruthy()
    expect(enc.finish()).not.toBeNull()

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

    expect(enc.processInterleaved(encData)).toBeTruthy()
    expect(enc.finish()).not.toBeNull()

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

    expect(enc.process(encDataAlt, totalSamples)).toBeTruthy()
    expect(enc.finish()).not.toBeNull()

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

    expect(enc.processInterleaved(encData)).toBeTruthy()
    expect(enc.finish()).not.toBeNull()

    expect(metadataBlock).not.toBeNull()
    expect(metadataBlock.type).toBe(0)
    expect(metadataBlock.totalSamples).toStrictEqual(totalSamples)
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
    expect(enc.finish()).not.toBeNull()
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
    expect(enc.finish()).not.toBeNull()
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
    expect(enc.finish()).not.toBeNull()
  })

  it('encoder get other properties work', () => {
    const enc = new api.EncoderBuilder()
      .setBitsPerSample(24)
      .setChannels(2)
      .setCompressionLevel(9)
      .setSampleRate(44100)
      .setVerify(true)
      .setTotalSamplesEstimate(totalSamples)
      .setLimitMinBitrate(false)
      .buildWithFile(
        tmpFile.path,
      )

    expect(enc.verify).toBeTruthy()

    expect(enc.channels).toBe(2)
    expect(enc.bitsPerSample).toBe(24)
    expect(enc.sampleRate).toBe(44100)
    expect(enc.blocksize).toBe(4096)
    expect(enc.doMidSideStereo).toBeTruthy()
    expect(enc.looseMidSideStereo).toBeFalsy()
    expect(enc.maxLpcOrder).toBe(12)
    expect(enc.qlpCoeffPrecision).toBe(15)
    expect(enc.doQlpCoeffPrecSearch).toBeFalsy()
    expect(enc.doEscapeCoding).toBeFalsy()
    expect(enc.doExhaustiveModelSearch).toBeFalsy()
    expect(enc.minResidualPartitionOrder).toBe(0)
    expect(enc.maxResidualPartitionOrder).toBe(6)
    expect(enc.riceParameterSearchDist).toBe(0)
    expect(enc.totalSamplesEstimate).toStrictEqual(totalSamples)
    expect(enc.limitMinBitrate).toBeFalsy()

    expect(enc.finish()).not.toBeNull()
  })

  it('encoder get other properties work (builder version)', () => {
    const enc = new api.EncoderBuilder()
      .setBitsPerSample(24)
      .setChannels(2)
      .setCompressionLevel(9)
      .setSampleRate(44100)
      .setVerify(true)
      .setTotalSamplesEstimate(totalSamples)
      .setLimitMinBitrate(false)

    expect(enc.getVerify()).toBeTruthy()

    expect(enc.getChannels()).toBe(2)
    expect(enc.getBitsPerSample()).toBe(24)
    expect(enc.getSampleRate()).toBe(44100)
    // NOTE: not set yet
    expect(enc.getBlocksize()).toBe(0)
    expect(enc.getDoMidSideStereo()).toBeTruthy()
    expect(enc.getLooseMidSideStereo()).toBeFalsy()
    expect(enc.getMaxLpcOrder()).toBe(12)
    // NOTE: not set yet
    expect(enc.getQlpCoeffPrecision()).toBe(0)
    expect(enc.getDoQlpCoeffPrecSearch()).toBeFalsy()
    expect(enc.getDoEscapeCoding()).toBeFalsy()
    expect(enc.getDoExhaustiveModelSearch()).toBeFalsy()
    expect(enc.getMinResidualPartitionOrder()).toBe(0)
    expect(enc.getMaxResidualPartitionOrder()).toBe(6)
    expect(enc.getRiceParameterSearchDist()).toBe(0)
    expect(enc.getTotalSamplesEstimate()).toStrictEqual(totalSamples)
    expect(enc.getLimitMinBitrate()).toBeFalsy()
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

  it('encoder builder can be reused', () => {
    const enc = new api.EncoderBuilder()
      .setBitsPerSample(24)
      .setChannels(2)
      .setCompressionLevel(9)
      .setSampleRate(44100)
      .buildWithFile(tmpFile.path, null)

    const builder = enc.finish()
    expect(builder).not.toBeNull()

    const enc2 = builder
      .setBitsPerSample(16)
      .setChannels(1)
      .setCompressionLevel(7)
      .setSampleRate(88200)
      .buildWithFile(tmpFile.path, null)

    expect(enc2.finish()).not.toBeNull()
  })

  it('decoder builder can be reused', () => {
    const dec = new api.DecoderBuilder()
      .buildWithFile(
        pathForFile('loop.flac'),
        () => api.Decoder.WriteStatus.CONTINUE,
        null,
        () => {},
      )

    const builder = dec.finish()
    expect(builder).not.toBeNull()

    const dec2 = builder
      .buildWithOggFile(
        pathForFile('loop.oga'),
        () => api.Decoder.WriteStatus.CONTINUE,
        null,
        () => {},
      )

    expect(dec2.finish()).not.toBeNull()
  })
})
