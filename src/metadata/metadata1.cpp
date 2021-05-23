#include "../mappings/mappings.hpp"
#include "../mappings/native_async_iterator.hpp"
#include "../mappings/native_iterator.hpp"
#include "../utils/async.hpp"
#include "../utils/enum.hpp"
#include <FLAC/metadata.h>
#include <memory>

namespace flac_bindings {

  using namespace Napi;

  class SimpleIterator: public ObjectWrap<SimpleIterator> {
    FLAC__Metadata_SimpleIterator* it;

    void throwIfStatusIsNotOk(const Napi::Env& env) {
      auto status = FLAC__metadata_simple_iterator_status(it);
      if (status != FLAC__METADATA_SIMPLE_ITERATOR_STATUS_OK) {
        throw Error::New(
          env,
          "Operation failed: "s + &FLAC__Metadata_SimpleIteratorStatusString[status][38]);
      }
    }

    template<typename T>
    void rejectIfStatusIsNotOk(typename AsyncBackgroundTask<T>::ExecutionProgress& c) {
      auto status = FLAC__metadata_simple_iterator_status(it);
      if (status != FLAC__METADATA_SIMPLE_ITERATOR_STATUS_OK) {
        c.reject("Operation failed: "s + &FLAC__Metadata_SimpleIteratorStatusString[status][38]);
      }
    }

    template<typename R>
    Napi::Value asyncImpl(
      const Napi::Env& env,
      const char* name,
      const std::initializer_list<Napi::Value>& args,
      std::function<R()> impl,
      std::function<Napi::Value(const Napi::Env&, R)> converter = booleanToJs<R>) {
      EscapableHandleScope scope(env);
      auto worker = new AsyncBackgroundTask<R>(
        env,
        [impl](auto c) {
          auto res = impl();
          c.resolve(res);
        },
        nullptr,
        name,
        converter);

      for (auto i = args.begin(); i != args.end(); i += 1) {
        worker->Receiver().Set(i - args.begin(), *i);
      }

      worker->Queue();
      return scope.Escape(worker->getPromise());
    }

    void checkStatus(Napi::Env env, bool succeeded) {
      if (!succeeded) {
        auto status = FLAC__metadata_simple_iterator_status(it);
        // remove prefix FLAC__METADATA_SIMPLE_ITERATOR_STATUS_
        auto statusString = FLAC__Metadata_SimpleIteratorStatusString[status] + 38;
        auto error = Error::New(env, "SimpleIterator initialization failed: "s + statusString);
        error.Set("status", numberToJs(env, status));
        error.Set("statusString", String::New(env, statusString));
        throw error;
      }
    }

  public:
    static Function init(Napi::Env env, FlacAddon& addon) {
      EscapableHandleScope scope(env);

      auto constructor = DefineClass(
        env,
        "SimpleIterator",
        {
          InstanceMethod(Napi::Symbol::WellKnown(env, "iterator"), &SimpleIterator::iterator),
          InstanceMethod(
            Napi::Symbol::WellKnown(env, "asyncIterator"),
            &SimpleIterator::asyncIterator),
          InstanceMethod("status", &SimpleIterator::status),
          InstanceMethod("init", &SimpleIterator::init),
          InstanceMethod("initAsync", &SimpleIterator::initAsync),
          InstanceMethod("isWritable", &SimpleIterator::isWritable),
          InstanceMethod("isLast", &SimpleIterator::isLast),
          InstanceMethod("next", &SimpleIterator::next),
          InstanceMethod("nextAsync", &SimpleIterator::nextAsync),
          InstanceMethod("prev", &SimpleIterator::prev),
          InstanceMethod("prevAsync", &SimpleIterator::prevAsync),
          InstanceMethod("getBlockOffset", &SimpleIterator::getBlockOffset),
          InstanceMethod("getBlockType", &SimpleIterator::getBlockType),
          InstanceMethod("getBlockLength", &SimpleIterator::getBlockLength),
          InstanceMethod("getApplicationId", &SimpleIterator::getApplicationId),
          InstanceMethod("getApplicationIdAsync", &SimpleIterator::getApplicationIdAsync),
          InstanceMethod("getBlock", &SimpleIterator::getBlock),
          InstanceMethod("getBlockAsync", &SimpleIterator::getBlockAsync),
          InstanceMethod("setBlock", &SimpleIterator::setBlock),
          InstanceMethod("setBlockAsync", &SimpleIterator::setBlockAsync),
          InstanceMethod("insertBlockAfter", &SimpleIterator::insertBlockAfter),
          InstanceMethod("insertBlockAfterAsync", &SimpleIterator::insertBlockAfterAsync),
          InstanceMethod("deleteBlock", &SimpleIterator::deleteBlock),
          InstanceMethod("deleteBlockAsync", &SimpleIterator::deleteBlockAsync),
        });
      c_enum::declareInObject(constructor, "Status", createStatusEnum);

      addon.simpleIteratorConstructor = Persistent(constructor);

      return scope.Escape(objectFreeze(constructor)).As<Function>();
    }

