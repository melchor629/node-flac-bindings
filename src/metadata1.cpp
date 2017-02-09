#include <nan.h>
#include "dl.hpp"

using namespace v8;
using namespace node;
#include "pointer.hpp"
#include "format.h"

#define _JOIN(a, b) a##b
#define _JOIN2(a,b,c) a##b##c

#define metadataFunction(ret, name, ...) \
typedef ret (*_JOIN2(FLAC__metadata_simple_iterator_, name, _t))(__VA_ARGS__); \
static _JOIN2(FLAC__metadata_simple_iterator_, name, _t) _JOIN(FLAC__metadata_simple_iterator_, name);

extern "C" {
    typedef void FLAC__Metadata_SimpleIterator;
    enum  	FLAC__Metadata_SimpleIteratorStatus {
      FLAC__METADATA_SIMPLE_ITERATOR_STATUS_OK = 0, FLAC__METADATA_SIMPLE_ITERATOR_STATUS_ILLEGAL_INPUT, FLAC__METADATA_SIMPLE_ITERATOR_STATUS_ERROR_OPENING_FILE, FLAC__METADATA_SIMPLE_ITERATOR_STATUS_NOT_A_FLAC_FILE,
      FLAC__METADATA_SIMPLE_ITERATOR_STATUS_NOT_WRITABLE, FLAC__METADATA_SIMPLE_ITERATOR_STATUS_BAD_METADATA, FLAC__METADATA_SIMPLE_ITERATOR_STATUS_READ_ERROR, FLAC__METADATA_SIMPLE_ITERATOR_STATUS_SEEK_ERROR,
      FLAC__METADATA_SIMPLE_ITERATOR_STATUS_WRITE_ERROR, FLAC__METADATA_SIMPLE_ITERATOR_STATUS_RENAME_ERROR, FLAC__METADATA_SIMPLE_ITERATOR_STATUS_UNLINK_ERROR, FLAC__METADATA_SIMPLE_ITERATOR_STATUS_MEMORY_ALLOCATION_ERROR,
      FLAC__METADATA_SIMPLE_ITERATOR_STATUS_INTERNAL_ERROR
    };
    metadataFunction(FLAC__Metadata_SimpleIterator*, new, void);
    metadataFunction(void, delete, FLAC__Metadata_SimpleIterator *iterator);
    metadataFunction(FLAC__Metadata_SimpleIteratorStatus, status, FLAC__Metadata_SimpleIterator *iterator);
    metadataFunction(FLAC__bool, init, FLAC__Metadata_SimpleIterator *iterator, const char *filename, FLAC__bool read_only, FLAC__bool preserve_file_stats);
    metadataFunction(FLAC__bool, is_writable, const FLAC__Metadata_SimpleIterator *iterator);
    metadataFunction(FLAC__bool, next, FLAC__Metadata_SimpleIterator *iterator);
    metadataFunction(FLAC__bool, prev, FLAC__Metadata_SimpleIterator *iterator);
    metadataFunction(FLAC__bool, is_last, const FLAC__Metadata_SimpleIterator *iterator);
    metadataFunction(off_t, get_block_offset, const FLAC__Metadata_SimpleIterator *iterator);
    metadataFunction(FLAC__MetadataType, get_block_type, const FLAC__Metadata_SimpleIterator *iterator);
    metadataFunction(unsigned, get_block_length, const FLAC__Metadata_SimpleIterator *iterator);
    metadataFunction(FLAC__bool, get_application_id, FLAC__Metadata_SimpleIterator *iterator, FLAC__byte *id);
    metadataFunction(FLAC__StreamMetadata*, get_block, FLAC__Metadata_SimpleIterator *iterator);
    metadataFunction(FLAC__bool, set_block, FLAC__Metadata_SimpleIterator *iterator, FLAC__StreamMetadata *block, FLAC__bool use_padding);
    metadataFunction(FLAC__bool, insert_block_after, FLAC__Metadata_SimpleIterator *iterator, FLAC__StreamMetadata *block, FLAC__bool use_padding);
    metadataFunction(FLAC__bool, delete_block, FLAC__Metadata_SimpleIterator *iterator, FLAC__bool use_padding);
    const char *const* FLAC__Metadata_SimpleIteratorStatusString;
}

namespace flac_bindings {

