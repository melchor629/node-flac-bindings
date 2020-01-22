#include "metadata2.hpp"
#include "../flac/format.h"
#include "../mappings/mappings.hpp"
#include "../mappings/native_iterator.hpp"
#include "../utils/converters.hpp"
#include "../utils/enum.hpp"
#include "../utils/pointer.hpp"

namespace flac_bindings {

    using namespace Napi;

    class Chain: public ObjectWrap<Chain> {
        FLAC__Metadata_Chain* chain;

        static Napi::Value simpleAsyncImpl(const Napi::Value& self, const char* name, const std::function<bool()>& impl) {
            EscapableHandleScope scope(self.Env());
            auto worker = new AsyncBackgroundTask<bool>(
                self.Env(),
                [impl] (auto c) {
                    c.resolve(impl());
                },
                nullptr,
                name,
                booleanToJs<FLAC__bool>
            );
            worker->Receiver().Set("this", self);
            worker->Queue();
            return scope.Escape(worker->getPromise());
        }

        static FunctionReference constructor;

        friend class Iterator;

    public:
        static Function init(const Napi::Env& env) {
            EscapableHandleScope scope(env);

            auto constructor = DefineClass(env, "Chain", {
                InstanceMethod("status", &Chain::status),
                InstanceMethod("read", &Chain::read),
                InstanceMethod("readAsync", &Chain::readAsync),
                InstanceMethod("readOgg", &Chain::readOgg),
                InstanceMethod("readOggAsync", &Chain::readOggAsync),
                InstanceMethod("readWithCallbacks", &Chain::readWithCallbacks),
                InstanceMethod("readOggWithCallbacks", &Chain::readOggWithCallbacks),
                InstanceMethod("write", &Chain::write),
                InstanceMethod("writeAsync", &Chain::writeAsync),
                InstanceMethod("writeWithCallbacks", &Chain::writeWithCallbacks),
                InstanceMethod("writeWithCallbacksAndTempFile", &Chain::writeWithCallbacksAndTempFile),
                InstanceMethod("checkIfTempFileIsNeeded", &Chain::checkIfTempFileIsNeeded),
                InstanceMethod("mergePadding", &Chain::mergePadding),
                InstanceMethod("sortPadding", &Chain::sortPadding),
                InstanceMethod("createIterator", &Chain::createIterator),
            });
            c_enum::declareInObject(constructor, "Status", createStatusEnum);

            Chain::constructor = Persistent(constructor);
            Chain::constructor.SuppressDestruct();

            return scope.Escape(objectFreeze(constructor)).As<Function>();
        }

        Chain(const CallbackInfo& info): ObjectWrap<Chain>(info) {
            chain = FLAC__metadata_chain_new();
            if(chain == nullptr) {
                throw Error::New(info.Env(), "Could not allocate memory");
            }
        }

        virtual ~Chain() {
            FLAC__metadata_chain_delete(chain);
        }

        Napi::Value status(const CallbackInfo& info) {
            auto status = FLAC__metadata_chain_status(chain);
            return numberToJs(info.Env(), status);
        }

        Napi::Value read(const CallbackInfo& info) {
            auto path = stringFromJs(info[0]);
            auto ret = FLAC__metadata_chain_read(chain, path.c_str());
            return booleanToJs(info.Env(), ret);
        }

        Napi::Value readAsync(const CallbackInfo& info) {
            auto path = stringFromJs(info[0]);
            return simpleAsyncImpl(
                info.This(),
                "flac_bindings::Chain::readAsync",
                [this, path] () { return FLAC__metadata_chain_read(chain, path.c_str()); }
            );
        }

        Napi::Value readOgg(const CallbackInfo& info) {
            auto path = stringFromJs(info[0]);
            auto ret = FLAC__metadata_chain_read_ogg(chain, path.c_str());
            return booleanToJs(info.Env(), ret);
        }