    SimpleIterator(const CallbackInfo& info): ObjectWrap<SimpleIterator>(info) {
      it = FLAC__metadata_simple_iterator_new();
      if (it == nullptr) {
        throw Error::New(info.Env(), "Could not allocate memory");
      }
    }

    ~SimpleIterator() {
      FLAC__metadata_simple_iterator_delete(it);
    }

    Napi::Value iterator(const CallbackInfo& info) {
      while (FLAC__metadata_simple_iterator_prev(it))
        ;
      std::shared_ptr<bool> pastEnd(new bool(false));
      return NativeIterator::newIterator(
        info.Env(),
        [this, pastEnd](auto env, auto) -> NativeIterator::IterationReturnValue {
          if (FLAC__metadata_simple_iterator_is_last(it)) {
            if (*pastEnd) {
              return {};
            }

            *pastEnd = true;
          }

          auto metadata = FLAC__metadata_simple_iterator_get_block(it);
          throwIfStatusIsNotOk(env);
          FLAC__metadata_simple_iterator_next(it);
          return Metadata::toJs(env, metadata, true);
        });
    }

    Napi::Value asyncIterator(const CallbackInfo& info) {
      using NAI = NativeAsyncIterator<FLAC__StreamMetadata*>;
      std::shared_ptr<bool> hasRollbacked(new bool(false));
      std::shared_ptr<bool> pastEnd(new bool(false));
      return NAI::newIterator(
        info.Env(),
        "flac_bindings::SimpleIterator::asyncIterator",
        [this, hasRollbacked, pastEnd](auto c, auto) -> NAI::IterationReturnValue {
          if (!*hasRollbacked) {
            while (FLAC__metadata_simple_iterator_prev(it))
              ;
            *hasRollbacked = true;
          }

          if (FLAC__metadata_simple_iterator_is_last(it)) {
            if (*pastEnd) {
              return std::nullopt;
            }

            *pastEnd = true;
          }

          auto metadata = FLAC__metadata_simple_iterator_get_block(it);
          rejectIfStatusIsNotOk<std::optional<FLAC__StreamMetadata*>>(c);
          FLAC__metadata_simple_iterator_next(it);
          return metadata;
        },
        [](auto env, auto* metadata) { return Metadata::toJs(env, metadata, true); });
    }

    Napi::Value status(const CallbackInfo& info) {
      return numberToJs(info.Env(), FLAC__metadata_simple_iterator_status(it));
    }

    void init(const CallbackInfo& info) {
      auto path = stringFromJs(info[0]);
      auto readOnly = maybeBooleanFromJs<FLAC__bool>(info[1]).value_or(false);
      auto preserve = maybeBooleanFromJs<FLAC__bool>(info[2]).value_or(false);
      auto res = FLAC__metadata_simple_iterator_init(it, path.c_str(), readOnly, preserve);
      checkStatus(info.Env(), res);
    }

    Napi::Value initAsync(const CallbackInfo& info) {
      auto path = stringFromJs(info[0]);
      auto readOnly = maybeBooleanFromJs<FLAC__bool>(info[1]).value_or(false);
      auto preserve = maybeBooleanFromJs<FLAC__bool>(info[2]).value_or(false);
      return asyncImpl<FLAC__bool>(
        info.Env(),
        "flac_bindings::SimpleIterator::initAsync",
        {info.This()},
        [this, path, readOnly, preserve]() {
          return FLAC__metadata_simple_iterator_init(it, path.c_str(), readOnly, preserve);
        },
        [this](auto env, auto value) {
          this->checkStatus(env, value);
          return env.Undefined();
        });
    }

    Napi::Value isWritable(const CallbackInfo& info) {
      return booleanToJs(info.Env(), FLAC__metadata_simple_iterator_is_writable(it));
    }

    Napi::Value isLast(const CallbackInfo& info) {
      return booleanToJs(info.Env(), FLAC__metadata_simple_iterator_is_last(it));
    }

