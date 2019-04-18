#include <nan.h>
#include "../utils/dl.hpp"

using namespace v8;
using namespace node;
#include "../utils/pointer.hpp"
#include "../format/format.h"
#include "../utils/defs.hpp"
#include "../mappings/mappings.hpp"

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
}

#define UNWRAP_IT \
SimpleIterator* self = Nan::ObjectWrap::Unwrap<SimpleIterator>(info.Holder()); \
FLAC__Metadata_SimpleIterator* it = self->it;

namespace flac_bindings {

    extern Library* libFlac;

    class SimpleIterator: public Nan::ObjectWrap {

        FLAC__Metadata_SimpleIterator* it;

        static NAN_METHOD(node_FLAC__metadata_simple_iterator_new) {
            if(throwIfNotConstructorCall(info)) return;
            FLAC__Metadata_SimpleIterator* it = FLAC__metadata_simple_iterator_new();
            if(it != nullptr) {
                SimpleIterator* si = new SimpleIterator;
                si->it = it;
                si->Wrap(info.This());
                Nan::Set(info.This(), Symbol::GetIterator(info.GetIsolate()), Nan::New<Function>(jsIterator));
                info.GetReturnValue().Set(info.This());
            } else {
                Nan::ThrowError("Could not allocate memory");
            }
        }

        static NAN_METHOD(jsIterator) {
            UNWRAP_IT
            Local<Object> obj = Nan::New<Object>();
            Nan::Set(obj, Nan::New("it").ToLocalChecked(), WrapPointer(it).ToLocalChecked());
            Nan::Set(obj, Nan::New("rt").ToLocalChecked(), Nan::True());
            Nan::SetMethod(obj, "next", [] (Nan::NAN_METHOD_ARGS_TYPE info) -> void {
                MaybeLocal<Value> parent = Nan::Get(info.This(), Nan::New("it").ToLocalChecked());
                MaybeLocal<Value> rt = Nan::Get(info.This(), Nan::New("rt").ToLocalChecked());
                if(parent.IsEmpty() || !parent.ToLocalChecked()->IsObject()) {
                    Nan::ThrowTypeError("Unexpected this type for iterator");
                    return;
                }

                FLAC__Metadata_SimpleIterator* it = UnwrapPointer<FLAC__Metadata_SimpleIterator>(parent.ToLocalChecked());
                bool prevReturn = rt.ToLocalChecked()->BooleanValue(Isolate::GetCurrent()->GetCurrentContext()).FromJust();
                Local<Object> ret = Nan::New<Object>();
                if(!prevReturn) {
                    Nan::Set(ret, Nan::New("done").ToLocalChecked(), Nan::True());
                } else {
                    FLAC__StreamMetadata* metadata = FLAC__metadata_simple_iterator_get_block(it);
                    Nan::Set(ret, Nan::New("value").ToLocalChecked(), structToJs(metadata));
                    Nan::Set(ret, Nan::New("done").ToLocalChecked(), Nan::False());
                    prevReturn = FLAC__metadata_simple_iterator_next(it);
                }
                Nan::Set(info.This(), Nan::New("rt").ToLocalChecked(), Nan::New<Boolean>(prevReturn));
                info.GetReturnValue().Set(ret);
            });

            while(FLAC__metadata_simple_iterator_prev(it));
            info.GetReturnValue().Set(obj);
        }

        static NAN_METHOD(node_FLAC__metadata_simple_iterator_delete) {
            UNWRAP_IT
            FLAC__metadata_simple_iterator_delete(it);
        }

        static NAN_METHOD(node_FLAC__metadata_simple_iterator_status) {
            UNWRAP_IT
            FLAC__Metadata_SimpleIteratorStatus s = FLAC__metadata_simple_iterator_status(it);
            info.GetReturnValue().Set(numberToJs<int>(s));
        }

