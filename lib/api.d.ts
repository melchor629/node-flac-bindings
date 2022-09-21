type Global_Iterator<T> = globalThis.Iterator<T>;

export type EnumValues<T> = T[keyof T];
export type ReverseEnum<T extends { [k in keyof T]: number }> = { [k in EnumValues<T>]: keyof T };
export type PerhapsAsync<T> = Promise<T> | T;

/**
 * Constructs the FLAC Decoder class.
 * @see https://xiph.org/flac/api/group__flac__stream__decoder.html
 */
export class DecoderBuilder {
  /**
   * Gets the state of the decoder.
   */
  getState(): EnumValues<Decoder.State>;
  getMd5Checking(): boolean;

  setOggSerialNumber(value: number): DecoderBuilder;
  setMd5Checking(value: boolean): DecoderBuilder;
  setMetadataRespond(type: metadata.MetadataTypes): DecoderBuilder;
  setMetadataRespondApplication(applicationId: Buffer): DecoderBuilder;
  setMetadataRespondAll(): DecoderBuilder;
  setMetadataIgnore(type: metadata.MetadataTypes): DecoderBuilder;
  setMetadataIgnoreApplication(applicationId: Buffer): DecoderBuilder;
  setMetadataIgnoreAll(): DecoderBuilder;

  /**
   * Builds a {@link Decoder} using a stream input. The decoder can only use **synchronous**
   * methods.
   * @param readCallback Read callback (mandatory)
   * @param seekCallback Seek callback
   * @param tellCallback Tell callback
   * @param lengthCallback Length callback
   * @param eofCallback End Of File callback
   * @param writeCallback Write callback (mandatory)
   * @param metadataCallback Metadata callback
   * @param errorCallback Decoder error callback
   */
  buildWithStream(
    readCallback: Decoder.ReadCallback,
    seekCallback: Decoder.SeekCallback | null,
    tellCallback: Decoder.TellCallback | null,
    lengthCallback: Decoder.LengthCallback | null,
    eofCallback: Decoder.EOFCallback | null,
    writeCallback: Decoder.WriteCallback,
    metadataCallback: Decoder.MetadataCallback | null,
    errorCallback: Decoder.ErrorCallback
  ): Decoder;
  /**
   * Builds a {@link Decoder} using an Ogg stream input. The decoder can only use **synchronous**
   * methods.
   * @param readCallback Read callback (mandatory)
   * @param seekCallback Seek callback
   * @param tellCallback Tell callback
   * @param lengthCallback Length callback
   * @param eofCallback End Of File callback
   * @param writeCallback Write callback (mandatory)
   * @param metadataCallback Metadata callback
   * @param errorCallback Decoder error callback (mndatory)
   */
  buildWithOggStream(
    readCallback: Decoder.ReadCallback,
    seekCallback: Decoder.SeekCallback | null,
    tellCallback: Decoder.TellCallback | null,
    lengthCallback: Decoder.LengthCallback | null,
    eofCallback: Decoder.EOFCallback | null,
    writeCallback: Decoder.WriteCallback,
    metadataCallback: Decoder.MetadataCallback | null,
    errorCallback: Decoder.ErrorCallback
  ): Decoder;
  /**
   * Builds a {@link Decoder} using a `.flac` file from the file system. The decoder can only
   * use **synchronous** methods.
   * @param path Path to the file in the file system
   * @param writeCallback Write callback (mandatory)
   * @param metadataCallback Metadata callback
   * @param errorCallback Decoder error callback (mandatory)
   */
  buildWithFile(
    path: string,
    writeCallback: Decoder.WriteCallback,
    metadataCallback: Decoder.MetadataCallback | null,
    errorCallback: Decoder.ErrorCallback
  ): Decoder;
  /**
   * Builds a {@link Decoder} using an `.ogg` file containing FLAC from the file system. The decoder
   * can only use **synchronous** methods.
   * @param path Path to the file in the file system
   * @param writeCallback Write callback (mandatory)
   * @param metadataCallback Metadata callback
   * @param errorCallback Decoder error callback (mandatory)
   */
  buildWithOggFile(
    path: string,
    writeCallback: Decoder.WriteCallback,
    metadataCallback: Decoder.MetadataCallback | null,
    errorCallback: Decoder.ErrorCallback
  ): Decoder;

  /**
   * Builds a {@link Decoder} using a stream input. The decoder can only use **asynchronous**
   * methods.
   * @param readCallback Read callback (mandatory)
   * @param seekCallback Seek callback
   * @param tellCallback Tell callback
   * @param lengthCallback Length callback
   * @param eofCallback End Of File callback
   * @param writeCallback Write callback (mandatory)
   * @param metadataCallback Metadata callback
   * @param errorCallback Decoder error callback
   */
  buildWithStreamAsync(
    readCallback: Decoder.ReadCallbackAsync,
    seekCallback: Decoder.SeekCallbackAsync | null,
    tellCallback: Decoder.TellCallbackAsync | null,
    lengthCallback: Decoder.LengthCallbackAsync | null,
    eofCallback: Decoder.EOFCallbackAsync | null,
    writeCallback: Decoder.WriteCallbackAsync,
    metadataCallback: Decoder.MetadataCallbackAsync | null,
    errorCallback: Decoder.ErrorCallbackAsync
  ): Promise<Decoder>;
  /**
   * Builds a {@link Decoder} using an Ogg stream input. The decoder can only use **asynchronous**
   * methods.
   * @param readCallback Read callback (mandatory)
   * @param seekCallback Seek callback
   * @param tellCallback Tell callback
   * @param lengthCallback Length callback
   * @param eofCallback End Of File callback
   * @param writeCallback Write callback (mandatory)
   * @param metadataCallback Metadata callback
   * @param errorCallback Decoder error callback (mndatory)
   */
  buildWithOggStreamAsync(
    readCallback: Decoder.ReadCallbackAsync,
    seekCallback: Decoder.SeekCallbackAsync | null,
    tellCallback: Decoder.TellCallbackAsync | null,
    lengthCallback: Decoder.LengthCallbackAsync | null,
    eofCallback: Decoder.EOFCallbackAsync | null,
    writeCallback: Decoder.WriteCallbackAsync,
    metadataCallback: Decoder.MetadataCallbackAsync | null,
    errorCallback: Decoder.ErrorCallbackAsync
  ): Promise<Decoder>;
  /**
   * Builds a {@link Decoder} using a `.flac` file from the file system. The decoder can only
   * use **asynchronous** methods.
   * @param path Path to the file in the file system
   * @param writeCallback Write callback (mandatory)
   * @param metadataCallback Metadata callback
   * @param errorCallback Decoder error callback (mandatory)
   */
  buildWithFileAsync(
    path: string,
    writeCallback: Decoder.WriteCallbackAsync,
    metadataCallback: Decoder.MetadataCallbackAsync | null,
    errorCallback: Decoder.ErrorCallbackAsync
  ): Promise<Decoder>;
  /**
   * Builds a {@link Decoder} using an `.ogg` file containing FLAC from the file system. The decoder
   * can only use **asynchronous** methods.
   * @param path Path to the file in the file system
   * @param writeCallback Write callback (mandatory)
   * @param metadataCallback Metadata callback
   * @param errorCallback Decoder error callback (mandatory)
   */
  buildWithOggFileAsync(
    path: string,
    writeCallback: Decoder.WriteCallbackAsync,
    metadataCallback: Decoder.MetadataCallbackAsync | null,
    errorCallback: Decoder.ErrorCallbackAsync
  ): Promise<Decoder>;
}

/**
 * @see https://xiph.org/flac/api/group__flac__stream__decoder.html
 */
export abstract class Decoder {
  getState(): EnumValues<Decoder.State>;
  getResolvedStateString(): string;
  getMd5Checking(): boolean;
  getTotalSamples(): number;
  getChannels(): number;
  getChannelAssignment(): 0 | 1 | 2 | 3;
  getBitsPerSample(): number;
  getSampleRate(): number;
  getBlocksize(): number;

  finish(): DecoderBuilder | null;
  flush(): boolean;
  reset(): boolean;
  processSingle(): boolean;
  processUntilEndOfStream(): boolean;
  processUntilEndOfMetadata(): boolean;
  skipSingleFrame(): boolean;
  seekAbsolute(position: number | bigint): boolean;
  getDecodePosition(): number | bigint | null;

  finishAsync(): Promise<DecoderBuilder | null>;
  flushAsync(): Promise<boolean>;
  processSingleAsync(): Promise<boolean>;
  processUntilEndOfStreamAsync(): Promise<boolean>;
  processUntilEndOfMetadataAsync(): Promise<boolean>;
  skipSingleFrameAsync(): Promise<boolean>;
  seekAbsoluteAsync(position: number | bigint): Promise<boolean>;
  getDecodePositionAsync(): Promise<number | bigint | null>;

