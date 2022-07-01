import { Readable } from 'stream'
import { BaseDecoder, DecoderOptions, DecoderPosition } from './interfaces.js'

/** Options to be set to the decoder before starting to decode. */
export interface FileDecoderOptions extends DecoderOptions {
  /** The file to decode from */
  file: string
}

/**
 * FLAC decoder which reads a FLAC or Ogg/FLAC file and outputs
 * a interleaved raw PCM stream.
 * @emits metadata When a metadata block is received, the event will be fired
 * @emits format When the decoder knows the exact format of the flac
 * @emits flac-error When there is an error on the decoding
 */
export default class FileDecoder extends Readable implements BaseDecoder {
  constructor(options: FileDecoderOptions);
  readonly processedSamples: number
  getTotalSamples(): number
  getChannels(): number
  getChannelAssignment(): 0 | 1 | 2 | 3
  getBitsPerSample(): number
  getOutputBitsPerSample(): number
  getSampleRate(): number
  getProgress(): DecoderPosition | undefined
}
