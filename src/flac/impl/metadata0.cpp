#include "../metadata0.hpp"

namespace flac_bindings {

    FunctionSymbol<FLAC__bool(const char *, FLAC__StreamMetadata *)> FLAC__metadata_get_streaminfo = "FLAC__metadata_get_streaminfo";
    FunctionSymbol<FLAC__bool(const char *, FLAC__StreamMetadata **)> FLAC__metadata_get_tags = "FLAC__metadata_get_tags";
    FunctionSymbol<FLAC__bool(const char *, FLAC__StreamMetadata **)> FLAC__metadata_get_cuesheet = "FLAC__metadata_get_cuesheet";
    FunctionSymbol<FLAC__bool(
        const char *,
        FLAC__StreamMetadata **,
        FLAC__StreamMetadata_Picture_Type,
        const char *,
        const FLAC__byte *,
        unsigned,
        unsigned,
        unsigned,
        unsigned
    )> FLAC__metadata_get_picture = "FLAC__metadata_get_picture";

}
