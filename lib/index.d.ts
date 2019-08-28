import { Transform, Writable, Readable } from 'stream';

type ReverseEnum<T> = { [index: number]: keyof T | undefined };
type Global_Iterator<T> = Iterator<T>; //Avoid type shadowing (in TS there's nothing like global:: or :: to refer global namespace)
type AsyncCallback<ReturnType> = (error: any, result?: ReturnType) => void;

declare namespace api {
    /**
     * @see https://xiph.org/flac/api/group__flac__stream__decoder.html
     */
    class Decoder {
        /** Creates a new instance of the decoder. */
        constructor();
        setOggSerialNumber(value: number): void;
        setMd5Checking(value: boolean): void;
        setMetadataRespond(type: number): void;
        setMetadataRespondApplication(applicationId: Buffer): boolean;
        setMetadataRespondAll(): boolean;
        setMetadataIgnore(type: number): void;
        setMetadataIgnoreApplication(applicationId: Buffer): boolean;
        setMetadataIgnoreAll(): boolean;
        getState(): number;
        getResolvedStateString(): string;
        getMd5Checking(): boolean;
        getTotalSamples(): number;
        getChannels(): number;
        getChannelAssignment(): 0 | 1 | 2 | 3;
        getBitsPerSample(): number;
        getSampleRate(): number;
        getBlocksize(): number;
        getDecodePosition(): number | bigint;
        initStream(
            readCallback: Decoder.ReadCallback,
            seekCallback: Decoder.SeekCallback | null,
            tellCallback: Decoder.TellCallback | null,
            lengthCallback: Decoder.LengthCallback | null,
            eofCallback: Decoder.EOFCallback | null,
            writeCallback: Decoder.WriteCallback,
            metadataCallback: Decoder.MetadataCallback | null,
            errorCallback: Decoder.ErrorCallback
        ): number;
        initOggStream(
            readCallback: Decoder.ReadCallback,
            seekCallback: Decoder.SeekCallback | null,
            tellCallback: Decoder.TellCallback | null,
            lengthCallback: Decoder.LengthCallback | null,
            eofCallback: Decoder.EOFCallback | null,
            writeCallback: Decoder.WriteCallback,
            metadataCallback: Decoder.MetadataCallback | null,
            errorCallback: Decoder.ErrorCallback
        ): number;
        initFile(
            path: string,
            writeCallback: Decoder.WriteCallback,
            metadataCallback: Decoder.MetadataCallback | null,
            errorCallback: Decoder.ErrorCallback
        ): number;
        initOggFile(
            path: string,
            writeCallback: Decoder.WriteCallback,
            metadataCallback: Decoder.MetadataCallback | null,
            errorCallback: Decoder.ErrorCallback
        ): number;
        finish(): boolean;
        flush(): boolean;
        reset(): boolean;
        processSingle(): boolean;
        processUntilEndOfStream(): boolean;
        processUntilEndOfMetadata(): boolean;
        skipSingleFrame(): boolean;
        seekAbsolute(position: number | bigint): boolean;

