import { Encoder, EnumValues, metadata } from '../api.js'

/** Options to be set to the encoder before starting to encode. */
export interface EncoderOptions {
  /** If true, then the output will be an Ogg/FLAC file */
  isOggStream?: boolean;
  /** Number of channels to be encoded */
  channels?: number;
  /** Sample resolution of the input to be encoded */
  bitsPerSample?: number;
  /** Alternative name for bitsPerSample */
  bitDepth?: number;
  /**
  * If set to `true`, all the input will be treated as 32 bit, if `false``
  * the input will be treated as `bitsPerSample` bit.
  * When true, the input must be int32 holding the values in range of the
  * original bit depth (for 16 bit values must be inside [-32768,32767]).
  * By default is set to false.
  **/
  inputAs32?: boolean;
  /**
   * Alias of {@link EncoderOptions.inputAs32}.
   */
  is32bit?: boolean;
  /** Sample rate in Hz of the input to be encoded */
  samplerate?: number;
  /** Sample rate in Hz of the input to be encoded */
  sampleRate?: number;
  /** If the output is Ogg, this serial number must be set */
  oggSerialNumber?: number;
  /** Number of samples expected to be written into the stream */
  totalSamplesEstimate?: number | bigint;
  /**
  * Sets the compression level (by default 5).
  * @see https://xiph.org/flac/api/group__flac__stream__encoder.html#gae49cf32f5256cb47eecd33779493ac85
  **/
  compressionLevel?: number;
  /**
  * Set to true to enable mid-side encoding on stereo input. The number of channels
  * must be 2 for this to have any effect.
  **/
  doMidSideStereo?: boolean;
  /**
  * Set to true to enable adaptive switching between mid-side and left-right encoding
  * on stereo input. Set to false to use exhaustive searching. Setting this to true
  * requires {@link EncoderOptions.doMidSideStereo} to also be set to true in
  * order to have any effect.
  */
  looseMidSideStereo?: boolean;
  /**
  * Sets the apodization function(s) the encoder will use when windowing audio data
  * for LPC analysis.
  * @see https://xiph.org/flac/api/group__flac__stream__encoder.html#ga6598f09ac782a1f2a5743ddf247c81c8
  */
  apodization?: string;
  /** Set the maximum LPC order, or 0 to use only the fixed predictors. */
  maxLpcOrder?: number;
  /**
  * Set the precision, in bits, of the quantized linear predictor coefficients,
  * or 0 to let the encoder select it based on the blocksize.
  */
  qlpCoeffPrecision?: number;
  /**
  * Set to false to use only the specified quantized linear predictor
  * coefficient precision, or true to search neighboring precision values
  * and use the best one.
  */
  doQlpCoeffPrecSearch?: boolean;
  /**
  * Set to false to let the encoder estimate the best model order based
  * on the residual signal energy, or true to force the encoder to
  * evaluate all order models and select the best.
  */
  doExhaustiveModelSearch?: boolean;
  /** @see https://xiph.org/flac/api/group__flac__stream__encoder.html#ga31867a9cf8bc7276942381e4a8145969 */
  minResidualPartitionOrder?: number;
  /** @see https://xiph.org/flac/api/group__flac__stream__encoder.html#gac2e2147be6e4edf68e02d011349fa08c */
  maxResidualPartitionOrder?: number;
  /**
  * Set the blocksize to use while encoding. The number of samples to use per frame.
  * Use 0 to let the encoder estimate a blocksize; this is usually best. By default,
  * 0 is used.
  **/
  blocksize?: number;
  /**
  * Set the metadata blocks to be emitted to the stream before encoding. The contents
  * of the metadata blocks cand be modified by the encoder. The objects must be alive
  * until the encoder has finished.
  * @see https://xiph.org/flac/api/group__flac__stream__encoder.html#ga9c1098e664d7997947493901ed869b64
  */
  metadata?: metadata.AnyMetadata[];
}

export interface BaseEncoder {
  /** Gets the number of processed samples */
  readonly processedSamples: number
  /** Gets the state of the encoder */
  getState(): EnumValues<Encoder.State>
}
