import events from 'node:events'
import fs from 'node:fs'
import tempUntracked from 'temp'
import {
  afterEach,
  beforeEach,
  describe,
  expect,
  it,
} from 'vitest'
import {
  FileDecoder,
  FileEncoder,
  StreamEncoder,
  StreamDecoder,
} from '../../lib/index.js'
import {
  pathForFile as fullPathForFile,
  comparePCM,
  gc,
  loopPcmAudio,
} from '../helper/index.js'

const { audio: pathForFile } = fullPathForFile
const {
  totalSamples,
  okData,
} = loopPcmAudio
const temp = tempUntracked.track()

let tmpFile

describe('encode & decode: js streams', () => {
  beforeEach(() => {
    tmpFile = temp.openSync('flac-bindings.encode-decode.js-streams')
    fs.closeSync(tmpFile.fd)
  })

  afterEach(() => {
    temp.cleanupSync()
  })

  describe('stream', () => {
    it('encode/decode using stream (non-ogg)', async () => {
      const dec = new StreamDecoder({ outputAs32: false })
      const enc = new StreamEncoder({
        samplerate: 44100,
        channels: 2,
        bitsPerSample: 24,
        compressionLevel: 9,
        inputAs32: false,
      })
      const input = fs.createReadStream(pathForFile('loop.flac'))
      const output = fs.createWriteStream(tmpFile.path)

      input.pipe(dec)
      dec.pipe(enc)
      enc.pipe(output)
      await events.once(output, 'close')

      expect((await fs.promises.stat(tmpFile.path)).size).toBeGreaterThan(660 * 1000)
      expect(dec.processedSamples).toStrictEqual(totalSamples)
      expect(enc.processedSamples).toStrictEqual(totalSamples)
      comparePCM(okData, tmpFile.path, 24)
    })

    it('encode/decode using stream (ogg)', async () => {
      const dec = new StreamDecoder({ outputAs32: false, isOggStream: true })
      const enc = new StreamEncoder({
        samplerate: 44100,
        channels: 2,
        bitsPerSample: 24,
        compressionLevel: 9,
        inputAs32: false,
        isOggStream: true,
      })
      const input = fs.createReadStream(pathForFile('loop.oga'))
      const output = fs.createWriteStream(tmpFile.path)

      input.pipe(dec)
      dec.pipe(enc)
      enc.pipe(output)
      await events.once(output, 'close')

      expect((await fs.promises.stat(tmpFile.path)).size).toBeGreaterThan(660 * 1000)
      expect(dec.processedSamples).toStrictEqual(totalSamples)
      expect(enc.processedSamples).toStrictEqual(totalSamples)
      comparePCM(okData, tmpFile.path, 24, true)
    })

    it('decode using stream and file-bit output', async () => {
      const input = fs.createReadStream(pathForFile('loop.flac'))
      const dec = new StreamDecoder({ outputAs32: false })
      const chunks = []

      input.pipe(dec)
      dec.on('data', (chunk) => chunks.push(chunk))
      await events.once(dec, 'end')

      const raw = Buffer.concat(chunks)
      expect(raw).toHaveLength(totalSamples * 3 * 2)
      expect(dec.processedSamples).toStrictEqual(totalSamples)
      comparePCM(okData, raw, 24)
    })

    it('decode using stream and 32-bit output', async () => {
      const input = fs.createReadStream(pathForFile('loop.flac'))
      const dec = new StreamDecoder({ outputAs32: true })
      const chunks = []

      input.pipe(dec)
      dec.on('data', (chunk) => chunks.push(chunk))
      await events.once(dec, 'end')

      const raw = Buffer.concat(chunks)
      expect(raw).toHaveLength(totalSamples * 4 * 2)
      expect(dec.processedSamples).toStrictEqual(totalSamples)
      comparePCM(okData, raw, 32)
    })

    it('decode using stream (ogg)', async () => {
      const input = fs.createReadStream(pathForFile('loop.oga'))
      const dec = new StreamDecoder({ outputAs32: false, isOggStream: true })
      const chunks = []

      input.pipe(dec)
      dec.on('data', (chunk) => chunks.push(chunk))
      await events.once(dec, 'end')

      const raw = Buffer.concat(chunks)
      expect(raw).toHaveLength(totalSamples * 3 * 2)
      expect(dec.processedSamples).toStrictEqual(totalSamples)
      comparePCM(okData, raw, 24)
    })

    it('encode using stream and file-bit input', async () => {
      const output = fs.createWriteStream(tmpFile.path)
      const enc = new StreamEncoder({
        samplerate: 44100,
        channels: 2,
        bitsPerSample: 24,
        inputAs32: false,
      })
      const raw = okData

      enc.pipe(output)
      enc.end(raw)
      await events.once(output, 'close')

      expect((await fs.promises.stat(tmpFile.path)).size).toBeGreaterThan(660 * 1000)
      expect(enc.processedSamples).toStrictEqual(totalSamples)
      comparePCM(okData, tmpFile.path, 24)
    })

    it('encode using stream and 32-bit input', async () => {
      const output = fs.createWriteStream(tmpFile.path)
      const enc = new StreamEncoder({
        samplerate: 44100,
        channels: 2,
        bitsPerSample: 24,
        inputAs32: true,
      })
      const raw = okData
      const chunkazo = Buffer.allocUnsafe(totalSamples * 4 * 2)
      for (let i = 0; i < totalSamples * 2; i += 1) {
        chunkazo.writeInt32LE(raw.readIntLE(i * 3, 3), i * 4)
      }

      enc.pipe(output)
      enc.end(chunkazo)
      await events.once(output, 'close')

      expect((await fs.promises.stat(tmpFile.path)).size).toBeGreaterThan(660 * 1000)
      expect(enc.processedSamples).toStrictEqual(totalSamples)
      comparePCM(okData, tmpFile.path, 24)
    })

    it('encode using stream (ogg)', async () => {
      const output = fs.createWriteStream(tmpFile.path)
      const enc = new StreamEncoder({
        samplerate: 44100,
        channels: 2,
        bitsPerSample: 24,
        inputAs32: false,
        isOggStream: true,
      })
      const raw = okData

      enc.pipe(output)
      enc.end(raw)
      await events.once(output, 'close')

      expect((await fs.promises.stat(tmpFile.path)).size).toBeGreaterThan(660 * 1000)
      expect(enc.processedSamples).toStrictEqual(totalSamples)
      comparePCM(okData, tmpFile.path, 24, true)
    })

    it('stream decoder should read properties', async () => {
      const input = fs.createReadStream(pathForFile('loop.flac'))
      const dec = new StreamDecoder({ outputAs32: true })

      input.pipe(dec)
      await events.once(dec, 'data')

      expect(dec.getBitsPerSample()).toBe(24)
      expect(dec.getChannels()).toBe(2)
      expect(dec.getChannelAssignment()).toBe(3)
      expect(dec.getTotalSamples()).toStrictEqual(totalSamples)
      expect(dec.getSampleRate()).toBe(44100)

      dec.on('data', () => undefined)
      await events.once(dec, 'end')

      expect(dec.getProgress()).toStrictEqual({
        position: totalSamples,
        totalSamples,
        percentage: 1,
        totalSeconds: totalSamples / 44100,
        currentSeconds: totalSamples / 44100,
      })
    })

    it('stream decoder should emit format', async () => {
      const input = fs.createReadStream(pathForFile('loop.flac'))
      const dec = new StreamDecoder({ outputAs32: true })
      let format = null

      dec.on('format', (fmt) => {
        format = fmt
      })
      input.pipe(dec)
      dec.on('data', () => undefined)
      await events.once(dec, 'end')

      expect(format).not.toBeNull()
      expect(format).toStrictEqual({
        channels: 2,
        bitDepth: 24,
        bitsPerSample: 24,
        is32bit: true,
        sampleRate: 44100,
        totalSamples,
      })
    })

    it('stream decoder should emit metadata when required', async () => {
      const input = fs.createReadStream(pathForFile('loop.flac'))
      const dec = new StreamDecoder({ outputAs32: true, metadata: true })
      const metadataBlocks = []

      dec.on('metadata', (metadata) => metadataBlocks.push(metadata))
      input.pipe(dec)
      dec.on('data', () => undefined)
      await events.once(dec, 'end')

      expect(metadataBlocks).not.toBeEmpty()
      expect(metadataBlocks).toHaveLength(4)
    })

    it('stream decoder should emit specific metadata when required', async () => {
      const input = fs.createReadStream(pathForFile('loop.flac'))
      const dec = new StreamDecoder({ outputAs32: true, metadata: [0] })
      const metadataBlocks = []

      dec.on('metadata', (metadata) => metadataBlocks.push(metadata))
      input.pipe(dec)
      dec.on('data', () => undefined)
      await events.once(dec, 'end')

      expect(metadataBlocks).not.toBeEmpty()
      expect(metadataBlocks).toHaveLength(1)
    })

    it('encode using ogg (stream)', async () => {
      const dec = new StreamDecoder({ outputAs32: false })
      const enc = new StreamEncoder({
        isOggStream: true,
        oggSerialNumber: 0x123456,
        samplerate: 44100,
        channels: 2,
        bitsPerSample: 24,
        inputAs32: false,
      })
      const input = fs.createReadStream(pathForFile('loop.flac'))
      const output = fs.createWriteStream(tmpFile.path)

      input.pipe(dec)
      dec.pipe(enc)
      enc.pipe(output)
      await events.once(output, 'close')

      expect((await fs.promises.stat(tmpFile.path)).size).toBeGreaterThan(660 * 1000)
      expect(dec.processedSamples).toStrictEqual(totalSamples)
      expect(enc.processedSamples).toStrictEqual(totalSamples)
      comparePCM(okData, tmpFile.path, 24, true)
    })

    it('encoder options should not throw exception', async () => {
      const enc = new StreamEncoder({
        bitsPerSample: 16,
        channels: 2,
        samplerate: 48000,
        apodization: 'tukey(0.5);partial_tukey(2);punchout_tukey(3)',
        blocksize: 1024,
        doExhaustiveModelSearch: false,
        doMidSideStereo: true,
        doQlpCoeffPrecSearch: false,
        looseMidSideStereo: false,
        maxLpcOrder: 12,
        maxResidualPartitionOrder: 6,
        minResidualPartitionOrder: 0,
        qlpCoeffPrecision: 0,
        totalSamplesEstimate: 48000,
        metadata: [],
      })

      await expect((async () => {
        enc.write(Buffer.alloc(1000 * 2 * 2))
        enc.end()
        enc.on('data', () => undefined)
        await events.once(enc, 'end')
      })()).resolves.not.toThrow()
    })

    it('encoder with no data does not write anything', async () => {
      const enc = new StreamEncoder({
        channels: 2,
        samplerate: 48000,
        bitsPerSample: 16,
      })

      await new Promise((resolve) => { enc.end(resolve) })

      expect(enc.processedSamples).toBe(0)
    })

    it('decoder with no data does not write anything', async () => {
      const dec = new StreamDecoder()

      await new Promise((resolve) => { dec.end(resolve) })

      expect(dec.processedSamples).toBe(0)
    })
  })

  describe('file', () => {
    it('encode/decode using file (non-ogg)', async () => {
      const dec = new FileDecoder({ file: pathForFile('loop.flac'), outputAs32: false })
      const enc = new FileEncoder({
        file: tmpFile.path,
        samplerate: 44100,
        channels: 2,
        bitsPerSample: 24,
        inputAs32: false,
      })

      dec.pipe(enc)
      await events.once(enc, 'finish')

      expect((await fs.promises.stat(tmpFile.path)).size).toBeGreaterThan(660 * 1000)
      expect(dec.processedSamples).toStrictEqual(totalSamples)
      expect(enc.processedSamples).toStrictEqual(totalSamples)
      comparePCM(okData, tmpFile.path, 24)
    })

    it('encode/decode using file (ogg)', async () => {
      const dec = new FileDecoder({ file: pathForFile('loop.oga'), outputAs32: false, isOggStream: true })
      const enc = new FileEncoder({
        file: tmpFile.path,
        samplerate: 44100,
        channels: 2,
        bitsPerSample: 24,
        inputAs32: false,
        isOggStream: true,
      })

      dec.pipe(enc)
      await events.once(enc, 'finish')

      expect((await fs.promises.stat(tmpFile.path)).size).toBeGreaterThan(660 * 1000)
      expect(dec.processedSamples).toStrictEqual(totalSamples)
      expect(enc.processedSamples).toStrictEqual(totalSamples)
      comparePCM(okData, tmpFile.path, 24, true)
    })

    it('encode/decode using 32 bit integers', async () => {
      const dec = new FileDecoder({ file: pathForFile('loop.flac'), outputAs32: true })
      const enc = new FileEncoder({
        file: tmpFile.path,
        samplerate: 44100,
        channels: 2,
        bitsPerSample: 24,
        inputAs32: true,
      })

      dec.pipe(enc)
      await events.once(enc, 'finish')

      expect((await fs.promises.stat(tmpFile.path)).size).toBeGreaterThan(660 * 1000)
      expect(dec.processedSamples).toStrictEqual(totalSamples)
      expect(enc.processedSamples).toStrictEqual(totalSamples)
      comparePCM(okData, tmpFile.path, 24)
    })

    it('decode using file and file-bit output', async () => {
      const dec = new FileDecoder({ outputAs32: false, file: pathForFile('loop.flac') })
      const chunks = []

      dec.on('data', (chunk) => chunks.push(chunk))
      await events.once(dec, 'end')

      const raw = Buffer.concat(chunks)
      expect(raw).toHaveLength(totalSamples * 3 * 2)
      expect(dec.processedSamples).toStrictEqual(totalSamples)
      comparePCM(okData, raw, 24)
    })

    it('decode using file and 32-bit output', async () => {
      const dec = new FileDecoder({ outputAs32: true, file: pathForFile('loop.flac') })
      const chunks = []

      dec.on('data', (chunk) => chunks.push(chunk))
      await events.once(dec, 'end')

      const raw = Buffer.concat(chunks)
      expect(raw).toHaveLength(totalSamples * 4 * 2)
      expect(dec.processedSamples).toStrictEqual(totalSamples)
      comparePCM(okData, raw, 32)
    })

    it('decode using file (ogg)', async () => {
      const dec = new FileDecoder({ outputAs32: false, file: pathForFile('loop.oga'), isOggStream: true })
      const chunks = []

      dec.on('data', (chunk) => chunks.push(chunk))
      await events.once(dec, 'end')

      const raw = Buffer.concat(chunks)
      expect(raw).toHaveLength(totalSamples * 3 * 2)
      expect(dec.processedSamples).toStrictEqual(totalSamples)
      comparePCM(okData, raw, 24)
    })

    it('encode using file and 24-bit input', async () => {
      const file = tmpFile.path
      const enc = new FileEncoder({
        samplerate: 44100,
        channels: 2,
        bitsPerSample: 24,
        inputAs32: false,
        file,
      })
      const raw = okData

      enc.end(raw)
      await events.once(enc, 'finish')

      expect((await fs.promises.stat(tmpFile.path)).size).toBeGreaterThan(660 * 1000)
      expect(enc.processedSamples).toStrictEqual(totalSamples)
      comparePCM(okData, tmpFile.path, 24)
    })

    it('encode using file and 32-bit input', async () => {
      const file = tmpFile.path
      const enc = new FileEncoder({
        samplerate: 44100,
        channels: 2,
        bitsPerSample: 24,
        inputAs32: true,
        file,
      })
      const raw = okData
      const chunkazo = Buffer.allocUnsafe(totalSamples * 4 * 2)
      for (let i = 0; i < totalSamples * 2; i += 1) {
        chunkazo.writeInt32LE(raw.readIntLE(i * 3, 3), i * 4)
      }

      enc.end(chunkazo)
      await events.once(enc, 'finish')

      expect((await fs.promises.stat(tmpFile.path)).size).toBeGreaterThan(660 * 1000)
      expect(enc.processedSamples).toStrictEqual(totalSamples)
      comparePCM(okData, tmpFile.path, 24)
    })

    it('encode using file (ogg)', async () => {
      const file = tmpFile.path
      const enc = new FileEncoder({
        samplerate: 44100,
        channels: 2,
        bitsPerSample: 24,
        inputAs32: false,
        file,
        isOggStream: true,
      })
      const raw = okData

      enc.end(raw)
      await events.once(enc, 'finish')

      expect((await fs.promises.stat(tmpFile.path)).size).toBeGreaterThan(660 * 1000)
      expect(enc.processedSamples).toStrictEqual(totalSamples)
      comparePCM(okData, tmpFile.path, 24, true)
    })

    it('file decoder should read properties', async () => {
      const dec = new FileDecoder({ outputAs32: true, file: pathForFile('loop.flac') })

      await events.once(dec, 'data')

      expect(dec.getBitsPerSample()).toBe(24)
      expect(dec.getChannels()).toBe(2)
      expect(dec.getChannelAssignment()).toBe(3)
      expect(dec.getTotalSamples()).toStrictEqual(totalSamples)
      expect(dec.getSampleRate()).toBe(44100)

      dec.on('data', () => undefined)
      await events.once(dec, 'end')

      expect(dec.getProgress()).toStrictEqual({
        position: totalSamples,
        totalSamples,
        percentage: 1,
        totalSeconds: totalSamples / 44100,
        currentSeconds: totalSamples / 44100,
      })
    })

    it('file decoder should emit format', async () => {
      const dec = new FileDecoder({ outputAs32: true, file: pathForFile('loop.flac') })
      let format = null

      dec.on('format', (fmt) => {
        format = fmt
      })
      dec.on('data', () => undefined)
      await events.once(dec, 'end')

      expect(format).not.toBeNull()
      expect(format).toStrictEqual({
        channels: 2,
        bitDepth: 24,
        bitsPerSample: 24,
        is32bit: true,
        sampleRate: 44100,
        totalSamples,
      })
    })

    it('file decoder should emit metadata when required', async () => {
      const dec = new FileDecoder({ outputAs32: true, metadata: true, file: pathForFile('loop.flac') })
      const metadataBlocks = []

      dec.on('metadata', (metadata) => metadataBlocks.push(metadata))
      dec.on('data', () => undefined)
      await events.once(dec, 'end')

      expect(metadataBlocks).not.toBeEmpty()
      expect(metadataBlocks).toHaveLength(4)
    })

    it('file decoder should fail if file does not exist', async () => {
      const dec = new FileDecoder({ file: pathForFile('does not exist.flac') })

      await expect(() => events.once(dec, 'data')).rejects.toThrow('Decoder initialization failed: ERROR_OPENING_FILE')
    })

    it('file encoder should fail if file cannot write', async () => {
      const enc = new FileEncoder({
        file: pathForFile('does/not/exist.flac'),
        channels: 1,
        bitsPerSample: 16,
        samplerate: 44100,
        totalSamplesEstimate: 44100,
      })

      enc.write(Buffer.alloc(1000 * 2))
      await expect(() => events.once(enc, 'data')).rejects.toThrow('Encoder initialization failed: ENCODER_ERROR')

      // ENCODER_ERROR means that the error is described in getState()
      expect(enc.getState()).toBe(6) // IO_ERROR
    })

    it('encode using ogg (file)', async () => {
      const dec = new FileDecoder({ file: pathForFile('loop.flac'), outputAs32: false })
      const enc = new FileEncoder({
        file: tmpFile.path,
        isOggStream: true,
        oggSerialNumber: 0x6543321,
        samplerate: 44100,
        channels: 2,
        bitsPerSample: 24,
        inputAs32: false,
      })

      dec.pipe(enc)
      await events.once(enc, 'finish')

      expect((await fs.promises.stat(tmpFile.path)).size).toBeGreaterThan(660 * 1000)
      expect(dec.processedSamples).toStrictEqual(totalSamples)
      expect(enc.processedSamples).toStrictEqual(totalSamples)
      comparePCM(okData, tmpFile.path, 24, true)
    })

    it('encoder with no data does not write anything', async () => {
      const enc = new FileEncoder({
        file: tmpFile.path,
        channels: 2,
        samplerate: 48000,
        bitsPerSample: 16,
      })

      await new Promise((resolve) => { enc.end(resolve) })

      expect(enc.processedSamples).toBe(0)
    })

    it('encoder without file throws', () => {
      expect(() => new FileEncoder({})).toThrow(/No file/)
    })

    it('decoder without file throws', () => {
      expect(() => new FileDecoder({})).toThrow(/No file/)
    })
  })

  it('gc should work', () => {
    expect(gc).not.toThrow()
  })
})