  static readonly State: Decoder.State;
  static readonly StateString: ReverseEnum<Decoder.State>;
  static readonly InitStatus: Decoder.InitStatus;
  static readonly InitStatusString: ReverseEnum<Decoder.InitStatus>;
  static readonly ReadStatus: Decoder.ReadStatus;
  static readonly ReadStatusString: ReverseEnum<Decoder.ReadStatus>;
  static readonly SeekStatus: Decoder.SeekStatus;
  static readonly SeekStatusString: ReverseEnum<Decoder.SeekStatus>;
  static readonly TellStatus: Decoder.TellStatus;
  static readonly TellStatusString: ReverseEnum<Decoder.TellStatus>;
  static readonly LengthStatus: Decoder.LengthStatus;
  static readonly LengthStatusString: ReverseEnum<Decoder.LengthStatus>;
  static readonly WriteStatus: Decoder.WriteStatus;
  static readonly WriteStatusString: ReverseEnum<Decoder.WriteStatus>;
  static readonly ErrorStatus: Decoder.ErrorStatus;
  static readonly ErrorStatusString: ReverseEnum<Decoder.ErrorStatus>;
}


declare namespace Decoder {
  type ReadCallbackReturnType = { bytes: number | bigint; returnValue: EnumValues<ReadStatus> };
  /**
   * Function that will be called when the decoder needs to read more data from the stream. The API
   * expects to fill the buffer, but is valid to partially fill the buffer.
   *  > **Note**: the offset can be a `bigint` if the number cannot be stored in a `number`.
   *  > **Note**: The buffer is valid only inside the callback. Write in it anything and don't try to
   *    use it outside the callback.
   * @returns The number of bytes read into the buffer and the {@link ReadStatus}.
   * @see https://xiph.org/flac/api/group__flac__stream__decoder.html#ga7a5f593b9bc2d163884348b48c4285fd
   */
  type ReadCallback = (buffer: Buffer) => ReadCallbackReturnType;
  /**
   * Function that will be called when the decoder needs to read more data from the stream. The API
   * expects to fill the buffer, but is valid to partially fill the buffer.
   *  > **Note**: the offset can be a `bigint` if the number cannot be stored in a `number`.
   *  > **Note**: The buffer is valid only inside the callback. Write in it anything and don't try to
   *    use it outside the callback.
   * @returns The number of bytes read into the buffer and the {@link ReadStatus}.
   * @see https://xiph.org/flac/api/group__flac__stream__decoder.html#ga7a5f593b9bc2d163884348b48c4285fd
   */
  type ReadCallbackAsync = (buffer: Buffer) => PerhapsAsync<ReadCallbackReturnType>;

  type SeekCallbackReturnType = EnumValues<SeekStatus>;
  /**
   * Function that will be called when the decoder to seek in the stream.
   *  > **Note**: the offset can be a `bigint` if the number cannot be stored in a `number`.
   * @returns The {@link SeekStatus}.
   * @see https://xiph.org/flac/api/group__flac__stream__decoder.html#ga375614289a1b868f1ead7fa70a581171
   */
  type SeekCallback = (offset: number | bigint) => SeekCallbackReturnType;
  /**
   * Function that will be called when the decoder to seek in the stream.
   *  > **Note**: the offset can be a `bigint` if the number cannot be stored in a `number`.
   * @returns The {@link SeekStatus}.
   * @see https://xiph.org/flac/api/group__flac__stream__decoder.html#ga375614289a1b868f1ead7fa70a581171
   */
  type SeekCallbackAsync = (offset: number | bigint) => PerhapsAsync<SeekCallbackReturnType>;

  type TellCallbackReturnType = { offset: number | bigint; returnValue: EnumValues<TellStatus>; };
  /**
   * Function that will be called when the decoder needs to know where the stream is.
   *  > **Note**: the offset can be a `bigint` if the number cannot be stored in a `number`.
   * @param offset You can ignore the offset, or see what is its value (just for fun). Don't expected something meaningful.
   * @returns The position of the stream in bytes and the {@link TellStatus}.
   * @see https://xiph.org/flac/api/group__flac__stream__decoder.html#ga02990309a9d30acc43ba01fe48021e39
   */
  type TellCallback = (offset?: number | bigint) => TellCallbackReturnType;
  /**
   * Function that will be called when the decoder needs to know where the stream is.
   *  > **Note**: the offset can be a `bigint` if the number cannot be stored in a `number`.
   * @param offset You can ignore the offset, or see what is its value (just for fun). Don't expected something meaningful.
   * @returns The position of the stream in bytes and the {@link TellStatus}.
   * @see https://xiph.org/flac/api/group__flac__stream__decoder.html#ga02990309a9d30acc43ba01fe48021e39
   */
  type TellCallbackAsync = (offset?: number | bigint) => PerhapsAsync<TellCallbackReturnType>;

  type LengthCallbackReturnType = { offset: number | bigint; returnValue: EnumValues<LengthStatus>; };
  /**
   * Function that will be called when the decoder needs to know the total length of the stream.
   *  > **Note**: the length can be a `bigint` if the number cannot be stored in a `number`.
   * @param length You can ignore the length, or see what is its value (just for fun). Don't expected something meaningful.
   * @returns The length of the stream in bytes and the {@link LengthStatus}.
   * @see https://xiph.org/flac/api/group__flac__stream__decoder.html#ga02990309a9d30acc43ba01fe48021e39
   */
  type LengthCallback = (length?: number | bigint) => LengthCallbackReturnType;
  /**
   * Function that will be called when the decoder needs to know the total length of the stream.
   *  > **Note**: the length can be a `bigint` if the number cannot be stored in a `number`.
   * @param length You can ignore the length, or see what is its value (just for fun). Don't expected something meaningful.
   * @returns The length of the stream in bytes and the {@link LengthStatus}.
   * @see https://xiph.org/flac/api/group__flac__stream__decoder.html#ga02990309a9d30acc43ba01fe48021e39
   */
  type LengthCallbackAsync = (length?: number | bigint) => PerhapsAsync<LengthCallbackReturnType>;

  type EOFCallbackReturnType = boolean;
  /**
   * Function that will be called when the decoder needs to know if the End of File (or Stream)
   * has been reached.
   * @return `true` if EOF is reached, `false` otherwise.
   */
  type EOFCallback = () => EOFCallbackReturnType;
  /**
   * Function that will be called when the decoder needs to know if the End of File (or Stream)
   * has been reached.
   * @return `true` if EOF is reached, `false` otherwise.
   */
  type EOFCallbackAsync = () => PerhapsAsync<EOFCallbackReturnType>;

  type WriteCallbackReturnType = EnumValues<WriteStatus>;
  /**
   * Function that will be called when the decoder has decoded a frame. Contains information about
   * the frame and the decoded (PCM) data separated in channels.
   *  > **Note**: The buffers are valid only inside the callback. If you need to use them outside the callback,
   *    use `buffers.map(b => Buffer.from(b))` to make a copy of all of them.
   * @param frame The {@link Frame} struct.
   * @param buffers PCM data for each channel ordered by channel assignment.
   * @returns The {@link WriteStatus}.
   */
  type WriteCallback = (frame: Frame, buffers: Buffer[]) => WriteCallbackReturnType;
  /**
   * Function that will be called when the decoder has decoded a frame. Contains information about
   * the frame and the decoded (PCM) data separated in channels.
   *  > **Note**: The buffers are valid only inside the callback. If you need to use them outside the callback,
   *    use `buffers.map(b => Buffer.from(b))` to make a copy of all of them.
   * @param frame The {@link Frame} struct.
   * @param buffers PCM data for each channel ordered by channel assignment.
   * @returns The {@link WriteStatus}.
   */
  type WriteCallbackAsync = (frame: Frame, buffers: Buffer[]) => PerhapsAsync<WriteCallbackReturnType>;

  /**
   * Function that will be called when a metadata block has been read. The metadata object will only
   * be valid inside the callback. If a copy is needed, clone the object with {@link Metadata#clone}.
   */
  type MetadataCallback = (metadata: metadata.AnyMetadata) => void;
  /**
   * Function that will be called when a metadata block has been read. The metadata object will only
   * be valid inside the callback. If a copy is needed, clone the object with {@link Metadata#clone}.
   */
  type MetadataCallbackAsync = (metadata: metadata.AnyMetadata) => PerhapsAsync<void>;

  /**
   * Function that will be called when an error while decoding occurs.
   */
  type ErrorCallback = (error: EnumValues<ErrorStatus>) => void;
  /**
   * Function that will be called when an error while decoding occurs.
   */
  type ErrorCallbackAsync = (error: EnumValues<ErrorStatus>) => PerhapsAsync<void>;

  /**
   * The decoder state.
   * @see https://xiph.org/flac/api/group__flac__stream__decoder.html#ga3adb6891c5871a87cd5bbae6c770ba2d
   */
  interface State {
    SEARCH_FOR_METADATA: 0;
    READ_METADATA: 1;
    SEARCH_FOR_FRAME_SYNC: 2;
    READ_FRAME: 3;
    END_OF_STREAM: 4;
    OGG_ERROR: 5;
    SEEK_ERROR: 6;
    ABORTED: 7;
    MEMORY_ALLOCATION_ERROR: 8;
    UNINITIALIZED: 9;
  }

  /**
   * A list of values that any of the init*() methods can return.
   * @see https://xiph.org/flac/api/group__flac__stream__decoder.html#gaaed54a24ac6310d29c5cafba79759c44
   */
  interface InitStatus {
    OK: 0;
    UNSUPPORTED_CONTAINER: 1;
    INVALID_CALLBACKS: 2;
    MEMORY_ALLOCATION_ERROR: 3;
    ERROR_OPENING_FILE: 4;
    ALREADY_INITIALIZED: 5;
  }

