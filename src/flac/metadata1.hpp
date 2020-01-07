#pragma once

#include "format.h"
#include "../utils/symbol.hpp"

namespace flac_bindings {

    typedef void FLAC__Metadata_SimpleIterator;
    enum FLAC__Metadata_SimpleIteratorStatus {
        FLAC__METADATA_SIMPLE_ITERATOR_STATUS_OK = 0,
        FLAC__METADATA_SIMPLE_ITERATOR_STATUS_ILLEGAL_INPUT,
        FLAC__METADATA_SIMPLE_ITERATOR_STATUS_ERROR_OPENING_FILE,
        FLAC__METADATA_SIMPLE_ITERATOR_STATUS_NOT_A_FLAC_FILE,
        FLAC__METADATA_SIMPLE_ITERATOR_STATUS_NOT_WRITABLE,
        FLAC__METADATA_SIMPLE_ITERATOR_STATUS_BAD_METADATA,
        FLAC__METADATA_SIMPLE_ITERATOR_STATUS_READ_ERROR,
        FLAC__METADATA_SIMPLE_ITERATOR_STATUS_SEEK_ERROR,
        FLAC__METADATA_SIMPLE_ITERATOR_STATUS_WRITE_ERROR,
        FLAC__METADATA_SIMPLE_ITERATOR_STATUS_RENAME_ERROR,
        FLAC__METADATA_SIMPLE_ITERATOR_STATUS_UNLINK_ERROR,
        FLAC__METADATA_SIMPLE_ITERATOR_STATUS_MEMORY_ALLOCATION_ERROR,
        FLAC__METADATA_SIMPLE_ITERATOR_STATUS_INTERNAL_ERROR
    };

    extern FunctionSymbol<FLAC__Metadata_SimpleIterator*(void)> FLAC__metadata_simple_iterator_new;
    extern FunctionSymbol<void(FLAC__Metadata_SimpleIterator *)> FLAC__metadata_simple_iterator_delete;
    extern FunctionSymbol<FLAC__Metadata_SimpleIteratorStatus(FLAC__Metadata_SimpleIterator *)> FLAC__metadata_simple_iterator_status;
    extern FunctionSymbol<FLAC__bool(FLAC__Metadata_SimpleIterator *, const char *, FLAC__bool, FLAC__bool)> FLAC__metadata_simple_iterator_init;
    extern FunctionSymbol<FLAC__bool(const FLAC__Metadata_SimpleIterator *)> FLAC__metadata_simple_iterator_is_writable;
    extern FunctionSymbol<FLAC__bool(FLAC__Metadata_SimpleIterator *)> FLAC__metadata_simple_iterator_next;
    extern FunctionSymbol<FLAC__bool(FLAC__Metadata_SimpleIterator *)> FLAC__metadata_simple_iterator_prev;
    extern FunctionSymbol<FLAC__bool(const FLAC__Metadata_SimpleIterator *)> FLAC__metadata_simple_iterator_is_last;
    extern FunctionSymbol<off_t(const FLAC__Metadata_SimpleIterator *)> FLAC__metadata_simple_iterator_get_block_offset;
    extern FunctionSymbol<FLAC__MetadataType(const FLAC__Metadata_SimpleIterator *)> FLAC__metadata_simple_iterator_get_block_type;
    extern FunctionSymbol<unsigned(const FLAC__Metadata_SimpleIterator *)> FLAC__metadata_simple_iterator_get_block_length;
    extern FunctionSymbol<FLAC__bool(FLAC__Metadata_SimpleIterator *, FLAC__byte *)> FLAC__metadata_simple_iterator_get_application_id;
    extern FunctionSymbol<FLAC__StreamMetadata*(FLAC__Metadata_SimpleIterator *)> FLAC__metadata_simple_iterator_get_block;
    extern FunctionSymbol<FLAC__bool(FLAC__Metadata_SimpleIterator *, FLAC__StreamMetadata *, FLAC__bool use_padding)> FLAC__metadata_simple_iterator_set_block;
    extern FunctionSymbol<FLAC__bool(FLAC__Metadata_SimpleIterator *, FLAC__StreamMetadata *, FLAC__bool use_padding)> FLAC__metadata_simple_iterator_insert_block_after;
    extern FunctionSymbol<FLAC__bool(FLAC__Metadata_SimpleIterator *, FLAC__bool)> FLAC__metadata_simple_iterator_delete_block;

    extern Symbol<const char* const*> FLAC__Metadata_SimpleIteratorStatusString;

}