        Napi::Value readOggAsync(const CallbackInfo& info) {
            auto path = stringFromJs(info[0]);
            return simpleAsyncImpl(
                info.This(),
                "flac_bindings::Chain::readOggAsync",
                [this, path] () { return FLAC__metadata_chain_read_ogg(chain, path.c_str()); }
            );
        }

        Napi::Value readWithCallbacks(const CallbackInfo& info) {
            auto obj = info[0];
            if(!obj.IsObject()) {
                throw TypeError::New(info.Env(), "Expected "s + obj.ToString().Utf8Value() + " to be object"s);
            }

            auto work = new AsyncFlacIOWork(
                [this] (FLAC__IOHandle io, FLAC__IOCallbacks c) { return FLAC__metadata_chain_read_with_callbacks(chain, io, c); },
                "flac_bindings::Chain::readWithCallbacks",
                obj.As<Object>()
            );
            work->Receiver().Set("this", info.This());
            work->Queue();
            return work->getPromise();
        }

        Napi::Value readOggWithCallbacks(const CallbackInfo& info) {
            auto obj = info[0];
            if(!obj.IsObject()) {
                throw TypeError::New(info.Env(), "Expected "s + obj.ToString().Utf8Value() + " to be object"s);
            }

            auto work = new AsyncFlacIOWork(
                [this] (FLAC__IOHandle io, FLAC__IOCallbacks c) { return FLAC__metadata_chain_read_ogg_with_callbacks(chain, io, c); },
                "flac_bindings::Chain::readWithCallbacks",
                obj.As<Object>()
            );
            work->Receiver().Set("this", info.This());
            work->Queue();
            return work->getPromise();
        }

        Napi::Value write(const CallbackInfo& info) {
            auto padding = maybeBooleanFromJs<FLAC__bool>(info[0]).value_or(true);
            auto preserve = maybeBooleanFromJs<FLAC__bool>(info[1]).value_or(false);

            auto ret = FLAC__metadata_chain_write(chain, padding, preserve);
            return booleanToJs(info.Env(), ret);
        }

        Napi::Value writeAsync(const CallbackInfo& info) {
            auto padding = maybeBooleanFromJs<FLAC__bool>(info[0]).value_or(true);
            auto preserve = maybeBooleanFromJs<FLAC__bool>(info[1]).value_or(false);
            return simpleAsyncImpl(
                info.This(),
                "flac_bindings::Chain::writeAsync",
                [this, padding, preserve] () { return FLAC__metadata_chain_write(chain, padding, preserve); }
            );
        }

        Napi::Value writeWithCallbacks(const CallbackInfo& info) {
            auto obj = info[0];
            auto padding = maybeBooleanFromJs<FLAC__bool>(info[0]).value_or(true);
            if(!obj.IsObject()) {
                throw TypeError::New(info.Env(), "Expected "s + obj.ToString().Utf8Value() + " to be object"s);
            }

            auto work = new AsyncFlacIOWork(
                [this, padding] (FLAC__IOHandle io, FLAC__IOCallbacks c) {
                    return FLAC__metadata_chain_write_with_callbacks(chain, padding, io, c);
                },
                "flac_bindings::Chain::writeWithCallbacks",
                obj.As<Object>()
            );
            work->Receiver().Set("this", info.This());
            work->Queue();
            return work->getPromise();
        }

        Napi::Value writeWithCallbacksAndTempFile(const CallbackInfo& info) {
            auto obj1 = info[1];
            auto obj2 = info[2];
            auto padding = maybeBooleanFromJs<FLAC__bool>(info[0]).value_or(true);
            if(!obj1.IsObject()) {
                throw TypeError::New(info.Env(), "Expected "s + obj1.ToString().Utf8Value() + " to be object"s);
            } else if(!obj2.IsObject()) {
                throw TypeError::New(info.Env(), "Expected "s + obj2.ToString().Utf8Value() + " to be object"s);
            }

            auto work = new AsyncFlacIOWork(
                [this, padding] (FLAC__IOHandle io1, FLAC__IOCallbacks c1, FLAC__IOHandle io2, FLAC__IOCallbacks c2) {
                    return FLAC__metadata_chain_write_with_callbacks_and_tempfile(chain, padding, io1, c1, io2, c2);
                },
                "flac_bindings::Chain::writeWithCallbacks",
                obj1.As<Object>(),
                obj2.As<Object>()
            );
            work->Receiver().Set("this", info.This());
            work->Queue();
            return work->getPromise();
        }