  /**
   * A list of values that can be used to return in the read callback.
   * @see https://xiph.org/flac/api/group__flac__stream__decoder.html#gad793ead451206c64a91dc0b851027b93
   */
  interface ReadStatus {
    CONTINUE: 0;
    END_OF_STREAM: 1;
    ABORT: 2;
  }

  /**
   * A list of values that can be used to return in the seek callback.
   * @see https://xiph.org/flac/api/group__flac__stream__decoder.html#gac8d269e3c7af1a5889d3bd38409ed67d
   */
  interface SeekStatus {
    OK: 0;
    ERROR: 1;
    UNSUPPORTED: 2;
  }

  /**
   * A list of values that can be used to return in the tell callback.
   * @see https://xiph.org/flac/api/group__flac__stream__decoder.html#ga83708207969383bd7b5c1e9148528845
   */
  interface TellStatus {
    OK: 0;
    ERROR: 1;
    UNSUPPORTED: 2;
  }

  /**
   * A list of values that can be used to return in the length callback.
   * @see https://xiph.org/flac/api/group__flac__stream__decoder.html#gad5860157c2bb34501b8b9370472d727a
   */
  interface LengthStatus {
    OK: 0;
    ERROR: 1;
    UNSUPPORTED: 2;
  }

  /**
   * A list of values that can be used to return in the write callback.
   * @see https://xiph.org/flac/api/group__flac__stream__decoder.html#ga73f67eb9e0ab57945afe038751bc62c8
   */
  interface WriteStatus {
    CONTINUE: 0;
    ABORT: 1;
  }

  /**
   * A list of values that can be received in the error callback.
   * @see https://xiph.org/flac/api/group__flac__stream__decoder.html#ga130e70bd9a73d3c2416247a3e5132ecf
   */
  interface ErrorStatus {
    LOST_SYNC: 0;
    BAD_HEADER: 1;
    FRAME_CRC_MISMATCH: 2;
    UNPARSEABLE_STREAM: 3;
    BAD_METADATA: 4;
  }
}


/**
 * Constructs the FLAC Encoder class.
 * @see https://xiph.org/flac/api/group__flac__stream__encoder.html
 */
export class EncoderBuilder {
  /**
   * Gets the state of the encoder.
   */
  getState(): EnumValues<Encoder.State>;
  getVerify(): boolean;
  getStreamableSubset(): boolean;
  getChannels(): number;
  getBitsPerSample(): number;
  getSampleRate(): number;
  getBlocksize(): number;
  getDoMidSideStereo(): boolean;
  getLooseMidSideStereo(): boolean;
  getMaxLpcOrder(): number;
  getQlpCoeffPrecision(): number;
  getDoQlpCoeffPrecSearch(): boolean;
  getDoEscapeCoding(): boolean;
  getDoExhaustiveModelSearch(): boolean;
  getMaxResidualPartitionOrder(): number;
  getMinResidualPartitionOrder(): number;
  getRiceParameterSearchDist(): number;
  getLimitMinBitrate(): boolean;
  getTotalSamplesEstimate(): number | bigint;

  setOggSerialNumber(value: number): EncoderBuilder;
  setCompressionLevel(value: number): EncoderBuilder;
  setMetadata(metadata: metadata.AnyMetadata[]): EncoderBuilder;
  setApodization(apodization: string): EncoderBuilder;
  setVerify(value: boolean): EncoderBuilder;
  setStreamableSubset(value: boolean): EncoderBuilder;
  setChannels(value: number): EncoderBuilder;
  setBitsPerSample(value: number): EncoderBuilder;
  setSampleRate(value: number): EncoderBuilder;
  setBlocksize(value: number): EncoderBuilder;
  setDoMidSideStereo(value: boolean): EncoderBuilder;
  setLooseMidSideStereo(value: boolean): EncoderBuilder;
  setMaxLpcOrder(value: number): EncoderBuilder;
  setQlpCoeffPrecision(value: number): EncoderBuilder;
  setDoQlpCoeffPrecSearch(value: boolean): EncoderBuilder;
  setDoEscapeCoding(value: boolean): EncoderBuilder;
  setDoExhaustiveModelSearch(value: boolean): EncoderBuilder;
  setMaxResidualPartitionOrder(value: number): EncoderBuilder;
  setMinResidualPartitionOrder(value: number): EncoderBuilder;
  setRiceParameterSearchDist(value: number): EncoderBuilder;
  setLimitMinBitrate(value: boolean): EncoderBuilder;
  setTotalSamplesEstimate(value: number | bigint): EncoderBuilder;

  /**
   * Builds a {@link Encoder} to write into a stream. The encoder can only use **synchronous**
   * methods.
   * @param writeCbk Write callback (mandatory)
   * @param seekCbk Seek callback
   * @param tellCbk Tell callback
   * @param metadataCbk Metadata callback
   */
  buildWithStream(
    writeCbk: Encoder.WriteCallback,
    seekCbk?: Encoder.SeekCallback | null,
    tellCbk?: Encoder.TellCallback | null,
    metadataCbk?: Encoder.MetadataCallback | null
  ): Encoder;
  /**
   * Builds a {@link Encoder} to write into an ogg stream. The encoder can only use **synchronous**
   * methods.
   * @param readCbk Read callback (recommended)
   * @param writeCbk Write callback (mandatory)
   * @param seekCbk Seek callback
   * @param tellCbk Tell callback
   * @param metadataCbk Metadata callback
   */
  buildWithOggStream(
    readCbk: Encoder.ReadCallback | null,
    writeCbk: Encoder.WriteCallback,
    seekCbk?: Encoder.SeekCallback | null,
    tellCbk?: Encoder.TellCallback | null,
    metadataCbk?: Encoder.MetadataCallback | null
  ): Encoder;
  /**
   * Builds a {@link Encoder} to write into a `.flac` file in the file system. The encoder can only
   * use **synchronous** methods. Can overwrite existing files.
   * @param file Path in the file system where the output is going to be stored
   * @param progressCbk Progress callback
   */
  buildWithFile(file: string, progressCbk?: Encoder.ProgressCallback | null): Encoder;
  /**
   * Builds a {@link Encoder} to write into an `.ogg` file in the file system. The encoder can only
   * use **synchronous** methods. Can overwrite existing files.
   * @param file Path in the file system where the output is going to be stored
   * @param progressCbk Progress callback
   */
  buildWithOggFile(file: string, progressCbk?: Encoder.ProgressCallback | null): Encoder;

  /**
   * Builds a {@link Encoder} to write into a stream. The encoder can only use **asynchronous**
   * methods.
   * @param writeCbk Write callback (mandatory)
   * @param seekCbk Seek callback
   * @param tellCbk Tell callback
   * @param metadataCbk Metadata callback
   */
  buildWithStreamAsync(
    writeCbk: Encoder.WriteCallbackAsync,
    seekCbk?: Encoder.SeekCallbackAsync,
    tellCbk?: Encoder.TellCallbackAsync,
    metadataCbk?: Encoder.MetadataCallbackAsync
  ): Promise<Encoder>;
  /**
   * Builds a {@link Encoder} to write into an ogg stream. The encoder can only use **asynchronous**
   * methods.
   * @param readCbk Read callback (recommended)
   * @param writeCbk Write callback (mandatory)
   * @param seekCbk Seek callback
   * @param tellCbk Tell callback
   * @param metadataCbk Metadata callback
   */
  buildWithOggStreamAsync(
    readCbk: Encoder.ReadCallbackAsync | null,
    writeCbk: Encoder.WriteCallbackAsync,
    seekCbk: Encoder.SeekCallbackAsync | null | undefined,
    tellCbk: Encoder.TellCallbackAsync | null | undefined,
    metadataCbk: Encoder.MetadataCallbackAsync | null | undefined
  ): Promise<Encoder>;
  /**
   * Builds a {@link Encoder} to write into a `.flac` file in the file system. The encoder can only
   * use **asynchronous** methods. Can overwrite existing files.
   * @param file Path in the file system where the output is going to be stored
   * @param progressCbk Progress callback
   */
  buildWithFileAsync(
    file: string,
    progressCbk: Encoder.ProgressCallbackAsync | null | undefined,
  ): Promise<Encoder>;
  /**
   * Builds a {@link Encoder} to write into an `.ogg` file in the file system. The encoder can only
   * use **asynchronous** methods. Can overwrite existing files.
   * @param file Path in the file system where the output is going to be stored
   * @param progressCbk Progress callback
   */
  buildWithOggFileAsync(
    file: string,
    progressCbk: Encoder.ProgressCallbackAsync | null | undefined
  ): Promise<Encoder>;
}

/**
 * FLAC Encoder class. Use {@link EncoderBuilder} to construct the encoder.
 * @see https://xiph.org/flac/api/group__flac__stream__encoder.html
 */
export abstract class Encoder {
  readonly verify: boolean;
  readonly streamableSubset: boolean;
  readonly channels: number;
  readonly bitsPerSample: number;
  readonly sampleRate: number;
  readonly blocksize: number;
  readonly doMidSideStereo: boolean;
  readonly looseMidSideStereo: boolean;
  readonly maxLpcOrder: number;
  readonly qlpCoeffPrecision: number;
  readonly doQlpCoeffPrecSearch: boolean;
  readonly doEscapeCoding: boolean;
  readonly doExhaustiveModelSearch: boolean;
  readonly maxResidualPartitionOrder: number;
  readonly minResidualPartitionOrder: number;
  readonly riceParameterSearchDist: number;
  readonly limitMinBitrate: boolean;
  readonly totalSamplesEstimate: number | bigint;

