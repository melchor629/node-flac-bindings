#pragma once

namespace flac_bindings {

    template<class EncoderOrDecoder>
    using enable_if_encoder_or_decoder_t = std::enable_if_t<
        std::is_same<EncoderOrDecoder, class StreamEncoder>::value || std::is_same<EncoderOrDecoder, class StreamDecoder>::value,
        bool
    >;

    template<
        class EncoderOrDecoder,
        class WorkRequest,
        enable_if_encoder_or_decoder_t<EncoderOrDecoder> = 0
    >
    static inline std::function<std::function<void (Local<Value>)>(const char*)>
    functionGeneratorForReturnNumber(const EncoderOrDecoder* enc, const WorkRequest* data) {
        return [enc, data] (const char* op) {
            return [enc, data, op] (Local<Value> res) {
                auto maybeReturnValue = numberFromJs<int>(res);
                if(maybeReturnValue.IsNothing()) {
                    std::string errorMessage = std::string(op) + " - Expected number or bigint as return value";
                    enc->asyncExecutionContext->reject(Nan::TypeError(errorMessage.c_str()));
                    return;
                }

                *data->returnValue = maybeReturnValue.FromJust();
            };
        };
    }

    template<
        class EncoderOrDecoder,
        class WorkRequest,
        enable_if_encoder_or_decoder_t<EncoderOrDecoder> = 0
    >
    static inline std::function<std::function<void (Local<Value>)>(const char*, const char*, std::function<void(uint64_t)>)>
    functionGeneratorForReturnObject(const EncoderOrDecoder* dec, const WorkRequest* data) {
        return [dec, data] (const char* op, const char* attributeName, std::function<void(uint64_t)> setter) {
            return [dec, data, op, attributeName, setter] (Local<Value> res) {
                if(!res->IsObject()) {
                    std::string errorMessage = std::string(op) + " - Expected object as return value";
                    dec->asyncExecutionContext->reject(Nan::TypeError(errorMessage.c_str()));
                    return;
                }

                auto obj = res.template As<Object>();
                auto maybeE = numberFromJs<uint64_t>(Nan::Get(obj, Nan::New(attributeName).ToLocalChecked()));
                auto maybeReturnValue = numberFromJs<int>(Nan::Get(obj, Nan::New("returnValue").ToLocalChecked()));
                if(maybeE.IsNothing()) {
                    std::string errorMessage = std::string(op) + " - " + std::string(attributeName) + " is not a number nor bigint";
                    dec->asyncExecutionContext->reject(Nan::TypeError(errorMessage.c_str()));
                    return;
                }

                if(maybeReturnValue.IsNothing()) {
                    std::string errorMessage = std::string(op) + " - returnValue is not a number nor bigint";
                    dec->asyncExecutionContext->reject(Nan::TypeError(errorMessage.c_str()));
                    return;
                }

                setter(maybeE.FromJust());
                *data->returnValue = maybeReturnValue.FromJust();
            };
        };
    }

    template<
        class EncoderOrDecoder,
        class AsyncWorkBaseExecutionContext,
        enable_if_encoder_or_decoder_t<EncoderOrDecoder> = 0
    >
    static std::function<void (
        AsyncWorkBaseExecutionContext &c
    )>
    decorate(EncoderOrDecoder* ed, std::function<bool(AsyncWorkBaseExecutionContext &)> func, std::function<const char*()> getError) {
        return [ed, func, getError] (auto &c) {
            if(ed->async != nullptr) {
                c.reject("There's already an operation on this object");
                return;
            }

            ed->async = c.getTask()->getAsyncResource();
            ed->asyncExecutionContext = &c;
            bool ok = func(c);
            if(!c.isCompleted()) {
                if(!ok) c.reject(getError());
                else c.resolve(ok);
            }
            ed->async = nullptr;
            ed->asyncExecutionContext = nullptr;
        };
    }

    template<typename DecBase, class Decoder, typename std::enable_if_t<std::is_same<Decoder, class StreamDecoder>::value, int> = 0>
    static inline DecBase* newWorker(
        std::function<bool(typename DecBase::ExecutionContext &)> function,
        const char* name,
        Decoder* dec,
        Nan::Callback* callback
    ) {
        return newWorker<DecBase, class AsyncDecoderWork, class PromisifiedAsyncDecoderWork>(callback, function, name, dec);
    }

    template<typename EncBase, class Encoder, typename std::enable_if_t<std::is_same<Encoder, class StreamEncoder>::value, int> = 0>
    static inline EncBase* newWorker(
        std::function<bool(typename EncBase::ExecutionContext &)> function,
        const char* name,
        Encoder* dec,
        Nan::Callback* callback
    ) {
        return newWorker<EncBase, class AsyncEncoderWork, class PromisifiedAsyncEncoderWork>(callback, function, name, dec);
    }

}