#include <nan.h>
#include "dl.hpp"

using namespace v8;
using namespace node;
#include "pointer.hpp"
#include "format.h"

#define _JOIN(a, b) a##b
#define _JOIN2(a,b,c) a##b##c

#define metadataFunction(ret, name, ...) \
typedef ret (*_JOIN2(FLAC__metadata_, name, _t))(__VA_ARGS__); \
static _JOIN2(FLAC__metadata_, name, _t) _JOIN(FLAC__metadata_, name);

extern "C" {
    typedef void FLAC__Metadata_Chain;
    typedef void FLAC__Metadata_Iterator;
    enum FLAC__Metadata_ChainStatus {
        FLAC__METADATA_CHAIN_STATUS_OK = 0, FLAC__METADATA_CHAIN_STATUS_ILLEGAL_INPUT, FLAC__METADATA_CHAIN_STATUS_ERROR_OPENING_FILE, FLAC__METADATA_CHAIN_STATUS_NOT_A_FLAC_FILE,
        FLAC__METADATA_CHAIN_STATUS_NOT_WRITABLE, FLAC__METADATA_CHAIN_STATUS_BAD_METADATA, FLAC__METADATA_CHAIN_STATUS_READ_ERROR, FLAC__METADATA_CHAIN_STATUS_SEEK_ERROR,
        FLAC__METADATA_CHAIN_STATUS_WRITE_ERROR, FLAC__METADATA_CHAIN_STATUS_RENAME_ERROR, FLAC__METADATA_CHAIN_STATUS_UNLINK_ERROR, FLAC__METADATA_CHAIN_STATUS_MEMORY_ALLOCATION_ERROR,
        FLAC__METADATA_CHAIN_STATUS_INTERNAL_ERROR, FLAC__METADATA_CHAIN_STATUS_INVALID_CALLBACKS, FLAC__METADATA_CHAIN_STATUS_READ_WRITE_MISMATCH, FLAC__METADATA_CHAIN_STATUS_WRONG_WRITE_CALL
    };
    metadataFunction(FLAC__Metadata_Chain*, chain_new, void);
    metadataFunction(void, chain_delete, FLAC__Metadata_Chain *chain);
    metadataFunction(FLAC__Metadata_ChainStatus, chain_status, FLAC__Metadata_Chain *chain);
    metadataFunction(FLAC__bool, chain_read, FLAC__Metadata_Chain *chain, const char *filename);
    metadataFunction(FLAC__bool, chain_read_ogg, FLAC__Metadata_Chain *chain, const char *filename);
    //metadataFunction(FLAC__bool, chain_read_with_callbacks, FLAC__Metadata_Chain *chain, FLAC__IOHandle handle, FLAC__IOCallbacks callbacks);
    //metadataFunction(FLAC__bool, chain_read_ogg_with_callbacks, FLAC__Metadata_Chain *chain, FLAC__IOHandle handle, FLAC__IOCallbacks callbacks);
    //metadataFunction(FLAC__bool, chain_check_if_tempfile_needed, FLAC__Metadata_Chain *chain, FLAC__bool use_padding);
    metadataFunction(FLAC__bool, chain_write, FLAC__Metadata_Chain *chain, FLAC__bool use_padding, FLAC__bool preserve_file_stats);
    //metadataFunction(FLAC__bool, chain_write_with_callbacks, FLAC__Metadata_Chain *chain, FLAC__bool use_padding, FLAC__IOHandle handle, FLAC__IOCallbacks callbacks);
    //metadataFunction(FLAC__bool, chain_write_with_callbacks_and_tempfile, FLAC__Metadata_Chain *chain, FLAC__bool use_padding, FLAC__IOHandle handle, FLAC__IOCallbacks callbacks, FLAC__IOHandle temp_handle, FLAC__IOCallbacks temp_callbacks);
    metadataFunction(void, chain_merge_padding, FLAC__Metadata_Chain *chain);
    metadataFunction(void, chain_sort_padding, FLAC__Metadata_Chain *chain);
    metadataFunction(FLAC__Metadata_Iterator*, iterator_new, void);
    metadataFunction(void, iterator_delete, FLAC__Metadata_Iterator *iterator);
    metadataFunction(void, iterator_init, FLAC__Metadata_Iterator *iterator, FLAC__Metadata_Chain *chain);
    metadataFunction(FLAC__bool, iterator_next, FLAC__Metadata_Iterator *iterator);
    metadataFunction(FLAC__bool, iterator_prev, FLAC__Metadata_Iterator *iterator);
    metadataFunction(FLAC__MetadataType, iterator_get_block_type, const FLAC__Metadata_Iterator *iterator);
    metadataFunction(FLAC__StreamMetadata*, iterator_get_block, FLAC__Metadata_Iterator *iterator);
    metadataFunction(FLAC__bool, iterator_set_block, FLAC__Metadata_Iterator *iterator, FLAC__StreamMetadata *block);
    metadataFunction(FLAC__bool, iterator_delete_block, FLAC__Metadata_Iterator *iterator, FLAC__bool replace_with_padding);
    metadataFunction(FLAC__bool, iterator_insert_block_before, FLAC__Metadata_Iterator *iterator, FLAC__StreamMetadata *block);
    metadataFunction(FLAC__bool, iterator_insert_block_after, FLAC__Metadata_Iterator *iterator, FLAC__StreamMetadata *block);
    const char *const* FLAC__Metadata_ChainStatusString;
}