  getState(): EnumValues<Encoder.State>;
  getVerifyDecoderState(): EnumValues<Decoder.State>;
  getResolvedStateString(): string;
  getVerifyDecoderErrorStats(): {
      absoluteSample: number | bigint;
    frameBuffer: number;
    channel: number;
    sample: number;
    expected: number;
    got: number;
  } | null;

  finish(): EncoderBuilder | null;
  process(buffers: Buffer[], samples: Number): boolean;
  processInterleaved(buffer: Buffer, samples?: Number | null): boolean;

  finishAsync(): Promise<EncoderBuilder | null>;
  processAsync(buffers: Buffer[], samples: Number): Promise<boolean>;
  processInterleavedAsync(buffer: Buffer, samples?: Number | null): Promise<boolean>;

  static readonly State: Encoder.State;
  static readonly StateString: ReverseEnum<Encoder.State>;
  static readonly InitStatus: Encoder.InitStatus;
  static readonly InitStatusString: ReverseEnum<Encoder.InitStatus>;
  static readonly ReadStatus: Encoder.ReadStatus;
  static readonly ReadStatusString: ReverseEnum<Encoder.ReadStatus>;
  static readonly WriteStatus: Encoder.WriteStatus;
  static readonly WriteStatusString: ReverseEnum<Encoder.WriteStatus>;
  static readonly SeekStatus: Encoder.SeekStatus;
  static readonly SeekStatusString: ReverseEnum<Encoder.SeekStatus>;
  static readonly TellStatus: Encoder.TellStatus;
  static readonly TellStatusString: ReverseEnum<Encoder.TellStatus>;
}


declare namespace Encoder {
  type ReadCallbackReturnType = { bytes: number; returnValue: EnumValues<ReadStatus>; };
  /**
   * Called when the encoder needs to read from the stream. The number of bytes
   * matches the size of the buffer. The callback should try to fill the buffer.
   * @returns The number of bytes read and the {@link ReadStatus}.
   * @see https://xiph.org/flac/api/group__flac__stream__encoder.html#ga97d25c75f49897422d93a9d8405043cd
   */
  type ReadCallback = (buffer: Buffer) => ReadCallbackReturnType;
  /**
   * Called when the encoder needs to read from the stream. The number of bytes
   * matches the size of the buffer. The callback should try to fill the buffer.
   * @returns The number of bytes read and the {@link ReadStatus}.
   * @see https://xiph.org/flac/api/group__flac__stream__encoder.html#ga97d25c75f49897422d93a9d8405043cd
   */
  type ReadCallbackAsync = (buffer: Buffer) => PerhapsAsync<ReadCallbackReturnType>;

  type WriteCallbackReturnType = EnumValues<WriteStatus>;
  /**
   * Called when encoder needs to write to the stream. The number of bytes matches
   * the size of the buffer.
   *  > **Note**: The buffer is valid only inside the callback. If you need to use it outside the callback,
   *    use `Buffer.from(buffer)` to make a copy of it.
   * @returns The {@link WriteStatus}.
   * @see https://xiph.org/flac/api/group__flac__stream__encoder.html#gabf2f9bb39c806111c83dd16936ff6d09
   */
  type WriteCallback = (buffer: Buffer, samples: number, frame: number) => WriteCallbackReturnType;
  /**
   * Called when encoder needs to write to the stream. The number of bytes matches
   * the size of the buffer.
   *  > **Note**: The buffer is valid only inside the callback. If you need to use it outside the callback,
   *    use `Buffer.from(buffer)` to make a copy of it.
   * @returns The {@link WriteStatus}.
   * @see https://xiph.org/flac/api/group__flac__stream__encoder.html#gabf2f9bb39c806111c83dd16936ff6d09
   */
  type WriteCallbackAsync = (buffer: Buffer, samples: number, frame: number) => PerhapsAsync<WriteCallbackReturnType>;

  type SeekCallbackReturnType = EnumValues<SeekStatus>;
  /**
   * Called when the encoder needs to seek to another position in the stream.
   *  > **Note**: the offset can be a `bigint` if the number cannot be stored in a `number`.
   * @returns the {@link SeekStatus}.
   * @see https://xiph.org/flac/api/group__flac__stream__encoder.html#ga3005a69a7883da53262ec8a124d48c9e
   */
  type SeekCallback = (offset: number | bigint) => SeekCallbackReturnType;
  /**
   * Called when the encoder needs to seek to another position in the stream.
   *  > **Note**: the offset can be a `bigint` if the number cannot be stored in a `number`.
   * @returns the {@link SeekStatus}.
   * @see https://xiph.org/flac/api/group__flac__stream__encoder.html#ga3005a69a7883da53262ec8a124d48c9e
   */
  type SeekCallbackAsync = (offset: number | bigint) => PerhapsAsync<SeekCallbackReturnType>;

  type TellCallbackReturnType = { offset: number | bigint; returnValue: EnumValues<TellStatus>; };
  /**
   * Called when the encoder wants to know in which position is the stream.
   *  > **Note**: the offset can be a `bigint` if the number cannot be stored in a `number`.
   * @returns The position in the stream and the {@link TellStatus}.
   * @see https://xiph.org/flac/api/group__flac__stream__encoder.html#ga4cab0b7556d8509a9f74693804c8c86e
   */
  type TellCallback = () => TellCallbackReturnType;
  /**
   * Called when the encoder wants to know in which position is the stream.
   *  > **Note**: the offset can be a `bigint` if the number cannot be stored in a `number`.
   * @returns The position in the stream and the {@link TellStatus}.
   * @see https://xiph.org/flac/api/group__flac__stream__encoder.html#ga4cab0b7556d8509a9f74693804c8c86e
   */
  type TellCallbackAsync = () => PerhapsAsync<TellCallbackReturnType>;

  /**
   * Called when the encoder wrote the `STREAMINFO` block into the stream.
   * @see https://xiph.org/flac/api/group__flac__stream__encoder.html#ga87778e16cdd0834a301ee8d8258cf946
   */
  type MetadataCallback = (metadata: metadata.AnyMetadata) => void;
  /**
   * Called when the encoder wrote the `STREAMINFO` block into the stream.
   * @see https://xiph.org/flac/api/group__flac__stream__encoder.html#ga87778e16cdd0834a301ee8d8258cf946
   */
  type MetadataCallbackAsync = (metadata: metadata.AnyMetadata) => PerhapsAsync<void>;

  /**
   * Called when the encoder has finished to write a frame.
   *  > **Note**: some of the numbers will be `bigint` if the value cannot be stored in a `number`.
   * @see https://xiph.org/flac/api/group__flac__stream__encoder.html#gac65f8ae0583b665933744b60fd5ba0d9
   */
  type ProgressCallback = (bytesWritten: number | bigint, samplesWritten: number | bigint, framesWritten: number, totalFramesEstimate: number) => void;
  /**
   * Called when the encoder has finished to write a frame.
   *  > **Note**: some of the numbers will be `bigint` if the value cannot be stored in a `number`.
   * @see https://xiph.org/flac/api/group__flac__stream__encoder.html#gac65f8ae0583b665933744b60fd5ba0d9
   */
  type ProgressCallbackAsync = (bytesWritten: number | bigint, samplesWritten: number | bigint, framesWritten: number, totalFramesEstimate: number) => PerhapsAsync<void>;

  /**
   * Encoder state.
   * @see https://xiph.org/flac/api/group__flac__stream__encoder.html#gac5e9db4fc32ca2fa74abd9c8a87c02a5
   */
  interface State {
    OK: 0;
    UNINITIALIZED: 1;
    OGG_ERROR: 2;
    VERIFY_DECODER_ERROR: 3;
    VERIFY_MISMATCH_IN_AUDIO_DATA: 4;
    CLIENT_ERROR: 5;
    IO_ERROR: 6;
    FRAMING_ERROR: 7;
    MEMORY_ALLOCATION_ERROR: 8;
  }

  /**
   * List of values that a init call can return.
   * @see https://xiph.org/flac/api/group__flac__stream__encoder.html#ga3bb869620af2b188d77982a5c30b047d
   */
  interface InitStatus {
    OK: 0;
    ENCODER_ERROR: 1;
    UNSUPPORTED_CONTAINER: 2;
    INVALID_CALLBACKS: 3;
    INVALID_NUMBER_OF_CHANNELS: 4;
    INVALID_BITS_PER_SAMPLE: 5;
    INVALID_SAMPLE_RATE: 6;
    INVALID_BLOCK_SIZE: 7;
    INVALID_MAX_LPC_ORDER: 8;
    INVALID_QLP_COEFF_PRECISION: 9;
    BLOCK_SIZE_TOO_SMALL_FOR_LPC_ORDER: 10;
    NOT_STREAMABLE: 11;
    INVALID_METADATA: 12;
    ALREADY_INITIALIZED: 13;
  }

  /**
   * List of values which can be used to return the status of a read.
   * @see https://xiph.org/flac/api/group__flac__stream__encoder.html#ga2e81f007fb0a7414c0bbb453f37ea37f
   */
  interface ReadStatus {
    CONTINUE: 0;
    END_OF_STREAM: 1;
    ABORT: 2;
    UNSUPPORTED: 3;
  }

