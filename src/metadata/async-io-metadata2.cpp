#include "metadata2.hpp"

namespace flac_bindings {

  using namespace Napi;
  using namespace std::placeholders;

  AsyncFlacIOWork::AsyncFlacIOWork(
    std::function<bool(FLAC__IOHandle, FLAC__IOCallbacks)> f,
    const char* name,
    const Object& obj,
    std::function<void(const Napi::Env&, bool)> checkStatus):
      AsyncBackgroundTask<bool, FlacIOWorkRequest>(
        obj.Env(),
        [this, f](auto c) {
          this->ptr1 = std::make_tuple(&this->cbk1, &c);
          auto ret = f(&this->ptr1, this->cbk1.generateIOCallbacks());
          c.resolve(ret);
        },
        std::bind(&AsyncFlacIOWork::doAsyncWork, this, _1, _2, _3),
        name,
        [checkStatus](auto env, auto value) {
          checkStatus(env, value);
          return env.Undefined();
        }),
      cbk1(obj), cbk2() {}

  AsyncFlacIOWork::AsyncFlacIOWork(
    std::function<bool(FLAC__IOHandle, FLAC__IOCallbacks, FLAC__IOHandle, FLAC__IOCallbacks)> f,
    const char* name,
    const Object& obj1,
    const Object& obj2,
    std::function<void(const Napi::Env&, bool)> checkStatus):
      AsyncBackgroundTask<bool, FlacIOWorkRequest>(
        obj1.Env(),
        [this, f](auto c) {
          this->ptr1 = std::make_tuple(&this->cbk1, &c);
          this->ptr2 = std::make_tuple(&this->cbk2, &c);
          auto ret =
            f(&this->ptr1,
              this->cbk1.generateIOCallbacks(),
              &this->ptr2,
              this->cbk2.generateIOCallbacks());
          c.resolve(ret);
        },
        std::bind(&AsyncFlacIOWork::doAsyncWork, this, _1, _2, _3),
        name,
        [checkStatus](auto env, auto value) {
          checkStatus(env, value);
          return env.Undefined();
        }),
      cbk1(obj1), cbk2(obj2) {}

  template<typename Type>
  static std::function<void(const Napi::Value& result)>
    generateProcessResult(const std::function<void(Type)>& setter) {
    return [setter](auto result) {
      auto returnValue = maybeNumberFromJs<Type>(result).value_or(booleanFromJs<Type>(result));
      setter(returnValue);
    };
  }

  void AsyncFlacIOWork::doAsyncWork(
    const Napi::Env& env,
    AsyncFlacIOWork::ExecutionProgress& prog,
    const std::shared_ptr<FlacIOWorkRequest>& work) {
    auto async = nullptr;
    Napi::Value result = env.Null();
    std::function<void(const Napi::Value& result)> processResult;
    auto io = (AsyncFlacIOWork::IOCallbacks*) work->cbks;

    switch (work->type) {
      case FlacIOWorkRequest::Read: {
        sharedBufferRef.setFromWrap(env, (uint8_t*) work->ptr, *work->bytes);
        result = io->readCallback.MakeCallback(
          env.Global(),
          {
            sharedBufferRef.value(),
            numberToJs(env, work->sizeOfItem),
            numberToJs(env, work->nitems),
          },
          async);
        processResult = generateProcessResult<size_t>([work](auto v) { *work->bytes = v; });
        break;
      }

      case FlacIOWorkRequest::Write: {
        sharedBufferRef.setFromWrap(env, (uint8_t*) work->ptr, *work->bytes);
        result = io->writeCallback.MakeCallback(
          env.Global(),
          {
            sharedBufferRef.value(),
            numberToJs(env, work->sizeOfItem),
            numberToJs(env, work->nitems),
          },
          async);
        processResult = generateProcessResult<size_t>([work](auto v) { *work->bytes = v; });
        break;
      }

      case FlacIOWorkRequest::Seek: {
        String direction;
        switch (*work->genericReturn) {
          case SEEK_SET:
            direction = String::New(env, "set");
            break;
          case SEEK_CUR:
            direction = String::New(env, "cur");
            break;
          case SEEK_END:
            direction = String::New(env, "end");
            break;
          default:
            direction = String::New(env, "set");
        }

        result = io->seekCallback.MakeCallback(
          env.Global(),
          {
            numberToJs(env, *work->offset),
            direction,
          },
          async);
        processResult = generateProcessResult<int>([work](auto v) { *work->genericReturn = v; });
        break;
      }

      case FlacIOWorkRequest::Tell: {
        result = io->tellCallback.MakeCallback(env.Global(), {}, async);
        processResult = generateProcessResult<int64_t>([work](auto v) { *work->offset = v; });
        break;
      }

      case FlacIOWorkRequest::Eof: {
        result = io->eofCallback.MakeCallback(env.Global(), {}, async);
        processResult = generateProcessResult<bool>([work](auto v) { *work->genericReturn = v; });
        break;
      }

      case FlacIOWorkRequest::Close: {
        result = io->closeCallback.MakeCallback(env.Global(), {}, async);
        processResult = generateProcessResult<int>([work](auto v) { *work->genericReturn = v; });
        break;
      }
    }

    if (result.IsPromise()) {
      prog.defer(result.As<Promise>(), [processResult](auto&, auto& info, auto) {
        if (processResult) {
          processResult(info[0]);
        }
      });
    } else {
      if (processResult) {
        processResult(result);
      }
    }
  }

