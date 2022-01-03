#pragma once

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <napi.h>
#include <optional>

namespace flac_bindings {

  using namespace Napi;

  template<typename DataType>
  struct ProgressRequest {
    std::shared_ptr<DataType> data;
    volatile bool completed = false;
    std::mutex mutex;
    std::condition_variable cond;
    volatile bool deferred = false;

    inline ProgressRequest(const std::shared_ptr<DataType>& data): data(data) {}

    inline void notifyCompleted() {
      std::lock_guard<std::mutex> lg(mutex);
      completed = true;
      cond.notify_one();
    }

    inline void wait() {
      std::unique_lock<std::mutex> ul(mutex);
      cond.wait(ul, [this]() { return completed; });
    }
  };

  template<typename P>
  using AsyncBackgroundTaskBase = AsyncProgressQueueWorker<std::shared_ptr<ProgressRequest<P>>>;

  template<typename T, typename P = char>
  class AsyncBackgroundTask: public AsyncBackgroundTaskBase<P> {
  public:
    typedef typename AsyncBackgroundTaskBase<P>::ExecutionProgress NapiExecutionProgress;

    class ExecutionProgress {
      AsyncBackgroundTask<T, P>* self;
      const NapiExecutionProgress& progress;
      volatile bool completed = false;
      std::shared_ptr<ProgressRequest<P>> currentProgressRequest;

      friend class AsyncBackgroundTask<T, P>;

      typedef typename std::function<void(const CallbackInfo&, const std::shared_ptr<P>&)>
        PromiseCallback;
      typedef typename std::function<
        void(ExecutionProgress&, const CallbackInfo&, const std::shared_ptr<P>&)>
        FullPromiseCallback;
      struct PromiseContext {
        PromiseCallback resolve, reject;
        std::shared_ptr<ProgressRequest<P>> req;
        AsyncBackgroundTask<T, P>* self;
      };

      ExecutionProgress(AsyncBackgroundTask<T, P>* self, const NapiExecutionProgress& progress):
          self(self), progress(progress) {}

      static void promiseThen(const CallbackInfo& info) {
        auto* p = (PromiseContext*) info.Data();
        if (p->resolve) {
          try {
            p->resolve(info, p->req->data);
          } catch (const Error& error) {
            p->self->context->reject(error.Value());
          }
        }

        p->req->notifyCompleted();
        delete p;
      }

      static void promiseCatch(const CallbackInfo& info) {
        auto* p = (PromiseContext*) info.Data();
        if (p->reject) {
          try {
            p->reject(info, p->req->data);
          } catch (const Error& error) {
            p->self->context->reject(error.Value());
          }
        } else if (info[0].IsObject()) {
          p->self->context->reject(info[0].template As<Object>());
        } else {
          p->self->context->reject(Error::New(info.Env(), info[0].ToString()));
        }

        p->req->notifyCompleted();
        delete p;
      }

    public:
      void resolve(const T& returnValue) noexcept {
        if (!completed) {
          self->returnValue = returnValue;
          completed = true;
        }
      }

      void reject(const std::string& message) noexcept {
        if (!completed) {
          self->SetError(message);
          completed = true;
        }
      }

      void reject(const Object& errorValue) {
        if (!completed) {
          self->SetError("<mock>");
          self->exceptionValue.Reset(errorValue, 1);
          completed = true;
        }
      }

      void reject(const Error& error) {
        if (!completed) {
          HandleScope scope(error.Env());
          self->SetError("<mock>");
          self->exceptionValue.Reset(error.Value(), 1);
          completed = true;
        }
      }

      void sendProgressAndWait(const std::shared_ptr<P>& data) {
        if (!completed) {
          auto req = std::make_shared<ProgressRequest<P>>(data);
          progress.Send(&req, 1);
          req->wait();
        }
      }

