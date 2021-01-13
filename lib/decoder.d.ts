import { Transform, Readable } from 'stream';
import { format } from './api';

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
        metadata?: format.MetadataType[] | true;
        /**
         * If set to `true`, samples will be 32 bit integers. By default, this
         * value is `false`, except when the flac is 24 bit. When `false`, the
         * output will be `bitsPerSample` bit.
         **/
        outputAs32?: boolean;
    }

    interface FlacDecoderPosition {
        /** Position where the decoding is at, in samples */
        position: number;
        /** Total samples (if flac provides it) */
        totalSamples: number;
        /** Value between 0 and 1 that represents the percentage of samples decoded (or NaN if there is no total samples) */
        percentage: number;
        /** Total seconds (if flac provides it) */
        totalSeconds: number;
        /** Current position in seconds */
        currentSeconds: number;
    }

    /**
     * FLAC decoder which transforms a stream of FLAC (or Ogg/FLAC) into
     * a interleaved raw PCM stream.
     * @emits metadata When a metadata block is received, the event will be fired
     */
    class StreamDecoder extends Transform {
        constructor(props: FlacDecoderOptions);

        /** Gets total number of samples if possible, once it started to process */
        getTotalSamples(): number | undefined;
        /** Gets the number of channels found in the stream, once it started to process */
        getChannels(): number | undefined;
        /** Gets the channel assignment for the stream, once it started to process */
        getChannelAssignment(): 0 | 1 | 2 | 3 | undefined;
        /** Gets the bits per sample found in the stream, once it started to process */
        getBitsPerSample(): number | undefined;
        /** Gets the sample rate of the stream, once it started to process */
        getSampleRate(): number | undefined;
        /** Returns the progress of the decoding (if possible) */
        getProgress(): FlacDecoderPosition | undefined;
    }

    /**
     * FLAC decoder which reads a FLAC or Ogg/FLAC file and outputs
     * a interleaved raw PCM stream.
     * @emits metadata When a metadata block is received, the event will be fired
     */
    class FileDecoder extends Readable {
        constructor(props: FlacDecoderOptions & { file: string });

        /** Gets total number of samples if possible, once it started to process */
        getTotalSamples(): number | undefined;
        /** Gets the number of channels found in the stream, once it started to process */
        getChannels(): number | undefined;
        /** Gets the channel assignment for the stream, once it started to process */
        getChannelAssignment(): 0 | 1 | 2 | 3 | undefined;
        /** Gets the bits per sample found in the stream, once it started to process */
        getBitsPerSample(): number | undefined;
        /** Gets the sample rate of the stream, once it started to process */
        getSampleRate(): number | undefined;
        /** Returns the progress of the decoding (if possible) */
        getProgress(): FlacDecoderPosition | undefined;
    }

}

export = decoder;