    Napi::Value next(const CallbackInfo& info) {
      return booleanToJs(info.Env(), FLAC__metadata_simple_iterator_next(it));
    }

    Napi::Value nextAsync(const CallbackInfo& info) {
      return asyncImpl<FLAC__bool>(
        info.Env(),
        "flac_bindings::SimpleIterator::nextAsync",
        {info.This()},
        [this]() { return FLAC__metadata_simple_iterator_next(it); });
    }

    Napi::Value prev(const CallbackInfo& info) {
      return booleanToJs(info.Env(), FLAC__metadata_simple_iterator_prev(it));
    }

    Napi::Value prevAsync(const CallbackInfo& info) {
      return asyncImpl<FLAC__bool>(
        info.Env(),
        "flac_bindings::SimpleIterator::prevAsync",
        {info.This()},
        [this]() { return FLAC__metadata_simple_iterator_prev(it); });
    }

    Napi::Value getBlockOffset(const CallbackInfo& info) {
      auto ret = FLAC__metadata_simple_iterator_get_block_offset(it);
      return numberToJs(info.Env(), ret);
    }

    Napi::Value getBlockType(const CallbackInfo& info) {
      auto ret = FLAC__metadata_simple_iterator_get_block_type(it);
      return numberToJs(info.Env(), ret);
    }

    Napi::Value getBlockLength(const CallbackInfo& info) {
      auto ret = FLAC__metadata_simple_iterator_get_block_length(it);
      return numberToJs(info.Env(), ret);
    }

    Napi::Value getApplicationId(const CallbackInfo& info) {
      FLAC__byte id[4];
      auto ret = FLAC__metadata_simple_iterator_get_application_id(it, id);
      if (ret) {
        return Buffer<FLAC__byte>::Copy(info.Env(), id, 4);
      }

      return info.Env().Null();
    }

    Napi::Value getApplicationIdAsync(const CallbackInfo& info) {
      return asyncImpl<FLAC__byte*>(
        info.Env(),
        "flac_bindings::SimpleIterator:getApplicationId",
        {info.This()},
        [this]() -> FLAC__byte* {
          FLAC__byte* id = new FLAC__byte[4];
          if (FLAC__metadata_simple_iterator_get_application_id(it, id)) {
            return id;
          }

          delete[] id;
          return nullptr;
        },
        [](auto env, FLAC__byte* id) -> Napi::Value {
          if (id != nullptr) {
            auto buffer = Buffer<FLAC__byte>::Copy(env, id, 4);
            delete[] id;
            return buffer;
          }

          return env.Null();
        });
    }

    Napi::Value getBlock(const CallbackInfo& info) {
      auto metadata = FLAC__metadata_simple_iterator_get_block(it);
      return Metadata::toJs(info.Env(), metadata);
    }

    Napi::Value getBlockAsync(const CallbackInfo& info) {
      return asyncImpl<FLAC__StreamMetadata*>(
        info.Env(),
        "flac_bindings::SimpleIterator::getBlockAsync",
        {info.This()},
        [this]() { return FLAC__metadata_simple_iterator_get_block(it); },
        [](auto env, auto metadata) { return Metadata::toJs(env, metadata, true); });
    }

    Napi::Value setBlock(const CallbackInfo& info) {
      auto metadata = Metadata::fromJs(info[0]);
      auto pad = maybeNumberFromJs<FLAC__bool>(info[1]).value_or(true);

      auto ret = FLAC__metadata_simple_iterator_set_block(it, metadata, pad);
      return booleanToJs(info.Env(), ret);
    }

    Napi::Value setBlockAsync(const CallbackInfo& info) {
      FLAC__StreamMetadata* metadata = Metadata::fromJs(info[0]);
      auto pad = maybeNumberFromJs<FLAC__bool>(info[1]).value_or(true);

      return asyncImpl<FLAC__bool>(
        info.Env(),
        "flac_bindings::SimpleIterator::setBlockAsync",
        {info.This(), info[0]},
        [this, metadata, pad]() {
          return FLAC__metadata_simple_iterator_set_block(it, metadata, pad);
        });
    }

    Napi::Value insertBlockAfter(const CallbackInfo& info) {
      auto metadata = Metadata::fromJs(info[0]);
      auto pad = maybeNumberFromJs<FLAC__bool>(info[1]).value_or(true);

      auto ret = FLAC__metadata_simple_iterator_insert_block_after(it, metadata, pad);
      return booleanToJs(info.Env(), ret);
    }