        finishAsync(): Promise<boolean>;
        finishAsync(callback: AsyncCallback<boolean>): void;
        flushAsync(): Promise<boolean>;
        flushAsync(callback: AsyncCallback<boolean>): void;
        processSingleAsync(): Promise<boolean>;
        processSingleAsync(callback: AsyncCallback<boolean>): void;
        processUntilEndOfStreamAsync(): Promise<boolean>;
        processUntilEndOfStreamAsync(callback: AsyncCallback<boolean>): void;
        processUntilEndOfMetadataAsync(): Promise<boolean>;
        processUntilEndOfMetadataAsync(callback: AsyncCallback<boolean>): void;
        skipSingleFrameAsync(): Promise<boolean>;
        skipSingleFrameAsync(callback: AsyncCallback<boolean>): void;
        seekAbsoluteAsync(position: number | bigint): Promise<boolean>;
        seekAbsoluteAsync(position: number | bigint, callback: AsyncCallback<boolean>): void;
        initStreamAsync(
            readCallback: Decoder.ReadCallbackAsync,
            seekCallback: Decoder.SeekCallbackAsync | null,
            tellCallback: Decoder.TellCallbackAsync | null,
            lengthCallback: Decoder.LengthCallbackAsync | null,
            eofCallback: Decoder.EOFCallbackAsync | null,
            writeCallback: Decoder.WriteCallbackAsync,
            metadataCallback: Decoder.MetadataCallbackAsync | null,
            errorCallback: Decoder.ErrorCallbackAsync
        ): Promise<true>;
        initStreamAsync(
            readCallback: Decoder.ReadCallbackAsync,
            seekCallback: Decoder.SeekCallbackAsync | null,
            tellCallback: Decoder.TellCallbackAsync | null,
            lengthCallback: Decoder.LengthCallbackAsync | null,
            eofCallback: Decoder.EOFCallbackAsync | null,
            writeCallback: Decoder.WriteCallbackAsync,
            metadataCallback: Decoder.MetadataCallbackAsync | null,
            errorCallback: Decoder.ErrorCallbackAsync,
            callback: AsyncCallback<true>
        ): void;
        initOggStreamAsync(
            readCallback: Decoder.ReadCallbackAsync,
            seekCallback: Decoder.SeekCallbackAsync | null,
            tellCallback: Decoder.TellCallbackAsync | null,
            lengthCallback: Decoder.LengthCallbackAsync | null,
            eofCallback: Decoder.EOFCallbackAsync | null,
            writeCallback: Decoder.WriteCallbackAsync,
            metadataCallback: Decoder.MetadataCallbackAsync | null,
            errorCallback: Decoder.ErrorCallbackAsync
        ): Promise<true>;
        initOggStreamAsync(
            readCallback: Decoder.ReadCallbackAsync,
            seekCallback: Decoder.SeekCallbackAsync | null,
            tellCallback: Decoder.TellCallbackAsync | null,
            lengthCallback: Decoder.LengthCallbackAsync | null,
            eofCallback: Decoder.EOFCallbackAsync | null,
            writeCallback: Decoder.WriteCallbackAsync,
            metadataCallback: Decoder.MetadataCallbackAsync | null,
            errorCallback: Decoder.ErrorCallbackAsync,
            callback: AsyncCallback<true>
        ): void;
        initFileAsync(
            path: string,
            writeCallback: Decoder.WriteCallbackAsync,
            metadataCallback: Decoder.MetadataCallbackAsync | null,
            errorCallback: Decoder.ErrorCallbackAsync
        ): Promise<true>;
        initFileAsync(
            path: string,
            writeCallback: Decoder.WriteCallbackAsync,
            metadataCallback: Decoder.MetadataCallbackAsync | null,
            errorCallback: Decoder.ErrorCallbackAsync,
            callback: AsyncCallback<true>
        ): void;
        initOggFileAsync(
            path: string,
            writeCallback: Decoder.WriteCallbackAsync,
            metadataCallback: Decoder.MetadataCallbackAsync | null,
            errorCallback: Decoder.ErrorCallbackAsync
        ): Promise<true>;
        initOggFileAsync(
            path: string,
            writeCallback: Decoder.WriteCallbackAsync,
            metadataCallback: Decoder.MetadataCallbackAsync | null,
            errorCallback: Decoder.ErrorCallbackAsync,
            callback: AsyncCallback<true>
        ): void;

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


    namespace Decoder {
        /**
         * Function that will be called when the decoder needs to read more data from the stream. The API
         * expects to fill the buffer, but is valid to partially fill the buffer.
         *  > **Note**: the offset can be a `bigint` if the number cannot be stored in a `number`.
         *  > **Note**: The buffer is valid only inside the callback. Write in it anything and don't try to
         *    use it outside the callback.
         * @returns The number of bytes read into the buffer and the {@link ReadStatus}.
         * @see https://xiph.org/flac/api/group__flac__stream__decoder.html#ga7a5f593b9bc2d163884348b48c4285fd
         */
        type ReadCallback = (buffer: Buffer) => { bytes: number | bigint; returnValue: number; };
        type ReadCallbackAsync = (buffer: Buffer) => Promise<{ bytes: number | bigint; returnValue: number; }>;
        /**
         * Function that will be called when the decoder to seek in the stream.
         *  > **Note**: the offset can be a `bigint` if the number cannot be stored in a `number`.
         * @returns The {@link SeekStatus}.
         * @see https://xiph.org/flac/api/group__flac__stream__decoder.html#ga375614289a1b868f1ead7fa70a581171
         */
        type SeekCallback = (offset: number | bigint) => number;
        type SeekCallbackAsync = (offset: number | bigint) => Promise<number>;
        /**
         * Function that will be called when the decoder needs to know where the stream is.
         *  > **Note**: the offset can be a `bigint` if the number cannot be stored in a `number`.
         * @param offset You can ignore the offset, or see what is its value (just for fun). Don't expected something meaningful.
         * @returns The position of the stream in bytes and the {@link TellStatus}.
         * @see https://xiph.org/flac/api/group__flac__stream__decoder.html#ga02990309a9d30acc43ba01fe48021e39
         */
        type TellCallback = (offset?: number | bigint) => { offset: number | bigint; returnValue: number; };
        type TellCallbackAsync = (offset?: number | bigint) => Promise<{ offset: number | bigint; returnValue: number; }>;
        /**
         * Function that will be called when the decoder needs to know the total length of the stream.
         *  > **Note**: the length can be a `bigint` if the number cannot be stored in a `number`.
         * @param length You can ignore the length, or see what is its value (just for fun). Don't expected something meaningful.
         * @returns The length of the stream in bytes and the {@link LengthStatus}.
         * @see https://xiph.org/flac/api/group__flac__stream__decoder.html#ga02990309a9d30acc43ba01fe48021e39
         */
        type LengthCallback = (length?: number | bigint) => { length: number | bigint; returnValue: number };
        type LengthCallbackAsync = (length?: number | bigint) => Promise<{ length: number | bigint; returnValue: number }>;
        /**
         * Function that will be called when the decoder needs to know if the End of File (or Stream)
         * has been reached.
         * @return `true` if EOF is reached, `false` otherwise.
         */
        type EOFCallback = () => boolean;
        type EOFCallbackAsync = () => Promise<boolean>;
        /**
         * Function that will be called when the decoder has decoded a frame. Contains information about
         * the frame and the decoded (PCM) data separated in channels.
         *  > **Note**: The buffers are valid only inside the callback. If you need to use them outside the callback,
         *    use `buffers.map(b => Buffer.from(b))` to make a copy of all of them.
         * @param frame The {@link Frame} struct.
         * @param buffers PCM data for each channel ordered by channel assignment.
         * @returns The {@link WriteStatus}.
         */
        type WriteCallback = (frame: Frame, buffers: Buffer[]) => number;
        type WriteCallbackAsync = (frame: Frame, buffers: Buffer[]) => Promise<number>;
        /**
         * Function that will be called when a metadata block has been read. The metadata object will only
         * be valid inside the callback. If a copy is needed, clone the object with {@link Metadata#clone}.
         */
        type MetadataCallback = (metadata: metadata.Metadata) => void;
        type MetadataCallbackAsync = (metadata: metadata.Metadata) => Promise<void>;
        /**
         * Function that will be called when an error while decoding occurs.
         */
        type ErrorCallback = (error: number) => void;
        type ErrorCallbackAsync = (error: number) => Promise<void>;

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
            DECODER_ABORTED: 7;
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
            ALREADY_INITIALIZED: 5
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
        }
    }


