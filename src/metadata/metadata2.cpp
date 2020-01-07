#include <nan.h>
#include "metadata2.hpp"
#include "../flac/format.h"

using namespace v8;
using namespace node;
#include "../utils/pointer.hpp"
#include "../utils/defs.hpp"
#include "../mappings/mappings.hpp"
#include "../utils/async.hpp"

#define UNWRAP_CHAIN \
Chain* self = Nan::ObjectWrap::Unwrap<Chain>(info.Holder()); \
FLAC__Metadata_Chain* m = self->m;

#define UNWRAP_IT \
Iterator* self = Nan::ObjectWrap::Unwrap<Iterator>(info.Holder()); \
FLAC__Metadata_Iterator* m = self->m;

namespace flac_bindings {

    static std::function<bool(void*, FLAC__IOCallbacks)>
    asyncWrap(FLAC__Metadata_Chain* m, std::function<bool(FLAC__IOCallbacks, void*)> f) {
        return [m, f] (auto c, auto io) {
            bool r = f(io, c);
            if(!r) {
                const char* msg = &FLAC__Metadata_ChainStatusString[FLAC__metadata_chain_status(m)][28];
                std::get<1>(*(std::tuple<void*, AsyncFlacIOWork::ExecutionContext*>*) c)->reject(msg);
            }
            return r;
        };
    }

    static std::function<bool(void*, FLAC__IOCallbacks, void*, FLAC__IOCallbacks)>
    asyncWrap(FLAC__Metadata_Chain* m, std::function<bool(FLAC__IOCallbacks, void*, FLAC__IOCallbacks, void*)> f) {
        return [m, f] (auto c, auto io, auto c2, auto io2) {
            bool r = f(io, c, io2, c2);
            if(!r) {
                const char* msg = &FLAC__Metadata_ChainStatusString[FLAC__metadata_chain_status(m)][28];
                std::get<1>(*(std::tuple<void*, AsyncFlacIOWork::ExecutionContext*>*) c)->reject(msg);
            }
            return r;
        };
    }

    class Chain: public Nan::ObjectWrap {

