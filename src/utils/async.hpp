#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <optional>
#include <napi.h>

#include "pointer.hpp"
#include "js_utils.hpp"

namespace flac_bindings {

    using namespace Napi;

    template<typename DataType>
    struct ProgressRequest {
        #if defined(__GNUC__) and not defined(__clang__)
        std::shared_ptr<DataType[]> data;
        #else
        std::shared_ptr<DataType> data;
        #endif
        size_t count = 0;
        std::shared_ptr<volatile bool> completed;
        std::shared_ptr<std::mutex> mutex;
        std::shared_ptr<std::condition_variable> cond;
        std::shared_ptr<volatile bool> deferred;

        inline ProgressRequest() {}

        inline ProgressRequest(const DataType* data, size_t count): count(count) {
            this->data.reset(new DataType[count]);
            std::copy(data, data + count, this->data.get());
            completed = std::make_shared<volatile bool>(false);
            mutex = std::make_shared<std::mutex>();
            cond = std::make_shared<std::condition_variable>();
            deferred = std::make_shared<volatile bool>(false);
        }

        inline void notifyCompleted() const {
            std::lock_guard<std::mutex> lg(*mutex);
            *completed = true;
            cond->notify_one();
        }

        inline void wait() const {
            std::unique_lock<std::mutex> ul(*mutex);
            cond->wait(ul, [this] () { return *completed; });
        }
    };

    template<typename T, typename P = char>
    class AsyncBackgroundTask: public AsyncProgressQueueWorker<ProgressRequest<P>> {
    public:
        typedef typename AsyncProgressQueueWorker<ProgressRequest<P>>::ExecutionProgress NapiExecutionProgress;

        class ExecutionProgress {
            AsyncBackgroundTask<T, P>* self;
            const NapiExecutionProgress& progress;
            volatile bool completed = false;
            const ProgressRequest<P>* currentProgressRequest = nullptr;

            friend class AsyncBackgroundTask<T, P>;

            typedef typename std::function<void(const CallbackInfo&, const P*)> PromiseCallback;
            typedef typename std::function<void(ExecutionProgress &, const CallbackInfo&, const P*)> FullPromiseCallback;
            struct PromiseContext {
                PromiseCallback resolve, reject;
                ProgressRequest<P>* req = nullptr;
                AsyncBackgroundTask<T, P>* self;

                ~PromiseContext() {
                    delete req;
                }
            };

            ExecutionProgress(
                AsyncBackgroundTask<T, P>* self,
                const NapiExecutionProgress& progress
            ): self(self), progress(progress) {}

            static Value promiseThen(const CallbackInfo& info) {
                auto* p = (PromiseContext*) info.Data();
                if(p->resolve) {
                    try {
                        p->resolve(info, p->req->data.get());
                    } catch(const Error& error) {
                        p->self->context->reject(error.Value());
                    }
                }

                p->req->notifyCompleted();
                delete p;
                return info.Env().Undefined();
            }

            static Value promiseCatch(const CallbackInfo& info) {
                auto* p = (PromiseContext*) info.Data();
                if(p->reject) {
                    try {
                        p->reject(info, p->req->data.get());
                    } catch(const Error& error) {
                        p->self->context->reject(error.Value());
                    }
                } else if(info[0].IsObject()) {
                    p->self->context->reject(info[0].template As<Object>());
                } else {
                    p->self->context->reject(Error::New(info.Env(), info[0].ToString()));
                }

                p->req->notifyCompleted();
                delete p;
                return info.Env().Undefined();
            }

        public:
            void resolve(const T& returnValue) noexcept {
                if(!completed) {
                    self->returnValue = returnValue;
                    completed = true;
                }
            }

            void reject(const std::string& message) noexcept {
                if(!completed) {
                    self->SetError(message);
                    completed = true;
                }
            }

            void reject(const Object& errorValue) {
                if(!completed) {
                    self->SetError("<mock>");
                    self->exceptionValue.Reset(errorValue, 1);
                    completed = true;
                }
            }

            void reject(const Error& error) {
                if(!completed) {
                    HandleScope scope(error.Env());
                    self->SetError("<mock>");
                    self->exceptionValue.Reset(error.Value(), 1);
                    completed = true;
                }
            }

