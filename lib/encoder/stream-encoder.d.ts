import { Transform } from 'stream'
import { Encoder, EnumValues } from '../api'
import { BaseEncoder, EncoderOptions } from './interfaces'

/**
 * FLAC encoder which receives an interleaved PCM stream and transforms it into
 * a FLAC stream.
 */
export default class StreamEncoder extends Transform implements BaseEncoder {
  constructor(options: EncoderOptions)
  readonly processedSamples: number
  getState(): EnumValues<Encoder.State>
}
