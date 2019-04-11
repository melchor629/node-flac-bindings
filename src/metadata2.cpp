#include <nan.h>
#include "dl.hpp"

using namespace v8;
using namespace node;
#include "pointer.hpp"
#include "format.h"
#include "defs.hpp"
#include "mappings/mappings.hpp"

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
}

#define UNWRAP_CHAIN \
Chain* self = Nan::ObjectWrap::Unwrap<Chain>(info.Holder()); \
FLAC__Metadata_Chain* m = self->m;

#define UNWRAP_IT \
Iterator* self = Nan::ObjectWrap::Unwrap<Iterator>(info.Holder()); \
FLAC__Metadata_Iterator* m = self->m;

namespace flac_bindings {

    extern Library* libFlac;

    class Chain: public Nan::ObjectWrap {

        static NAN_METHOD(node_FLAC__metadata_chain_new) {
            if(throwIfNotConstructorCall(info)) return;
            FLAC__Metadata_Chain* m = FLAC__metadata_chain_new();
            if(m != nullptr) {
                Chain* chain = new Chain;
                chain->m = m;
                chain->Wrap(info.This());
                info.GetReturnValue().Set(info.This());
            } else {
                Nan::ThrowError("Could not allocate memory");
            }
        }

        static NAN_METHOD(node_FLAC__metadata_chain_status) {
            UNWRAP_CHAIN
            FLAC__Metadata_ChainStatus s = FLAC__metadata_chain_status(m);
            info.GetReturnValue().Set(Nan::New(s));
        }

        static NAN_METHOD(node_FLAC__metadata_chain_read) {
            UNWRAP_CHAIN
            Nan::Utf8String str(info[0]);
            FLAC__bool s = FLAC__metadata_chain_read(m, *str);
            info.GetReturnValue().Set(Nan::New<Boolean>(s));
        }

        static NAN_METHOD(node_FLAC__metadata_chain_read_ogg) {
            UNWRAP_CHAIN
            Nan::Utf8String str(info[0]);
            FLAC__bool s = FLAC__metadata_chain_read_ogg(m, *str);
            info.GetReturnValue().Set(Nan::New<Boolean>(s));
        }

        static NAN_METHOD(node_FLAC__metadata_chain_write) {
            UNWRAP_CHAIN
            FLAC__bool padding = numberFromJs<int>(info[0]).FromMaybe(1);
            FLAC__bool preserve = numberFromJs<int>(info[1]).FromMaybe(0);
            FLAC__bool r = FLAC__metadata_chain_write(m, padding, preserve);
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
        }

        static NAN_METHOD(node_FLAC__metadata_chain_merge_padding) {
            UNWRAP_CHAIN
            FLAC__metadata_chain_merge_padding(m);
        }

        static NAN_METHOD(node_FLAC__metadata_chain_sort_padding) {
            UNWRAP_CHAIN
            FLAC__metadata_chain_sort_padding(m);
        }

        static NAN_METHOD(createIterator);

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
            flacEnum_defineValue(obj1, obj2, "INVALID_CALLBACKS", 13);
            flacEnum_defineValue(obj1, obj2, "READ_WRITE_MISMATCH", 14);
            flacEnum_defineValue(obj1, obj2, "WRONG_WRITE_CALL", 15);
            return std::make_tuple(obj1, obj2);
        }

    public:

        FLAC__Metadata_Chain* m;