            void sendProgress(const P* data, size_t count = 1) {
                if(!completed) {
                    ProgressRequest<P> req(data, count);
                    progress.Send(&req, 1);
                }
            }

            inline void sendProgress(const P& data) {
                sendProgress(&data);
            }

            void sendProgressAndWait(const P* data, size_t count = 1) {
                if(!completed) {
                    ProgressRequest<P> req(data, count);
                    progress.Send(&req, 1);
                    req.wait();
                }
            }

            inline void sendProgressAndWait(const P& data) {
                sendProgressAndWait(&data);
            }

            void defer(
                Promise promise,
                FullPromiseCallback resolve = nullptr,
                FullPromiseCallback reject = nullptr
            ) {
                using namespace std::placeholders;
                Napi::Env env = self->Env();
                HandleScope scope(env);

                auto context = new PromiseContext {
                    resolve ? std::bind(resolve, *this, _1, _2) : PromiseCallback(nullptr),
                    reject ? std::bind(reject, *this, _1, _2) : PromiseCallback(nullptr),
                    new ProgressRequest<P>(*currentProgressRequest),
                    self
                };
                auto thenFunction = Function::New(env, promiseThen, "asyncBackgroundTask_executionProgress_then", context);
                auto catchFunction = Function::New(env, promiseCatch, "asyncBackgroundTask_executionProgress_catch", context);

                handleAsync(
                    promise,
                    thenFunction,
                    catchFunction
                );

                *currentProgressRequest->deferred = true;
            }

            inline AsyncBackgroundTask<T, P>* getTask() const {
                return self;
            }

            inline bool isCompleted() {
                return completed;
            }
        };

        typedef typename std::function<void(Napi::Env&, ExecutionProgress&, const P*, size_t)> ProgressCallback;
        typedef std::function<void(ExecutionProgress&)> FunctionCallback;
        typedef std::function<Value(Napi::Env, T)> ValueMapFunction;

    protected:
        ExecutionProgress* context = nullptr;
        Promise::Deferred resolver;
        FunctionCallback function;
        ProgressCallback progress;
        ValueMapFunction converter;
        std::optional<T> returnValue = std::nullopt;
        Reference<Object> exceptionValue;

        static Value _doNothing(const CallbackInfo& i) { return i.Env().Undefined(); }

    public:
        AsyncBackgroundTask(
            const Napi::Env& env,
            FunctionCallback function,
            ProgressCallback progress,
            const char* name,
            ValueMapFunction converter
        ): AsyncProgressQueueWorker<ProgressRequest<P>>(Function::New(env, _doNothing, "_doNothing"), name),
            resolver(Promise::Deferred::New(env)),
            function(function),
            progress(progress),
            converter(converter)
        {}

        ~AsyncBackgroundTask() {
            if(!exceptionValue.IsEmpty()) {
                exceptionValue.Unref();
            }
        }

        inline ExecutionProgress* getExecutionProgress() const {
            return context;
        }

        inline Promise getPromise() const {
            EscapableHandleScope scope(this->Env());
            return scope.Escape(resolver.Promise()).template As<Promise>();
        }

        virtual void Execute(const NapiExecutionProgress& progress) override {
            if(function) {
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
            if(returnValue && converter) {
                resolver.Resolve(converter(env, returnValue.value()));
            } else {
                resolver.Resolve(env.Undefined());
            }
        }

        virtual void OnError(const Error& error) override {
            Napi::Env env = this->Env();
            HandleScope scope(env);
            if(!exceptionValue.IsEmpty()) {
                resolver.Reject(exceptionValue.Value());
            } else {
                resolver.Reject(error.Value());
            }
        }

        virtual void OnProgress(const ProgressRequest<P>* req, size_t size) override {
            (void) size; //In RELEASE this variable is not used :)
            assert(size == 1);
            if(progress) {
                Napi::Env env = this->Env();
                HandleScope scope(env);

                try {
                    context->currentProgressRequest = req;
                    progress(env, *context, req->data.get(), req->count);
                } catch(const Error& e) {
                    context->reject(e);
                }

                context->currentProgressRequest = nullptr;
                if(!*req->deferred) {
                    req->notifyCompleted();
                }
            }
        }
    };

}
