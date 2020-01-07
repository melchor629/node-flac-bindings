#include "../encoder.hpp"

namespace flac_bindings {

    FunctionSymbol<FLAC__StreamEncoder*(void)> FLAC__stream_encoder_new = "FLAC__stream_encoder_new";
    FunctionSymbol<void(FLAC__StreamEncoder*)> FLAC__stream_encoder_delete = "FLAC__stream_encoder_delete";
    FunctionSymbol<int(FLAC__StreamEncoder*, FLAC__StreamEncoderWriteCallback, FLAC__StreamEncoderSeekCallback, FLAC__StreamEncoderTellCallback, FLAC__StreamEncoderMetadataCallback, void*)> FLAC__stream_encoder_init_stream = "FLAC__stream_encoder_init_stream";
    FunctionSymbol<int(FLAC__StreamEncoder*, FLAC__StreamEncoderReadCallback, FLAC__StreamEncoderWriteCallback, FLAC__StreamEncoderSeekCallback, FLAC__StreamEncoderTellCallback, FLAC__StreamEncoderMetadataCallback, void*)> FLAC__stream_encoder_init_ogg_stream = "FLAC__stream_encoder_init_ogg_stream";
    FunctionSymbol<int(FLAC__StreamEncoder*, const char*, FLAC__StreamEncoderProgressCallback, void*)> FLAC__stream_encoder_init_file = "FLAC__stream_encoder_init_file";
    FunctionSymbol<int(FLAC__StreamEncoder*, const char*, FLAC__StreamEncoderProgressCallback, void*)> FLAC__stream_encoder_init_ogg_file = "FLAC__stream_encoder_init_ogg_file";
    FunctionSymbol<FLAC__bool(FLAC__StreamEncoder*)> FLAC__stream_encoder_finish = "FLAC__stream_encoder_finish";
    FunctionSymbol<FLAC__bool(FLAC__StreamEncoder*, const int32_t* const [], unsigned)> FLAC__stream_encoder_process = "FLAC__stream_encoder_process";
    FunctionSymbol<FLAC__bool(FLAC__StreamEncoder*, const int32_t [], unsigned)> FLAC__stream_encoder_process_interleaved = "FLAC__stream_encoder_process_interleaved";
    FunctionSymbol<FLAC__bool(FLAC__StreamEncoder*, const char*)> FLAC__stream_encoder_set_apodization = "FLAC__stream_encoder_set_apodization";
    FunctionSymbol<const char*(const FLAC__StreamEncoder*)> FLAC__stream_encoder_get_resolved_state_string = "FLAC__stream_encoder_get_resolved_state_string";
    FunctionSymbol<void(const FLAC__StreamEncoder*, uint64_t*, unsigned*, unsigned*, unsigned*, int32_t*, int32_t*)> FLAC__stream_encoder_get_verify_decoder_error_stats = "FLAC__stream_encoder_get_verify_decoder_error_stats";
    FunctionSymbol<FLAC__bool(FLAC__StreamEncoder*, FLAC__StreamMetadata**, unsigned)> FLAC__stream_encoder_set_metadata = "FLAC__stream_encoder_set_metadata";

