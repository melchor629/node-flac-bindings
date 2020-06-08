#pragma once

#include "format.h"
#include "../utils/symbol.hpp"

typedef struct _FLAC__StreamEncoder FLAC__StreamEncoder;

namespace flac_bindings {

    typedef int (*FLAC__StreamEncoderReadCallback)(const FLAC__StreamEncoder *encoder, char buffer[], size_t *bytes, void *client_data);
    typedef int (*FLAC__StreamEncoderWriteCallback)(const FLAC__StreamEncoder *encoder, const char buffer[], size_t bytes, unsigned samples, unsigned current_frame, void *client_data);
    typedef int (*FLAC__StreamEncoderSeekCallback)(const FLAC__StreamEncoder *encoder, uint64_t absolute_byte_offset, void *client_data);
    typedef int (*FLAC__StreamEncoderTellCallback)(const FLAC__StreamEncoder *encoder, uint64_t *absolute_byte_offset, void *client_data);
    typedef void(*FLAC__StreamEncoderMetadataCallback)(const FLAC__StreamEncoder *encoder, const FLAC__StreamMetadata *metadata, void *client_data);
    typedef void(*FLAC__StreamEncoderProgressCallback)(const FLAC__StreamEncoder *encoder, uint64_t bytes_written, uint64_t samples_written, unsigned frames_written, unsigned total_frames_estimate, void *client_data);

    template<typename Type>
    using FlacEncoderGetter = FunctionSymbol<Type(FLAC__StreamEncoder*)>;
    template<typename Type>
    using FlacEncoderSetter = FunctionSymbol<FLAC__bool(FLAC__StreamEncoder*, Type)>;