    extern Library* libFlac;

    NAN_METHOD(node_FLAC__metadata_simple_iterator_new) {
        FLAC__Metadata_SimpleIterator* it = FLAC__metadata_simple_iterator_new();
        if(it != nullptr) {
            info.GetReturnValue().Set(WrapPointer(it).ToLocalChecked());
        } else {
            info.GetReturnValue().SetNull();
        }
    }

    NAN_METHOD(node_FLAC__metadata_simple_iterator_delete) {
        FLAC__Metadata_SimpleIterator* it = UnwrapPointer(info[0]);
        FLAC__metadata_simple_iterator_delete(it);
    }

    NAN_METHOD(node_FLAC__metadata_simple_iterator_status) {
        FLAC__Metadata_SimpleIterator* it = UnwrapPointer(info[0]);
        FLAC__Metadata_SimpleIteratorStatus s = FLAC__metadata_simple_iterator_status(it);
        info.GetReturnValue().Set(Nan::New<Number>(s));
    }

    NAN_METHOD(node_FLAC__metadata_simple_iterator_init) {
        FLAC__Metadata_SimpleIterator* it = UnwrapPointer(info[0]);
        Nan::Utf8String filename(info[1]);
        FLAC__bool read_only = Nan::To<int>(info[2]).FromMaybe(0);
        FLAC__bool preserve = Nan::To<int>(info[3]).FromMaybe(0);
        FLAC__bool r = FLAC__metadata_simple_iterator_init(it, *filename, read_only, preserve);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_simple_iterator_is_writable) {
        FLAC__Metadata_SimpleIterator* it = UnwrapPointer(info[0]);
        FLAC__bool r = FLAC__metadata_simple_iterator_is_writable(it);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_simple_iterator_next) {
        FLAC__Metadata_SimpleIterator* it = UnwrapPointer(info[0]);
        FLAC__bool r = FLAC__metadata_simple_iterator_next(it);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_simple_iterator_prev) {
        FLAC__Metadata_SimpleIterator* it = UnwrapPointer(info[0]);
        FLAC__bool r = FLAC__metadata_simple_iterator_prev(it);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_simple_iterator_is_last) {
        FLAC__Metadata_SimpleIterator* it = UnwrapPointer(info[0]);
        FLAC__bool r = FLAC__metadata_simple_iterator_is_last(it);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_simple_iterator_get_block_offset) {
        FLAC__Metadata_SimpleIterator* it = UnwrapPointer(info[0]);
        off_t r = FLAC__metadata_simple_iterator_get_block_offset(it);
        info.GetReturnValue().Set(Nan::New<Number>(r));
    }

    NAN_METHOD(node_FLAC__metadata_simple_iterator_get_block_type) {
        FLAC__Metadata_SimpleIterator* it = UnwrapPointer(info[0]);
        FLAC__MetadataType r = FLAC__metadata_simple_iterator_get_block_type(it);
        info.GetReturnValue().Set(Nan::New<Number>(r));
    }

    NAN_METHOD(node_FLAC__metadata_simple_iterator_get_block_length) {
        FLAC__Metadata_SimpleIterator* it = UnwrapPointer(info[0]);
        unsigned r = FLAC__metadata_simple_iterator_get_block_length(it);
        info.GetReturnValue().Set(Nan::New<Number>(r));
    }

    NAN_METHOD(node_FLAC__metadata_simple_iterator_get_application_id) {
        FLAC__Metadata_SimpleIterator* it = UnwrapPointer(info[0]);
        FLAC__byte id[4];
        FLAC__bool r = FLAC__metadata_simple_iterator_get_application_id(it, id);
        if(r) {
            Local<Array> arr = Nan::New<Array>();
            Nan::Set(arr, 0, Nan::New<Number>(id[0]));
            Nan::Set(arr, 1, Nan::New<Number>(id[1]));
            Nan::Set(arr, 2, Nan::New<Number>(id[2]));
            Nan::Set(arr, 3, Nan::New<Number>(id[3]));
            info.GetReturnValue().Set(arr);
        } else {
            info.GetReturnValue().Set(Nan::New<Boolean>(false));
        }
    }

