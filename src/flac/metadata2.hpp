#pragma once

#include "format.h"
#include "../utils/symbol.hpp"

extern "C" {
    typedef void* FLAC__IOHandle;
    typedef struct FLAC__IOCallbacks {
        size_t(*read)(void *ptr, size_t size, size_t nmemb, FLAC__IOHandle handle);
        size_t(*write)(const void *ptr, size_t size, size_t nmemb, FLAC__IOHandle handle);
        int(*seek)(FLAC__IOHandle handle, int64_t offset, int whence);
        int64_t(*tell)(FLAC__IOHandle handle);
        int(*eof)(FLAC__IOHandle handle);
        int(*close)(FLAC__IOHandle handle);
    } FLAC__IOCallbacks;
}

namespace flac_bindings {

    typedef void FLAC__Metadata_Chain;
    typedef void FLAC__Metadata_Iterator;
    enum FLAC__Metadata_ChainStatus {
        FLAC__METADATA_CHAIN_STATUS_OK = 0,
        FLAC__METADATA_CHAIN_STATUS_ILLEGAL_INPUT,
        FLAC__METADATA_CHAIN_STATUS_ERROR_OPENING_FILE,
        FLAC__METADATA_CHAIN_STATUS_NOT_A_FLAC_FILE,
        FLAC__METADATA_CHAIN_STATUS_NOT_WRITABLE,
        FLAC__METADATA_CHAIN_STATUS_BAD_METADATA,
        FLAC__METADATA_CHAIN_STATUS_READ_ERROR,
        FLAC__METADATA_CHAIN_STATUS_SEEK_ERROR,
        FLAC__METADATA_CHAIN_STATUS_WRITE_ERROR,
        FLAC__METADATA_CHAIN_STATUS_RENAME_ERROR,
        FLAC__METADATA_CHAIN_STATUS_UNLINK_ERROR,
        FLAC__METADATA_CHAIN_STATUS_MEMORY_ALLOCATION_ERROR,
        FLAC__METADATA_CHAIN_STATUS_INTERNAL_ERROR,
        FLAC__METADATA_CHAIN_STATUS_INVALID_CALLBACKS,
        FLAC__METADATA_CHAIN_STATUS_READ_WRITE_MISMATCH,
        FLAC__METADATA_CHAIN_STATUS_WRONG_WRITE_CALL
    };

    extern FunctionSymbol<FLAC__Metadata_Chain*(void)> FLAC__metadata_chain_new;
    extern FunctionSymbol<void(FLAC__Metadata_Chain *)> FLAC__metadata_chain_delete;
    extern FunctionSymbol<FLAC__Metadata_ChainStatus(FLAC__Metadata_Chain *)> FLAC__metadata_chain_status;
    extern FunctionSymbol<FLAC__bool(FLAC__Metadata_Chain *, const char *)> FLAC__metadata_chain_read;
    extern FunctionSymbol<FLAC__bool(FLAC__Metadata_Chain *, const char *)> FLAC__metadata_chain_read_ogg;
    extern FunctionSymbol<FLAC__bool(FLAC__Metadata_Chain *, FLAC__IOHandle, FLAC__IOCallbacks)> FLAC__metadata_chain_read_with_callbacks;
    extern FunctionSymbol<FLAC__bool(FLAC__Metadata_Chain *, FLAC__IOHandle, FLAC__IOCallbacks)> FLAC__metadata_chain_read_ogg_with_callbacks;
    extern FunctionSymbol<FLAC__bool(FLAC__Metadata_Chain *, FLAC__bool)> FLAC__metadata_chain_check_if_tempfile_needed;
    extern FunctionSymbol<FLAC__bool(FLAC__Metadata_Chain *, FLAC__bool, FLAC__bool)> FLAC__metadata_chain_write;
    extern FunctionSymbol<FLAC__bool(FLAC__Metadata_Chain *, FLAC__bool, FLAC__IOHandle, FLAC__IOCallbacks)> FLAC__metadata_chain_write_with_callbacks;
    extern FunctionSymbol<FLAC__bool(FLAC__Metadata_Chain *, FLAC__bool, FLAC__IOHandle, FLAC__IOCallbacks, FLAC__IOHandle, FLAC__IOCallbacks)> FLAC__metadata_chain_write_with_callbacks_and_tempfile;
    extern FunctionSymbol<void(FLAC__Metadata_Chain *)> FLAC__metadata_chain_merge_padding;
    extern FunctionSymbol<void(FLAC__Metadata_Chain *)> FLAC__metadata_chain_sort_padding;
    extern FunctionSymbol<FLAC__Metadata_Iterator*(void)> FLAC__metadata_iterator_new;
    extern FunctionSymbol<void(FLAC__Metadata_Iterator *)> FLAC__metadata_iterator_delete;
    extern FunctionSymbol<void(FLAC__Metadata_Iterator *, FLAC__Metadata_Chain *chain)> FLAC__metadata_iterator_init;
    extern FunctionSymbol<FLAC__bool(FLAC__Metadata_Iterator *)> FLAC__metadata_iterator_next;
    extern FunctionSymbol<FLAC__bool(FLAC__Metadata_Iterator *)> FLAC__metadata_iterator_prev;
    extern FunctionSymbol<FLAC__MetadataType(const FLAC__Metadata_Iterator *)> FLAC__metadata_iterator_get_block_type;
    extern FunctionSymbol<FLAC__StreamMetadata*(FLAC__Metadata_Iterator *)> FLAC__metadata_iterator_get_block;
    extern FunctionSymbol<FLAC__bool(FLAC__Metadata_Iterator *, FLAC__StreamMetadata *)> FLAC__metadata_iterator_set_block;
    extern FunctionSymbol<FLAC__bool(FLAC__Metadata_Iterator *, FLAC__bool)> FLAC__metadata_iterator_delete_block;
    extern FunctionSymbol<FLAC__bool(FLAC__Metadata_Iterator *, FLAC__StreamMetadata *)> FLAC__metadata_iterator_insert_block_before;
    extern FunctionSymbol<FLAC__bool(FLAC__Metadata_Iterator *, FLAC__StreamMetadata *)> FLAC__metadata_iterator_insert_block_after;

    extern Symbol<const char* const*> FLAC__Metadata_ChainStatusString;

}