    extern FunctionSymbol<FLAC__StreamEncoder*(void)> FLAC__stream_encoder_new;
    extern FunctionSymbol<void(FLAC__StreamEncoder*)> FLAC__stream_encoder_delete;
    extern FunctionSymbol<int(FLAC__StreamEncoder*, FLAC__StreamEncoderWriteCallback, FLAC__StreamEncoderSeekCallback, FLAC__StreamEncoderTellCallback, FLAC__StreamEncoderMetadataCallback, void*)> FLAC__stream_encoder_init_stream;
    extern FunctionSymbol<int(FLAC__StreamEncoder*, FLAC__StreamEncoderReadCallback, FLAC__StreamEncoderWriteCallback, FLAC__StreamEncoderSeekCallback, FLAC__StreamEncoderTellCallback, FLAC__StreamEncoderMetadataCallback, void*)> FLAC__stream_encoder_init_ogg_stream;
    extern FunctionSymbol<int(FLAC__StreamEncoder*, const char*, FLAC__StreamEncoderProgressCallback, void*)> FLAC__stream_encoder_init_file;
    extern FunctionSymbol<int(FLAC__StreamEncoder*, const char*, FLAC__StreamEncoderProgressCallback, void*)> FLAC__stream_encoder_init_ogg_file;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamEncoder*)> FLAC__stream_encoder_finish;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamEncoder*, const int32_t* const [], unsigned)> FLAC__stream_encoder_process;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamEncoder*, const int32_t [], unsigned)> FLAC__stream_encoder_process_interleaved;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamEncoder*, const char*)> FLAC__stream_encoder_set_apodization;
    extern FunctionSymbol<const char*(const FLAC__StreamEncoder*)> FLAC__stream_encoder_get_resolved_state_string;
    extern FunctionSymbol<void(const FLAC__StreamEncoder*, uint64_t*, unsigned*, unsigned*, unsigned*, int32_t*, int32_t*)> FLAC__stream_encoder_get_verify_decoder_error_stats;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamEncoder*, FLAC__StreamMetadata**, unsigned)> FLAC__stream_encoder_set_metadata;

    extern FlacEncoderSetter<long> FLAC__stream_encoder_set_ogg_serial_number;
    extern FlacEncoderGetter<FLAC__bool> FLAC__stream_encoder_get_verify;
    extern FlacEncoderSetter<FLAC__bool> FLAC__stream_encoder_set_verify;
    extern FlacEncoderGetter<FLAC__bool> FLAC__stream_encoder_get_streamable_subset;
    extern FlacEncoderSetter<FLAC__bool> FLAC__stream_encoder_set_streamable_subset;
    extern FlacEncoderGetter<unsigned> FLAC__stream_encoder_get_channels;
    extern FlacEncoderSetter<unsigned> FLAC__stream_encoder_set_channels;
    extern FlacEncoderGetter<unsigned> FLAC__stream_encoder_get_bits_per_sample;
    extern FlacEncoderSetter<unsigned> FLAC__stream_encoder_set_bits_per_sample;
    extern FlacEncoderGetter<unsigned> FLAC__stream_encoder_get_sample_rate;
    extern FlacEncoderSetter<unsigned> FLAC__stream_encoder_set_sample_rate;
    extern FlacEncoderSetter<unsigned> FLAC__stream_encoder_set_compression_level;
    extern FlacEncoderGetter<unsigned> FLAC__stream_encoder_get_blocksize;
    extern FlacEncoderSetter<unsigned> FLAC__stream_encoder_set_blocksize;
    extern FlacEncoderGetter<FLAC__bool> FLAC__stream_encoder_get_do_mid_side_stereo;
    extern FlacEncoderSetter<FLAC__bool> FLAC__stream_encoder_set_do_mid_side_stereo;
    extern FlacEncoderGetter<FLAC__bool> FLAC__stream_encoder_get_loose_mid_side_stereo;
    extern FlacEncoderSetter<FLAC__bool> FLAC__stream_encoder_set_loose_mid_side_stereo;
    extern FlacEncoderGetter<unsigned> FLAC__stream_encoder_get_max_lpc_order;
    extern FlacEncoderSetter<unsigned> FLAC__stream_encoder_set_max_lpc_order;
    extern FlacEncoderGetter<unsigned> FLAC__stream_encoder_get_qlp_coeff_precision;
    extern FlacEncoderSetter<unsigned> FLAC__stream_encoder_set_qlp_coeff_precision;
    extern FlacEncoderGetter<FLAC__bool> FLAC__stream_encoder_get_do_qlp_coeff_prec_search;
    extern FlacEncoderSetter<FLAC__bool> FLAC__stream_encoder_set_do_qlp_coeff_prec_search;
    extern FlacEncoderGetter<FLAC__bool> FLAC__stream_encoder_get_do_escape_coding;
    extern FlacEncoderSetter<FLAC__bool> FLAC__stream_encoder_set_do_escape_coding;
    extern FlacEncoderGetter<FLAC__bool> FLAC__stream_encoder_get_do_exhaustive_model_search;
    extern FlacEncoderSetter<FLAC__bool> FLAC__stream_encoder_set_do_exhaustive_model_search;
    extern FlacEncoderGetter<unsigned> FLAC__stream_encoder_get_min_residual_partition_order;
    extern FlacEncoderSetter<unsigned> FLAC__stream_encoder_set_min_residual_partition_order;
    extern FlacEncoderGetter<unsigned> FLAC__stream_encoder_get_max_residual_partition_order;
    extern FlacEncoderSetter<unsigned> FLAC__stream_encoder_set_max_residual_partition_order;
    extern FlacEncoderGetter<unsigned> FLAC__stream_encoder_get_rice_parameter_search_dist;
    extern FlacEncoderSetter<unsigned> FLAC__stream_encoder_set_rice_parameter_search_dist;
    extern FlacEncoderGetter<uint64_t> FLAC__stream_encoder_get_total_samples_estimate;
    extern FlacEncoderSetter<uint64_t> FLAC__stream_encoder_set_total_samples_estimate;
    extern FlacEncoderGetter<int> FLAC__stream_encoder_get_state;
    extern FlacEncoderGetter<int> FLAC__stream_encoder_get_verify_decoder_state;

}