  /**
   * List of values which can be used to return the status of a write.
   * @see https://xiph.org/flac/api/group__flac__stream__encoder.html#ga3737471fd49730bb8cf9b182bdeda05e
   */
  interface WriteStatus {
    OK: 0,
    FATAL_ERROR: 1,
  }

  /**
   * List of values which can be used to return the status of a seek.
   * @see https://xiph.org/flac/api/group__flac__stream__encoder.html#ga6d5be3489f45fcf0c252022c65d87aca
   */
  interface SeekStatus {
    CONTINUE: 0;
    ERROR: 1;
    UNSUPPORTED: 2;
  }

  /**
   * List of values which can be used to return the status of a tell.
   * @see https://xiph.org/flac/api/group__flac__stream__encoder.html#gab628f63181250eb977a28bf12b7dd9ff
   */
  interface TellStatus {
    CONTINUE: 0;
    ERROR: 1;
    UNSUPPORTED: 2;
  }
}


/** @see https://xiph.org/flac/api/group__flac__format.html */
export namespace format {
  interface MetadataType {
    STREAMINFO: 0;
    PADDING: 1;
    APPLICATION: 2;
    SEEKTABLE: 3;
    VORBIS_COMMENT: 4;
    CUESHEET: 5;
    PICTURE: 6;
    UNDEFINED: 7;
    MAX_METADATA_TYPE: 126;
  }

  interface EntropyCodingMethodType {
    PARTITIONED_RICE: 0;
    PARTITIONED_RICE2: 1;
  }

  interface SubframeType {
    CONSTANT: 0;
    VERBATIM: 1;
    FIXED: 2;
    LPC: 3;
  }

  interface ChannelAssignment {
    INDEPENDENT: 0;
    LEFT_SIDE: 1;
    RIGHT_SIDE: 2;
    MID_SIDE: 3;
  }

  interface FrameNumberType {
    FRAME_NUMBER: 0;
    SAMPLE_NUMBER: 1;
  }

  interface StreamMetadataPictureType {
    OTHER: 0;
    FILE_ICON_STANDARD: 1;
    FILE_ICON: 2;
    FRONT_COVER: 3;
    BACK_COVER: 4;
    LEAFLET_PAGE: 5;
    MEDIA: 6;
    LEAD_ARTIST: 7;
    ARTIST: 8;
    CONDUCTOR: 9;
    BAND: 10;
    COMPOSER: 11;
    LYRICIST: 12;
    RECORDING_LOCATION: 13;
    DURING_RECORDING: 14;
    DURING_PERFORMANCE: 15;
    VIDEO_SCREEN_CAPTURE: 16;
    FISH: 17;
    ILLUSTRATION: 18;
    BAND_LOGOTYPE: 19;
    PUBLISHER_LOGOTYPE: 20;
    UNDEFINED: 21;
  }

  /** @see https://xiph.org/flac/api/group__flac__format.html#ga52e2616f9a0b94881cd7711c18d62a35 */
  const VERSION_STRING: string;
  /** @see https://xiph.org/flac/api/group__flac__format.html#gad5cccab0de3adda58914edf3c31fd64f */
  const VENDOR_STRING: string;
  const STREAM_SYNC: number;
  const STREAM_SYNC_STRING: string;

  const API_SUPPORTS_OGG_FLAC: boolean;

  const MAX_METADATA_TYPE_CODE: number;
  const MIN_BLOCK_SIZE: number;
  const MAX_BLOCK_SIZE: number;
  const SUBSET_MAX_BLOCK_SIZE_48000HZ: number;
  const MAX_CHANNELS: number;
  const MIN_BITS_PER_SAMPLE: number;
  const MAX_BITS_PER_SAMPLE: number;
  const REFERENCE_CODEC_MAX_BITS_PER_SAMPLE: number;
  const MAX_SAMPLE_RATE: number;
  const MAX_LPC_ORDER: number;
  const SUBSET_MAX_LPC_ORDER_48000HZ: number;
  const MIN_QLP_COEFF_PRECISION: number;
  const MAX_QLP_COEFF_PRECISION: number;
  const MAX_FIXED_ORDER: number;
  const MAX_RICE_PARTITION_ORDER: number;
  const SUBSET_MAX_RICE_PARTITION_ORDER: number;

  /** @see https://xiph.org/flac/api/group__flac__format.html#ga985a32bf66e3a69a48e8f9ccd7c5e2e9 */
  function sampleRateIsValid(sampleRate: number): boolean;
  /** @see https://xiph.org/flac/api/group__flac__format.html#ga5370258a7aae32ad18b4c69fbd5e4a36 */
  function blocksizeIsSubset(blockSize: number, sampleRate: number): boolean;
  /** @see https://xiph.org/flac/api/group__flac__format.html#gae305f200f9f4fca80f8ee3d004cf1164 */
  function sampleRateIsSubset(sampleRate: number): boolean;
  /** @see https://xiph.org/flac/api/group__flac__format.html#gae5fb55cd5977ebf178c5b38da831c057 */
  function vorbiscommentEntryNameIsLegal(key: string): boolean;
  /** @see https://xiph.org/flac/api/group__flac__format.html#gad4509984c8a8a0b926a4fb1ba25ec449 */
  function vorbiscommentEntryValueIsLegal(value: string): boolean;
  /** @see https://xiph.org/flac/api/group__flac__format.html#gab98da8754f99fdf7ba0583275b200de3 */
  function vorbiscommentEntryIsLegal(entry: string): boolean;
  /** @see https://xiph.org/flac/api/group__flac__format.html#ga02ed0843553fb8f718fe8e7c54d12244 */
  function seektableIsLegal(seekTable: metadata.SeekTableMetadata): boolean;
  /** @see https://xiph.org/flac/api/group__flac__format.html#ga64dede2811616c7aa41caaed9c855cd4 */
  function seektableSort(seekTable: metadata.SeekTableMetadata): number;
  /** @see https://xiph.org/flac/api/group__flac__format.html#gaa9ed0fa4ed04dbfdaa163d0f5308c080 */
  function cuesheetIsLegal(cueSheet: metadata.CueSheetMetadata): string | null;
  /** @see https://xiph.org/flac/api/group__flac__format.html#ga82ca3ffc97c106c61882134f1a7fb1be */
  function pictureIsLegal(picture: metadata.PictureMetadata): string | null;

  const MetadataType: MetadataType;
  const MetadataTypeString: ReverseEnum<MetadataType>;
  const EntropyCodingMethodType: EntropyCodingMethodType;
  const EntropyCodingMethodTypeString: ReverseEnum<EntropyCodingMethodType>;
  const SubframeType: SubframeType;
  const SubframeTypeString: ReverseEnum<SubframeType>;
  const ChannelAssignment: ChannelAssignment;
  const ChannelAssignmentString: ReverseEnum<ChannelAssignment>;
  const FrameNumberType: FrameNumberType;
  const FrameNumberTypeString: ReverseEnum<FrameNumberType>;
  const PictureType: StreamMetadataPictureType;
  const PictureTypeString: ReverseEnum<StreamMetadataPictureType>;
}


/** @see https://xiph.org/flac/api/group__flac__metadata__object.html */
declare namespace metadata {
  // NOTE: TS has a limitation in the values inside the enum type (see https://github.com/microsoft/TypeScript/issues/43505)
  type UnknownMetadataTypes = 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 | 24 | 25 | 26 | 27 | 28 | 29 | 30 | 31 | 126;
  type MetadataTypes = 0 | 1 | 2 | 3 | 4 | 5 | 6 | UnknownMetadataTypes;
  type AnyMetadata = ApplicationMetadata | CueSheetMetadata | PaddingMetadata | PictureMetadata
    | SeekTableMetadata | StreamInfoMetadata | VorbisCommentMetadata | UnknownMetadata;

  abstract class Metadata<T extends MetadataTypes = MetadataTypes> {
    readonly type: T;
    readonly isLast: boolean;
    readonly length: number;

    /**
     * Clones the metadata object.
     * @returns A copy of the object.
     * @throws Error if there's no enough memory.
     */
    clone<MT extends Metadata<T>>(): MT;

    /**
     * Clones the metadata object.
     * @returns A copy of the object.
     * @throws Error if there's no enough memory.
     */
    clone(): Metadata<T>;

    /**
     * Compares this object with another to see if they are equal.
     * @param other The other object to compare.
     * @returns `true` if they are equal, `false` otherwise.
     * @see https://xiph.org/flac/api/group__flac__metadata__object.html#ga6853bcafe731b1db37105d49f3085349
     */
    isEqual<MT extends Metadata<T>>(other: MT): boolean;

    /**
     * Compares this object with another to see if they are equal.
     * @param other The other object to compare.
     * @returns `true` if they are equal, `false` otherwise.
     * @see https://xiph.org/flac/api/group__flac__metadata__object.html#ga6853bcafe731b1db37105d49f3085349
     */
    isEqual(other: Metadata): false;
  }

  /** @see https://xiph.org/flac/api/structFLAC____StreamMetadata__Application.html */
  class ApplicationMetadata extends Metadata<format.MetadataType['APPLICATION']> {
    id: Buffer; //4 bytes - always
    data: Buffer;
  }

  /** @see https://xiph.org/flac/api/structFLAC____StreamMetadata__CueSheet__Index.html */
  class CueSheetIndex {
    offset: number | bigint;
    number: number;