    /**
     * FLAC Encoder class.
     * @see https://xiph.org/flac/api/group__flac__stream__encoder.html
     */
    class Encoder {
        /** Creates a new instance of the encoder. */
        constructor();
        setOggSerialNumber(value: number): void;
        setVerify(value: boolean): void;
        setStreamableSubset(value: boolean): void;
        setChannels(value: number): void;
        setBitsPerSample(value: number): void;
        setSampleRate(value: number): void;
        setCompressionLevel(value: number): void;
        setBlocksize(value: number): void;
        setDoMidSideStereo(value: boolean): void;
        setLooseMidSideStereo(value: boolean): void;
        setApodization(value: string): void;
        setMaxLpcOrder(value: number): void;
        setQlpCoeffPrecision(value: number): void;
        setDoQlpCoeffPrecSearch(value: boolean): void;
        setDoEscapeCoding(value: boolean): void;
        setDoExhaustiveModelSearch(value: boolean): void;
        setMinResidualPartitionOrder(value: number): void;
        setMaxResidualPartitionOrder(value: number): void;
        setRiceParameterSearchDist(value: number): void;
        setTotalSamplesEstimate(value: number | bigint): void;
        setMetadata(metadata: metadata.Metadata[]): void;
        getState(): number;
        getVerifyDecoderState(): number;
        getResolvedStateString(): string;
        getVerifyDecoderErrorStats(): {
            absoluteSample: number | bigint;
            frameBuffer: number;
            channel: number;
            sample: number;
            expected: number;
            got: number;
        };
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
        getMinResidualPartitionOrder(): number;
        getMaxResidualPartitionOrder(): number;
        getRiceParameterSearchDist(): number;
        getTotalSamplesEstimate(): number | bigint;
        initStream(
            writeCbk: Encoder.WriteCallback,
            seekCbk?: Encoder.SeekCallback | null,
            tellCbk?: Encoder.TellCallback | null,
            metadataCbk?: Encoder.MetadataCallback | null
        ): number;
        initOggStream(
            readCbk: Encoder.ReadCallback | null,
            writeCbk: Encoder.WriteCallback,
            seekCbk?: Encoder.SeekCallback | null,
            tellCbk?: Encoder.TellCallback | null,
            metadataCbk?: Encoder.MetadataCallback | null
        ): number;
        initFile(file: string, progressCbk?: Encoder.ProgressCallback | null): number;
        initOggFile(file: string, progressCbk?: Encoder.ProgressCallback | null): number;
        finish(): boolean;
        process(buffers: Buffer[], samples: Number): boolean;
        processInterleaved(buffer: Buffer, samples?: Number | null): boolean;

