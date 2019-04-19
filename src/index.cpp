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
        AsyncBackgroundTask<bool, char>::FunctionCallback asyncFunction = [] (auto resolve, const auto &reject, const auto &progress) {
            for(char c = '0'; c <= '9'; c++) {
                progress.Send(&c, 1);
                std::this_thread::sleep_for(100ms);
            }
            std::this_thread::sleep_for(1s);
            reject.withMessage("e");
        };

        AsyncBackgroundTask<bool, char>::ProgressCallback asyncFUNction = [] (const auto *self, const auto e, auto s) {
            auto func = self->GetFromPersistent("cbk").template As<v8::Function>();
            Local<Value> args[] = { Nan::New(e, 1).ToLocalChecked() };
            Nan::Call(func, func, 1, args);
        };

        const auto worker = new PromisifiedAsyncBackgroundTask<bool, char>(
            asyncFunction,
            asyncFUNction,
            "flac:testAsync",
            [] (bool v) { return Nan::New<Boolean>(v); }
        );
        worker->SaveToPersistent("cbk", info[0]);
        AsyncQueueWorker(worker);
        info.GetReturnValue().Set(worker->getPromise());
    }

    NAN_MODULE_INIT(init) {
        module.Reset(target);
        Nan::Set(target, Nan::New("testAsync").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(testAsync)).ToLocalChecked());
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