        static NAN_METHOD(node_FLAC__metadata_simple_iterator_init) {
            UNWRAP_IT
            if(!info[0]->IsString()) {
                Nan::ThrowTypeError("Expected first argument to be string");
                return;
            }

            Nan::Utf8String filename(info[0]);
            FLAC__bool read_only = booleanFromJs<FLAC__bool>(info[1]).FromMaybe(false);
            FLAC__bool preserve = booleanFromJs<FLAC__bool>(info[2]).FromMaybe(false);
            FLAC__bool r = FLAC__metadata_simple_iterator_init(it, *filename, read_only, preserve);
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
        }

        static NAN_METHOD(node_FLAC__metadata_simple_iterator_is_writable) {
            UNWRAP_IT
            FLAC__bool r = FLAC__metadata_simple_iterator_is_writable(it);
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
        }

        static NAN_METHOD(node_FLAC__metadata_simple_iterator_next) {
            UNWRAP_IT
            FLAC__bool r = FLAC__metadata_simple_iterator_next(it);
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
        }

        static NAN_METHOD(node_FLAC__metadata_simple_iterator_prev) {
            UNWRAP_IT
            FLAC__bool r = FLAC__metadata_simple_iterator_prev(it);
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
        }

        static NAN_METHOD(node_FLAC__metadata_simple_iterator_is_last) {
            UNWRAP_IT
            FLAC__bool r = FLAC__metadata_simple_iterator_is_last(it);
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
        }

        static NAN_METHOD(node_FLAC__metadata_simple_iterator_get_block_offset) {
            UNWRAP_IT
            off_t r = FLAC__metadata_simple_iterator_get_block_offset(it);
            info.GetReturnValue().Set(numberToJs(r));
        }

        static NAN_METHOD(node_FLAC__metadata_simple_iterator_get_block_type) {
            UNWRAP_IT
            FLAC__MetadataType r = FLAC__metadata_simple_iterator_get_block_type(it);
            info.GetReturnValue().Set(numberToJs<int>(r));
        }

        static NAN_METHOD(node_FLAC__metadata_simple_iterator_get_block_length) {
            UNWRAP_IT
            unsigned r = FLAC__metadata_simple_iterator_get_block_length(it);
            info.GetReturnValue().Set(numberToJs(r));
        }

        static NAN_METHOD(node_FLAC__metadata_simple_iterator_get_application_id) {
            UNWRAP_IT
            FLAC__byte id[4];
            FLAC__bool r = FLAC__metadata_simple_iterator_get_application_id(it, id);
            if(r) {
                Local<Array> arr = Nan::New<Array>();
                Nan::Set(arr, 0, numberToJs(id[0]));
                Nan::Set(arr, 1, numberToJs(id[1]));
                Nan::Set(arr, 2, numberToJs(id[2]));
                Nan::Set(arr, 3, numberToJs(id[3]));
                info.GetReturnValue().Set(arr);
            } else {
                info.GetReturnValue().Set(Nan::New<Boolean>(false));
            }
        }

        static NAN_METHOD(node_FLAC__metadata_simple_iterator_get_block) {
            UNWRAP_IT
            FLAC__StreamMetadata* r = FLAC__metadata_simple_iterator_get_block(it);
            info.GetReturnValue().Set(structToJs(r));
        }

        static NAN_METHOD(node_FLAC__metadata_simple_iterator_set_block) {
            UNWRAP_IT
            FLAC__StreamMetadata* m = jsToStruct<FLAC__StreamMetadata>(info[0]);
            if(m == nullptr) {
                Nan::ThrowTypeError("Unknown metadata object");
                return;
            }
            FLAC__bool pad = numberFromJs<int>(info[1]).FromMaybe(1);
            FLAC__bool r = FLAC__metadata_simple_iterator_set_block(it, m, pad);
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
        }

        static NAN_METHOD(node_FLAC__metadata_simple_iterator_insert_block_after) {
            UNWRAP_IT
            FLAC__StreamMetadata* m = jsToStruct<FLAC__StreamMetadata>(info[0]);
            if(m == nullptr) {
                Nan::ThrowTypeError("Unknown metadata object");
                return;
            }
            FLAC__bool pad = numberFromJs<int>(info[1]).FromMaybe(1);
            FLAC__bool r = FLAC__metadata_simple_iterator_insert_block_after(it, m, pad);
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
        }