namespace flac_bindings {

    extern Library* libFlac;

    NAN_METHOD(node_FLAC__metadata_chain_new) {
        FLAC__Metadata_Chain* m = FLAC__metadata_chain_new();
        if(m == nullptr) {
            info.GetReturnValue().SetNull();
        } else {
            info.GetReturnValue().Set(WrapPointer(m).ToLocalChecked());
        }
    }

    NAN_METHOD(node_FLAC__metadata_chain_delete) {
        FLAC__Metadata_Chain* m = UnwrapPointer(info[0]);
        FLAC__metadata_chain_delete(m);
    }

    NAN_METHOD(node_FLAC__metadata_chain_status) {
        FLAC__Metadata_Chain* m = UnwrapPointer(info[0]);
        FLAC__Metadata_ChainStatus s = FLAC__metadata_chain_status(m);
        info.GetReturnValue().Set(Nan::New(s));
    }

    NAN_METHOD(node_FLAC__metadata_chain_read) {
        FLAC__Metadata_Chain* m = UnwrapPointer(info[0]);
        Nan::Utf8String str(info[1]);
        FLAC__bool s = FLAC__metadata_chain_read(m, *str);
        info.GetReturnValue().Set(Nan::New<Boolean>(s));
    }

    NAN_METHOD(node_FLAC__metadata_chain_read_ogg) {
        FLAC__Metadata_Chain* m = UnwrapPointer(info[0]);
        Nan::Utf8String str(info[1]);
        FLAC__bool s = FLAC__metadata_chain_read_ogg(m, *str);
        info.GetReturnValue().Set(Nan::New<Boolean>(s));
    }

    NAN_METHOD(node_FLAC__metadata_chain_write) {
        FLAC__Metadata_Chain* m = UnwrapPointer(info[0]);
        FLAC__bool padding = Nan::To<int>(info[1]).FromMaybe(1);
        FLAC__bool preserve = Nan::To<int>(info[2]).FromMaybe(0);
        FLAC__bool r = FLAC__metadata_chain_write(m, padding, preserve);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_chain_merge_padding) {
        FLAC__Metadata_Chain* m = UnwrapPointer(info[0]);
        FLAC__metadata_chain_merge_padding(m);
    }

