#include "../format.h"
#include "../../utils/symbol.hpp"

namespace flac_bindings {

    Symbol<const char *> FLAC__VERSION_STRING = "FLAC__VERSION_STRING";
    Symbol<const char *> FLAC__VENDOR_STRING = "FLAC__VENDOR_STRING";
    Symbol<const char * const*> FLAC__EntropyCodingMethodTypeString = "FLAC__EntropyCodingMethodTypeString";
    Symbol<const char * const*> FLAC__SubframeTypeString = "FLAC__SubframeTypeString";
    Symbol<const char * const*> FLAC__ChannelAssignmentString = "FLAC__ChannelAssignmentString";
    Symbol<const char * const*> FLAC__FrameNumberTypeString = "FLAC__FrameNumberTypeString";
    Symbol<const char * const*> FLAC__MetadataTypeString = "FLAC__MetadataTypeString";
    Symbol<const char * const*> FLAC__StreamMetadata_Picture_TypeString = "FLAC__StreamMetadata_Picture_TypeString";

    FunctionSymbol<FLAC__bool(unsigned sample_rate)> FLAC__format_sample_rate_is_valid = "FLAC__format_sample_rate_is_valid";
    FunctionSymbol<FLAC__bool(unsigned blocksize, unsigned sample_rate)> FLAC__format_blocksize_is_subset = "FLAC__format_blocksize_is_subset";
    FunctionSymbol<FLAC__bool(unsigned sample_rate)> FLAC__format_sample_rate_is_subset = "FLAC__format_sample_rate_is_subset";
    FunctionSymbol<FLAC__bool(const char *name)> FLAC__format_vorbiscomment_entry_name_is_legal = "FLAC__format_vorbiscomment_entry_name_is_legal";
    FunctionSymbol<FLAC__bool(const FLAC__byte *value, unsigned length)> FLAC__format_vorbiscomment_entry_value_is_legal = "FLAC__format_vorbiscomment_entry_value_is_legal";
    FunctionSymbol<FLAC__bool(const FLAC__byte *entry, unsigned length)> FLAC__format_vorbiscomment_entry_is_legal = "FLAC__format_vorbiscomment_entry_is_legal";
    FunctionSymbol<FLAC__bool(const FLAC__StreamMetadata_SeekTable *seek_table)> FLAC__format_seektable_is_legal = "FLAC__format_seektable_is_legal";
    FunctionSymbol<unsigned(FLAC__StreamMetadata_SeekTable *seek_table)> FLAC__format_seektable_sort = "FLAC__format_seektable_sort";
    FunctionSymbol<FLAC__bool(const FLAC__StreamMetadata_CueSheet *cue_sheet, FLAC__bool check_cd_da_subset, const char **violation)> FLAC__format_cuesheet_is_legal = "FLAC__format_cuesheet_is_legal";
    FunctionSymbol<FLAC__bool(const FLAC__StreamMetadata_Picture *picture, const char **violation)> FLAC__format_picture_is_legal = "FLAC__format_picture_is_legal";

}