    constructor(offset?: number | bigint, number?: number);
  }

  /** @see https://xiph.org/flac/api/structFLAC____StreamMetadata__CueSheet__Track.html */
  class CueSheetTrack implements Iterable<CueSheetIndex> {
    offset: number | bigint;
    number: number;
    isrc: string;
    type: 0 | 1;
    preEmphasis: boolean;
    readonly count: number;

    /**
     * Clones the CueSheet Track. If the object cannot be cloned, `null`
     * will be returned.
     */
    clone(): CueSheetTrack | null;

    /**
     * Returns an iterator which will iterate over the {@link indices} list.
     */
    [Symbol.iterator](): Global_Iterator<CueSheetIndex>;
  }

  /** @see https://xiph.org/flac/api/structFLAC____StreamMetadata__CueSheet.html */
  class CueSheetMetadata extends Metadata<format.MetadataType['CUESHEET']> implements Iterable<CueSheetTrack> {
    mediaCatalogNumber: string;
    leadIn: number | bigint;
    isCd: boolean;
    readonly count: number;

    /** @see https://xiph.org/flac/api/group__flac__metadata__object.html#ga484f21de7d533e4825cf807d29ef0204 */
    trackResizeIndices(trackNum: number, newIndicesSize: number): boolean;
    /** @see https://xiph.org/flac/api/group__flac__metadata__object.html#gae8be6ad7b27a18c91eb0b91dc305e433 */
    trackInsertIndex(trackNum: number, indexNum: number, index: CueSheetIndex): boolean;
    /** @see https://xiph.org/flac/api/group__flac__metadata__object.html#gafd40ef6dcc277f99934deee5367cc627 */
    trackInsertBlankIndex(trackNum: number, indexNum: number): boolean;
    /** @see https://xiph.org/flac/api/group__flac__metadata__object.html#ga84584f2244b7d8597b8fec1d81ea5fb8 */
    trackDeleteIndex(trackNum: number, indexNum: number): boolean;
    /** @see https://xiph.org/flac/api/group__flac__metadata__object.html#gac99e38ed08f342665c63913bd0cc33fc */
    resizeTracks(newTrackSize: number): boolean;
    /**
     * > **Note**: the CueSheet Track object is copied, always.
     * @see https://xiph.org/flac/api/group__flac__metadata__object.html#gaa4708c652be442b19227695621b62088
     **/
    setTrack(trackNum: number, track: CueSheetTrack): boolean;
    /**
     * > **Note**: the CueSheet Track object is copied, always.
     * @see https://xiph.org/flac/api/group__flac__metadata__object.html#gaa974947ae8ec1c86cbb155e0af7593e9
     **/
    insertTrack(trackNum: number, track: CueSheetTrack): boolean;
    /** @see https://xiph.org/flac/api/group__flac__metadata__object.html#gab4a8c0855971e650df3331daf84d3fd1 */
    insertBlankTrack(trackNum: number): boolean;
    /** @see https://xiph.org/flac/api/group__flac__metadata__object.html#gae1912dfbc599c79732025fd5a5f279cc */
    deleteTrack(trackNum: number): boolean;
    /**
     * @returns `null` if the CueSheet is legal, a `string` if it is not legal.
     * @see https://xiph.org/flac/api/group__flac__metadata__object.html#ga1a443d9299ce69694ad59bec4519d7b2
     **/
    isLegal(checkCDDASubset: boolean): string | null;
    /**
     * @throws Error if {@link isCd} is `false`.
     * @see https://xiph.org/flac/api/group__flac__metadata__object.html#gaff2f825950b3e4dda4c8ddbf8e2f7ecd
     **/
    calculateCddbId(): number;

    /**
     * Returns an iterator which will iterate over the {@link tracks} list.
     */
    [Symbol.iterator](): Global_Iterator<CueSheetTrack>;
  }

  /** @see https://xiph.org/flac/api/structFLAC____StreamMetadata__Padding.html */
  class PaddingMetadata extends Metadata<format.MetadataType['PADDING']> {
    /**
     * Creates a new Padding block and, optionally, define the size of it
     * @param size Size of the Padding block
     */
    constructor(size?: number);
  }

  /** @see https://xiph.org/flac/api/structFLAC____StreamMetadata__Picture.html */
  class PictureMetadata extends Metadata<format.MetadataType['PICTURE']> {
    pictureType: number;
    mimeType: string;
    description: string;
    width: number;
    height: number;
    depth: number;
    colors: number;
    data: Buffer | null;

    /**
     * @returns `null` if the Picture object is valid, and a string if it is not valid.
     */
    isLegal(): string | null;
  }

  /**
   * If some of the values does not fit in a `number`, `bigint` will be used instead.
   * @see https://xiph.org/flac/api/structFLAC____StreamMetadata__SeekPoint.html
   */
  class SeekPoint {
    sampleNumber: number | bigint;
    streamOffset: number | bigint;
    frameSamples: number;

    constructor(sampleNumber?: number | bigint, streamOffset?: number | bigint, frameSamples?: number | bigint);
  }

  /** @see https://xiph.org/flac/api/structFLAC____StreamMetadata__SeekTable.html */
  class SeekTableMetadata extends Metadata<format.MetadataType['SEEKTABLE']> implements Iterable<SeekPoint> {
    readonly count: number;
    /** @see https://xiph.org/flac/api/group__flac__metadata__object.html#gaffa16bae5e3683c983dc137fd56f0c26 */
    resizePoints(n: number): boolean;
    /** @see https://xiph.org/flac/api/group__flac__metadata__object.html#gaa1a69eb95a3c17aa973466589e85f3c1 */
    setPoint(pos: number, seekPoint: SeekPoint): void;
    /** @see https://xiph.org/flac/api/group__flac__metadata__object.html#ga794109d40ff0065659c005f1cf86d3c9 */
    insertPoint(pos: number, seekPoint: SeekPoint): boolean;
    /** @see https://xiph.org/flac/api/group__flac__metadata__object.html#ga8e42ac803f857eaa7814838c49a15c5f */
    deletePoint(pos: number): boolean;
    /** @see https://xiph.org/flac/api/group__flac__metadata__object.html#gacd3e1b83fabc1dabccb725b2876c8f53 */
    isLegal(): boolean;
    /** @see https://xiph.org/flac/api/group__flac__metadata__object.html#ga6280327ed000ee85846a5533fd40a33b */
    templateAppendPlaceholders(num: number): boolean;
    /** @see https://xiph.org/flac/api/group__flac__metadata__object.html#ga0b3aca4fbebc206cd79f13ac36f653f0 */
    templateAppendPoint(sampleNumber: number | bigint): boolean;
    /** @see https://xiph.org/flac/api/group__flac__metadata__object.html#gac838116fa0e48242651944ab94bab508 */
    templateAppendPoints(sampleNumbers: Array<number | bigint>): boolean;
    /** @see https://xiph.org/flac/api/group__flac__metadata__object.html#ga7dcbd38a3a71a8aa26e93a6992a8f83e */
    templateAppendSpacedPoints(num: number, totalSamples: number | bigint): boolean;
    /** @see https://xiph.org/flac/api/group__flac__metadata__object.html#ga6bce5ee9332ea070d65482a2c1ce1c2d */
    templateAppendSpacedPointsBySamples(samples: number, totalSamples: number | bigint): boolean;
    /** @see https://xiph.org/flac/api/group__flac__metadata__object.html#gafb0449b639ba5c618826d893c2961260 */
    templateSort(compact?: boolean): boolean;

    /**
     * Returns an iterator that will iterate over the {@link points} list.
     */
    [Symbol.iterator](): Global_Iterator<SeekPoint>;
  }

  /** @see https://xiph.org/flac/api/structFLAC____StreamMetadata__StreamInfo.html */
  class StreamInfoMetadata extends Metadata<format.MetadataType['STREAMINFO']> {
    minBlocksize: number;
    maxBlocksize: number;
    minFramesize: number;
    maxFramesize: number;
    channels: number;
    bitsPerSample: number;
    sampleRate: number;
    totalSamples: number | bigint;
    md5sum: Buffer;
  }

  /** @see https://xiph.org/flac/api/structFLAC____StreamMetadata__Unknown.html */
  class UnknownMetadata extends Metadata<UnknownMetadataTypes> { //From 7 to 126
    readonly data: Buffer | null;

    constructor(type?: number);
  }

  /** @see https://xiph.org/flac/api/structFLAC____StreamMetadata__VorbisComment.html */
  class VorbisCommentMetadata extends Metadata<format.MetadataType['VORBIS_COMMENT']> implements Iterable<string> {
    vendorString: string;
    readonly count: number;

