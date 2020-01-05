#include <vector>
#include <nan.h>
#include "../utils/dl.hpp"

using namespace v8;
using namespace node;
#include "../utils/pointer.hpp"
#include "../format/format.h"
#include "../utils/defs.hpp"
#include "../mappings/mappings.hpp"
#include "../utils/async.hpp"

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

    const char* const* FLAC__Metadata_SimpleIteratorStatusString;
}

#define UNWRAP_IT \
SimpleIterator* self = Nan::ObjectWrap::Unwrap<SimpleIterator>(info.Holder()); \
FLAC__Metadata_SimpleIterator* it = self->it;

namespace flac_bindings {

    extern Library* libFlac;

    class SimpleIterator: public Nan::ObjectWrap {

        FLAC__Metadata_SimpleIterator* it;

        template<typename R = bool>
        static Local<Value> asyncImpl(
            void* it,
            std::vector<Local<Value>> p,
            const char* name, std::function<R()> impl,
            std::function<Local<Value>(R)> conv = booleanToJs<R>
        ) {
            Nan::EscapableHandleScope scope;
            auto* worker = new AsyncBackgroundTask<R>(
                [it, impl] (auto &c) {
                    R v = impl();
                    if(v) {
                        c.resolve(v);
                    } else {
                        c.reject(&FLAC__Metadata_SimpleIteratorStatusString[FLAC__metadata_simple_iterator_status(it)][38]);
                    }
                },
                nullptr,
                name,
                conv
            );
            for(auto i = p.begin(); i != p.end(); i++) {
                worker->SaveToPersistent(i - p.begin(), *i);
            }
            Nan::AsyncQueueWorker(worker);
            return scope.Escape(worker->getReturnValue());
        }

        static NAN_METHOD(create) {
            if(throwIfNotConstructorCall(info)) return;
            FLAC__Metadata_SimpleIterator* it = FLAC__metadata_simple_iterator_new();
            if(it != nullptr) {
                SimpleIterator* si = new SimpleIterator;
                si->it = it;
                si->Wrap(info.This());
                Nan::Set(info.This(), Symbol::GetIterator(info.GetIsolate()), Nan::New<Function>(jsIterator));

                auto script = Nan::CompileScript(Nan::New("Symbol.asyncIterator").ToLocalChecked());
                if(!script.IsEmpty()) {
                    auto ret = Nan::RunScript(script.ToLocalChecked());
                    if(!ret.IsEmpty() && ret.ToLocalChecked()->IsSymbol()) {
                        Nan::Set(info.This(), ret.ToLocalChecked(), Nan::New<Function>(jsAsyncIterator));
                    }
                }
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
                    Nan::Set(ret, Nan::New("value").ToLocalChecked(), structToJs(metadata, true));
                    Nan::Set(ret, Nan::New("done").ToLocalChecked(), Nan::False());
                    prevReturn = FLAC__metadata_simple_iterator_next(it);
                }
                Nan::Set(info.This(), Nan::New("rt").ToLocalChecked(), Nan::New<Boolean>(prevReturn));
                info.GetReturnValue().Set(ret);
            });

