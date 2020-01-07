#pragma once

#include "format.h"
#include "../utils/symbol.hpp"

namespace flac_bindings {

    extern Symbol<const char *> FLAC__VERSION_STRING;
    extern Symbol<const char *> FLAC__VENDOR_STRING;
    extern Symbol<const char * const*> FLAC__EntropyCodingMethodTypeString;
    extern Symbol<const char * const*> FLAC__SubframeTypeString;
    extern Symbol<const char * const*> FLAC__ChannelAssignmentString;
    extern Symbol<const char * const*> FLAC__FrameNumberTypeString;
    extern Symbol<const char * const*> FLAC__MetadataTypeString;
    extern Symbol<const char * const*> FLAC__StreamMetadata_Picture_TypeString;

    extern FunctionSymbol<FLAC__bool(unsigned sample_rate)> FLAC__format_sample_rate_is_valid;
    extern FunctionSymbol<FLAC__bool(unsigned blocksize, unsigned sample_rate)> FLAC__format_blocksize_is_subset;
    extern FunctionSymbol<FLAC__bool(unsigned sample_rate)> FLAC__format_sample_rate_is_subset;
    extern FunctionSymbol<FLAC__bool(const char *name)> FLAC__format_vorbiscomment_entry_name_is_legal;
    extern FunctionSymbol<FLAC__bool(const FLAC__byte *value, unsigned length)> FLAC__format_vorbiscomment_entry_value_is_legal;
    extern FunctionSymbol<FLAC__bool(const FLAC__byte *entry, unsigned length)> FLAC__format_vorbiscomment_entry_is_legal;
    extern FunctionSymbol<FLAC__bool(const FLAC__StreamMetadata_SeekTable *seek_table)> FLAC__format_seektable_is_legal;
    extern FunctionSymbol<unsigned(FLAC__StreamMetadata_SeekTable *seek_table)> FLAC__format_seektable_sort;
    extern FunctionSymbol<FLAC__bool(const FLAC__StreamMetadata_CueSheet *cue_sheet, FLAC__bool check_cd_da_subset, const char **violation)> FLAC__format_cuesheet_is_legal;
    extern FunctionSymbol<FLAC__bool(const FLAC__StreamMetadata_Picture *picture, const char **violation)> FLAC__format_picture_is_legal;

}