        Napi::Value checkIfTempFileIsNeeded(const CallbackInfo& info) {
            auto padding = maybeBooleanFromJs<FLAC__bool>(info[0]).value_or(true);
            auto res = FLAC__metadata_chain_check_if_tempfile_needed(chain, padding);
            return booleanToJs(info.Env(), res);
        }

        void mergePadding(const CallbackInfo&) {
            FLAC__metadata_chain_merge_padding(chain);
        }

        void sortPadding(const CallbackInfo&) {
            FLAC__metadata_chain_sort_padding(chain);
        }

        Napi::Value createIterator(const CallbackInfo&);

        static c_enum::DefineReturnType createStatusEnum(const Napi::Env& env) {
            Object obj1 = Object::New(env);
            Object obj2 = Object::New(env);
            c_enum::defineValue(obj1, obj2, "OK", 0);
            c_enum::defineValue(obj1, obj2, "ILLEGAL_INPUT", 1);
            c_enum::defineValue(obj1, obj2, "ERROR_OPENING_FILE", 2);
            c_enum::defineValue(obj1, obj2, "NOT_A_FLAC_FILE", 3);
            c_enum::defineValue(obj1, obj2, "NOT_WRITABLE", 4);
            c_enum::defineValue(obj1, obj2, "BAD_METADATA", 5);
            c_enum::defineValue(obj1, obj2, "READ_ERROR", 6);
            c_enum::defineValue(obj1, obj2, "SEEK_ERROR", 7);
            c_enum::defineValue(obj1, obj2, "WRITE_ERROR", 8);
            c_enum::defineValue(obj1, obj2, "RENAME_ERROR", 9);
            c_enum::defineValue(obj1, obj2, "UNLINK_ERROR", 10);
            c_enum::defineValue(obj1, obj2, "MEMORY_ALLOCATION_ERROR", 11);
            c_enum::defineValue(obj1, obj2, "INTERNAL_ERROR", 12);
            c_enum::defineValue(obj1, obj2, "INVALID_CALLBACKS", 13);
            c_enum::defineValue(obj1, obj2, "READ_WRITE_MISMATCH", 14);
            c_enum::defineValue(obj1, obj2, "WRONG_WRITE_CALL", 15);
            return std::make_tuple(obj1, obj2);
        }
    };

    class Iterator: public ObjectWrap<Iterator> {
        FLAC__Metadata_Iterator* iterator;

        static FunctionReference constructor;

        friend class Chain;

    public:
        static Function init(const Napi::Env& env) {
            EscapableHandleScope scope(env);

            auto constructor = DefineClass(env, "Iterator", {
                InstanceMethod(Napi::Symbol::WellKnown(env, "iterator"), &Iterator::jsIterator),
                InstanceMethod("init", &Iterator::init),
                InstanceMethod("next", &Iterator::next),
                InstanceMethod("prev", &Iterator::prev),
                InstanceMethod("getBlockType", &Iterator::getBlockType),
                InstanceMethod("getBlock", &Iterator::getBlock),
                InstanceMethod("setBlock", &Iterator::setBlock),
                InstanceMethod("deleteBlock", &Iterator::deleteBlock),
                InstanceMethod("insertBlockBefore", &Iterator::insertBlockBefore),
                InstanceMethod("insertBlockAfter", &Iterator::insertBlockAfter),
            });

            Iterator::constructor = Persistent(constructor);
            Iterator::constructor.SuppressDestruct();

            return scope.Escape(objectFreeze(constructor)).As<Function>();
        }