        finishAsync(): Promise<boolean>;
        finishAsync(callback: AsyncCallback<boolean>): void;
        processAsync(buffers: Buffer[], samples?: Number | null): Promise<boolean>;
        processAsync(buffers: Buffer[], callback: AsyncCallback<boolean>): void;
        processAsync(buffers: Buffer[], samples: Number | null, callback: AsyncCallback<boolean>): void;
        processInterleavedAsync(buffer: Buffer, samples: Number): Promise<boolean>;
        processInterleavedAsync(buffer: Buffer, samples: Number, callback: AsyncCallback<boolean>): void;
        initStreamAsync(
            writeCbk: Encoder.WriteCallbackAsync,
            seekCbk: Encoder.SeekCallbackAsync | null | undefined,
            tellCbk: Encoder.TellCallbackAsync | null | undefined,
            metadataCbk: Encoder.MetadataCallbackAsync | null | undefined,
            callback: AsyncCallback<true>
        ): void;
        initStreamAsync(
            writeCbk: Encoder.WriteCallbackAsync,
            seekCbk?: Encoder.SeekCallbackAsync,
            tellCbk?: Encoder.TellCallbackAsync,
            metadataCbk?: Encoder.MetadataCallbackAsync
        ): Promise<true>;
        initOggStreamAsync(
            readCbk: Encoder.ReadCallbackAsync | null,
            writeCbk: Encoder.WriteCallbackAsync,
            seekCbk: Encoder.SeekCallbackAsync | null | undefined,
            tellCbk: Encoder.TellCallbackAsync | null | undefined,
            metadataCbk: Encoder.MetadataCallbackAsync | null | undefined,
            callback: AsyncCallback<true>
        ): void;
        initOggStreamAsync(
            readCbk: Encoder.ReadCallbackAsync | null,
            writeCbk: Encoder.WriteCallbackAsync,
            seekCbk: Encoder.SeekCallbackAsync | null | undefined,
            tellCbk: Encoder.TellCallbackAsync | null | undefined,
            metadataCbk: Encoder.MetadataCallbackAsync | null | undefined
        ): Promise<true>;
        initFileAsync(
            file: string,
            progressCbk: Encoder.ProgressCallbackAsync | null | undefined,
            callback: AsyncCallback<true>
        ): void;
        initFileAsync(
            file: string,
            progressCbk: Encoder.ProgressCallbackAsync | null | undefined,
        ): Promise<true>;
        initOggFileAsync(
            file: string,
            progressCbk: Encoder.ProgressCallbackAsync | null | undefined,
            callback: AsyncCallback<true>
        ): void;
        initOggFileAsync(
            file: string,
            progressCbk: Encoder.ProgressCallbackAsync | null | undefined
        ): Promise<true>;

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


