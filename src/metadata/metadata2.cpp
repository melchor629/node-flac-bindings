#include "metadata2.hpp"
#include "../mappings/mappings.hpp"
#include "../mappings/native_iterator.hpp"
#include "../utils/converters.hpp"
#include "../utils/enum.hpp"
#include "../utils/pointer.hpp"

namespace flac_bindings {

  using namespace Napi;

  class Chain: public ObjectWrap<Chain> {
    FLAC__Metadata_Chain* chain;

    Napi::Value simpleAsyncImpl(
      const Napi::Value& self,
      const char* name,
      const std::function<bool()>& impl) {
      EscapableHandleScope scope(self.Env());
      auto worker = new AsyncBackgroundTask<bool>(
        self.Env(),
        [impl](auto c) { c.resolve(impl()); },
        nullptr,
        name,
        [this](auto env, auto value) {
          this->checkStatus(env, value);
          return env.Undefined();
        });
      worker->Receiver().Set("this", self);
      worker->Queue();
      return scope.Escape(worker->getPromise());
    }

    void checkStatus(const Napi::Env& env, bool succeeded) {
      if (!succeeded) {
        auto status = FLAC__metadata_chain_status(chain);
        // remove prefix FLAC__METADATA_CHAIN_STATUS_
        auto statusString = FLAC__Metadata_ChainStatusString[status] + 28;
        auto error = Error::New(env, "Chain operation failed: "s + statusString);
        error.Set("status", numberToJs(env, status));
        error.Set("statusString", String::New(env, statusString));
        throw error;
      }
    }

    friend class Iterator;