    NAN_METHOD(node_FLAC__metadata_chain_sort_padding) {
        FLAC__Metadata_Chain* m = UnwrapPointer(info[0]);
        FLAC__metadata_chain_sort_padding(m);
    }

    NAN_METHOD(node_FLAC__metadata_iterator_new) {
        FLAC__Metadata_Iterator* m = FLAC__metadata_iterator_new();
        if(m == nullptr) {
            info.GetReturnValue().SetNull();
        } else {
            info.GetReturnValue().Set(WrapPointer(m).ToLocalChecked());
        }
    }

    NAN_METHOD(node_FLAC__metadata_iterator_delete) {
        FLAC__Metadata_Iterator* m = UnwrapPointer(info[0]);
        FLAC__metadata_iterator_delete(m);
    }

    NAN_METHOD(node_FLAC__metadata_iterator_init) {
        FLAC__Metadata_Iterator* m = UnwrapPointer(info[0]);
        FLAC__Metadata_Chain* n = UnwrapPointer(info[1]);
        FLAC__metadata_iterator_init(m, n);
    }

    NAN_METHOD(node_FLAC__metadata_iterator_next) {
        FLAC__Metadata_Iterator* m = UnwrapPointer(info[0]);
        FLAC__bool r = FLAC__metadata_iterator_next(m);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_iterator_prev) {
        FLAC__Metadata_Iterator* m = UnwrapPointer(info[0]);
        FLAC__bool r = FLAC__metadata_iterator_prev(m);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_iterator_get_block_type) {
        FLAC__Metadata_Iterator* m = UnwrapPointer(info[0]);
        FLAC__MetadataType r = FLAC__metadata_iterator_get_block_type(m);
        info.GetReturnValue().Set(Nan::New<Number>(r));
    }

    NAN_METHOD(node_FLAC__metadata_iterator_get_block) {
        FLAC__Metadata_Iterator* m = UnwrapPointer(info[0]);
        FLAC__StreamMetadata* r = FLAC__metadata_iterator_get_block(m);
        info.GetReturnValue().Set(tojs(r));
    }