        Iterator(const CallbackInfo& info): ObjectWrap<Iterator>(info) {
            iterator = FLAC__metadata_iterator_new();
            if(iterator == nullptr) {
                throw Error::New(info.Env(), "Could not allocate memory");
            }
        }

        virtual ~Iterator() {
            FLAC__metadata_iterator_delete(iterator);
        }

        Napi::Value jsIterator(const CallbackInfo& info) {
            auto lastReturn = std::shared_ptr<bool>(new bool(true));
            return NativeIterator::newIterator(info.Env(), [this, lastReturn] (auto env, auto) -> NativeIterator::IterationReturnValue {
                if(!*lastReturn) {
                    return {};
                }

                auto metadata = FLAC__metadata_iterator_get_block(iterator);
                *lastReturn = FLAC__metadata_iterator_next(iterator);
                return Metadata::toJs(env, metadata);
            });
        }

        void init(const CallbackInfo& info) {
            if(!info[0].IsObject()) {
                throw TypeError::New(info.Env(), "Expected "s + info[0].ToString().Utf8Value() + " to be object");
            }

            Chain* chain = Chain::Unwrap(info[0].As<Object>());
            FLAC__metadata_iterator_init(iterator, chain->chain);
        }

        Napi::Value next(const CallbackInfo& info) {
            auto ret = FLAC__metadata_iterator_next(iterator);
            return booleanToJs(info.Env(), ret);
        }

        Napi::Value prev(const CallbackInfo& info) {
            auto ret = FLAC__metadata_iterator_prev(iterator);
            return booleanToJs(info.Env(), ret);
        }

        Napi::Value getBlockType(const CallbackInfo& info) {
            auto ret = FLAC__metadata_iterator_get_block_type(iterator);
            return numberToJs(info.Env(), ret);
        }

        Napi::Value getBlock(const CallbackInfo& info) {
            auto metadata = FLAC__metadata_iterator_get_block(iterator);
            return Metadata::toJs(info.Env(), metadata);
        }

        Napi::Value setBlock(const CallbackInfo& info) {
            auto& metadata = Metadata::fromJs(info[0]);
            auto ret = FLAC__metadata_iterator_set_block(iterator, metadata);
            metadata.setDeletion(false);
            return booleanToJs(info.Env(), ret);
        }

        Napi::Value deleteBlock(const CallbackInfo& info) {
            auto padding = maybeBooleanFromJs<FLAC__bool>(info[0]).value_or(true);
            auto ret = FLAC__metadata_iterator_delete_block(iterator, padding);
            return booleanToJs(info.Env(), ret);
        }

        Napi::Value insertBlockBefore(const CallbackInfo& info) {
            auto& metadata = Metadata::fromJs(info[0]);
            auto ret = FLAC__metadata_iterator_insert_block_before(iterator, metadata);
            metadata.setDeletion(false);
            return booleanToJs(info.Env(), ret);
        }

        Napi::Value insertBlockAfter(const CallbackInfo& info) {
            auto& metadata = Metadata::fromJs(info[0]);
            auto ret = FLAC__metadata_iterator_insert_block_after(iterator, metadata);
            metadata.setDeletion(false);
            return booleanToJs(info.Env(), ret);
        }
    };

    Napi::Value Chain::createIterator(const CallbackInfo&) {
        auto iterator = Iterator::constructor.New({});
        FLAC__metadata_iterator_init(Iterator::Unwrap(iterator)->iterator, chain);
        return iterator;
    }

    FunctionReference Chain::constructor;
    FunctionReference Iterator::constructor;

    void initMetadata2(const Env& env, Object& exports) {
        HandleScope scope(env);
        exports["Chain"] = Chain::init(env);
        exports["Iterator"] = Iterator::init(env);
    }

}