    FlacEncoderSetter<long> FLAC__stream_encoder_set_ogg_serial_number = "FLAC__stream_encoder_set_ogg_serial_number";
    FlacEncoderGetter<FLAC__bool> FLAC__stream_encoder_get_verify = "FLAC__stream_encoder_get_verify";
    FlacEncoderSetter<FLAC__bool> FLAC__stream_encoder_set_verify = "FLAC__stream_encoder_set_verify";
    FlacEncoderGetter<FLAC__bool> FLAC__stream_encoder_get_streamable_subset = "FLAC__stream_encoder_get_streamable_subset";
    FlacEncoderSetter<FLAC__bool> FLAC__stream_encoder_set_streamable_subset = "FLAC__stream_encoder_set_streamable_subset";
    FlacEncoderGetter<unsigned> FLAC__stream_encoder_get_channels = "FLAC__stream_encoder_get_channels";
    FlacEncoderSetter<unsigned> FLAC__stream_encoder_set_channels = "FLAC__stream_encoder_set_channels";
    FlacEncoderGetter<unsigned> FLAC__stream_encoder_get_bits_per_sample = "FLAC__stream_encoder_get_bits_per_sample";
    FlacEncoderSetter<unsigned> FLAC__stream_encoder_set_bits_per_sample = "FLAC__stream_encoder_set_bits_per_sample";
    FlacEncoderGetter<unsigned> FLAC__stream_encoder_get_sample_rate = "FLAC__stream_encoder_get_sample_rate";
    FlacEncoderSetter<unsigned> FLAC__stream_encoder_set_sample_rate = "FLAC__stream_encoder_set_sample_rate";
    FlacEncoderSetter<unsigned> FLAC__stream_encoder_set_compression_level = "FLAC__stream_encoder_set_compression_level";
    FlacEncoderGetter<unsigned> FLAC__stream_encoder_get_blocksize = "FLAC__stream_encoder_get_blocksize";
    FlacEncoderSetter<unsigned> FLAC__stream_encoder_set_blocksize = "FLAC__stream_encoder_set_blocksize";
    FlacEncoderGetter<FLAC__bool> FLAC__stream_encoder_get_do_mid_side_stereo = "FLAC__stream_encoder_get_do_mid_side_stereo";
    FlacEncoderSetter<FLAC__bool> FLAC__stream_encoder_set_do_mid_side_stereo = "FLAC__stream_encoder_set_do_mid_side_stereo";
    FlacEncoderGetter<FLAC__bool> FLAC__stream_encoder_get_loose_mid_side_stereo = "FLAC__stream_encoder_get_loose_mid_side_stereo";
    FlacEncoderSetter<FLAC__bool> FLAC__stream_encoder_set_loose_mid_side_stereo = "FLAC__stream_encoder_set_loose_mid_side_stereo";
    FlacEncoderGetter<unsigned> FLAC__stream_encoder_get_max_lpc_order = "FLAC__stream_encoder_get_max_lpc_order";
    FlacEncoderSetter<unsigned> FLAC__stream_encoder_set_max_lpc_order = "FLAC__stream_encoder_set_max_lpc_order";
    FlacEncoderGetter<unsigned> FLAC__stream_encoder_get_qlp_coeff_precision = "FLAC__stream_encoder_get_qlp_coeff_precision";
    FlacEncoderSetter<unsigned> FLAC__stream_encoder_set_qlp_coeff_precision = "FLAC__stream_encoder_set_qlp_coeff_precision";
    FlacEncoderGetter<FLAC__bool> FLAC__stream_encoder_get_do_qlp_coeff_prec_search = "FLAC__stream_encoder_get_do_qlp_coeff_prec_search";
    FlacEncoderSetter<FLAC__bool> FLAC__stream_encoder_set_do_qlp_coeff_prec_search = "FLAC__stream_encoder_set_do_qlp_coeff_prec_search";
    FlacEncoderGetter<FLAC__bool> FLAC__stream_encoder_get_do_escape_coding = "FLAC__stream_encoder_get_do_escape_coding";
    FlacEncoderSetter<FLAC__bool> FLAC__stream_encoder_set_do_escape_coding = "FLAC__stream_encoder_set_do_escape_coding";
    FlacEncoderGetter<FLAC__bool> FLAC__stream_encoder_get_do_exhaustive_model_search = "FLAC__stream_encoder_get_do_exhaustive_model_search";
    FlacEncoderSetter<FLAC__bool> FLAC__stream_encoder_set_do_exhaustive_model_search = "FLAC__stream_encoder_set_do_exhaustive_model_search";
    FlacEncoderGetter<unsigned> FLAC__stream_encoder_get_min_residual_partition_order = "FLAC__stream_encoder_get_min_residual_partition_order";
    FlacEncoderSetter<unsigned> FLAC__stream_encoder_set_min_residual_partition_order = "FLAC__stream_encoder_set_min_residual_partition_order";
    FlacEncoderGetter<unsigned> FLAC__stream_encoder_get_max_residual_partition_order = "FLAC__stream_encoder_get_max_residual_partition_order";
    FlacEncoderSetter<unsigned> FLAC__stream_encoder_set_max_residual_partition_order = "FLAC__stream_encoder_set_max_residual_partition_order";
    FlacEncoderGetter<unsigned> FLAC__stream_encoder_get_rice_parameter_search_dist = "FLAC__stream_encoder_get_rice_parameter_search_dist";
    FlacEncoderSetter<unsigned> FLAC__stream_encoder_set_rice_parameter_search_dist = "FLAC__stream_encoder_set_rice_parameter_search_dist";
    FlacEncoderGetter<uint64_t> FLAC__stream_encoder_get_total_samples_estimate = "FLAC__stream_encoder_get_total_samples_estimate";
    FlacEncoderSetter<uint64_t> FLAC__stream_encoder_set_total_samples_estimate = "FLAC__stream_encoder_set_total_samples_estimate";
    FlacEncoderGetter<int> FLAC__stream_encoder_get_state = "FLAC__stream_encoder_get_state";
    FlacEncoderGetter<int> FLAC__stream_encoder_get_verify_decoder_state = "FLAC__stream_encoder_get_verify_decoder_state";

}