    /** @see https://xiph.org/flac/api/group__flac__metadata__object.html#ga829152404c9d160c7bc67699dd7f857e */
    resizeComments(newSize: number): boolean;
    /**
     *  > **Note**: the string is always copied, no extra care is needed.
     * @see https://xiph.org/flac/api/group__flac__metadata__object.html#gaefadb16fe0fff9600beab0edbac8d226
     */
    setComment(pos: number, entry: string): boolean;
    /**
     *  > **Note**: the string is always copied, no extra care is needed.
     * @see https://xiph.org/flac/api/group__flac__metadata__object.html#ga44ec196a99c8cd7d1d50817c8532ddb3
     */
    insertComment(pos: number, entry: string): boolean;
    /**
     *  > **Note**: the string is always copied, no extra care is needed.
     * @see https://xiph.org/flac/api/group__flac__metadata__object.html#ga889b8b9c5bbd1070a1214c3da8b72863
     */
    appendComment(entry: string): boolean;
    /**
     *  > **Note**: the string is always copied, no extra care is needed.
     * @see https://xiph.org/flac/api/group__flac__metadata__object.html#ga0608308e8c4c09aa610747d8dff90a34
     */
    replaceComment(entry: string, replaceAll?: boolean): boolean;
    /** @see https://xiph.org/flac/api/group__flac__metadata__object.html#gafe14896322f7d638f5de0c61addd1dc7 */
    deleteComment(pos: number): boolean;
    /** @see https://xiph.org/flac/api/group__flac__metadata__object.html#ga23d79d11e427e1590f406a7137c8bff2 */
    findEntryFrom(initialPos: number, key: string): number;
    /** @see https://xiph.org/flac/api/group__flac__metadata__object.html#ga017d743b3200a27b8567ef33592224b8 */
    removeEntryMatching(key: string): -1 | 0 | 1;
    /** @see https://xiph.org/flac/api/group__flac__metadata__object.html#ga5a3ff5856098c449622ba850684aec75 */
    removeEntriesMatching(key: string): number;
    /**
     * Easy way to get the value of a tag. It returns only the value of the tag.
     * @param key Tag key to find and retrieve
     * @returns The value of the entry if found, or `null` otherwise.
     */
    get(key: string): string | null;

    /**
     * Returns an iterator that will iterate over the {@link comments} list.
     */
    [Symbol.iterator](): Global_Iterator<string>;
  }
}


declare namespace metadata0 {
  /** @see https://xiph.org/flac/api/group__flac__metadata__level0.html#ga804b42d9da714199b4b383ce51078d51 */
  function getStreaminfo(path: string): metadata.StreamInfoMetadata | null;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level0.html#ga804b42d9da714199b4b383ce51078d51 */
  function getStreaminfoAsync(path: string): Promise<metadata.StreamInfoMetadata | null>;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level0.html#ga1626af09cd39d4fa37d5b46ebe3790fd */
  function getTags(path: string): metadata.VorbisCommentMetadata | null;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level0.html#ga1626af09cd39d4fa37d5b46ebe3790fd */
  function getTagsAsync(path: string): Promise<metadata.StreamInfoMetadata | null>;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level0.html#ga0f47949dca514506718276205a4fae0b */
  function getCuesheet(path: string): metadata.CueSheetMetadata | null;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level0.html#ga0f47949dca514506718276205a4fae0b */
  function getCuesheetAsync(path: string): Promise<metadata.CueSheetMetadata | null>;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level0.html#ga0c9cd22296400c8ce16ee1db011342cb */
  function getPicture(path: string, type: number, maxWidth?: number, maxHeight?: number, maxDepth?: number, maxColors?: number): metadata.PictureMetadata | null;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level0.html#ga0c9cd22296400c8ce16ee1db011342cb */
  function getPictureAsync(path: string, type: number, maxWidth?: number, maxHeight?: number, maxDepth?: number, maxColors?: number): Promise<metadata.PictureMetadata | null>;
}


/**
 * Metadata Level 1 iterator with an iterable interface implemented.
 * @see https://xiph.org/flac/api/group__flac__metadata__level1.html
 */
export class SimpleIterator implements Iterable<metadata.AnyMetadata>, AsyncIterable<metadata.AnyMetadata> {
  /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#gae8fd236fe6049c61f7f3b4a6ecbcd240 */
  status(): EnumValues<SimpleIterator.Status>;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#gaba8daf276fd7da863a2522ac050125fd */
  init(path: string, readOnly?: boolean, preserve?: boolean): void;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#gaba8daf276fd7da863a2522ac050125fd */
  initAsync(path: string, readOnly?: boolean, preserve?: boolean): Promise<void>;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#ga5150ecd8668c610f79192a2838667790 */
  isWritable(): boolean;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#gabb7de0a1067efae353e0792dc6e51905 */
  next(): boolean;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#gabb7de0a1067efae353e0792dc6e51905 */
  nextAsync(): Promise<boolean>;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#ga6db5313b31120b28e210ae721d6525a8 */
  prev(): boolean;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#ga6db5313b31120b28e210ae721d6525a8 */
  prevAsync(): Promise<boolean>;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#ga9eb215059840960de69aa84469ba954f */
  isLast(): boolean;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#gade0a61723420daeb4bc226713671c6f0 */
  getBlockOffset(): number | bigint;
  /**
   * @see https://xiph.org/flac/api/group__flac__metadata__level1.html#ga17b61d17e83432913abf4334d6e0c073
   * @see {@link metadata.MetadataType}
   **/
  getBlockType(): metadata.MetadataTypes;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#ga93ec66e9cfb99f04ce4125b8be906cef */
  getBlockLength(): number;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#gad4fea2d7d98d16e75e6d8260f690a5dc */
  getApplicationId(): Buffer | null;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#gad4fea2d7d98d16e75e6d8260f690a5dc */
  getApplicationIdAsync(): Promise<Buffer | null>;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#ga1b7374cafd886ceb880b050dfa1e387a */
  getBlock(): metadata.AnyMetadata | null;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#ga1b7374cafd886ceb880b050dfa1e387a */
  getBlockAsync(): Promise<metadata.AnyMetadata | null>;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#gae1dd863561606658f88c492682de7b80 */
  setBlock(metadata: metadata.AnyMetadata, pad?: boolean): boolean;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#gae1dd863561606658f88c492682de7b80 */
  setBlockAsync(metadata: metadata.AnyMetadata, pad?: boolean): Promise<boolean>;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#ga7a0c00e93bb37324a20926e92e604102 */
  insertBlockAfter(metadata: metadata.AnyMetadata, pad?: boolean): boolean;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#ga7a0c00e93bb37324a20926e92e604102 */
  insertBlockAfterAsync(metadata: metadata.AnyMetadata, pad?: boolean): Promise<boolean>;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#gac3116c8e6e7f59914ae22c0c4c6b0a23 */
  deleteBlock(pad?: boolean): boolean;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#gac3116c8e6e7f59914ae22c0c4c6b0a23 */
  deleteBlockAsync(pad?: boolean): Promise<boolean>;

  /**
   * Returns an iterator that iterates over the metadata blocks.
   */
  [Symbol.iterator]: () => Global_Iterator<metadata.AnyMetadata>;

  /**
   * Returns an async iterator that iterates over the metadata blocks.
   */
  [Symbol.asyncIterator]: () => AsyncIterator<metadata.AnyMetadata>;

  static Status: SimpleIterator.Status;
  static StatusString: ReverseEnum<SimpleIterator.Status>;
}


declare namespace SimpleIterator {
  /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#gac926e7d2773a05066115cac9048bbec9 */
  interface Status {
    OK: 0;
    ILLEGAL_INPUT: 1;
    ERROR_OPENING_FILE: 2;
    NOT_A_FLAC_FILE: 3;
    NOT_WRITABLE: 4;
    BAD_METADATA: 5;
    READ_ERROR: 6;
    SEEK_ERROR: 7;
    WRITE_ERROR: 8;
    RENAME_ERROR: 9;
    UNLINK_ERROR: 10;
    MEMORY_ALLOCATION_ERROR: 11;
    INTERNAL_ERROR: 12;
  }
}


/**
 * Metadata Level 2 Chain class.
 * @see https://xiph.org/flac/api/group__flac__metadata__level2.html
 */
export class Chain {
  /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga8e74773f8ca2bb2bc0b56a65ca0299f4 */
  status(): EnumValues<Chain.Status>;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga5a4f2056c30f78af5a79f6b64d5bfdcd */
  read(path: string): void;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga5a4f2056c30f78af5a79f6b64d5bfdcd */
  readAsync(path: string): Promise<void>;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga3995010aab28a483ad9905669e5c4954 */
  readOgg(path: string): void;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga3995010aab28a483ad9905669e5c4954 */
  readOggAsync(path: string): Promise<void>;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga595f55b611ed588d4d55a9b2eb9d2add */
  readWithCallbacks(callbacks: Chain.IOCallbacks): Promise<void>;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#gaccc2f991722682d3c31d36f51985066c */
  readOggWithCallbacks(callbacks: Chain.IOCallbacks): Promise<void>;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga46bf9cf7d426078101b9297ba80bb835 */
  write(padding?: boolean, preserve?: boolean): void;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga46bf9cf7d426078101b9297ba80bb835 */
  writeAsync(padding?: boolean, preserve?: boolean): Promise<void>;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga70532b3705294dc891d8db649a4d4843 */
  writeWithCallbacks(callbacks: Chain.IOCallbacks, usePadding?: boolean): Promise<void>;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga72facaa621e8d798036a4a7da3643e41 */
  writeWithCallbacksAndTempFile(usePadding: boolean, callbacks: Chain.IOCallbacks, tempCallbacks: Chain.IOCallbacks): Promise<void>;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga46602f64d423cfe5d5f8a4155f8a97e2 */
  checkIfTempFileIsNeeded(usePadding?: boolean): boolean;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga0a43897914edb751cb87f7e281aff3dc */
  mergePadding(): void;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga82b66fe71c727adb9cf80a1da9834ce5 */
  sortPadding(): void;
  /**
   * Creates an iterator of the chain and returns it.
   * @returns An iterator for this chain
   */
  createIterator(): Iterator;