    namespace Encoder {
        /**
         * Called when the encoder needs to read from the stream. The number of bytes
         * matches the size of the buffer. The callback should try to fill the buffer.
         * @returns The number of bytes read and the {@link ReadStatus}.
         * @see https://xiph.org/flac/api/group__flac__stream__encoder.html#ga97d25c75f49897422d93a9d8405043cd
         */
        type ReadCallback = (buffer: Buffer) => { bytes: number; returnValue: ReadStatus; };
        type ReadCallbackAsync = (buffer: Buffer) => Promise<{ bytes: number; returnValue: ReadStatus; }>;
        /**
         * Called when encoder needs to write to the stream. The number of bytes matches
         * the size of the buffer.
         *  > **Note**: The buffer is valid only inside the callback. If you need to use it outside the callback,
         *    use `Buffer.from(buffer)` to make a copy of it.
         * @returns The {@link WriteStatus}.
         * @see https://xiph.org/flac/api/group__flac__stream__encoder.html#gabf2f9bb39c806111c83dd16936ff6d09
         */
        type WriteCallback = (buffer: Buffer, samples: number, frame: number) => number;
        type WriteCallbackAsync = (buffer: Buffer, samples: number, frame: number) => Promise<number>;
        /**
         * Called when the encoder needs to seek to another position in the stream.
         *  > **Note**: the offset can be a `bigint` if the number cannot be stored in a `number`.
         * @returns the {@link SeekStatus}.
         * @see https://xiph.org/flac/api/group__flac__stream__encoder.html#ga3005a69a7883da53262ec8a124d48c9e
         */
        type SeekCallback = (offset: number | bigint) => number;
        type SeekCallbackAsync = (offset: number | bigint) => Promise<number>;
        /**
         * Called when the encoder wants to know in which position is the stream.
         *  > **Note**: the offset can be a `bigint` if the number cannot be stored in a `number`.
         * @returns The position in the stream and the {@link TellStatus}.
         * @see https://xiph.org/flac/api/group__flac__stream__encoder.html#ga4cab0b7556d8509a9f74693804c8c86e
         */
        type TellCallback = () => { offset: number | bigint; returnValue: number; };
        type TellCallbackAsync = () => Promise<{ offset: number | bigint; returnValue: number; }>;
        /**
         * Called when the encoder wrote the `STREAMINFO` block into the stream.
         * @see https://xiph.org/flac/api/group__flac__stream__encoder.html#ga87778e16cdd0834a301ee8d8258cf946
         */
        type MetadataCallback = (metadata: metadata.Metadata) => void;
        type MetadataCallbackAsync = (metadata: metadata.Metadata) => Promise<void>;
        /**
         * Called when the encoder has finished to write a frame.
         *  > **Note**: some of the numbers will be `bigint` if the value cannot be stored in a `number`.
         * @see https://xiph.org/flac/api/group__flac__stream__encoder.html#gac65f8ae0583b665933744b60fd5ba0d9
         */
        type ProgressCallback = (bytesWritten: number | bigint, samplesWritten: number | bigint, framesWritten: number, totalFramesEstimate: number) => void;
        type ProgressCallbackAsync = (bytesWritten: number | bigint, samplesWritten: number | bigint, framesWritten: number, totalFramesEstimate: number) => Promise<void>;

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
            CONTINUE: 0,
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
    namespace format {
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
        const FLAC__VERSION_STRING: string;
        /** @see https://xiph.org/flac/api/group__flac__format.html#gad5cccab0de3adda58914edf3c31fd64f */
        const FLAC__VENDOR_STRING: string;

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
        function cuesheetIsLegal(cueSheet: metadata.CueSheetMetadata): true | string;
        /** @see https://xiph.org/flac/api/group__flac__format.html#ga82ca3ffc97c106c61882134f1a7fb1be */
        function pictureIsLegal(picture: metadata.PictureMetadata): true | string;

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
    namespace metadata {
        type MetadataTypes = 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7;

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
        class ApplicationMetadata extends Metadata<2> {
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
            readonly indices: CueSheetIndex[];

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
        class CueSheetMetadata extends Metadata<5> implements Iterable<CueSheetTrack> {
            mediaCatalogNumber: string;
            leadIn: number | bigint;
            isCd: boolean;
            readonly tracks: CueSheetTrack[];

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
             * @returns `true` if the CueSheet is legal, a `string` if it is not legal.
             * @see https://xiph.org/flac/api/group__flac__metadata__object.html#ga1a443d9299ce69694ad59bec4519d7b2
             **/
            isLegal(checkCDDASubset: boolean): true | string;
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
        class PaddingMetadata extends Metadata<1> {
            /**
             * Creates a new Padding block and, optionally, define the size of it
             * @param size Size of the Padding block
             */
            constructor(size?: number);
        }

        /** @see https://xiph.org/flac/api/structFLAC____StreamMetadata__Picture.html */
        class PictureMetadata extends Metadata<6> {
            pictureType: number;
            mimeType: string;
            description: string;
            width: number;
            height: number;
            depth: number;
            colors: number;
            data: Buffer;

            /**
             * @returns `true` if the Picture object is valid, and a string if it is not valid.
             */
            isLegal(): true | string;
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
        class SeekTableMetadata extends Metadata<3> implements Iterable<SeekPoint> {
            readonly points: SeekPoint[];

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
        class StreamInfoMetadata extends Metadata<0> {
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
        class UnknownMetadata extends Metadata { //From 7 to 126
            readonly data: Buffer;

            constructor(type?: number);
        }

        /** @see https://xiph.org/flac/api/structFLAC____StreamMetadata__VorbisComment.html */
        class VorbisCommentMetadata extends Metadata<4> implements Iterable<string> {
            vendorString: string;
            readonly comments: string[];

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
            removeEntryMatching(key: string): -1 | 0 | 1;
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


    namespace metadata0 {
        /** @see https://xiph.org/flac/api/group__flac__metadata__level0.html#ga804b42d9da714199b4b383ce51078d51 */
        function getStreaminfo(path: string): metadata.StreamInfoMetadata | false;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level0.html#ga804b42d9da714199b4b383ce51078d51 */
        function getStreaminfoAsync(path: string, callback: AsyncCallback<metadata.StreamInfoMetadata>): void;
        function getStreaminfoAsync(path: string): Promise<metadata.StreamInfoMetadata>;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level0.html#ga1626af09cd39d4fa37d5b46ebe3790fd */
        function getTags(path: string): metadata.VorbisCommentMetadata | false;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level0.html#ga1626af09cd39d4fa37d5b46ebe3790fd */
        function getTagsAsync(path: string, callback: AsyncCallback<metadata.StreamInfoMetadata>): void;
        function getTagsAsync(path: string): Promise<metadata.StreamInfoMetadata>;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level0.html#ga0f47949dca514506718276205a4fae0b */
        function getCuesheet(path: string): metadata.CueSheetMetadata | false;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level0.html#ga0f47949dca514506718276205a4fae0b */
        function getCuesheetAsync(path: string, callback: AsyncCallback<metadata.StreamInfoMetadata>): void;
        function getCuesheetAsync(path: string): Promise<metadata.StreamInfoMetadata>;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level0.html#ga0c9cd22296400c8ce16ee1db011342cb */
        function getPicture(path: string, type: number, maxWidth?: number, maxHeight?: number, maxDepth?: number, maxColors?: number): metadata.PictureMetadata | false;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level0.html#ga0c9cd22296400c8ce16ee1db011342cb */
        function getPicture(path: string, type: number, maxWidth: number | null, maxHeight: number | null, maxDepth: number | null, maxColors: number | null, callback: AsyncCallback<metadata.PictureMetadata>): void;
        function getPicture(path: string, type: number, maxWidth?: number, maxHeight?: number, maxDepth?: number, maxColors?: number): Promise<metadata.PictureMetadata>;
    }