        ~Chain();
        static NAN_MODULE_INIT(initMetadata2);

    };

    class Iterator: public Nan::ObjectWrap {

        static NAN_METHOD(node_FLAC__metadata_iterator_new) {
            if(throwIfNotConstructorCall(info)) return;
            FLAC__Metadata_Iterator* m = FLAC__metadata_iterator_new();
            if(m != nullptr) {
                Iterator* it = new Iterator;
                it->m = m;
                it->Wrap(info.This());
                Nan::Set(info.This(), Symbol::GetIterator(info.GetIsolate()), Nan::New<Function>(jsIterator));
                info.GetReturnValue().Set(info.This());
            } else {
                Nan::ThrowError("Cannot allocate memory");
            }
        }

        static NAN_METHOD(jsIterator) {
            UNWRAP_IT
            Local<Object> obj = Nan::New<Object>();
            Nan::Set(obj, Nan::New("it").ToLocalChecked(), WrapPointer(m).ToLocalChecked());
            Nan::Set(obj, Nan::New("rt").ToLocalChecked(), Nan::True());
            Nan::SetMethod(obj, "next", [] (Nan::NAN_METHOD_ARGS_TYPE info) -> void {
                MaybeLocal<Value> parent = Nan::Get(info.This(), Nan::New("it").ToLocalChecked());
                MaybeLocal<Value> rt = Nan::Get(info.This(), Nan::New("rt").ToLocalChecked());
                if(parent.IsEmpty() || !parent.ToLocalChecked()->IsObject()) {
                    Nan::ThrowTypeError("Unexpected this type for iterator");
                    return;
                }

                FLAC__Metadata_Iterator* it = UnwrapPointer<FLAC__Metadata_Iterator>(parent.ToLocalChecked());
                bool prevReturn = rt.ToLocalChecked()->BooleanValue(Isolate::GetCurrent()->GetCurrentContext()).FromJust();
                Local<Object> ret = Nan::New<Object>();
                if(!prevReturn) {
                    Nan::Set(ret, Nan::New("done").ToLocalChecked(), Nan::True());
                } else {
                    FLAC__StreamMetadata* metadata = FLAC__metadata_iterator_get_block(it);
                    Nan::Set(ret, Nan::New("value").ToLocalChecked(), structToJs(metadata));
                    Nan::Set(ret, Nan::New("done").ToLocalChecked(), Nan::False());
                    prevReturn = FLAC__metadata_iterator_next(it);
                }
                Nan::Set(info.This(), Nan::New("rt").ToLocalChecked(), Nan::New<Boolean>(prevReturn));
                info.GetReturnValue().Set(ret);
            });

            while(FLAC__metadata_iterator_prev(m));
            info.GetReturnValue().Set(obj);
        }

        static NAN_METHOD(node_FLAC__metadata_iterator_init) {
            UNWRAP_IT
            MaybeLocal<Object> maybeChain = Nan::To<Object>(info[0]);
            if(maybeChain.IsEmpty()) {
                Nan::ThrowTypeError("Expected object as first argument");
                return;
            }

            Chain* n = Nan::ObjectWrap::Unwrap<Chain>(maybeChain.ToLocalChecked());
            FLAC__metadata_iterator_init(m, n->m);
        }

        static NAN_METHOD(node_FLAC__metadata_iterator_next) {
            UNWRAP_IT
            FLAC__bool r = FLAC__metadata_iterator_next(m);
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
        }

        static NAN_METHOD(node_FLAC__metadata_iterator_prev) {
            UNWRAP_IT
            FLAC__bool r = FLAC__metadata_iterator_prev(m);
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
        }

        static NAN_METHOD(node_FLAC__metadata_iterator_get_block_type) {
            UNWRAP_IT
            FLAC__MetadataType r = FLAC__metadata_iterator_get_block_type(m);
            info.GetReturnValue().Set(numberToJs<int>(r));
        }

        static NAN_METHOD(node_FLAC__metadata_iterator_get_block) {
            UNWRAP_IT
            FLAC__StreamMetadata* r = FLAC__metadata_iterator_get_block(m);
            info.GetReturnValue().Set(structToJs(r));
        }

        static NAN_METHOD(node_FLAC__metadata_iterator_set_block) {
            UNWRAP_IT
            FLAC__StreamMetadata* n = jsToStruct<FLAC__StreamMetadata>(info[0]);
            if(n == nullptr) {
                Nan::ThrowTypeError("Unknown metadata block");
                return;
            }
            FLAC__bool r = FLAC__metadata_iterator_set_block(m, n);
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
            Nan::ObjectWrap::Unwrap<Metadata>(info[0].As<Object>())->hasToBeDeleted = false;
        }

        static NAN_METHOD(node_FLAC__metadata_iterator_delete_block) {
            UNWRAP_IT
            FLAC__bool padding = numberFromJs<int>(info[0]).FromMaybe(1);
            FLAC__bool r = FLAC__metadata_iterator_delete_block(m, padding);
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
        }

        static NAN_METHOD(node_FLAC__metadata_iterator_insert_block_before) {
            UNWRAP_IT
            FLAC__StreamMetadata* n = jsToStruct<FLAC__StreamMetadata>(info[0]);
            if(n == nullptr) {
                Nan::ThrowTypeError("Unknown metadata block");
                return;
            }
            FLAC__bool r = FLAC__metadata_iterator_insert_block_before(m, n);
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
            Nan::ObjectWrap::Unwrap<Metadata>(info[0].As<Object>())->hasToBeDeleted = false;
        }

        static NAN_METHOD(node_FLAC__metadata_iterator_insert_block_after) {
            UNWRAP_IT
            FLAC__StreamMetadata* n = jsToStruct<FLAC__StreamMetadata>(info[0]);
            if(n == nullptr) {
                Nan::ThrowTypeError("Unknown metadata block");
                return;
            }
            FLAC__bool r = FLAC__metadata_iterator_insert_block_after(m, n);
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
            Nan::ObjectWrap::Unwrap<Metadata>(info[0].As<Object>())->hasToBeDeleted = false;
        }

    public:

        FLAC__Metadata_Iterator* m;

        ~Iterator();
        static NAN_MODULE_INIT(initMetadata2);
        static Nan::Persistent<Function> iteratorClass;

    };

    Chain::~Chain() {
        FLAC__metadata_chain_delete(m);
    }

    Iterator::~Iterator() {
        FLAC__metadata_iterator_delete(m);
    }


