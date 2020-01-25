import { Transform, Readable } from 'stream';

declare namespace decoder {

    /** Options to be set to the decoder before starting to decode. */
    interface FlacDecoderOptions {
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
    class StreamDecoder extends Transform {
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
    class FileDecoder extends Readable {
        constructor(props: FlacDecoderOptions & { file: string });

        /** Gets the number of channels found in the stream, once it started to process */
        getChannels(): number | undefined;
        /** Gets the channel assignment for the stream, once it started to process */
        getChannelAssignment(): 0 | 1 | 2 | 3 | undefined;
        /** Gets the bits per sample found in the stream, once it started to process */
        getBitsPerSample(): number | undefined;
    }

}

export = decoder;