  AsyncFlacIOWork::IOCallbacks::IOCallbacks() {}

  AsyncFlacIOWork::IOCallbacks::IOCallbacks(const Object& obj) {
    maybeFunctionIntoRef(this->readCallback, obj["read"]);
    maybeFunctionIntoRef(this->writeCallback, obj["write"]);
    maybeFunctionIntoRef(this->seekCallback, obj["seek"]);
    maybeFunctionIntoRef(this->tellCallback, obj["tell"]);
    maybeFunctionIntoRef(this->eofCallback, obj["eof"]);
    maybeFunctionIntoRef(this->closeCallback, obj["close"]);
  }

  AsyncFlacIOWork::IOCallbacks::~IOCallbacks() {
    if (!readCallback.IsEmpty())
      readCallback.Unref();
    if (!writeCallback.IsEmpty())
      writeCallback.Unref();
    if (!seekCallback.IsEmpty())
      seekCallback.Unref();
    if (!tellCallback.IsEmpty())
      tellCallback.Unref();
    if (!eofCallback.IsEmpty())
      eofCallback.Unref();
    if (!closeCallback.IsEmpty())
      closeCallback.Unref();
  }

  typedef std::tuple<void*, AsyncFlacIOWork::ExecutionProgress*> FlacIOArg;
  static size_t flacIORead(void* ptr, size_t size, size_t numberOfMembers, void* c) {
    FlacIOArg& ctx = *(FlacIOArg*) c;
    auto* ec = std::get<1>(ctx);
    auto req = std::make_shared<FlacIOWorkRequest>(FlacIOWorkRequest::Read);
    size_t bytes = size * numberOfMembers;

    req->cbks = std::get<0>(ctx);
    req->ptr = ptr;
    req->bytes = &bytes;
    req->nitems = numberOfMembers;
    req->sizeOfItem = size;

    ec->sendProgressAndWait(req);

    return bytes / size;
  }

  static size_t flacIOWrite(const void* ptr, size_t size, size_t numberOfMembers, void* c) {
    FlacIOArg& ctx = *(FlacIOArg*) c;
    auto* ec = std::get<1>(ctx);
    auto req = std::make_shared<FlacIOWorkRequest>(FlacIOWorkRequest::Write);
    size_t bytes = size * numberOfMembers;

    if (size * numberOfMembers == 0) {
      // Sometimes, the callback is called with 0 bytes to write
      // Discard the call completely :)
      return 0;
    }

    req->cbks = std::get<0>(ctx);
    req->ptr = (void*) ptr;
    req->bytes = &bytes;
    req->nitems = numberOfMembers;
    req->sizeOfItem = size;

    ec->sendProgressAndWait(req);

    return bytes / size;
  }

  static int flacIOSeek(void* c, int64_t offset, int whence) {
    FlacIOArg& ctx = *(FlacIOArg*) c;
    auto* ec = std::get<1>(ctx);
    auto req = std::make_shared<FlacIOWorkRequest>(FlacIOWorkRequest::Seek);
    int ret = whence;

    req->cbks = std::get<0>(ctx);
    req->offset = &offset;
    req->genericReturn = &ret;

    ec->sendProgressAndWait(req);

    return ret;
  }

  static int64_t flacIOTell(void* c) {
    FlacIOArg& ctx = *(FlacIOArg*) c;
    auto* ec = std::get<1>(ctx);
    auto req = std::make_shared<FlacIOWorkRequest>(FlacIOWorkRequest::Tell);
    int64_t offset = -1;

    req->cbks = std::get<0>(ctx);
    req->offset = &offset;

    ec->sendProgressAndWait(req);

    return offset;
  }

  static int flacIOEof(void* c) {
    FlacIOArg& ctx = *(FlacIOArg*) c;
    auto* ec = std::get<1>(ctx);
    auto req = std::make_shared<FlacIOWorkRequest>(FlacIOWorkRequest::Eof);
    int ret = 0;

    req->cbks = std::get<0>(ctx);
    req->genericReturn = &ret;

    ec->sendProgressAndWait(req);

    return ret;
  }

  static int flacIOClose(void* c) {
    FlacIOArg& ctx = *(FlacIOArg*) c;
    auto* ec = std::get<1>(ctx);
    auto req = std::make_shared<FlacIOWorkRequest>(FlacIOWorkRequest::Close);
    int ret = 0;

    req->cbks = std::get<0>(ctx);
    req->genericReturn = &ret;

    ec->sendProgressAndWait(req);

    return ret;
  }

  FLAC__IOCallbacks AsyncFlacIOWork::IOCallbacks::generateIOCallbacks() {
    return {
      !readCallback.IsEmpty() ? flacIORead : nullptr,
      !writeCallback.IsEmpty() ? flacIOWrite : nullptr,
      !seekCallback.IsEmpty() ? flacIOSeek : nullptr,
      !tellCallback.IsEmpty() ? flacIOTell : nullptr,
      !eofCallback.IsEmpty() ? flacIOEof : nullptr,
      !closeCallback.IsEmpty() ? flacIOClose : nullptr,
    };
  }

}
