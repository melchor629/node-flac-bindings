#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <nan.h>

namespace flac_bindings {

    template<typename T, typename P>
    class AsyncBackgroundTask: public Nan::AsyncProgressQueueWorker<P> {
    public:
        typedef typename Nan::AsyncProgressQueueWorker<P>::ExecutionProgress ExecutionProgress;

        class ExecutionContext {
            AsyncBackgroundTask<T, P>* self;
            const ExecutionProgress &progress;

            friend class AsyncBackgroundTask<T, P>;

            ExecutionContext(
                AsyncBackgroundTask<T, P>* s,
                const ExecutionProgress &progress
            ): self(s), progress(progress) {}

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

    public:
        explicit AsyncBackgroundTask(
            FunctionCallback function,
            ProgressCallback progress,
            const char* name,
            ToV8ConverterFunction convertFunction,
            Nan::Callback* callback
        ): Nan::AsyncProgressQueueWorker<P>(callback, name), function(function), progress(progress), convertFunction(convertFunction) {}

        ~AsyncBackgroundTask() {
            if(!this->exceptionValue.IsEmpty()) {
                this->exceptionValue.Reset();
            }
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

    template<typename T, typename P>
    class PromisifiedAsyncBackgroundTask: public AsyncBackgroundTask<T, P> {

        node::async_context asyncContext;

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
            asyncContext = node::EmitAsyncInit(v8::Isolate::GetCurrent(), resolver, name);
        }

        ~PromisifiedAsyncBackgroundTask() {
            node::EmitAsyncDestroy(v8::Isolate::GetCurrent(), asyncContext);
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
            node::CallbackScope callbackScope(v8::Isolate::GetCurrent(), resolver, asyncContext);
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
            node::CallbackScope callbackScope(v8::Isolate::GetCurrent(), resolver, asyncContext);
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