  static Status: Chain.Status;
  static StatusString: ReverseEnum<Chain.Status>;
}


/**
 * Metadata Level 2 Iterator class which implements the iterable interface.
 * @see https://xiph.org/flac/api/group__flac__metadata__level2.html
 */
export class Iterator implements Iterable<metadata.AnyMetadata> {
  /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga2e93196b17a1c73e949e661e33d7311a */
  init(chain: Chain): void;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga60449d0c1d76a73978159e3aa5e79459 */
  next(): boolean;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#gaa28df1c5aa56726f573f90e4bae2fe50 */
  prev(): boolean;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga83ecb59ffa16bfbb1e286e64f9270de1 */
  getBlockType(): metadata.MetadataTypes;
  /**
   * > **Note**: The metadata is valid while the Chain is still alive.
   * @see https://xiph.org/flac/api/group__flac__metadata__level2.html#gad3e7fbc3b3d9c192a3ac425c7b263641
   **/
  getBlock(): metadata.AnyMetadata;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#gaf61795b21300a2b0c9940c11974aab53 */
  setBlock(metadata: metadata.AnyMetadata): boolean;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#gadf860af967d2ee483be01fc0ed8767a9 */
  deleteBlock(padding?: boolean): boolean;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga8ac45e2df8b6fd6f5db345c4293aa435 */
  insertBlockBefore(metadata: metadata.AnyMetadata): boolean;
  /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga55e53757f91696e2578196a2799fc632 */
  insertBlockAfter(metadata: metadata.AnyMetadata): boolean;

  /**
   * Returns an iterator that will iterate over all the metadata objects.
   */
  [Symbol.iterator]: () => Global_Iterator<metadata.AnyMetadata>;
}


declare namespace Chain {
  /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#gafe2a924893b0800b020bea8160fd4531 */
  interface Status {
    OK: 0;
    ILLEGAL_INPUT: 1;
    ERROR_OPENING_FILE: 2;
    NOT_A_FLAC_FILE: 3;
    NOT_WRITABLE: 4;
    BAD_METADATA: 5;
    READ_ERROR: 6;
    SEEK_ERROR: 7;
    WRITE_ERROR: 8;
    RENAME_ERROR: 9;
    UNLINK_ERROR: 10;
    MEMORY_ALLOCATION_ERROR: 11;
    INTERNAL_ERROR: 12;
    INVALID_CALLBACKS: 13;
    READ_WRITE_MISMATCH: 14;
    WRONG_WRITE_CALL: 15;
  }

  /**
   * JS Friendly interface for the IO Callbacks.
   * @see https://xiph.org/flac/api/structFLAC____IOCallbacks.html
   */
  interface IOCallbacks {
    /**
     * Read operation callback. It must read a multiple of `sizeOfItem` bytes.
     * @param outputBuffer Buffer where to write the data after reading.
     * @param sizeOfItem The size of an item to read in bytes.
     * @param numberOfItems The number of items to read.
     * @returns the number of bytes read.
     */
    read?: (outputBuffer: Buffer, sizeOfItem: number, numberOfItems: number) => PerhapsAsync<number | bigint>;

    /**
     * Write operation callback. It must write a multiple of `sizeOfItem` bytes.
     * @param inputBuffer Buffer with the data to be written.
     * @param sizeOfItem The size of an item to read in bytes.
     * @param numberOfItems The number of items to read.
     * @returns the number of bytes written.
     */
    write?: (inputBuffer: Buffer, sizeOfItem: number, numberOfItems: number) => PerhapsAsync<number | bigint>;

    /**
     * Seek operation callback.
     * @param offset Position offset where to move, relative to `whence`.
     * @param whence Relative mark. 0 (`set`) is relative to the beginning
     * of the stream. 1 (`cur`) is relative to the current position. 2
     * (`end`) is relative to the end of the stream.
     * @returns 0 on success, -1 on error.
     */
    seek?: (offset: number | bigint, whence: 'set' | 'cur' | 'end') => PerhapsAsync<0 | -1>;

    /**
     * Tell operation callback.
     * @returns The current position in bytes of the stream or -1 in case of error.
     */
    tell?: () => PerhapsAsync<number | bigint | -1>;

    /**
     * EOF check callback.
     * @returns `true` if the End Of File is reached, `false` otherwise.
     */
    eof?: () => PerhapsAsync<boolean>;

    /**
     * Close operation callback.
     * @returns 0 on success, -1 on error.
     */
    close: () => PerhapsAsync<0 | -1>;
  }
}


declare namespace fns {
  interface ZipAudioOptions {
    /** The number of samples in the buffers. */
    samples: number | bigint;
    /** An array of buffers (its length determines the number of channels) */
    buffers: Buffer[];
    /** The input bytes per sample (by default 4) */
    inBps?: 1 | 2 | 3 | 4;
    /** The output bytes per sample (by default 4) */
    outBps?: 1 | 2 | 3 | 4;
  }

  /**
   * Zips non-interleaved PCM audio (one buffer per channel) into an interleaved PCM audio
   * (one buffer for all channels), and optionally converting from one sample format to
   * another. The `samples` option must be defined in order to determine how many of
   * them there are. The channels is determined by the number of buffers inside `buffers`.
   * @param opts The parameters to the function.
   * @see ZipAudioOptions The options interface.
   * @returns An interleaved PCM audio buffer.
   */
  function zipAudio(opts: ZipAudioOptions): Buffer;

  interface UnzipAudioOptions {
    /** The interleaved PCM buffer */
    buffer: Buffer;
    /** The number of channels that has the interleaved audio */
    channels: number | bigint;
    /**
     * The number of samples in the buffers (by default buffer.byteLength / inBps` /
     * `channels`).
     */
    samples?: number | bigint;
    /** The input bytes per sample (by default 4) */
    inBps?: 1 | 2 | 3 | 4;
    /** The output bytes per sample (by default 4) */
    outBps?: 1 | 2 | 3 | 4;
  }

  /**
   * Unzips interleaved PCM audio into a non interleaved array of PCM audio (one buffer
   * per channel), and optionally converting from one sample format to another.
   * @param opts The parameters to the function
   * @see UnzipAudioOptions The options interface.
   * @returns A non interleaved array of PCM audio buffers.
   */
  function unzipAudio(opts: UnzipAudioOptions): Buffer[];

  interface ConvertSampleFormatOptions {
    /** The buffer to convert from one sample format to another */
    buffer: Buffer;
    /**
     * The number of samples in the buffers (by default buffer.byteLength / inBps`). If
     * input buffer is interleaved, count the channels as samples.
     */
    samples?: number | bigint;
    /** The input bytes per sample (by default 4) */
    inBps?: 1 | 2 | 3 | 4;
    /** The output bytes per sample (by default 4) */
    outBps?: 1 | 2 | 3 | 4;
  }

  /**
   * Converts some PCM audio from one sample format to another. The number of samples is
   * determined by `buffer.byteLength / inBps` if `samples` is not defined.
   */
  function convertSampleFormat(opts: ConvertSampleFormatOptions): Buffer;
}


/** @see https://xiph.org/flac/api/structFLAC____FrameHeader.html */
export interface Header {
  blocksize: number;
  sampleRate: number;
  channels: number;
  channelAssignment: 0 | 1 | 2 | 3;
  bitsPerSample: number;
  crc: number;
  frameNumber?: number;
  sampleNumber?: number;
}

/** @see https://xiph.org/flac/api/structFLAC____FrameFooter.html */
export interface Footer {
  crc: number;
}

/** @see https://xiph.org/flac/api/structFLAC____Subframe.html */
export interface SubFrame<T extends 0 | 1 | 2 | 3 = 0 | 1 | 2 | 3> {
  type: T;
  wastedBits: number;
}

/** @see https://xiph.org/flac/api/structFLAC____Subframe__Constant.html */
export interface ConstantSubFrame extends SubFrame<0> {
  value: number;
}

/** @see https://xiph.org/flac/api/structFLAC____Subframe__Verbatim.html */
export interface VerbatimSubFrame extends SubFrame<1> {
  data: Buffer;
}

/** @see https://xiph.org/flac/api/structFLAC____Subframe__Fixed.html */
export interface FixedSubFrame extends SubFrame<2> {
  entropyCodingMethod: unknown;
  order: number;
  warmup: number[];
  residual: Buffer;
}

/** @see https://xiph.org/flac/api/structFLAC____Subframe__LPC.html */
export interface LPCSubFrame extends SubFrame<3> {
  entropyCodingMethod: unknown;
  order: number;
  qlpCoeffPrecision: number;
  quantizationLevel: number;
  warmup: number[];
  qlpCoeff: number[];
  residual: Buffer;
}

/** @see https://xiph.org/flac/api/structFLAC____Frame.html */
export interface Frame {
  footer: Footer;
  header: Header;
  subframes: Array<ConstantSubFrame | VerbatimSubFrame | FixedSubFrame | LPCSubFrame>;
}


export function _testAsync(
  mode: 'reject' | 'exception' | 'resolve',
  progress: (char: string) => Promise<void> | void,
): Promise<true>;

export const napiVersion: number;
