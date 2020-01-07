#include "../metadata1.hpp"

namespace flac_bindings {

    FunctionSymbol<FLAC__Metadata_SimpleIterator*(void)> FLAC__metadata_simple_iterator_new = "FLAC__metadata_simple_iterator_new";
    FunctionSymbol<void(FLAC__Metadata_SimpleIterator *)> FLAC__metadata_simple_iterator_delete = "FLAC__metadata_simple_iterator_delete";
    FunctionSymbol<FLAC__Metadata_SimpleIteratorStatus(FLAC__Metadata_SimpleIterator *)> FLAC__metadata_simple_iterator_status = "FLAC__metadata_simple_iterator_status";
    FunctionSymbol<FLAC__bool(FLAC__Metadata_SimpleIterator *, const char *, FLAC__bool, FLAC__bool)> FLAC__metadata_simple_iterator_init = "FLAC__metadata_simple_iterator_init";
    FunctionSymbol<FLAC__bool(const FLAC__Metadata_SimpleIterator *)> FLAC__metadata_simple_iterator_is_writable = "FLAC__metadata_simple_iterator_is_writable";
    FunctionSymbol<FLAC__bool(FLAC__Metadata_SimpleIterator *)> FLAC__metadata_simple_iterator_next = "FLAC__metadata_simple_iterator_next";
    FunctionSymbol<FLAC__bool(FLAC__Metadata_SimpleIterator *)> FLAC__metadata_simple_iterator_prev = "FLAC__metadata_simple_iterator_prev";
    FunctionSymbol<FLAC__bool(const FLAC__Metadata_SimpleIterator *)> FLAC__metadata_simple_iterator_is_last = "FLAC__metadata_simple_iterator_is_last";
    FunctionSymbol<off_t(const FLAC__Metadata_SimpleIterator *)> FLAC__metadata_simple_iterator_get_block_offset = "FLAC__metadata_simple_iterator_get_block_offset";
    FunctionSymbol<FLAC__MetadataType(const FLAC__Metadata_SimpleIterator *)> FLAC__metadata_simple_iterator_get_block_type = "FLAC__metadata_simple_iterator_get_block_type";
    FunctionSymbol<unsigned(const FLAC__Metadata_SimpleIterator *)> FLAC__metadata_simple_iterator_get_block_length = "FLAC__metadata_simple_iterator_get_block_length";
    FunctionSymbol<FLAC__bool(FLAC__Metadata_SimpleIterator *, FLAC__byte *)> FLAC__metadata_simple_iterator_get_application_id = "FLAC__metadata_simple_iterator_get_application_id";
    FunctionSymbol<FLAC__StreamMetadata*(FLAC__Metadata_SimpleIterator *)> FLAC__metadata_simple_iterator_get_block = "FLAC__metadata_simple_iterator_get_block";
    FunctionSymbol<FLAC__bool(FLAC__Metadata_SimpleIterator *, FLAC__StreamMetadata *, FLAC__bool use_padding)> FLAC__metadata_simple_iterator_set_block = "FLAC__metadata_simple_iterator_set_block";
    FunctionSymbol<FLAC__bool(FLAC__Metadata_SimpleIterator *, FLAC__StreamMetadata *, FLAC__bool use_padding)> FLAC__metadata_simple_iterator_insert_block_after = "FLAC__metadata_simple_iterator_insert_block_after";
    FunctionSymbol<FLAC__bool(FLAC__Metadata_SimpleIterator *, FLAC__bool)> FLAC__metadata_simple_iterator_delete_block = "FLAC__metadata_simple_iterator_delete_block";

    Symbol<const char* const*> FLAC__Metadata_SimpleIteratorStatusString = "FLAC__Metadata_SimpleIteratorStatusString";

}
