#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <nan.h>

#include "./pointer.hpp"

namespace flac_bindings {

    template<typename T, typename P = char>
    class AsyncBackgroundTask: public Nan::AsyncProgressQueueWorker<P> {
    public:
        typedef typename Nan::AsyncProgressQueueWorker<P>::ExecutionProgress ExecutionProgress;

        class ExecutionContext {
            AsyncBackgroundTask<T, P>* self;
            const ExecutionProgress &progress;

            friend class AsyncBackgroundTask<T, P>;

            typedef typename std::function<void(const P*, Nan::NAN_METHOD_ARGS_TYPE)> PromiseCallback;
            typedef typename std::function<void(ExecutionContext &, const P*, Nan::NAN_METHOD_ARGS_TYPE)> FullPromiseCallback;
            struct PromiseContext {
                PromiseCallback resolve, reject;
                P* data;
                AsyncBackgroundTask<T, P>* self;

                ~PromiseContext() { delete data; }
            };

            ExecutionContext(
                AsyncBackgroundTask<T, P>* s,
                const ExecutionProgress &progress
            ): self(s), progress(progress) {}

            static NAN_METHOD(promiseThen) {
                auto* p = UnwrapPointer<PromiseContext>(info.Data());
                //Workaround https://github.com/nodejs/node/issues/5691 - This allows to handle promises without any warnings from node
                node::CallbackScope callbackScope(v8::Isolate::GetCurrent(), Nan::New<v8::Object>(), p->self->asyncContext);
                if(p->resolve) {
                    p->resolve(p->data, info);
                }
                delete p;
            }

            static NAN_METHOD(promiseCatch) {
                auto* p = UnwrapPointer<PromiseContext>(info.Data());
                //Workaround https://github.com/nodejs/node/issues/5691 - This allows to handle promises without any warnings from node
                node::CallbackScope callbackScope(v8::Isolate::GetCurrent(), Nan::New<v8::Object>(), p->self->asyncContext);
                if(p->reject) {
                    p->reject(p->data, info);
                } else {
                    p->self->executionContext->reject(info[0]);
                }
                delete p;
            }

        public:
            void resolve(const T &returnValue) {
                if(!self->completed) {
                    self->returnValue = Nan::Just(returnValue);
                    self->completed = true;
                }
            }

            void reject(const std::string &message) {
                if(!self->completed) {
                    self->SetErrorMessage(message.c_str());
                    self->completed = true;
                }
            }

            void reject(v8::Local<v8::Value> exception) {
                assert(v8::Isolate::GetCurrent() != nullptr);
                if(!self->completed) {
                    self->exceptionValue.Reset(exception);
                    self->SetErrorMessage("<mock>");
                    self->completed = true;
                }
            }

            void sendProgress(const P* data, size_t count = 1) {
                if(!self->completed) {
                    progress.Send(data, count);
                }
            }

            inline void sendProgress(const P &data) {
                sendProgress(&data);
            }

            void defer(v8::Local<v8::Promise> promise, const P* data, FullPromiseCallback resolve = nullptr, FullPromiseCallback reject = nullptr) {
                using namespace std::placeholders;
                auto context = new PromiseContext {
                    resolve ? std::bind(resolve, *this, _1, _2) : PromiseCallback(nullptr),
                    reject ? std::bind(reject, *this, _1, _2) : PromiseCallback(nullptr),
                    data != nullptr ? new P(*data) : nullptr,
                    self
                };
                auto contextBuffer = WrapPointer(context).ToLocalChecked();
                auto thenFunction = Nan::New<v8::Function>(promiseThen, contextBuffer);
                auto catchFunction = Nan::New<v8::Function>(promiseCatch, contextBuffer);
                promise->Then(Nan::GetCurrentContext(), thenFunction)
                    .ToLocalChecked()
                    ->Catch(Nan::GetCurrentContext(), catchFunction)
                    .IsEmpty();
            }

            inline AsyncBackgroundTask<T, P>* getTask() {
                return self;
            }

            inline bool isCompleted() {
                return self->completed;
            }
        };

        typedef typename std::function<void (ExecutionContext &, const P *data, size_t size)> ProgressCallback;
        typedef std::function<void (ExecutionContext &)> FunctionCallback;
        typedef std::function<v8::Local<v8::Value> (T value)> ToV8ConverterFunction;

