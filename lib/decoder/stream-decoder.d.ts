import { Transform } from 'stream'
import { BaseDecoder, DecoderOptions, DecoderPosition } from './interfaces'

/**
 * FLAC decoder which transforms a stream of FLAC (or Ogg/FLAC) into
 * a interleaved raw PCM stream.
 * @emits metadata When a metadata block is received, the event will be fired
 */
export default class StreamDecoder extends Transform implements BaseDecoder {
  constructor(props: DecoderOptions)
  readonly processedSamples: number
  getTotalSamples(): number | undefined
  getChannels(): number | undefined
  getChannelAssignment(): 0 | 1 | 2 | 3 | undefined
  getBitsPerSample(): number | undefined
  getOutputBitsPerSample(): number | undefined
  getSampleRate(): number | undefined
  getProgress(): DecoderPosition | undefined
}
