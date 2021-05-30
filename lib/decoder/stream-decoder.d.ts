import { Transform } from 'stream'
import { BaseDecoder, DecoderOptions, DecoderPosition } from './interfaces'

/**
 * FLAC decoder which transforms a stream of FLAC (or Ogg/FLAC) into
 * a interleaved raw PCM stream.
 * @emits metadata When a metadata block is received, the event will be fired
 * @emits format When the decoder knows the exact format of the flac
 */
export default class StreamDecoder extends Transform implements BaseDecoder {
  constructor(props: DecoderOptions)
  readonly processedSamples: number
  getTotalSamples(): number
  getChannels(): number
  getChannelAssignment(): 0 | 1 | 2 | 3
  getBitsPerSample(): number
  getOutputBitsPerSample(): number
  getSampleRate(): number
  getProgress(): DecoderPosition | undefined
}