      void defer(
        Promise promise,
        FullPromiseCallback resolve = nullptr,
        FullPromiseCallback reject = nullptr) {
        using namespace std::placeholders;
        Napi::Env env = self->Env();
        HandleScope scope(env);

        auto context = new PromiseContext {
          resolve ? std::bind(resolve, *this, _1, _2) : PromiseCallback(nullptr),
          reject ? std::bind(reject, *this, _1, _2) : PromiseCallback(nullptr),
          currentProgressRequest,
          self};
        auto thenFunction =
          Function::New(env, promiseThen, "asyncBackgroundTask_executionProgress_then", context);
        auto catchFunction =
          Function::New(env, promiseCatch, "asyncBackgroundTask_executionProgress_catch", context);

        Promise promiseAfterThen =
          promise.Get("then").As<Function>().Call(promise, {thenFunction}).template As<Promise>();
        promiseAfterThen.Get("catch")
          .As<Function>()
          .Call(promiseAfterThen, {catchFunction})
          .template As<Promise>();

        currentProgressRequest->deferred = true;
      }

      inline AsyncBackgroundTask<T, P>* getTask() const {
        return self;
      }

      inline bool isCompleted() {
        return completed;
      }
    };

    typedef typename std::function<void(Napi::Env&, ExecutionProgress&, const std::shared_ptr<P>&)>
      ProgressCallback;
    typedef std::function<void(ExecutionProgress&)> FunctionCallback;
    typedef std::function<Value(Napi::Env, T)> ValueMapFunction;
    typedef T ValueType;

  protected:
    ExecutionProgress* context = nullptr;
    Promise::Deferred resolver;
    FunctionCallback function;
    ProgressCallback progress;
    ValueMapFunction converter;
    std::optional<T> returnValue = std::nullopt;
    Reference<Object> exceptionValue;

    static Value _doNothing(const CallbackInfo& i) {
      return i.Env().Undefined();
    }

  public:
    AsyncBackgroundTask(
      const Napi::Env& env,
      FunctionCallback function,
      ProgressCallback progress,
      const char* name,
      ValueMapFunction converter):
        AsyncBackgroundTaskBase<P>(Function::New(env, _doNothing, "_doNothing"), name),
        resolver(Promise::Deferred::New(env)), function(function), progress(progress),
        converter(converter) {}

    ~AsyncBackgroundTask() {
      if (!exceptionValue.IsEmpty()) {
        exceptionValue.Unref();
      }
    }

    inline Promise getPromise() const {
      EscapableHandleScope scope(this->Env());
      return scope.Escape(resolver.Promise()).template As<Promise>();
    }

    virtual void Execute(const NapiExecutionProgress& progress) override {
      if (function) {
        context = new ExecutionProgress(this, progress);
        function(*context);
        delete context;
      } else {
        this->SetError("No lambda function received in AsyncBackgroundTask");
      }
    }

    virtual void OnOK() override {
      Napi::Env env = this->Env();
      HandleScope scope(env);
      assert(exceptionValue.IsEmpty());
      if (returnValue && converter) {
        try {
          resolver.Resolve(converter(env, returnValue.value()));
        } catch (const Napi::Error& error) {
          resolver.Reject(error.Value());
        }
      } else {
        resolver.Resolve(env.Undefined());
      }
    }

    virtual void OnError(const Error& error) override {
      Napi::Env env = this->Env();
      HandleScope scope(env);
      if (!exceptionValue.IsEmpty()) {
        resolver.Reject(exceptionValue.Value());
      } else {
        resolver.Reject(error.Value());
      }
    }

    virtual void
      OnProgress(const std::shared_ptr<ProgressRequest<P>>* requestPtr, size_t size) override {
      (void) size; // In RELEASE this variable is not used :)
      assert(size == 1);
      if (progress) {
        const auto& req = *requestPtr;
        Napi::Env env = this->Env();
        HandleScope scope(env);

        try {
          context->currentProgressRequest = req;
          progress(env, *context, req->data);
        } catch (const Error& e) {
          context->reject(e);
        }

        context->currentProgressRequest.reset();
        if (!req->deferred) {
          req->notifyCompleted();
        }
      }
    }
  };

}