        static NAN_METHOD(create) {
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

        static NAN_METHOD(status) {
            UNWRAP_CHAIN
            FLAC__Metadata_ChainStatus s = FLAC__metadata_chain_status(m);
            info.GetReturnValue().Set(Nan::New(s));
        }

        static NAN_METHOD(read) {
            UNWRAP_CHAIN
            if(!info[0]->IsString()) {
                Nan::ThrowTypeError("Expected argument to be string");
                return;
            }

            Nan::Utf8String str(info[0]);
            FLAC__bool s = FLAC__metadata_chain_read(m, *str);
            info.GetReturnValue().Set(Nan::New<Boolean>(s));
        }

        static Local<Value> simpleAsyncImpl(void* m, Local<Value> This, const char* name, std::function<bool()> impl) {
            Nan::EscapableHandleScope scope;
            auto* worker = new AsyncBackgroundTask<bool>(
                [m, impl] (auto &c) {
                    if(impl()) {
                        c.resolve(true);
                    } else {
                        c.reject(&FLAC__Metadata_ChainStatusString[FLAC__metadata_chain_status(m)][28]);
                    }
                },
                nullptr,
                name,
                booleanToJs<bool>
            );
            worker->SaveToPersistent("this", This);
            Nan::AsyncQueueWorker(worker);
            return scope.Escape(worker->getReturnValue());
        }

        static NAN_METHOD(readAsync) {
            UNWRAP_CHAIN
            if(!info[0]->IsString()) {
                Nan::ThrowTypeError("Expected argument to be string");
                return;
            }

            Nan::Utf8String str(info[0]);
            std::string path = *str;
            info.GetReturnValue().Set(simpleAsyncImpl(
                m,
                info.This(),
                "flac_bindings:metadata2:readAsync",
                [m, path] () { return FLAC__metadata_chain_read(m, path.c_str()); }
            ));
        }

        static NAN_METHOD(readOgg) {
            UNWRAP_CHAIN
            if(!info[0]->IsString()) {
                Nan::ThrowTypeError("Expected argument to be string");
                return;
            }

            Nan::Utf8String str(info[0]);
            FLAC__bool s = FLAC__metadata_chain_read_ogg(m, *str);
            info.GetReturnValue().Set(Nan::New<Boolean>(s));
        }

        static NAN_METHOD(readOggAsync) {
            UNWRAP_CHAIN
            if(!info[0]->IsString()) {
                Nan::ThrowTypeError("Expected argument to be string");
                return;
            }

            Nan::Utf8String str(info[0]);
            std::string path = *str;
            info.GetReturnValue().Set(simpleAsyncImpl(
                m,
                info.This(),
                "flac_bindings:metadata2:readOggAsync",
                [m, path] () { return FLAC__metadata_chain_read_ogg(m, path.c_str()); }
            ));
        }

        static NAN_METHOD(readWithCallbacks) {
            UNWRAP_CHAIN
            if(!info[0]->IsObject()) {
                Nan::ThrowTypeError("Expected argument to be object");
                return;
            }

            Local<Object> obj = info[0].As<Object>();
            auto* work = new AsyncFlacIOWork(
                asyncWrap(m, [m] (auto io, auto c) { return FLAC__metadata_chain_read_with_callbacks(m, c, io); }),
                "metadata2:readWithCallbacks",
                obj
            );
            work->SaveToPersistent("this", info.This());
            info.GetReturnValue().Set(work->getPromise());
            AsyncQueueWorker(work);
        }

        static NAN_METHOD(readOggWithCallbacks) {
            UNWRAP_CHAIN
            if(!info[0]->IsObject()) {
                Nan::ThrowTypeError("Expected argument to be object");
                return;
            }

            Local<Object> obj = info[0].As<Object>();
            auto* work = new AsyncFlacIOWork(
                asyncWrap(m, [m] (auto io, auto c) { return FLAC__metadata_chain_read_ogg_with_callbacks(m, c, io); }),
                "metadata2:readOggWithCallbacks",
                obj
            );
            work->SaveToPersistent("this", info.This());
            info.GetReturnValue().Set(work->getPromise());
            AsyncQueueWorker(work);
        }

        static NAN_METHOD(write) {
            UNWRAP_CHAIN
            FLAC__bool padding = booleanFromJs<FLAC__bool>(info[0]).FromMaybe(true);
            FLAC__bool preserve = booleanFromJs<FLAC__bool>(info[1]).FromMaybe(false);
            FLAC__bool r = FLAC__metadata_chain_write(m, padding, preserve);
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
        }

        static NAN_METHOD(writeAsync) {
            UNWRAP_CHAIN
            FLAC__bool padding = booleanFromJs<FLAC__bool>(info[0]).FromMaybe(true);
            FLAC__bool preserve = booleanFromJs<FLAC__bool>(info[1]).FromMaybe(false);
            info.GetReturnValue().Set(simpleAsyncImpl(
                m,
                info.This(),
                "flac_bindings:metadata2:writeAsync",
                [m, padding, preserve] () { return FLAC__metadata_chain_write(m, padding, preserve); }
            ));
        }

        static NAN_METHOD(writeWithCallbacks) {
            UNWRAP_CHAIN
            if(!info[0]->IsObject()) {
                Nan::ThrowTypeError("Expected argument to be object");
                return;
            }

            Local<Object> obj = info[0].As<Object>();
            FLAC__bool padding = booleanFromJs<FLAC__bool>(info[1]).FromMaybe(true);
            auto* work = new AsyncFlacIOWork(
                asyncWrap(m, [m, padding] (auto io, auto c) { return FLAC__metadata_chain_write_with_callbacks(m, padding, c, io); }),
                "metadata2:writeWithCallbacks",
                obj
            );
            work->SaveToPersistent("this", info.This());
            info.GetReturnValue().Set(work->getPromise());
            AsyncQueueWorker(work);
        }

        static NAN_METHOD(writeWithCallbacksAndTempFile) {
            UNWRAP_CHAIN
            if(!info[1]->IsObject()) {
                Nan::ThrowTypeError("Expected second argument to be object");
                return;
            }

            if(!info[2]->IsObject()) {
                Nan::ThrowTypeError("Expected third argument to be object");
                return;
            }

            FLAC__bool padding = booleanFromJs<FLAC__bool>(info[0]).FromMaybe(true);
            Local<Object> obj1 = info[1].As<Object>();
            Local<Object> obj2 = info[2].As<Object>();
            auto* work = new AsyncFlacIOWork(
                asyncWrap(m, [m, padding] (auto io, auto c, auto io2, auto c2) {
                    return FLAC__metadata_chain_write_with_callbacks_and_tempfile(m, padding, c, io, c2, io2);
                }),
                "metadata2:writeWithCallbacksAndTempFile",
                obj1,
                obj2
            );
            work->SaveToPersistent("this", info.This());
            info.GetReturnValue().Set(work->getPromise());
            AsyncQueueWorker(work);
        }

        static NAN_METHOD(checkIfTempFileIsNeeded) {
            UNWRAP_CHAIN
            FLAC__bool padding = booleanFromJs<FLAC__bool>(info[0]).FromMaybe(true);
            FLAC__bool result = FLAC__metadata_chain_check_if_tempfile_needed(m, padding);
            info.GetReturnValue().Set(booleanToJs(result));
        }

        static NAN_METHOD(mergePadding) {
            UNWRAP_CHAIN
            FLAC__metadata_chain_merge_padding(m);
        }

        static NAN_METHOD(sortPadding) {
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

        static NAN_METHOD(create) {
            if(throwIfNotConstructorCall(info)) return;
            FLAC__Metadata_Iterator* m = FLAC__metadata_iterator_new();
            if(m != nullptr) {
                Iterator* it = new Iterator;
                it->m = m;
                it->Wrap(info.This());
                Nan::Set(info.This(), v8::Symbol::GetIterator(info.GetIsolate()), Nan::New<Function>(jsIterator));
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

        static NAN_METHOD(init) {
            UNWRAP_IT
            MaybeLocal<Object> maybeChain = Nan::To<Object>(info[0]);
            if(maybeChain.IsEmpty()) {
                Nan::ThrowTypeError("Expected object as first argument");
                return;
            }

            Chain* n = Nan::ObjectWrap::Unwrap<Chain>(maybeChain.ToLocalChecked());
            FLAC__metadata_iterator_init(m, n->m);
        }

        static NAN_METHOD(next) {
            UNWRAP_IT
            FLAC__bool r = FLAC__metadata_iterator_next(m);
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
        }

        static NAN_METHOD(prev) {
            UNWRAP_IT
            FLAC__bool r = FLAC__metadata_iterator_prev(m);
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
        }

        static NAN_METHOD(getBlockType) {
            UNWRAP_IT
            FLAC__MetadataType r = FLAC__metadata_iterator_get_block_type(m);
            info.GetReturnValue().Set(numberToJs<int>(r));
        }

        static NAN_METHOD(getBlock) {
            UNWRAP_IT
            FLAC__StreamMetadata* r = FLAC__metadata_iterator_get_block(m);
            info.GetReturnValue().Set(structToJs(r));
        }

        static NAN_METHOD(setBlock) {
            UNWRAP_IT
            FLAC__StreamMetadata* n = jsToStruct<FLAC__StreamMetadata>(info[0]);
            if(n == nullptr) {
                Nan::ThrowTypeError("Unknown metadata block");
                return;
            }
            FLAC__bool r = FLAC__metadata_iterator_set_block(m, n);
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
            markDeleteObjectTo<FLAC__StreamMetadata>(info[0], false);
        }

        static NAN_METHOD(deleteBlock) {
            UNWRAP_IT
            FLAC__bool padding = numberFromJs<int>(info[0]).FromMaybe(1);
            FLAC__bool r = FLAC__metadata_iterator_delete_block(m, padding);
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
        }

        static NAN_METHOD(insertBlockBefore) {
            UNWRAP_IT
            FLAC__StreamMetadata* n = jsToStruct<FLAC__StreamMetadata>(info[0]);
            if(n == nullptr) {
                Nan::ThrowTypeError("Unknown metadata block");
                return;
            }
            FLAC__bool r = FLAC__metadata_iterator_insert_block_before(m, n);
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
            markDeleteObjectTo<FLAC__StreamMetadata>(info[0], false);
        }

        static NAN_METHOD(insertBlockAfter) {
            UNWRAP_IT
            FLAC__StreamMetadata* n = jsToStruct<FLAC__StreamMetadata>(info[0]);
            if(n == nullptr) {
                Nan::ThrowTypeError("Unknown metadata block");
                return;
            }
            FLAC__bool r = FLAC__metadata_iterator_insert_block_after(m, n);
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
            markDeleteObjectTo<FLAC__StreamMetadata>(info[0], false);
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


#define setMethod(fn, jsFn) \
Nan::SetPrototypeMethod(obj, #jsFn, jsFn);

    NAN_MODULE_INIT(Chain::initMetadata2) {
        Local<FunctionTemplate> obj = Nan::New<FunctionTemplate>(create);
        obj->SetClassName(Nan::New("Chain").ToLocalChecked());
        obj->InstanceTemplate()->SetInternalFieldCount(1);

        setMethod(chain_status, status);
        setMethod(chain_read, read);
        setMethod(chain_read_ogg, readOgg);
        setMethod(chain_write, write);
        setMethod(chain_merge_padding, mergePadding);
        setMethod(chain_sort_padding, sortPadding);
        setMethod(chain_read_with_callbacks, readWithCallbacks);
        setMethod(chain_read_ogg_with_callbacks, readOggWithCallbacks);
        setMethod(chain_write_with_callbacks, writeWithCallbacks);
        setMethod(chain_write_with_callbacks_and_tempfile, writeWithCallbacksAndTempFile);
        setMethod(chain_check_if_tempfile_needed, checkIfTempFileIsNeeded);
        Nan::SetPrototypeMethod(obj, "createIterator", createIterator);
        Nan::SetPrototypeMethod(obj, "readAsync", readAsync);
        Nan::SetPrototypeMethod(obj, "readOggAsync", readOggAsync);
        Nan::SetPrototypeMethod(obj, "writeAsync", writeAsync);

        Local<Function> functionClass = Nan::GetFunction(obj).ToLocalChecked();

        flacEnum_declareInObject(functionClass, Status, createStatusEnum());

        Nan::Set(target, Nan::New("Chain").ToLocalChecked(), functionClass);
    }

    Nan::Persistent<Function> Iterator::iteratorClass;
    NAN_MODULE_INIT(Iterator::initMetadata2) {
        Local<FunctionTemplate> obj = Nan::New<FunctionTemplate>(create);
        obj->SetClassName(Nan::New("Iterator").ToLocalChecked());
        obj->InstanceTemplate()->SetInternalFieldCount(1);

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
