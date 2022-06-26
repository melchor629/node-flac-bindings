import { format } from '../api.js'

/** Options to be set to the decoder before starting to decode. */
export interface DecoderOptions {
  /** If set to true, the input must be an Ogg/FLAC stream. */
  isOggStream?: boolean
  /**
  * If set to true, it will emit the `metadata` for each metadata block.
  * If set to an array, it will only emit `metadata` for the types
  * specified in it.
  **/
  metadata?: format.MetadataType[] | true
  /**
  * If set to `true`, samples will be 32 bit integers. By default, this
  * value is `false`, except when the flac is 24 bit. When `false`, the
  * output will be `bitsPerSample` bit.
  **/
  outputAs32?: boolean
}

export interface DecoderPosition {
  /** Position where the decoding is at, in samples */
  position: number
  /** Total samples (if flac provides it) */
  totalSamples: number
  /** Value between 0 and 1 that represents the percentage of samples decoded (or NaN if there is no total samples) */
  percentage: number
  /** Total seconds (if flac provides it) */
  totalSeconds: number
  /** Current position in seconds */
  currentSeconds: number
}

export interface BaseDecoder {
  /** Gets total number of samples if possible, once it started to process */
  getTotalSamples(): number
  /** Gets the number of channels found in the stream, once it started to process */
  getChannels(): number
  /** Gets the channel assignment for the stream, once it started to process */
  getChannelAssignment(): 0 | 1 | 2 | 3
  /** Gets the bits per sample found in the stream, once it started to process */
  getBitsPerSample(): number
  /** Gets the bits per sample of the output */
  getOutputBitsPerSample(): number
  /** Gets the sample rate of the stream, once it started to process */
  getSampleRate(): number
  /** Returns the progress of the decoding (if possible) */
  getProgress(): DecoderPosition | undefined
  /** Get the number of processed samples in the process */
  readonly processedSamples: number;
}
