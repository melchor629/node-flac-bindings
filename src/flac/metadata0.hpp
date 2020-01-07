#pragma once

#include "format.h"
#include "../utils/symbol.hpp"

namespace flac_bindings {

    extern FunctionSymbol<FLAC__bool(const char *, FLAC__StreamMetadata *)> FLAC__metadata_get_streaminfo;
    extern FunctionSymbol<FLAC__bool(const char *, FLAC__StreamMetadata **)> FLAC__metadata_get_tags;
    extern FunctionSymbol<FLAC__bool(const char *, FLAC__StreamMetadata **)> FLAC__metadata_get_cuesheet;
    extern FunctionSymbol<FLAC__bool(
        const char *,
        FLAC__StreamMetadata **,
        FLAC__StreamMetadata_Picture_Type,
        const char *,
        const FLAC__byte *,
        unsigned,
        unsigned,
        unsigned,
        unsigned
    )> FLAC__metadata_get_picture;

}