    /**
     * Metadata Level 1 iterator with an iterable interface implemented.
     * @see https://xiph.org/flac/api/group__flac__metadata__level1.html
     */
    class SimpleIterator implements Iterable<metadata.Metadata>, AsyncIterable<metadata.Metadata> {
        /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#gae8fd236fe6049c61f7f3b4a6ecbcd240 */
        status(): number;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#gaba8daf276fd7da863a2522ac050125fd */
        init(path: string, readOnly?: boolean, preserve?: boolean): boolean;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#gaba8daf276fd7da863a2522ac050125fd */
        initAsync(path: string, readOnly: boolean | undefined | null, preserve: boolean | undefined | null, callback: AsyncCallback<true>): void;
        init(path: string, readOnly?: boolean, preserve?: boolean): Promise<true>;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#ga5150ecd8668c610f79192a2838667790 */
        isWritable(): boolean;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#gabb7de0a1067efae353e0792dc6e51905 */
        next(): boolean;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#gabb7de0a1067efae353e0792dc6e51905 */
        nextAsync(callback: AsyncCallback<boolean>): void;
        nextAsync(): Promise<boolean>;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#ga6db5313b31120b28e210ae721d6525a8 */
        prev(): boolean;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#ga6db5313b31120b28e210ae721d6525a8 */
        prevAsync(callback: AsyncCallback<boolean>): void;
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
        getApplicationId(): Buffer | false;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#gad4fea2d7d98d16e75e6d8260f690a5dc */
        getApplicationIdAsync(callback: AsyncCallback<Buffer>): void;
        getApplicationIdAsync(): Promise<Buffer>;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#ga1b7374cafd886ceb880b050dfa1e387a */
        getBlock(): metadata.Metadata;
        getBlock<T extends metadata.MetadataTypes, MT extends metadata.Metadata<T>>(): MT;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#ga1b7374cafd886ceb880b050dfa1e387a */
        getBlockAsync(callback: AsyncCallback<metadata.Metadata>): void;
        getBlockAsync(): Promise<metadata.Metadata>;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#gae1dd863561606658f88c492682de7b80 */
        setBlock(metadata: metadata.Metadata, pad?: boolean): boolean;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#gae1dd863561606658f88c492682de7b80 */
        setBlockAsync(metadata: metadata.Metadata, pad: boolean | undefined | null, callback: AsyncCallback<true>): void;
        setBlockAsync(metadata: metadata.Metadata, pad?: boolean): Promise<true>;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#ga7a0c00e93bb37324a20926e92e604102 */
        insertBlockAfter(metadata: metadata.Metadata, pad?: boolean): boolean;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#ga7a0c00e93bb37324a20926e92e604102 */
        insertBlockAfterAsync(metadata: metadata.Metadata, pad: boolean | undefined | null, callback: AsyncCallback<true>): void;
        insertBlockAfterAsync(metadata: metadata.Metadata, pad?: boolean): Promise<true>;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#gac3116c8e6e7f59914ae22c0c4c6b0a23 */
        deleteBlock(pad?: boolean): void;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level1.html#gac3116c8e6e7f59914ae22c0c4c6b0a23 */
        deleteBlockAsync(pad: boolean | null | undefined, callback: AsyncCallback<true>): void;
        deleteBlockAsync(pad?: boolean): Promise<true>;

        /**
         * Returns an iterator that iterates over the metadata blocks.
         */
        [Symbol.iterator]: () => Global_Iterator<metadata.Metadata>;

        /**
         * Returns an async iterator that iterates over the metadata blocks.
         */
        [Symbol.asyncIterator]: () => AsyncIterator<metadata.Metadata>;

        static Status: SimpleIterator.Status;
        static StatusString: ReverseEnum<SimpleIterator.Status>;
    }


