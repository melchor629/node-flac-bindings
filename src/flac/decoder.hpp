#pragma once

#include "format.h"
#include "../utils/symbol.hpp"

namespace flac_bindings {

    typedef int(*FLAC__StreamDecoderReadCallback)(const FLAC__StreamDecoder*, FLAC__byte [], size_t*, void*);
    typedef int(*FLAC__StreamDecoderSeekCallback)(const FLAC__StreamDecoder*, uint64_t, void*);
    typedef int(*FLAC__StreamDecoderTellCallback)(const FLAC__StreamDecoder*, uint64_t*, void*);
    typedef int(*FLAC__StreamDecoderLengthCallback)(const FLAC__StreamDecoder*, uint64_t*, void*);
    typedef FLAC__bool(*FLAC__StreamDecoderEofCallback)(const FLAC__StreamDecoder*, void*);
    typedef int(*FLAC__StreamDecoderWriteCallback)(const FLAC__StreamDecoder*, const FLAC__Frame*, const int32_t *const [], void*);
    typedef void(*FLAC__StreamDecoderMetadataCallback)(const FLAC__StreamDecoder*, const FLAC__StreamMetadata*, void*);
    typedef void(*FLAC__StreamDecoderErrorCallback)(const FLAC__StreamDecoder*, int, void*);

    template<typename Type>
    using FlacDecoderGetter = FunctionSymbol<Type(FLAC__StreamDecoder*)>;
    template<typename Type>
    using FlacDecoderSetter = FunctionSymbol<FLAC__bool(FLAC__StreamDecoder*, Type)>;

    extern FunctionSymbol<FLAC__StreamDecoder*(void)> FLAC__stream_decoder_new;
    extern FunctionSymbol<void(FLAC__StreamDecoder*)> FLAC__stream_decoder_delete;
    extern FunctionSymbol<int(
        FLAC__StreamDecoder*,
        FLAC__StreamDecoderReadCallback,
        FLAC__StreamDecoderSeekCallback,
        FLAC__StreamDecoderTellCallback,
        FLAC__StreamDecoderLengthCallback,
        FLAC__StreamDecoderEofCallback,
        FLAC__StreamDecoderWriteCallback,
        FLAC__StreamDecoderMetadataCallback,
        FLAC__StreamDecoderErrorCallback,
        void*
    )> FLAC__stream_decoder_init_stream;
    extern FunctionSymbol<int(
        FLAC__StreamDecoder*,
        FLAC__StreamDecoderReadCallback,
        FLAC__StreamDecoderSeekCallback,
        FLAC__StreamDecoderTellCallback,
        FLAC__StreamDecoderLengthCallback,
        FLAC__StreamDecoderEofCallback,
        FLAC__StreamDecoderWriteCallback,
        FLAC__StreamDecoderMetadataCallback,
        FLAC__StreamDecoderErrorCallback,
        void*
    )> FLAC__stream_decoder_init_ogg_stream;
    extern FunctionSymbol<int(
        FLAC__StreamDecoder*,
        const char*,
        FLAC__StreamDecoderWriteCallback,
        FLAC__StreamDecoderMetadataCallback,
        FLAC__StreamDecoderErrorCallback,
        void*
    )> FLAC__stream_decoder_init_file;
    extern FunctionSymbol<int(
        FLAC__StreamDecoder*,
        const char*,
        FLAC__StreamDecoderWriteCallback,
        FLAC__StreamDecoderMetadataCallback,
        FLAC__StreamDecoderErrorCallback,
        void*
    )> FLAC__stream_decoder_init_ogg_file;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamDecoder*)> FLAC__stream_decoder_finish;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamDecoder*)> FLAC__stream_decoder_flush;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamDecoder*)> FLAC__stream_decoder_reset;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamDecoder*)> FLAC__stream_decoder_process_single;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamDecoder*)> FLAC__stream_decoder_process_until_end_of_metadata;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamDecoder*)> FLAC__stream_decoder_process_until_end_of_stream;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamDecoder*)> FLAC__stream_decoder_skip_single_frame;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamDecoder*, uint64_t)> FLAC__stream_decoder_seek_absolute;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamDecoder*, const FLAC__byte id[4])> FLAC__stream_decoder_set_metadata_respond_application;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamDecoder*)> FLAC__stream_decoder_set_metadata_respond_all;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamDecoder*, const FLAC__byte id[4])> FLAC__stream_decoder_set_metadata_ignore_application;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamDecoder*)> FLAC__stream_decoder_set_metadata_ignore_all;
    extern FunctionSymbol<int(const FLAC__StreamDecoder*)> FLAC__stream_decoder_get_state;
    extern FunctionSymbol<const char*(const FLAC__StreamDecoder*)> FLAC__stream_decoder_get_resolved_state_string;
    extern FunctionSymbol<FLAC__bool(const FLAC__StreamDecoder*, uint64_t*)> FLAC__stream_decoder_get_decode_position;

    extern FlacDecoderSetter<long> FLAC__stream_decoder_set_ogg_serial_number;
    extern FlacDecoderGetter<FLAC__bool> FLAC__stream_decoder_get_md5_checking;
    extern FlacDecoderSetter<FLAC__bool> FLAC__stream_decoder_set_md5_checking;
    extern FlacDecoderSetter<FLAC__MetadataType> FLAC__stream_decoder_set_metadata_respond;
    extern FlacDecoderSetter<FLAC__MetadataType> FLAC__stream_decoder_set_metadata_ignore;
    extern FlacDecoderGetter<uint64_t> FLAC__stream_decoder_get_total_samples;
    extern FlacDecoderGetter<unsigned> FLAC__stream_decoder_get_channels;
    extern FlacDecoderGetter<FLAC__ChannelAssignment> FLAC__stream_decoder_get_channel_assignment;
    extern FlacDecoderGetter<unsigned> FLAC__stream_decoder_get_bits_per_sample;
    extern FlacDecoderGetter<unsigned> FLAC__stream_decoder_get_sample_rate;
    extern FlacDecoderGetter<unsigned> FLAC__stream_decoder_get_blocksize;

}
