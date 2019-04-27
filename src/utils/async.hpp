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
        struct RejectCallbacks {
            std::function<void (const std::string &message)> withMessage;
            std::function<void (v8::Local<v8::Value> exception)> withException;
        };

        typedef typename Nan::AsyncProgressQueueWorker<P>::ExecutionProgress ExecutionProgress;
        typedef typename std::function<void (const AsyncBackgroundTask<T, P>*, const P *data, size_t size)> ProgressCallback;
        typedef std::function<void (T returnValue)> ResolveCallback;
        typedef std::function<void (ResolveCallback, const RejectCallbacks &, const ExecutionProgress &)> FunctionCallback;
        typedef std::function<v8::Local<v8::Value> (T value)> ToV8ConverterFunction;

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
            Nan::Callback* callback,
            const char* name,
            ToV8ConverterFunction convertFunction = nullptr
        ): Nan::AsyncProgressQueueWorker<P>(callback, name), function(function), progress(progress), convertFunction(convertFunction) {}

        ~AsyncBackgroundTask() {
            if(!this->exceptionValue.IsEmpty()) {
                this->exceptionValue.Reset();
            }
        }

        virtual void Execute(const ExecutionProgress &progress) override {
            if(function) {
                RejectCallbacks rejectCbks = {
                    [this] (const auto &message) {
                        if(!this->resolved) {
                            this->SetErrorMessage(message.c_str());
                            this->resolved = true;
                        }
                    },
                    [this] (auto object) {
                        assert(v8::Isolate::GetCurrent() != nullptr);
                        if(!this->resolved) {
                            this->exceptionValue.Reset(object);
                            this->SetErrorMessage("<mock>");
                            this->resolved = true;
                        }
                    }
                };
                auto resolveCbk = [this] (T returnValue) {
                    if(!this->resolved) {
                        this->returnValue = Nan::Just(returnValue);
                        this->resolved = true;
                    }
                };

                function(resolveCbk, rejectCbks, progress);
            } else {
                this->SetErrorMessage("Impl - No lambda function received in AsyncBackgroundTask");
            }
        }

        virtual void HandleProgressCallback(const P *data, size_t size) override {
            Nan::HandleScope scope;
            if(this->progress) {
                this->progress(this, data, size);
            }
        }

        const Nan::AsyncResource* getAsyncResource() const {
            return this->async_resource;
        }

    protected:
        bool resolved = false;

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
        typedef typename AsyncBackgroundTask<T, P>::RejectCallbacks RejectCallbacks;
        typedef typename AsyncBackgroundTask<T, P>::ExecutionProgress ExecutionProgress;
        typedef typename AsyncBackgroundTask<T, P>::ProgressCallback ProgressCallback;
        typedef typename AsyncBackgroundTask<T, P>::ResolveCallback ResolveCallback;
        typedef typename AsyncBackgroundTask<T, P>::FunctionCallback FunctionCallback;
        typedef typename AsyncBackgroundTask<T, P>::ToV8ConverterFunction ToV8ConverterFunction;

        explicit PromisifiedAsyncBackgroundTask(
            FunctionCallback function,
            ProgressCallback progress,
            const char* name,
            ToV8ConverterFunction convertFunction = nullptr
        ): AsyncBackgroundTask<T, P>(function, progress, (Nan::Callback*) (0x1), name, convertFunction) {
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