#define loadFunction(fn) \
_JOIN(FLAC__metadata_, fn) = libFlac->getSymbolAddress<_JOIN2(FLAC__metadata_, fn, _t)>("FLAC__metadata_" #fn); \
if(_JOIN(FLAC__metadata_, fn) == nullptr) printf("%s\n", libFlac->getLastError().c_str());

#define setMethod(fn, jsFn) \
Nan::SetPrototypeMethod(obj, #jsFn, _JOIN(node_FLAC__metadata_, fn)); \
loadFunction(fn)

#define propertyGetter(func, jsName) \
Local<ObjectTemplate> _JOIN(func, Var) = Nan::New<ObjectTemplate>(); \
Nan::SetNamedPropertyHandler(_JOIN(func, Var), jsName, nullptr, nullptr, nullptr, jsName); \
Nan::Set(functionClass, Nan::New(#jsName).ToLocalChecked(), Nan::NewInstance(_JOIN(func, Var)).ToLocalChecked());

#define indexGetter(func, jsName) \
_JOIN(FLAC__Metadata_, func) = libFlac->getSymbolAddress<const char* const*>("FLAC__Metadata_" #func); \
Local<ObjectTemplate> _JOIN(func, _template) = Nan::New<ObjectTemplate>(); \
Nan::SetIndexedPropertyHandler(_JOIN(func, _template), jsName, nullptr, nullptr, nullptr, jsName); \
Nan::Set(functionClass, Nan::New(#jsName).ToLocalChecked(), Nan::NewInstance(_JOIN(func, _template)).ToLocalChecked());

    NAN_MODULE_INIT(Chain::initMetadata2) {
        Local<FunctionTemplate> obj = Nan::New<FunctionTemplate>(node_FLAC__metadata_chain_new);
        obj->SetClassName(Nan::New("Chain").ToLocalChecked());
        obj->InstanceTemplate()->SetInternalFieldCount(1);

        loadFunction(chain_new);
        loadFunction(chain_delete);
        setMethod(chain_status, status);
        setMethod(chain_read, read);
        setMethod(chain_read_ogg, readOgg);
        setMethod(chain_write, write);
        setMethod(chain_merge_padding, mergePadding);
        setMethod(chain_sort_padding, sortPadding);
        Nan::SetPrototypeMethod(obj, "createIterator", createIterator);

        Local<Function> functionClass = Nan::GetFunction(obj).ToLocalChecked();

        flacEnum_declareInObject(functionClass, Status, createStatusEnum());

        Nan::Set(target, Nan::New("Chain").ToLocalChecked(), functionClass);
    }

    Nan::Persistent<Function> Iterator::iteratorClass;
    NAN_MODULE_INIT(Iterator::initMetadata2) {
        Local<FunctionTemplate> obj = Nan::New<FunctionTemplate>(node_FLAC__metadata_iterator_new);
        obj->SetClassName(Nan::New("Iterator").ToLocalChecked());
        obj->InstanceTemplate()->SetInternalFieldCount(1);

        loadFunction(iterator_new);
        loadFunction(iterator_delete);
        setMethod(iterator_init, init);
        setMethod(iterator_next, next);
        setMethod(iterator_prev, prev);
        setMethod(iterator_get_block_type, getBlockType);
        setMethod(iterator_get_block, getBlock);
        setMethod(iterator_set_block, setBlock);
        setMethod(iterator_delete_block, deleteBlock);
        setMethod(iterator_insert_block_before, insertBlockBefore);
        setMethod(iterator_insert_block_after, insertBlockAfter);

        Local<Function> it = Nan::GetFunction(obj).ToLocalChecked();
        iteratorClass.Reset(it);
        Nan::Set(target, Nan::New("Iterator").ToLocalChecked(), it);
    }


    NAN_METHOD(Chain::createIterator) {
        UNWRAP_CHAIN
        MaybeLocal<Object> maybeIt = Nan::NewInstance(Iterator::iteratorClass.Get(info.GetIsolate()));
        if(maybeIt.IsEmpty()) return;

        Local<Object> it = maybeIt.ToLocalChecked();
        Iterator* i = Nan::ObjectWrap::Unwrap<Iterator>(it);
        FLAC__metadata_iterator_init(i->m, m);

        info.GetReturnValue().Set(it);
    }

}