    NAN_METHOD(node_FLAC__metadata_simple_iterator_get_block) {
        FLAC__Metadata_SimpleIterator* it = UnwrapPointer(info[0]);
        FLAC__StreamMetadata* r = FLAC__metadata_simple_iterator_get_block(it);
        info.GetReturnValue().Set(structToJs(r));
    }

    NAN_METHOD(node_FLAC__metadata_simple_iterator_set_block) {
        FLAC__Metadata_SimpleIterator* it = UnwrapPointer(info[0]);
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[1]);
        if(m == nullptr) return;
        FLAC__bool pad = Nan::To<int>(info[2]).FromMaybe(1);
        FLAC__bool r = FLAC__metadata_simple_iterator_set_block(it, m, pad);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_simple_iterator_insert_block_after) {
        FLAC__Metadata_SimpleIterator* it = UnwrapPointer(info[0]);
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[1]);
        if(m == nullptr) return;
        FLAC__bool pad = Nan::To<int>(info[2]).FromMaybe(1);
        FLAC__bool r = FLAC__metadata_simple_iterator_insert_block_after(it, m, pad);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_simple_iterator_delete_block) {
        FLAC__Metadata_SimpleIterator* it = UnwrapPointer(info[0]);
        FLAC__bool pad = Nan::To<int>(info[1]).FromMaybe(1);
        FLAC__bool r = FLAC__metadata_simple_iterator_delete_block(it, pad);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_PROPERTY_GETTER(SimpleIteratorStatus) {
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
        else info.GetReturnValue().SetUndefined();
    }

    NAN_INDEX_GETTER(SimpleIteratorStatusString) {
        if(index < 13) {
            info.GetReturnValue().Set(Nan::New(FLAC__Metadata_SimpleIteratorStatusString[index]).ToLocalChecked());
        } else {
            info.GetReturnValue().SetUndefined();
        }
    }

    NAN_INDEX_ENUMERATOR(SimpleIteratorStatusString) {
        Local<Array> array = Nan::New<Array>();
        for(int i = 0; i < 13; i++) Nan::Set(array, i, Nan::New(i));
        info.GetReturnValue().Set(array);
    }

    NAN_MODULE_INIT(initMetadata1) {
        Local<Object> obj = Nan::New<Object>();
        #define setMethod(fn) \
        Nan::SetMethod(obj, #fn, _JOIN(node_FLAC__metadata_simple_iterator_, fn)); \
        _JOIN(FLAC__metadata_simple_iterator_, fn) = libFlac->getSymbolAddress<_JOIN2(FLAC__metadata_simple_iterator_, fn, _t)>("FLAC__metadata_simple_iterator_" #fn); \
        if(_JOIN(FLAC__metadata_simple_iterator_, fn) == nullptr) printf("%s\n", libFlac->getLastError().c_str());

        setMethod(new);
        setMethod(delete);
        setMethod(status);
        setMethod(init);
        setMethod(is_writable);
        setMethod(next);
        setMethod(prev);
        setMethod(is_last);
        setMethod(get_block_offset);
        setMethod(get_block_type);
        setMethod(get_block_length);
        setMethod(get_application_id);
        setMethod(get_block);
        setMethod(set_block);
        setMethod(insert_block_after);
        setMethod(delete_block);

        #define propertyGetter(func) \
        Local<ObjectTemplate> _JOIN(func, Var) = Nan::New<ObjectTemplate>(); \
        Nan::SetNamedPropertyHandler(_JOIN(func, Var), func); \
        Nan::Set(obj, Nan::New(#func).ToLocalChecked(), Nan::NewInstance(_JOIN(func, Var)).ToLocalChecked());
        #define indexGetter(func) \
        _JOIN(FLAC__Metadata_, func) = libFlac->getSymbolAddress<const char* const*>("FLAC__Metadata_" #func); \
        Local<ObjectTemplate> _JOIN(func, _template) = Nan::New<ObjectTemplate>(); \
        Nan::SetIndexedPropertyHandler(_JOIN(func, _template), func, nullptr, nullptr, nullptr, func); \
        Nan::Set(obj, Nan::New(#func).ToLocalChecked(), Nan::NewInstance(_JOIN(func, _template)).ToLocalChecked());

        propertyGetter(SimpleIteratorStatus);
        indexGetter(SimpleIteratorStatusString);


        Nan::Set(target, Nan::New("metadata1").ToLocalChecked(), obj);
    }
}
