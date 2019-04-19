#pragma once

namespace flac_bindings {

    template<class EncoderOrDecoder, class WorkRequest>
    static inline std::function<std::function<void (Local<Value>)>(const char*)>
    functionGeneratorForReturnNumber(const EncoderOrDecoder* enc, const WorkRequest* data) {
        return [enc, data] (const char* op) {
            return [enc, data, op] (Local<Value> res) {
                auto maybeReturnValue = numberFromJs<int>(res);
                if(maybeReturnValue.IsNothing()) {
                    std::string errorMessage = std::string(op) + " - Expected number or bigint as return value";
                    enc->reject->withException(Nan::TypeError(errorMessage.c_str()));
                    return;
                }

                *data->returnValue = maybeReturnValue.FromJust();
            };
        };
    }

    template<class EncoderOrDecoder, class WorkRequest>
    static inline std::function<std::function<void (Local<Value>)>(const char*, const char*, std::function<void(uint64_t)>)>
    functionGeneratorForReturnObject(const EncoderOrDecoder* dec, const WorkRequest* data) {
        return [dec, data] (const char* op, const char* attributeName, std::function<void(uint64_t)> setter) {
            return [dec, data, op, attributeName, setter] (Local<Value> res) {
                if(!res->IsObject()) {
                    std::string errorMessage = std::string(op) + " - Expected object as return value";
                    dec->reject->withException(Nan::TypeError(errorMessage.c_str()));
                    return;
                }

                auto obj = res.template As<Object>();
                auto maybeE = numberFromJs<uint64_t>(Nan::Get(obj, Nan::New(attributeName).ToLocalChecked()));
                auto maybeReturnValue = numberFromJs<int>(Nan::Get(obj, Nan::New("returnValue").ToLocalChecked()));
                if(maybeE.IsNothing()) {
                    std::string errorMessage = std::string(op) + " - " + std::string(attributeName) + " is not a number nor bigint";
                    dec->reject->withException(Nan::TypeError(errorMessage.c_str()));
                    return;
                }

                if(maybeReturnValue.IsNothing()) {
                    std::string errorMessage = std::string(op) + " - returnValue is not a number nor bigint";
                    dec->reject->withException(Nan::TypeError(errorMessage.c_str()));
                    return;
                }

                setter(maybeE.FromJust());
                *data->returnValue = maybeReturnValue.FromJust();
            };
        };
    }

}