    namespace SimpleIterator {
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
    class Chain {
        /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga8e74773f8ca2bb2bc0b56a65ca0299f4 */
        status(): number;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga5a4f2056c30f78af5a79f6b64d5bfdcd */
        read(path: string): boolean;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga5a4f2056c30f78af5a79f6b64d5bfdcd */
        readAsync(path: string, callback: AsyncCallback<true>): void;
        readAsync(path: string): Promise<true>;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga3995010aab28a483ad9905669e5c4954 */
        readOgg(path: string): boolean;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga3995010aab28a483ad9905669e5c4954 */
        readOggAsync(path: string, callback: AsyncCallback<true>): void;
        readOggAsync(path: string): Promise<true>;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga595f55b611ed588d4d55a9b2eb9d2add */
        readWithCallbacks(callbacks: Chain.IOCallbacks): Promise<true>;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#gaccc2f991722682d3c31d36f51985066c */
        readOggWithCallbacks(callbacks: Chain.IOCallbacks): Promise<true>;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga46bf9cf7d426078101b9297ba80bb835 */
        write(padding?: boolean, preserve?: boolean): boolean;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga46bf9cf7d426078101b9297ba80bb835 */
        writeAsync(padding: boolean | undefined | null, preserve: boolean | undefined | null, callback: AsyncCallback<true>): void;
        writeAsync(padding?: boolean, preserve?: boolean): Promise<true>;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga70532b3705294dc891d8db649a4d4843 */
        writeWithCallbacks(callbacks: Chain.IOCallbacks, usePadding?: boolean): Promise<true>;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga72facaa621e8d798036a4a7da3643e41 */
        writeWithCallbacksAndTempFile(usePadding: boolean, callbacks: Chain.IOCallbacks, tempCallbacks: Chain.IOCallbacks): Promise<true>;
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
    class Iterator implements Iterable<metadata.Metadata> {
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
        getBlock(): metadata.Metadata;
        getBlock<T extends metadata.MetadataTypes, MT extends metadata.Metadata<T>>(): MT;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#gaf61795b21300a2b0c9940c11974aab53 */
        setBlock(metadata: metadata.Metadata): boolean;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#gadf860af967d2ee483be01fc0ed8767a9 */
        deleteBlock(padding?: boolean): boolean;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga8ac45e2df8b6fd6f5db345c4293aa435 */
        insertBlockBefore(metadata: metadata.Metadata): boolean;
        /** @see https://xiph.org/flac/api/group__flac__metadata__level2.html#ga55e53757f91696e2578196a2799fc632 */
        insertBlockAfter(metadata: metadata.Metadata): boolean;

        /**
         * Returns an iterator that will iterate over all the metadata objects.
         */
        [Symbol.iterator]: () => Global_Iterator<metadata.Metadata>;
    }


    namespace Chain {
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
            read?: (outputBuffer: Buffer, sizeOfItem: number, numberOfItems: number) => number | bigint | Promise<number | bigint>;

            /**
             * Write operation callback. It must write a multiple of `sizeOfItem` bytes.
             * @param inputBuffer Buffer with the data to be written.
             * @param sizeOfItem The size of an item to read in bytes.
             * @param numberOfItems The number of items to read.
             * @returns the number of bytes written.
             */
            write?: (inputBuffer: Buffer, sizeOfItem: number, numberOfItems: number) => number | bigint | Promise<number | bigint>;

            /**
             * Seek operation callback.
             * @param offset Position offset where to move, relative to `whence`.
             * @param whence Relative mark. 0 (`set`) is relative to the beginning
             * of the stream. 1 (`cur`) is relative to the current position. 2
             * (`end`) is relative to the end of the stream.
             * @returns 0 on success, -1 on error.
             */
            seek?: (offset: number | bigint, whence: 'set' | 'cur' | 'end') => 0 | -1 | Promise<0 | -1>;

            /**
             * Tell operation callback.
             * @returns The current position in bytes of the stream or -1 in case of error.
             */
            tell?: () => number | bigint | -1 | Promise<number | bigint | -1>;

            /**
             * EOF check callback.
             * @returns `true` if the End Of File is reached, `false` otherwise.
             */
            eof?: () => boolean | Promise<boolean>;

            /**
             * Close operation callback.
             * @returns 0 on success, -1 on error.
             */
            close: () => 0 | -1 | Promise<0 | -1>;
        }
    }



    /** @see https://xiph.org/flac/api/structFLAC____FrameHeader.html */
    interface Header {
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
    interface Footer {
        crc: number;
    }

    /** @see https://xiph.org/flac/api/structFLAC____Subframe.html */
    interface SubFrame<T extends 0 | 1 | 2 | 3 = 0 | 1 | 2 | 3> {
        type: T;
        wastedBits: number;
    }

    /** @see https://xiph.org/flac/api/structFLAC____Subframe__Constant.html */
    interface ConstantSubFrame extends SubFrame<0> {
        value: number;
    }