        ExecutionContext* executionContext = nullptr;

    protected:
        FunctionCallback function;
        ProgressCallback progress;
        ToV8ConverterFunction convertFunction;
        Nan::Maybe<T> returnValue = Nan::Nothing<T>();
        Nan::Persistent<v8::Value> exceptionValue;
        node::async_context asyncContext;

    public:
        explicit AsyncBackgroundTask(
            FunctionCallback function,
            ProgressCallback progress,
            const char* name,
            ToV8ConverterFunction convertFunction,
            Nan::Callback* callback
        ): Nan::AsyncProgressQueueWorker<P>(callback, name), function(function), progress(progress), convertFunction(convertFunction) {
            asyncContext = node::EmitAsyncInit(v8::Isolate::GetCurrent(), Nan::New<v8::Object>(), name);
        }

        ~AsyncBackgroundTask() {
            if(!this->exceptionValue.IsEmpty()) {
                this->exceptionValue.Reset();
            }

            node::EmitAsyncDestroy(v8::Isolate::GetCurrent(), asyncContext);
        }

        virtual inline v8::Local<v8::Value> getReturnValue() const {
            return Nan::Undefined();
        }

        virtual void Execute(const ExecutionProgress &progress) override {
            if(function) {
                executionContext = new ExecutionContext(this, progress);
                function(*executionContext);
                delete executionContext;
            } else {
                this->SetErrorMessage("Impl - No lambda function received in AsyncBackgroundTask");
            }
        }

        virtual void HandleProgressCallback(const P *data, size_t size) override {
            if(this->progress) {
                Nan::HandleScope scope;
                //Workaround https://github.com/nodejs/node/issues/5691 - This allows to handle promises without any warnings from node
                node::CallbackScope callbackScope(v8::Isolate::GetCurrent(), Nan::New<v8::Object>(), asyncContext);
                Nan::TryCatch tryCatch;
                this->progress(*executionContext, data, size);
                if(tryCatch.HasCaught()) {
                    executionContext->reject(tryCatch.Exception());
                }
            }
        }

        Nan::AsyncResource* getAsyncResource() {
            return this->async_resource;
        }

        const Nan::AsyncResource* getAsyncResource() const {
            return this->async_resource;
        }

    protected:
        bool completed = false;

        virtual void HandleOKCallback() override {
            Nan::HandleScope scope;

            if(returnValue.IsJust() && convertFunction) {
                v8::Local<v8::Value> argv[] = {
                    Nan::Undefined(),
                    convertFunction(returnValue.FromJust())
                };
                this->callback->Call(2, argv, this->async_resource);
            } else {
                this->callback->Call(0, NULL, this->async_resource);
            }
        }

        virtual void HandleErrorCallback() override {
            Nan::HandleScope scope;

            if(exceptionValue.IsEmpty()) {
                v8::Local<v8::Value> argv[] = {
                    v8::Exception::Error(Nan::New<v8::String>(this->ErrorMessage()).ToLocalChecked())
                };
                this->callback->Call(1, argv, this->async_resource);
            } else {
                v8::Local<v8::Value> argv[] = {
                    Nan::New(exceptionValue)
                };
                this->callback->Call(1, argv, this->async_resource);
            }
        }

    };

    template<typename T, typename P = char>
    class PromisifiedAsyncBackgroundTask: public AsyncBackgroundTask<T, P> {

        inline v8::Local<v8::Promise::Resolver> getResolver() const {
            return this->GetFromPersistent("promiseResolver").template As<v8::Promise::Resolver>();
        }

    public:
        typedef typename AsyncBackgroundTask<T, P>::ProgressCallback ProgressCallback;
        typedef typename AsyncBackgroundTask<T, P>::FunctionCallback FunctionCallback;
        typedef typename AsyncBackgroundTask<T, P>::ToV8ConverterFunction ToV8ConverterFunction;

