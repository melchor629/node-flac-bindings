import { Readable } from 'stream'
import { BaseDecoder, DecoderOptions, DecoderPosition } from './interfaces'

/** Options to be set to the decoder before starting to decode. */
export interface FileDecoderOptions extends DecoderOptions {
  /** The file to decode from */
  file: string
}

/**
 * FLAC decoder which reads a FLAC or Ogg/FLAC file and outputs
 * a interleaved raw PCM stream.
 * @emits metadata When a metadata block is received, the event will be fired
 */
export default class FileDecoder extends Readable implements BaseDecoder {
  constructor(options: FileDecoderOptions);
  readonly processedSamples: number
  getTotalSamples(): number | undefined
  getChannels(): number | undefined
  getChannelAssignment(): 0 | 1 | 2 | 3 | undefined
  getBitsPerSample(): number | undefined
  getOutputBitsPerSample(): number | undefined
  getSampleRate(): number | undefined
  getProgress(): DecoderPosition
}