    Napi::Value insertBlockAfterAsync(const CallbackInfo& info) {
      FLAC__StreamMetadata* metadata = Metadata::fromJs(info[0]);
      auto pad = maybeNumberFromJs<FLAC__bool>(info[1]).value_or(true);

      return asyncImpl<FLAC__bool>(
        info.Env(),
        "flac_bindings::SimpleIterator::insertBlockAfterAsync",
        {info.This(), info[0]},
        [this, metadata, pad]() {
          return FLAC__metadata_simple_iterator_insert_block_after(it, metadata, pad);
        });
    }

    Napi::Value deleteBlock(const CallbackInfo& info) {
      auto pad = maybeNumberFromJs<FLAC__bool>(info[0]).value_or(true);

      auto ret = FLAC__metadata_simple_iterator_delete_block(it, pad);
      return booleanToJs(info.Env(), ret);
    }

    Napi::Value deleteBlockAsync(const CallbackInfo& info) {
      auto pad = maybeNumberFromJs<FLAC__bool>(info[0]).value_or(true);

      return asyncImpl<FLAC__bool>(
        info.Env(),
        "flac_bindings::SimpleIterator::deleteBlockAsync",
        {info.This(), info[0]},
        [this, pad]() { return FLAC__metadata_simple_iterator_delete_block(it, pad); });
    }

    static c_enum::DefineReturnType createStatusEnum(const Napi::Env& env) {
      auto obj1 = Object::New(env);
      auto obj2 = Object::New(env);
      c_enum::defineValue(obj1, obj2, "OK", FLAC__METADATA_SIMPLE_ITERATOR_STATUS_OK);
      c_enum::defineValue(
        obj1,
        obj2,
        "ILLEGAL_INPUT",
        FLAC__METADATA_SIMPLE_ITERATOR_STATUS_ILLEGAL_INPUT);
      c_enum::defineValue(
        obj1,
        obj2,
        "ERROR_OPENING_FILE",
        FLAC__METADATA_SIMPLE_ITERATOR_STATUS_ERROR_OPENING_FILE);
      c_enum::defineValue(
        obj1,
        obj2,
        "NOT_A_FLAC_FILE",
        FLAC__METADATA_SIMPLE_ITERATOR_STATUS_NOT_A_FLAC_FILE);
      c_enum::defineValue(
        obj1,
        obj2,
        "NOT_WRITABLE",
        FLAC__METADATA_SIMPLE_ITERATOR_STATUS_NOT_WRITABLE);
      c_enum::defineValue(
        obj1,
        obj2,
        "BAD_METADATA",
        FLAC__METADATA_SIMPLE_ITERATOR_STATUS_BAD_METADATA);
      c_enum::defineValue(
        obj1,
        obj2,
        "READ_ERROR",
        FLAC__METADATA_SIMPLE_ITERATOR_STATUS_READ_ERROR);
      c_enum::defineValue(
        obj1,
        obj2,
        "SEEK_ERROR",
        FLAC__METADATA_SIMPLE_ITERATOR_STATUS_SEEK_ERROR);
      c_enum::defineValue(
        obj1,
        obj2,
        "WRITE_ERROR",
        FLAC__METADATA_SIMPLE_ITERATOR_STATUS_WRITE_ERROR);
      c_enum::defineValue(
        obj1,
        obj2,
        "RENAME_ERROR",
        FLAC__METADATA_SIMPLE_ITERATOR_STATUS_RENAME_ERROR);
      c_enum::defineValue(
        obj1,
        obj2,
        "UNLINK_ERROR",
        FLAC__METADATA_SIMPLE_ITERATOR_STATUS_UNLINK_ERROR);
      c_enum::defineValue(
        obj1,
        obj2,
        "MEMORY_ALLOCATION_ERROR",
        FLAC__METADATA_SIMPLE_ITERATOR_STATUS_MEMORY_ALLOCATION_ERROR);
      c_enum::defineValue(
        obj1,
        obj2,
        "INTERNAL_ERROR",
        FLAC__METADATA_SIMPLE_ITERATOR_STATUS_INTERNAL_ERROR);
      return std::make_tuple(obj1, obj2);
    }
  };

  Function initMetadata1(Env env, FlacAddon& addon) {
    EscapableHandleScope scope(env);
    return scope.Escape(SimpleIterator::init(env, addon)).As<Function>();
  }

}