        explicit PromisifiedAsyncBackgroundTask(
            FunctionCallback function,
            ProgressCallback progress,
            const char* name,
            ToV8ConverterFunction convertFunction = nullptr
        ): AsyncBackgroundTask<T, P>(function, progress, name, convertFunction, (Nan::Callback*) (0x1)) {
            auto resolver = v8::Promise::Resolver::New(Nan::GetCurrentContext()).ToLocalChecked();
            this->SaveToPersistent("promiseResolver", resolver);
        }

        inline v8::Local<v8::Promise> getPromise() const {
            Nan::EscapableHandleScope scope;
            return scope.Escape(this->getResolver()->GetPromise());
        }

        virtual inline v8::Local<v8::Value> getReturnValue() const override {
            return getPromise();
        }

    protected:
        virtual void HandleOKCallback() override {
            this->callback = nullptr;
            Nan::HandleScope scope;
            auto resolver = this->getResolver();
            //Workaround https://github.com/nodejs/node/issues/5691
            node::CallbackScope callbackScope(v8::Isolate::GetCurrent(), resolver, this->asyncContext);
            auto context = Nan::GetCurrentContext();

            if(this->returnValue.IsJust() && this->convertFunction) {
                resolver->Resolve(context, this->convertFunction(this->returnValue.FromJust())).FromJust();
            } else {
                resolver->Resolve(context, Nan::Null()).FromJust();
            }
        }

        virtual void HandleErrorCallback() override {
            this->callback = nullptr;
            Nan::HandleScope scope;
            auto resolver = this->getResolver();
            //Workaround https://github.com/nodejs/node/issues/5691
            node::CallbackScope callbackScope(v8::Isolate::GetCurrent(), resolver, this->asyncContext);
            auto context = Nan::GetCurrentContext();

            if(this->exceptionValue.IsEmpty()) {
                auto exception = v8::Exception::Error(Nan::New<v8::String>(this->ErrorMessage()).ToLocalChecked());
                resolver->Reject(context, exception).FromJust();
            } else {
                auto exception = Nan::New(this->exceptionValue);
                resolver->Reject(context, exception).FromJust();
            }
        }

    };


    template<typename WorkerBase, class Worker, class PromisifiedWorker, class... Args>
    static inline WorkerBase* newWorker(
        Nan::Callback* callback,
        Args... args
    ) {
        if(callback) {
            return new Worker(std::forward<Args>(args)..., callback);
        } else {
            return new PromisifiedWorker(std::forward<Args>(args)...);
        }
    }

    template<typename Worker, class PromisifiedWorker, class... Args>
    static inline Worker* newWorker(
        Nan::Callback* callback,
        Args... args
    ) {
        return newWorker<Worker, Worker, PromisifiedWorker, Args...>(callback, std::forward<Args>(args)...);
    }

    static inline Nan::Callback* newCallback(v8::Local<v8::Value> value) {
        if(value.IsEmpty() || !value->IsFunction()) {
            return nullptr;
        } else {
            return new Nan::Callback(v8::Local<v8::Function>::Cast(value));
        }
    }

    static inline Nan::Callback* newCallback(Nan::MaybeLocal<v8::Value> maybeValue) {
        if(maybeValue.IsEmpty()) {
            return nullptr;
        } else {
            return newCallback(maybeValue.ToLocalChecked());
        }
    }


    struct SyncronizableWorkRequest {
        std::shared_ptr<std::atomic_bool> workDone = std::shared_ptr<std::atomic_bool>(new std::atomic_bool(false));
        std::shared_ptr<std::mutex> mutex = std::shared_ptr<std::mutex>(new std::mutex);
        std::shared_ptr<std::condition_variable> cond = std::shared_ptr<std::condition_variable>(new std::condition_variable);

        inline SyncronizableWorkRequest() {}
        inline SyncronizableWorkRequest(const SyncronizableWorkRequest &o) {
            workDone = o.workDone;
            mutex = o.mutex;
            cond = o.cond;
        }

        inline void notifyWorkDone() const {
            std::lock_guard<std::mutex> lg(*mutex);
            workDone->store(true);
            cond->notify_one();
        }

        inline void waitForWorkDone() const {
            std::unique_lock<std::mutex> ul(*mutex);
            cond->wait(ul, [this] () { return workDone->load(); });
        }

        ~SyncronizableWorkRequest() {
            workDone.reset();
            mutex.reset();
            cond.reset();
        }
    };

}