    NAN_METHOD(node_FLAC__metadata_iterator_set_block) {
        FLAC__Metadata_Iterator* m = UnwrapPointer(info[0]);
        FLAC__StreamMetadata* n = fromjs<FLAC__StreamMetadata>(info[1]);
        if(n == nullptr) return;
        FLAC__bool r = FLAC__metadata_iterator_set_block(m, n);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_iterator_delete_block) {
        FLAC__Metadata_Iterator* m = UnwrapPointer(info[0]);
        FLAC__bool padding = Nan::To<int>(info[1]).FromMaybe(1);
        FLAC__bool r = FLAC__metadata_iterator_delete_block(m, padding);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_iterator_insert_block_before) {
        FLAC__Metadata_Iterator* m = UnwrapPointer(info[0]);
        FLAC__StreamMetadata* n = fromjs<FLAC__StreamMetadata>(info[1]);
        if(n == nullptr) return;
        FLAC__bool r = FLAC__metadata_iterator_insert_block_before(m, n);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_iterator_insert_block_after) {
        FLAC__Metadata_Iterator* m = UnwrapPointer(info[0]);
        FLAC__StreamMetadata* n = fromjs<FLAC__StreamMetadata>(info[1]);
        if(n == nullptr) return;
        FLAC__bool r = FLAC__metadata_iterator_insert_block_after(m, n);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_PROPERTY_GETTER(ChainStatus) {
        Nan::Utf8String p(property);
        std::string PropertyName(*p);

        if(PropertyName == "OK") info.GetReturnValue().Set(0);
        else if(PropertyName == "ILLEGAL_INPUT") info.GetReturnValue().Set(1);
        else if(PropertyName == "ERROR_OPENING_FILE") info.GetReturnValue().Set(2);
        else if(PropertyName == "NOT_A_FLAC_FILE") info.GetReturnValue().Set(3);
        else if(PropertyName == "NOT_WRITABLE") info.GetReturnValue().Set(4);
        else if(PropertyName == "BAD_METADATA") info.GetReturnValue().Set(5);
        else if(PropertyName == "READ_ERROR") info.GetReturnValue().Set(6);
        else if(PropertyName == "SEEK_ERROR") info.GetReturnValue().Set(7);
        else if(PropertyName == "WRITE_ERROR") info.GetReturnValue().Set(8);
        else if(PropertyName == "RENAME_ERROR") info.GetReturnValue().Set(9);
        else if(PropertyName == "UNLINK_ERROR") info.GetReturnValue().Set(10);
        else if(PropertyName == "MEMORY_ALLOCATION_ERROR") info.GetReturnValue().Set(11);
        else if(PropertyName == "INTERNAL_ERROR") info.GetReturnValue().Set(12);
        else if(PropertyName == "INVALID_CALLBACKS") info.GetReturnValue().Set(13);
        else if(PropertyName == "READ_WRITE_MISMATCH") info.GetReturnValue().Set(14);
        else if(PropertyName == "WRONG_WRITE_CALL") info.GetReturnValue().Set(15);
        else info.GetReturnValue().SetUndefined();
    }

    NAN_INDEX_GETTER(ChainStatusString) {
        if(index < 16) {
            info.GetReturnValue().Set(Nan::New(FLAC__Metadata_ChainStatusString[index]).ToLocalChecked());
        } else {
            info.GetReturnValue().SetUndefined();
        }
    }


    NAN_MODULE_INIT(initMetadata2) {
        Local<Object> obj = Nan::New<Object>();
        #define setMethod(fn) \
        Nan::SetMethod(obj, #fn, _JOIN(node_FLAC__metadata_, fn)); \
        _JOIN(FLAC__metadata_, fn) = libFlac->getSymbolAddress<_JOIN2(FLAC__metadata_, fn, _t)>("FLAC__metadata_" #fn); \
        if(_JOIN(FLAC__metadata_, fn) == nullptr) printf("%s\n", libFlac->getLastError().c_str());

        setMethod(chain_new);
        setMethod(chain_delete);
        setMethod(chain_status);
        setMethod(chain_read);
        setMethod(chain_read_ogg);
        setMethod(chain_write);
        setMethod(chain_merge_padding);
        setMethod(chain_sort_padding);
        setMethod(iterator_new);
        setMethod(iterator_delete);
        setMethod(iterator_init);
        setMethod(iterator_next);
        setMethod(iterator_prev);
        setMethod(iterator_get_block_type);
        setMethod(iterator_get_block);
        setMethod(iterator_set_block);
        setMethod(iterator_delete_block);
        setMethod(iterator_insert_block_before);
        setMethod(iterator_insert_block_after);

        #define propertyGetter(func) \
        Local<ObjectTemplate> _JOIN(func, Var) = Nan::New<ObjectTemplate>(); \
        Nan::SetNamedPropertyHandler(_JOIN(func, Var), func); \
        Nan::Set(obj, Nan::New(#func).ToLocalChecked(), Nan::NewInstance(_JOIN(func, Var)).ToLocalChecked());
        #define indexGetter(func) \
        _JOIN(FLAC__Metadata_, func) = libFlac->getSymbolAddress<const char* const*>("FLAC__Metadata_" #func); \
        Local<ObjectTemplate> _JOIN(func, _template) = Nan::New<ObjectTemplate>(); \
        Nan::SetIndexedPropertyHandler(_JOIN(func, _template), func); \
        Nan::Set(obj, Nan::New(#func).ToLocalChecked(), Nan::NewInstance(_JOIN(func, _template)).ToLocalChecked());

        propertyGetter(ChainStatus);
        indexGetter(ChainStatusString);

        Nan::Set(target, Nan::New("metadata2").ToLocalChecked(), obj);
    }
}
