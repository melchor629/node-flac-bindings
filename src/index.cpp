#include <nan.h>
#include "utils/dl.hpp"
#include "utils/async.hpp"
#include <thread>

using namespace v8;
using namespace node;

namespace flac_bindings {
    class StreamEncoder { public: static NAN_MODULE_INIT(initEncoder); };
    class StreamDecoder { public: static NAN_MODULE_INIT(initDecoder); };
    NAN_MODULE_INIT(initMetadata0);
    class SimpleIterator { public: static NAN_MODULE_INIT(initMetadata1); };
    class Chain { public: static NAN_MODULE_INIT(initMetadata2); };
    class Iterator { public: static NAN_MODULE_INIT(initMetadata2); };
    NAN_MODULE_INIT(initMetadataObjectMethods);
    NAN_MODULE_INIT(initFormat);

    bool isLibFlacLoaded = false;
    Library* libFlac;
    Nan::Persistent<Object> module;

    static void atExit(void*) {
        if(isLibFlacLoaded) {
            delete libFlac;
        }
    }

    NAN_METHOD(loadLibFlac) {
        if(info[0]->IsUndefined()) {
            Nan::ThrowError("Needs the path to the flac library");
        } else if(!info[0]->IsString()) {
            Nan::ThrowError("String needed representing the path to flac library");
        } else {
            Nan::Utf8String Path(info[0]);
            libFlac = Library::load(*Path);
            if(libFlac == nullptr) {
                Nan::ThrowError("Could not load library: check path");
            } else {
                isLibFlacLoaded = true;
                Handle<Object> obj = Nan::New(module);
                StreamEncoder::initEncoder(obj);
                StreamDecoder::initDecoder(obj);
                initFormat(obj);
                initMetadata0(obj);
                SimpleIterator::initMetadata1(obj);
                Chain::initMetadata2(obj);
                Iterator::initMetadata2(obj);
                initMetadataObjectMethods(obj);
                Nan::Delete(obj, Nan::New("load").ToLocalChecked());
                info.GetReturnValue().Set(obj);
                AtExit(atExit);
            }
        }
    }

    NAN_METHOD(testAsync) {
        using namespace std::chrono_literals;
        Nan::Utf8String endModeJsStr(info[0]);
        std::string endMode(*endModeJsStr);
        AsyncBackgroundTask<bool, char>::FunctionCallback asyncFunction = [endMode] (auto &c) {
            for(char ch = '0'; ch <= '9'; ch++) {
                c.sendProgress(ch);
                if(c.isCompleted()) return;
                std::this_thread::sleep_for(5ms);
            }

            std::this_thread::sleep_for(10ms);

            if(endMode == "reject") {
                c.reject("Rejected :(");
            } else if(endMode == "resolve") {
                c.resolve(true);
            }
        };

        AsyncBackgroundTask<bool, char>::ProgressCallback asyncFUNction = [endMode] (auto &self, const auto e, auto s) {
            Nan::HandleScope scope;
            auto func = self.getTask()->GetFromPersistent("cbk").template As<v8::Function>();
            Local<Value> args[] = { Nan::New(e, 1).ToLocalChecked() };
            auto maybeResult = Nan::Call(func, func, 1, args);
            if(!maybeResult.IsEmpty()) {
                auto res = maybeResult.ToLocalChecked();
                if(res->IsPromise()) {
                    self.defer(res.template As<Promise>(), e, [endMode=endMode] (auto &c, auto e, auto &info) {
                        if(*e == '9' && endMode == "exception") {
                            Nan::ThrowError(Nan::Error("Thrown :("));
                        }
                    });
                    return;
                }
            }
            if(*e == '9' && endMode == "exception") {
                Nan::ThrowError(Nan::Error("Thrown :("));
            }
        };

        AsyncBackgroundTask<bool, char>* worker = new AsyncBackgroundTask<bool, char>(
            asyncFunction,
            asyncFUNction,
            "flac:testAsync",
            [] (bool v) { return Nan::New<Boolean>(v); }
        );
        info.GetReturnValue().Set(worker->getReturnValue());
        worker->SaveToPersistent("cbk", info[1]);
        AsyncQueueWorker(worker);
    }

    NAN_MODULE_INIT(init) {
        module.Reset(target);

        Nan::Set(
            target,
            Nan::New("testAsync").ToLocalChecked(),
            Nan::GetFunction(Nan::New<FunctionTemplate>(testAsync)).ToLocalChecked()
        );

        libFlac = Library::load("libFLAC", "so.8");
        if(libFlac == nullptr) {
#ifdef __linux__
            //Workaround for some Linux distros
            libFlac = Library::load("libFLAC", "so.8");
            if(libFlac == nullptr) {
#endif
#ifdef WIN32
            libFlac = Library::load("FLAC", "dll");
            if(libFlac == nullptr) {
#endif
            isLibFlacLoaded = false;
            Nan::Set(target, Nan::New("load").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(loadLibFlac)).ToLocalChecked());
            return;
#if defined(__linux__) || defined(WIN32)
            }
#endif
        }

        isLibFlacLoaded = true;

        StreamEncoder::initEncoder(target);
        StreamDecoder::initDecoder(target);
        initMetadata0(target);
        SimpleIterator::initMetadata1(target);
        Chain::initMetadata2(target);
        Iterator::initMetadata2(target);
        initMetadataObjectMethods(target);
        initFormat(target);

        AtExit(atExit);
    }

};

NODE_MODULE(flac_bindings, flac_bindings::init)