    /** @see https://xiph.org/flac/api/structFLAC____Subframe__Verbatim.html */
    interface VerbatimSubFrame extends SubFrame<1> {
        data: Buffer;
    }

    /** @see https://xiph.org/flac/api/structFLAC____Subframe__Fixed.html */
    interface FixedSubFrame extends SubFrame<2> {
        entropyCodingMethod: unknown;
        order: number;
        warmup: number[];
        residual: Buffer;
    }

    /** @see https://xiph.org/flac/api/structFLAC____Subframe__LPC.html */
    interface LPCSubFrame extends SubFrame<3> {
        entropyCodingMethod: unknown;
        order: number;
        qlpCoeffPrecision: number;
        quantizationLevel: number;
        warmup: number[];
        qlpCoeff: number[];
        residual: Buffer;
    }

    /** @see https://xiph.org/flac/api/structFLAC____Frame.html */
    interface Frame {
        footer: Footer;
        header: Header;
        subframes: SubFrame[];
    }


    /**
     * If a suitable library cannot be loaded, this function will be available.
     * Given a path to the `libflac` library, tries to load it as flac library.
     * If the library is not a the FLAC one, the behaviour is undefined and crashes
     * may occur.
     */
    const load: ((flacDynamicLibraryPath: string) => typeof api) | undefined;

    function testAsync(mode: 'reject' | 'exception' | 'resolve', progress: (char: string) => Promise<void> | void): Promise<true>;
    function testAsync(mode: 'reject' | 'exception' | 'resolve', progress: (char: string) => Promise<void> | void, callback: (err: any, res?: true) => void): void;
}


// Encoder
/** Options to be set to the encoder before starting to encode. */
declare interface FlacEncoderOptions {
    /** If true, then the output will be an Ogg/FLAC file */
    isOggStream?: boolean;
    /** Number of channels to be encoded */
    channels: number;
    /** Sample resolution of the input to be encoded */
    bitsPerSample: number;
    /**
     * If set to `true`, all the input will be treated as 32 bit, if `false``
     * the input will be treated as `bitsPerSample` bit.
     * If `bitsPerSample` is 24, this will be set to `true` by default.
     * In the rest of the cases will be set to `false`.
     **/
    inputAs32?: boolean;
    /** Sample rate in Hz of the input to be encoded */
    samplerate: number;
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
     * requires {@link FlacEncoderOptions.doMidSideStereo} to also be set to true in
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
    metadata?: api.metadata.Metadata[];
}

/**
 * FLAC encoder which receives an interleaved PCM stream and transforms it into
 * a FLAC stream.
 */
declare class StreamEncoder extends Transform {
    constructor(props: FlacEncoderOptions);
}

/**
 * FLAC encoder which receives an interleaved PCM stream and writes it into
 * a file.
 */
declare class FileEncoder extends Writable {
    constructor(props: FlacEncoderOptions & { file: string });
}


// Decoder
/** Options to be set to the decoder before starting to decode. */
declare interface FlacDecoderOptions {
    /** If set to true, the input must be an Ogg/FLAC stream. */
    isOggStream?: boolean;
    /**
     * If set to true, it will emit the `metadata` for each metadata block.
     * If set to an array, it will only emit `metadata` for the types
     * specified in it.
     **/
    metadata?: number[] | true;
    /**
     * If set to `true`, samples will be 32 bit integers. By default, this
     * value is `false`, except when the flac is 24 bit. When `false`, the
     * output will be `bitsPerSample` bit.
     **/
    outputAs32?: boolean;
}

/**
 * FLAC decoder which transforms a stream of FLAC (or Ogg/FLAC) into
 * a interleaved raw PCM stream.
 * @emits metadata When a metadata block is received, the event will be fired
 */
declare class StreamDecoder extends Transform {
    constructor(props: FlacDecoderOptions);

    /** Gets the number of channels found in the stream, once it started to process */
    getChannels(): number | undefined;
    /** Gets the channel assignment for the stream, once it started to process */
    getChannelAssignment(): 0 | 1 | 2 | 3 | undefined;
    /** Gets the bits per sample found in the stream, once it started to process */
    getBitsPerSample(): number | undefined;
}

/**
 * FLAC decoder which reads a FLAC or Ogg/FLAC file and outputs
 * a interleaved raw PCM stream.
 * @emits metadata When a metadata block is received, the event will be fired
 */
declare class FileDecoder extends Readable {
    constructor(props: FlacDecoderOptions & { file: string });

    /** Gets the number of channels found in the stream, once it started to process */
    getChannels(): number | undefined;
    /** Gets the channel assignment for the stream, once it started to process */
    getChannelAssignment(): 0 | 1 | 2 | 3 | undefined;
    /** Gets the bits per sample found in the stream, once it started to process */
    getBitsPerSample(): number | undefined;
}