        static NAN_METHOD(node_FLAC__metadata_simple_iterator_delete_block) {
            UNWRAP_IT
            FLAC__bool pad = numberFromJs<int>(info[0]).FromMaybe(1);
            FLAC__bool r = FLAC__metadata_simple_iterator_delete_block(it, pad);
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
        }

        static FlacEnumDefineReturnType createStatusEnum() {
            Local<Object> obj1 = Nan::New<Object>();
            Local<Object> obj2 = Nan::New<Object>();
            flacEnum_defineValue(obj1, obj2, "OK", 0);
            flacEnum_defineValue(obj1, obj2, "ILLEGAL_INPUT", 1);
            flacEnum_defineValue(obj1, obj2, "ERROR_OPENING_FILE", 2);
            flacEnum_defineValue(obj1, obj2, "NOT_A_FLAC_FILE", 3);
            flacEnum_defineValue(obj1, obj2, "NOT_WRITABLE", 4);
            flacEnum_defineValue(obj1, obj2, "BAD_METADATA", 5);
            flacEnum_defineValue(obj1, obj2, "READ_ERROR", 6);
            flacEnum_defineValue(obj1, obj2, "SEEK_ERROR", 7);
            flacEnum_defineValue(obj1, obj2, "WRITE_ERROR", 8);
            flacEnum_defineValue(obj1, obj2, "RENAME_ERROR", 9);
            flacEnum_defineValue(obj1, obj2, "UNLINK_ERROR", 10);
            flacEnum_defineValue(obj1, obj2, "MEMORY_ALLOCATION_ERROR", 11);
            flacEnum_defineValue(obj1, obj2, "INTERNAL_ERROR", 12);
            return std::make_tuple(obj1, obj2);
        }

    public:

        static NAN_MODULE_INIT(initMetadata1);
        ~SimpleIterator();

    };

    SimpleIterator::~SimpleIterator() {
        FLAC__metadata_simple_iterator_delete(it);
    }

    NAN_MODULE_INIT(SimpleIterator::initMetadata1) {
        Local<FunctionTemplate> classFunction = Nan::New<FunctionTemplate>(node_FLAC__metadata_simple_iterator_new);
        classFunction->SetClassName(Nan::New("SimpleIterator").ToLocalChecked());
        classFunction->InstanceTemplate()->SetInternalFieldCount(1);

        #define loadFunction(fn) \
        _JOIN(FLAC__metadata_simple_iterator_, fn) = libFlac->getSymbolAddress<_JOIN2(FLAC__metadata_simple_iterator_, fn, _t)>("FLAC__metadata_simple_iterator_" #fn); \
        if(_JOIN(FLAC__metadata_simple_iterator_, fn) == nullptr) printf("%s\n", libFlac->getLastError().c_str());
        #define setMethod(fn, jsFn) \
        Nan::SetPrototypeMethod(classFunction, #jsFn, _JOIN(node_FLAC__metadata_simple_iterator_, fn)); \
        loadFunction(fn)

        loadFunction(new);
        loadFunction(delete);
        setMethod(status, status);
        setMethod(init, init);
        setMethod(is_writable, isWritable);
        setMethod(next, next);
        setMethod(prev, prev);
        setMethod(is_last, isLast);
        setMethod(get_block_offset, getBlockOffset);
        setMethod(get_block_type, getBlockType);
        setMethod(get_block_length, getBlockLength);
        setMethod(get_application_id, getApplicationId);
        setMethod(get_block, getBlock);
        setMethod(set_block, setBlock);
        setMethod(insert_block_after, insertBlockAfter);
        setMethod(delete_block, deleteBlock);

        Local<Object> obj = Nan::GetFunction(classFunction).ToLocalChecked();

        flacEnum_declareInObject(obj, Status, createStatusEnum());


        Nan::Set(target, Nan::New("SimpleIterator").ToLocalChecked(), obj);
    }
}
