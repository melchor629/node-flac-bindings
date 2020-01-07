#include "../decoder.hpp"

namespace flac_bindings {

    FunctionSymbol<FLAC__StreamDecoder*(void)> FLAC__stream_decoder_new = "FLAC__stream_decoder_new";
    FunctionSymbol<void(FLAC__StreamDecoder*)> FLAC__stream_decoder_delete = "FLAC__stream_decoder_delete";
    FunctionSymbol<int(
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
    )> FLAC__stream_decoder_init_stream = "FLAC__stream_decoder_init_stream";
    FunctionSymbol<int(
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
    )> FLAC__stream_decoder_init_ogg_stream = "FLAC__stream_decoder_init_ogg_stream";
    FunctionSymbol<int(
        FLAC__StreamDecoder*,
        const char*,
        FLAC__StreamDecoderWriteCallback,
        FLAC__StreamDecoderMetadataCallback,
        FLAC__StreamDecoderErrorCallback,
        void*
    )> FLAC__stream_decoder_init_file = "FLAC__stream_decoder_init_file";
    FunctionSymbol<int(
        FLAC__StreamDecoder*,
        const char*,
        FLAC__StreamDecoderWriteCallback,
        FLAC__StreamDecoderMetadataCallback,
        FLAC__StreamDecoderErrorCallback,
        void*
    )> FLAC__stream_decoder_init_ogg_file = "FLAC__stream_decoder_init_ogg_file";
    FunctionSymbol<FLAC__bool(FLAC__StreamDecoder*)> FLAC__stream_decoder_finish = "FLAC__stream_decoder_finish";
    FunctionSymbol<FLAC__bool(FLAC__StreamDecoder*)> FLAC__stream_decoder_flush = "FLAC__stream_decoder_flush";
    FunctionSymbol<FLAC__bool(FLAC__StreamDecoder*)> FLAC__stream_decoder_reset = "FLAC__stream_decoder_reset";
    FunctionSymbol<FLAC__bool(FLAC__StreamDecoder*)> FLAC__stream_decoder_process_single = "FLAC__stream_decoder_process_single";
    FunctionSymbol<FLAC__bool(FLAC__StreamDecoder*)> FLAC__stream_decoder_process_until_end_of_metadata = "FLAC__stream_decoder_process_until_end_of_metadata";
    FunctionSymbol<FLAC__bool(FLAC__StreamDecoder*)> FLAC__stream_decoder_process_until_end_of_stream = "FLAC__stream_decoder_process_until_end_of_stream";
    FunctionSymbol<FLAC__bool(FLAC__StreamDecoder*)> FLAC__stream_decoder_skip_single_frame = "FLAC__stream_decoder_skip_single_frame";
    FunctionSymbol<FLAC__bool(FLAC__StreamDecoder*, uint64_t)> FLAC__stream_decoder_seek_absolute = "FLAC__stream_decoder_seek_absolute";
    FunctionSymbol<FLAC__bool(FLAC__StreamDecoder*, const FLAC__byte id[4])> FLAC__stream_decoder_set_metadata_respond_application = "FLAC__stream_decoder_set_metadata_respond_application";
    FunctionSymbol<FLAC__bool(FLAC__StreamDecoder*)> FLAC__stream_decoder_set_metadata_respond_all = "FLAC__stream_decoder_set_metadata_respond_all";
    FunctionSymbol<FLAC__bool(FLAC__StreamDecoder*, const FLAC__byte id[4])> FLAC__stream_decoder_set_metadata_ignore_application = "FLAC__stream_decoder_set_metadata_ignore_application";
    FunctionSymbol<FLAC__bool(FLAC__StreamDecoder*)> FLAC__stream_decoder_set_metadata_ignore_all = "FLAC__stream_decoder_set_metadata_ignore_all";
    FunctionSymbol<int(const FLAC__StreamDecoder*)> FLAC__stream_decoder_get_state = "FLAC__stream_decoder_get_state";
    FunctionSymbol<const char*(const FLAC__StreamDecoder*)> FLAC__stream_decoder_get_resolved_state_string = "FLAC__stream_decoder_get_resolved_state_string";
    FunctionSymbol<FLAC__bool(const FLAC__StreamDecoder*, uint64_t*)> FLAC__stream_decoder_get_decode_position = "FLAC__stream_decoder_get_decode_position";

    FlacDecoderSetter<long> FLAC__stream_decoder_set_ogg_serial_number = "FLAC__stream_decoder_set_ogg_serial_number";
    FlacDecoderGetter<FLAC__bool> FLAC__stream_decoder_get_md5_checking = "FLAC__stream_decoder_get_md5_checking";
    FlacDecoderSetter<FLAC__bool> FLAC__stream_decoder_set_md5_checking = "FLAC__stream_decoder_set_md5_checking";
    FlacDecoderSetter<FLAC__MetadataType> FLAC__stream_decoder_set_metadata_respond = "FLAC__stream_decoder_set_metadata_respond";
    FlacDecoderSetter<FLAC__MetadataType> FLAC__stream_decoder_set_metadata_ignore = "FLAC__stream_decoder_set_metadata_ignore";
    FlacDecoderGetter<uint64_t> FLAC__stream_decoder_get_total_samples = "FLAC__stream_decoder_get_total_samples";
    FlacDecoderGetter<unsigned> FLAC__stream_decoder_get_channels = "FLAC__stream_decoder_get_channels";
    FlacDecoderGetter<FLAC__ChannelAssignment> FLAC__stream_decoder_get_channel_assignment = "FLAC__stream_decoder_get_channel_assignment";
    FlacDecoderGetter<unsigned> FLAC__stream_decoder_get_bits_per_sample = "FLAC__stream_decoder_get_bits_per_sample";
    FlacDecoderGetter<unsigned> FLAC__stream_decoder_get_sample_rate = "FLAC__stream_decoder_get_sample_rate";
    FlacDecoderGetter<unsigned> FLAC__stream_decoder_get_blocksize = "FLAC__stream_decoder_get_blocksize";

}