  public:
    static Function init(Napi::Env env, FlacAddon& addon) {
      EscapableHandleScope scope(env);

      auto constructor = DefineClass(
        env,
        "Chain",
        {
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

      addon.chainConstructor = Persistent(constructor);

      return scope.Escape(objectFreeze(constructor)).As<Function>();
    }

    Chain(const CallbackInfo& info): ObjectWrap<Chain>(info) {
      chain = FLAC__metadata_chain_new();
      if (chain == nullptr) {
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

    void read(const CallbackInfo& info) {
      auto path = stringFromJs(info[0]);
      auto ret = FLAC__metadata_chain_read(chain, path.c_str());
      checkStatus(info.Env(), ret);
    }

    Napi::Value readAsync(const CallbackInfo& info) {
      auto path = stringFromJs(info[0]);
      return simpleAsyncImpl(info.This(), "flac_bindings::Chain::readAsync", [this, path]() {
        return FLAC__metadata_chain_read(chain, path.c_str());
      });
    }

    void readOgg(const CallbackInfo& info) {
      auto path = stringFromJs(info[0]);
      auto ret = FLAC__metadata_chain_read_ogg(chain, path.c_str());
      checkStatus(info.Env(), ret);
    }

    Napi::Value readOggAsync(const CallbackInfo& info) {
      auto path = stringFromJs(info[0]);
      return simpleAsyncImpl(info.This(), "flac_bindings::Chain::readOggAsync", [this, path]() {
        return FLAC__metadata_chain_read_ogg(chain, path.c_str());
      });
    }

    Napi::Value readWithCallbacks(const CallbackInfo& info) {
      auto obj = info[0];
      if (!obj.IsObject()) {
        throw TypeError::New(
          info.Env(),
          "Expected "s + obj.ToString().Utf8Value() + " to be object"s);
      }

      auto work = new AsyncFlacIOWork(
        [this](FLAC__IOHandle io, FLAC__IOCallbacks c) {
          return FLAC__metadata_chain_read_with_callbacks(chain, io, c);
        },
        "flac_bindings::Chain::readWithCallbacks",
        obj.As<Object>(),
        std::bind(&Chain::checkStatus, this, std::placeholders::_1, std::placeholders::_2));
      work->Receiver().Set("this", info.This());
      work->Queue();
      return work->getPromise();
    }

    Napi::Value readOggWithCallbacks(const CallbackInfo& info) {
      auto obj = info[0];
      if (!obj.IsObject()) {
        throw TypeError::New(
          info.Env(),
          "Expected "s + obj.ToString().Utf8Value() + " to be object"s);
      }

      auto work = new AsyncFlacIOWork(
        [this](FLAC__IOHandle io, FLAC__IOCallbacks c) {
          return FLAC__metadata_chain_read_ogg_with_callbacks(chain, io, c);
        },
        "flac_bindings::Chain::readWithCallbacks",
        obj.As<Object>(),
        std::bind(&Chain::checkStatus, this, std::placeholders::_1, std::placeholders::_2));
      work->Receiver().Set("this", info.This());
      work->Queue();
      return work->getPromise();
    }

    void write(const CallbackInfo& info) {
      auto padding = maybeBooleanFromJs<FLAC__bool>(info[0]).value_or(true);
      auto preserve = maybeBooleanFromJs<FLAC__bool>(info[1]).value_or(false);

      auto ret = FLAC__metadata_chain_write(chain, padding, preserve);
      checkStatus(info.Env(), ret);
    }

    Napi::Value writeAsync(const CallbackInfo& info) {
      auto padding = maybeBooleanFromJs<FLAC__bool>(info[0]).value_or(true);
      auto preserve = maybeBooleanFromJs<FLAC__bool>(info[1]).value_or(false);
      return simpleAsyncImpl(
        info.This(),
        "flac_bindings::Chain::writeAsync",
        [this, padding, preserve]() {
          return FLAC__metadata_chain_write(chain, padding, preserve);
        });
    }

    Napi::Value writeWithCallbacks(const CallbackInfo& info) {
      auto obj = info[0];
      auto padding = maybeBooleanFromJs<FLAC__bool>(info[0]).value_or(true);
      if (!obj.IsObject()) {
        throw TypeError::New(
          info.Env(),
          "Expected "s + obj.ToString().Utf8Value() + " to be object"s);
      }

      auto work = new AsyncFlacIOWork(
        [this, padding](FLAC__IOHandle io, FLAC__IOCallbacks c) {
          return FLAC__metadata_chain_write_with_callbacks(chain, padding, io, c);
        },
        "flac_bindings::Chain::writeWithCallbacks",
        obj.As<Object>(),
        std::bind(&Chain::checkStatus, this, std::placeholders::_1, std::placeholders::_2));
      work->Receiver().Set("this", info.This());
      work->Queue();
      return work->getPromise();
    }

    Napi::Value writeWithCallbacksAndTempFile(const CallbackInfo& info) {
      auto obj1 = info[1];
      auto obj2 = info[2];
      auto padding = maybeBooleanFromJs<FLAC__bool>(info[0]).value_or(true);
      if (!obj1.IsObject()) {
        throw TypeError::New(
          info.Env(),
          "Expected "s + obj1.ToString().Utf8Value() + " to be object"s);
      } else if (!obj2.IsObject()) {
        throw TypeError::New(
          info.Env(),
          "Expected "s + obj2.ToString().Utf8Value() + " to be object"s);
      }

      auto work = new AsyncFlacIOWork(
        [this, padding](
          FLAC__IOHandle io1,
          FLAC__IOCallbacks c1,
          FLAC__IOHandle io2,
          FLAC__IOCallbacks c2) {
          return FLAC__metadata_chain_write_with_callbacks_and_tempfile(
            chain,
            padding,
            io1,
            c1,
            io2,
            c2);
        },
        "flac_bindings::Chain::writeWithCallbacks",
        obj1.As<Object>(),
        obj2.As<Object>(),
        std::bind(&Chain::checkStatus, this, std::placeholders::_1, std::placeholders::_2));
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
      c_enum::defineValue(obj1, obj2, "OK", FLAC__METADATA_CHAIN_STATUS_OK);
      c_enum::defineValue(obj1, obj2, "ILLEGAL_INPUT", FLAC__METADATA_CHAIN_STATUS_ILLEGAL_INPUT);
      c_enum::defineValue(
        obj1,
        obj2,
        "ERROR_OPENING_FILE",
        FLAC__METADATA_CHAIN_STATUS_ERROR_OPENING_FILE);
      c_enum::defineValue(
        obj1,
        obj2,
        "NOT_A_FLAC_FILE",
        FLAC__METADATA_CHAIN_STATUS_NOT_A_FLAC_FILE);
      c_enum::defineValue(obj1, obj2, "NOT_WRITABLE", FLAC__METADATA_CHAIN_STATUS_NOT_WRITABLE);
      c_enum::defineValue(obj1, obj2, "BAD_METADATA", FLAC__METADATA_CHAIN_STATUS_BAD_METADATA);
      c_enum::defineValue(obj1, obj2, "READ_ERROR", FLAC__METADATA_CHAIN_STATUS_READ_ERROR);
      c_enum::defineValue(obj1, obj2, "SEEK_ERROR", FLAC__METADATA_CHAIN_STATUS_SEEK_ERROR);
      c_enum::defineValue(obj1, obj2, "WRITE_ERROR", FLAC__METADATA_CHAIN_STATUS_WRITE_ERROR);
      c_enum::defineValue(obj1, obj2, "RENAME_ERROR", FLAC__METADATA_CHAIN_STATUS_RENAME_ERROR);
      c_enum::defineValue(obj1, obj2, "UNLINK_ERROR", FLAC__METADATA_CHAIN_STATUS_UNLINK_ERROR);
      c_enum::defineValue(
        obj1,
        obj2,
        "MEMORY_ALLOCATION_ERROR",
        FLAC__METADATA_CHAIN_STATUS_MEMORY_ALLOCATION_ERROR);
      c_enum::defineValue(obj1, obj2, "INTERNAL_ERROR", FLAC__METADATA_CHAIN_STATUS_INTERNAL_ERROR);
      c_enum::defineValue(
        obj1,
        obj2,
        "INVALID_CALLBACKS",
        FLAC__METADATA_CHAIN_STATUS_INVALID_CALLBACKS);
      c_enum::defineValue(
        obj1,
        obj2,
        "READ_WRITE_MISMATCH",
        FLAC__METADATA_CHAIN_STATUS_READ_WRITE_MISMATCH);
      c_enum::defineValue(
        obj1,
        obj2,
        "WRONG_WRITE_CALL",
        FLAC__METADATA_CHAIN_STATUS_WRONG_WRITE_CALL);
      return std::make_tuple(obj1, obj2);
    }
  };

  class Iterator: public ObjectWrap<Iterator> {
    FLAC__Metadata_Iterator* iterator;

    friend class Chain;

  public:
    static Function init(Napi::Env env, FlacAddon& addon) {
      EscapableHandleScope scope(env);

      auto constructor = DefineClass(
        env,
        "Iterator",
        {
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

      addon.iteratorConstructor = Persistent(constructor);

      return scope.Escape(objectFreeze(constructor)).As<Function>();
    }

    Iterator(const CallbackInfo& info): ObjectWrap<Iterator>(info) {
      iterator = FLAC__metadata_iterator_new();
      if (iterator == nullptr) {
        throw Error::New(info.Env(), "Could not allocate memory");
      }
    }

    virtual ~Iterator() {
      FLAC__metadata_iterator_delete(iterator);
    }

    Napi::Value jsIterator(const CallbackInfo& info) {
      auto lastReturn = std::shared_ptr<bool>(new bool(true));
      return NativeIterator::newIterator(
        info.Env(),
        [this, lastReturn](auto env, auto) -> NativeIterator::IterationReturnValue {
          if (!*lastReturn) {
            return {};
          }

          auto metadata = FLAC__metadata_iterator_get_block(iterator);
          *lastReturn = FLAC__metadata_iterator_next(iterator);
          return Metadata::toJs(env, metadata);
        });
    }

    void init(const CallbackInfo& info) {
      if (!info[0].IsObject()) {
        throw TypeError::New(
          info.Env(),
          "Expected "s + info[0].ToString().Utf8Value() + " to be object");
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

  Napi::Value Chain::createIterator(const CallbackInfo& info) {
    EscapableHandleScope scope(info.Env());
    auto addon = info.Env().GetInstanceData<FlacAddon>();
    auto iterator = addon->iteratorConstructor.New({});
    FLAC__metadata_iterator_init(Iterator::Unwrap(iterator)->iterator, chain);
    return scope.Escape(iterator);
  }

  Napi::Value initMetadata2Chain(Env env, FlacAddon& addon) {
    EscapableHandleScope scope(env);
    return scope.Escape(Chain::init(env, addon));
  }

  Napi::Value initMetadata2Iterator(Env env, FlacAddon& addon) {
    EscapableHandleScope scope(env);
    return scope.Escape(Iterator::init(env, addon));
  }

}