            while(FLAC__metadata_simple_iterator_prev(it));
            info.GetReturnValue().Set(obj);
        }

        static NAN_METHOD(jsAsyncIterator) {
            UNWRAP_IT
            Local<Object> obj = Nan::New<Object>();
            Nan::Set(obj, Nan::New("it").ToLocalChecked(), WrapPointer(it).ToLocalChecked());
            Nan::Set(obj, Nan::New("rt").ToLocalChecked(), Nan::True());
            Nan::Set(obj, Nan::New("rb").ToLocalChecked(), Nan::False());
            Nan::SetMethod(obj, "next", [] (Nan::NAN_METHOD_ARGS_TYPE info) -> void {
                MaybeLocal<Value> parent = Nan::Get(info.This(), Nan::New("it").ToLocalChecked());
                MaybeLocal<Value> rt = Nan::Get(info.This(), Nan::New("rt").ToLocalChecked());
                MaybeLocal<Value> rb = Nan::Get(info.This(), Nan::New("rb").ToLocalChecked());
                if(parent.IsEmpty() || !parent.ToLocalChecked()->IsObject()) {
                    Nan::ThrowTypeError("Unexpected this type for iterator");
                    return;
                }

                FLAC__Metadata_SimpleIterator* it = UnwrapPointer<FLAC__Metadata_SimpleIterator>(parent.ToLocalChecked());
                bool prevReturn = booleanFromJs<bool>(rt.ToLocalChecked()).FromJust();
                bool rollbacked = booleanFromJs<bool>(rb.ToLocalChecked()).FromJust();
                Local<Value> promise;
                if(!prevReturn) {
                    auto resolver = Promise::Resolver::New(Nan::GetCurrentContext()).ToLocalChecked();
                    Local<Object> ret = Nan::New<Object>();
                    Nan::Set(ret, Nan::New("done").ToLocalChecked(), Nan::True());
                    resolver->Resolve(Nan::GetCurrentContext(), ret).FromMaybe(false);
                    promise = resolver->GetPromise();
                } else {
                    using ReturnType = std::pair<FLAC__StreamMetadata*, bool>;
                    using AsyncTask = AsyncBackgroundTask<ReturnType>;
                    auto self = info.This();
                    AsyncTask** ptrToWorker = new AsyncTask*;
                    auto worker = new AsyncTask(
                        [it, rollbacked] (auto &c) {
                            if(!rollbacked) {
                                while(FLAC__metadata_simple_iterator_prev(it));
                            }

                            auto m = FLAC__metadata_simple_iterator_get_block(it);
                            if(m == nullptr) {
                                c.reject(FLAC__Metadata_SimpleIteratorStatusString[FLAC__metadata_simple_iterator_status(it)]);
                            } else {
                                bool n = FLAC__metadata_simple_iterator_next(it);
                                c.resolve(std::make_pair(m, n));
                            }
                        },
                        nullptr,
                        "flac_bindings:metadata1:asyncIterator",
                        [ptrToWorker] (auto pair) {
                            Local<Object> self = (*ptrToWorker)->GetFromPersistent("this").As<Object>();
                            Local<Object> ret = Nan::New<Object>();
                            Nan::Set(ret, Nan::New("value").ToLocalChecked(), structToJs(std::get<0>(pair), true));
                            Nan::Set(ret, Nan::New("done").ToLocalChecked(), Nan::False());
                            Nan::Set(self, Nan::New("rt").ToLocalChecked(), booleanToJs(std::get<1>(pair)));
                            Nan::Set(self, Nan::New("rb").ToLocalChecked(), Nan::True());
                            return ret;
                        }
                    );
                    *ptrToWorker = worker;
                    promise = worker->getReturnValue();
                    worker->SaveToPersistent("this", self);
                    Nan::AsyncQueueWorker(worker);
                }

                info.GetReturnValue().Set(promise);
            });

            info.GetReturnValue().Set(obj);
        }

        static NAN_METHOD(status) {
            UNWRAP_IT
            FLAC__Metadata_SimpleIteratorStatus s = FLAC__metadata_simple_iterator_status(it);
            info.GetReturnValue().Set(numberToJs<int>(s));
        }

        static NAN_METHOD(init) {
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

        static NAN_METHOD(initAsync) {
            UNWRAP_IT
            if(!info[0]->IsString()) {
                Nan::ThrowTypeError("Expected first argument to be string");
                return;
            }

            Nan::Utf8String filename(info[0]);
            std::string fileName = *filename;
            FLAC__bool read_only = booleanFromJs<FLAC__bool>(info[1]).FromMaybe(false);
            FLAC__bool preserve = booleanFromJs<FLAC__bool>(info[2]).FromMaybe(false);
            info.GetReturnValue().Set(asyncImpl<bool>(
                it,
                { info.This() },
                "flac_bindings:metadata1:initAsync",
                [it, fileName, read_only, preserve] () { return FLAC__metadata_simple_iterator_init(it, fileName.c_str(), read_only, preserve); }
            ));
        }

        static NAN_METHOD(isWritable) {
            UNWRAP_IT
            FLAC__bool r = FLAC__metadata_simple_iterator_is_writable(it);
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
        }

        static NAN_METHOD(next) {
            UNWRAP_IT
            FLAC__bool r = FLAC__metadata_simple_iterator_next(it);
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
        }

        static NAN_METHOD(nextAsync) {
            UNWRAP_IT
            info.GetReturnValue().Set(asyncImpl<bool*>(
                it,
                { info.This() },
                "flac_bindings:metadata1:nextAsync",
                [it] () { return new bool(FLAC__metadata_simple_iterator_next(it)); },
                [] (bool* b) { bool bb = *b; delete b; return booleanToJs(bb); }
            ));
        }

        static NAN_METHOD(prev) {
            UNWRAP_IT
            FLAC__bool r = FLAC__metadata_simple_iterator_prev(it);
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
        }

        static NAN_METHOD(prevAsync) {
            UNWRAP_IT
            info.GetReturnValue().Set(asyncImpl<bool*>(
                it,
                { info.This() },
                "flac_bindings:metadata1:prevAsync",
                [it] () { return new bool(FLAC__metadata_simple_iterator_prev(it)); },
                [] (bool* b) { bool bb = *b; delete b; return booleanToJs(bb); }
            ));
        }

        static NAN_METHOD(isLast) {
            UNWRAP_IT
            FLAC__bool r = FLAC__metadata_simple_iterator_is_last(it);
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
        }

        static NAN_METHOD(getBlockOffset) {
            UNWRAP_IT
            off_t r = FLAC__metadata_simple_iterator_get_block_offset(it);
            info.GetReturnValue().Set(numberToJs(r));
        }

        static NAN_METHOD(getBlockType) {
            UNWRAP_IT
            FLAC__MetadataType r = FLAC__metadata_simple_iterator_get_block_type(it);
            info.GetReturnValue().Set(numberToJs<int>(r));
        }

        static NAN_METHOD(getBlockLength) {
            UNWRAP_IT
            unsigned r = FLAC__metadata_simple_iterator_get_block_length(it);
            info.GetReturnValue().Set(numberToJs(r));
        }

        static NAN_METHOD(getApplicationId) {
            UNWRAP_IT
            FLAC__byte id[4];
            FLAC__bool r = FLAC__metadata_simple_iterator_get_application_id(it, id);
            if(r) {
                Local<Value> arr = node::Buffer::Copy(Isolate::GetCurrent(), (const char*) id, 4).ToLocalChecked();
                info.GetReturnValue().Set(arr);
            } else {
                info.GetReturnValue().Set(Nan::New<Boolean>(false));
            }
        }

        static NAN_METHOD(getApplicationIdAsync) {
            UNWRAP_IT
            info.GetReturnValue().Set(asyncImpl<FLAC__byte*>(
                it,
                { info.This() },
                "flac_bindings:metadata1:getApplicationIdAsync",
                [it] () -> FLAC__byte* {
                    FLAC__byte* id = new FLAC__byte[4];
                    if(FLAC__metadata_simple_iterator_get_application_id(it, id)) return id;
                    return nullptr;
                },
                [] (FLAC__byte* b) {
                    auto v = node::Buffer::Copy(Isolate::GetCurrent(), (const char*) b, 4);
                    delete[] b;
                    return v.ToLocalChecked();
                }
            ));
        }

        static NAN_METHOD(getBlock) {
            UNWRAP_IT
            FLAC__StreamMetadata* r = FLAC__metadata_simple_iterator_get_block(it);
            info.GetReturnValue().Set(structToJs(r, true));
        }

        static NAN_METHOD(getBlockAsync) {
            using namespace std::placeholders;
            UNWRAP_IT
            info.GetReturnValue().Set(asyncImpl<FLAC__StreamMetadata*>(
                it,
                { info.This() },
                "flac_bindings:metadata1:getBlockAsync",
                [it] () { return FLAC__metadata_simple_iterator_get_block(it); },
                std::bind(structToJs<FLAC__StreamMetadata>, _1, true)
            ));
        }

        static NAN_METHOD(setBlock) {
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

        static NAN_METHOD(setBlockAsync) {
            UNWRAP_IT
            FLAC__StreamMetadata* m = jsToStruct<FLAC__StreamMetadata>(info[0]);
            if(m == nullptr) {
                Nan::ThrowTypeError("Unknown metadata object");
                return;
            }
            FLAC__bool pad = numberFromJs<int>(info[1]).FromMaybe(1);
            info.GetReturnValue().Set(asyncImpl<bool>(
                it,
                { info.This(), info[0] },
                "flac_bindings:metadata1:setBlockAsync",
                [it, m, pad] () { return FLAC__metadata_simple_iterator_set_block(it, m, pad); }
            ));
        }

        static NAN_METHOD(insertBlockAfter) {
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

        static NAN_METHOD(insertBlockAfterAsync) {
            UNWRAP_IT
            FLAC__StreamMetadata* m = jsToStruct<FLAC__StreamMetadata>(info[0]);
            if(m == nullptr) {
                Nan::ThrowTypeError("Unknown metadata object");
                return;
            }
            FLAC__bool pad = numberFromJs<int>(info[1]).FromMaybe(1);
            info.GetReturnValue().Set(asyncImpl<bool>(
                it,
                { info.This(), info[0] },
                "flac_bindings:metadata1:insertBlockAfterAsync",
                [it, m, pad] () { return FLAC__metadata_simple_iterator_insert_block_after(it, m, pad); }
            ));
        }

        static NAN_METHOD(deleteBlock) {
            UNWRAP_IT
            FLAC__bool pad = numberFromJs<int>(info[0]).FromMaybe(1);
            FLAC__bool r = FLAC__metadata_simple_iterator_delete_block(it, pad);
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
        }

        static NAN_METHOD(deleteBlockAsync) {
            UNWRAP_IT
            FLAC__bool pad = numberFromJs<int>(info[0]).FromMaybe(1);
            info.GetReturnValue().Set(asyncImpl<bool>(
                it,
                { info.This() },
                "flac_bindings:metadata1:deleteBlockAsync",
                [it, pad] () { return FLAC__metadata_simple_iterator_delete_block(it, pad); }
            ));
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
        Local<FunctionTemplate> classFunction = Nan::New<FunctionTemplate>(create);
        classFunction->SetClassName(Nan::New("SimpleIterator").ToLocalChecked());
        classFunction->InstanceTemplate()->SetInternalFieldCount(1);

        #define loadFunction(fn) \
        _JOIN(FLAC__metadata_simple_iterator_, fn) = libFlac->getSymbolAddress<_JOIN2(FLAC__metadata_simple_iterator_, fn, _t)>("FLAC__metadata_simple_iterator_" #fn); \
        if(_JOIN(FLAC__metadata_simple_iterator_, fn) == nullptr) printf("%s\n", libFlac->getLastError().c_str());
        #define setMethod(fn, jsFn) \
        Nan::SetPrototypeMethod(classFunction, #jsFn, jsFn); \
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
        FLAC__Metadata_SimpleIteratorStatusString = libFlac->getSymbolAddress<const char* const*>("FLAC__Metadata_SimpleIteratorStatusString");

        Nan::SetPrototypeMethod(classFunction, "initAsync", initAsync);
        Nan::SetPrototypeMethod(classFunction, "nextAsync", nextAsync);
        Nan::SetPrototypeMethod(classFunction, "prevAsync", prevAsync);
        Nan::SetPrototypeMethod(classFunction, "getApplicationIdAsync", getApplicationIdAsync);
        Nan::SetPrototypeMethod(classFunction, "getBlockAsync", getBlockAsync);
        Nan::SetPrototypeMethod(classFunction, "setBlockAsync", setBlockAsync);
        Nan::SetPrototypeMethod(classFunction, "insertBlockAfterAsync", insertBlockAfterAsync);
        Nan::SetPrototypeMethod(classFunction, "deleteBlockAsync", deleteBlockAsync);

        Local<Object> obj = Nan::GetFunction(classFunction).ToLocalChecked();

        flacEnum_declareInObject(obj, Status, createStatusEnum());


        Nan::Set(target, Nan::New("SimpleIterator").ToLocalChecked(), obj);
    }
}
