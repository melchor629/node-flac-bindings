#include "../metadata2.hpp"

namespace flac_bindings {

    FunctionSymbol<FLAC__Metadata_Chain*(void)> FLAC__metadata_chain_new = "FLAC__metadata_chain_new";
    FunctionSymbol<void(FLAC__Metadata_Chain *)> FLAC__metadata_chain_delete = "FLAC__metadata_chain_delete";
    FunctionSymbol<FLAC__Metadata_ChainStatus(FLAC__Metadata_Chain *)> FLAC__metadata_chain_status = "FLAC__metadata_chain_status";
    FunctionSymbol<FLAC__bool(FLAC__Metadata_Chain *, const char *)> FLAC__metadata_chain_read = "FLAC__metadata_chain_read";
    FunctionSymbol<FLAC__bool(FLAC__Metadata_Chain *, const char *)> FLAC__metadata_chain_read_ogg = "FLAC__metadata_chain_read_ogg";
    FunctionSymbol<FLAC__bool(FLAC__Metadata_Chain *, FLAC__IOHandle, FLAC__IOCallbacks)> FLAC__metadata_chain_read_with_callbacks = "FLAC__metadata_chain_read_with_callbacks";
    FunctionSymbol<FLAC__bool(FLAC__Metadata_Chain *, FLAC__IOHandle, FLAC__IOCallbacks)> FLAC__metadata_chain_read_ogg_with_callbacks = "FLAC__metadata_chain_read_ogg_with_callbacks";
    FunctionSymbol<FLAC__bool(FLAC__Metadata_Chain *, FLAC__bool)> FLAC__metadata_chain_check_if_tempfile_needed = "FLAC__metadata_chain_check_if_tempfile_needed";
    FunctionSymbol<FLAC__bool(FLAC__Metadata_Chain *, FLAC__bool, FLAC__bool)> FLAC__metadata_chain_write = "FLAC__metadata_chain_write";
    FunctionSymbol<FLAC__bool(FLAC__Metadata_Chain *, FLAC__bool, FLAC__IOHandle, FLAC__IOCallbacks)> FLAC__metadata_chain_write_with_callbacks = "FLAC__metadata_chain_write_with_callbacks";
    FunctionSymbol<FLAC__bool(FLAC__Metadata_Chain *, FLAC__bool, FLAC__IOHandle, FLAC__IOCallbacks, FLAC__IOHandle, FLAC__IOCallbacks)> FLAC__metadata_chain_write_with_callbacks_and_tempfile = "FLAC__metadata_chain_write_with_callbacks_and_tempfile";
    FunctionSymbol<void(FLAC__Metadata_Chain *)> FLAC__metadata_chain_merge_padding = "FLAC__metadata_chain_merge_padding";
    FunctionSymbol<void(FLAC__Metadata_Chain *)> FLAC__metadata_chain_sort_padding = "FLAC__metadata_chain_sort_padding";
    FunctionSymbol<FLAC__Metadata_Iterator*(void)> FLAC__metadata_iterator_new = "FLAC__metadata_iterator_new";
    FunctionSymbol<void(FLAC__Metadata_Iterator *)> FLAC__metadata_iterator_delete = "FLAC__metadata_iterator_delete";
    FunctionSymbol<void(FLAC__Metadata_Iterator *, FLAC__Metadata_Chain *chain)> FLAC__metadata_iterator_init = "FLAC__metadata_iterator_init";
    FunctionSymbol<FLAC__bool(FLAC__Metadata_Iterator *)> FLAC__metadata_iterator_next = "FLAC__metadata_iterator_next";
    FunctionSymbol<FLAC__bool(FLAC__Metadata_Iterator *)> FLAC__metadata_iterator_prev = "FLAC__metadata_iterator_prev";
    FunctionSymbol<FLAC__MetadataType(const FLAC__Metadata_Iterator *)> FLAC__metadata_iterator_get_block_type = "FLAC__metadata_iterator_get_block_type";
    FunctionSymbol<FLAC__StreamMetadata*(FLAC__Metadata_Iterator *)> FLAC__metadata_iterator_get_block = "FLAC__metadata_iterator_get_block";
    FunctionSymbol<FLAC__bool(FLAC__Metadata_Iterator *, FLAC__StreamMetadata *)> FLAC__metadata_iterator_set_block = "FLAC__metadata_iterator_set_block";
    FunctionSymbol<FLAC__bool(FLAC__Metadata_Iterator *, FLAC__bool)> FLAC__metadata_iterator_delete_block = "FLAC__metadata_iterator_delete_block";
    FunctionSymbol<FLAC__bool(FLAC__Metadata_Iterator *, FLAC__StreamMetadata *)> FLAC__metadata_iterator_insert_block_before = "FLAC__metadata_iterator_insert_block_before";
    FunctionSymbol<FLAC__bool(FLAC__Metadata_Iterator *, FLAC__StreamMetadata *)> FLAC__metadata_iterator_insert_block_after = "FLAC__metadata_iterator_insert_block_after";

    Symbol<const char* const*> FLAC__Metadata_ChainStatusString = "FLAC__Metadata_ChainStatusString";

}
