import { Writable } from 'stream'
import { Encoder, EnumValues } from '../api'
import { BaseEncoder, EncoderOptions } from './interfaces'

/** Options to be set to the encoder before starting to encode. */
export interface FileEncoderOptions extends EncoderOptions {
  /** The output file for the encoded FLAC audio */
  file: string
}

/**
 * FLAC encoder which receives an interleaved PCM stream and writes it into
 * a file.
 */
export default class FileEncoder extends Writable implements BaseEncoder {
  constructor(options: FileEncoderOptions)
  readonly processedSamples: number
  getState(): EnumValues<Encoder.State>
}
