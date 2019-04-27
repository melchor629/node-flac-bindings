#include <memory>
#include <nan.h>
#include "../utils/dl.hpp"

using namespace v8;
using namespace node;
#define ENCODER_IMPL
#include "encoder.hpp"
#include "../utils/pointer.hpp"
#include "../mappings/mappings.hpp"


#define _JOIN(a, b) a##b
#define _JOIN2(a,b,c) a##b##c

#define UNWRAP_FLAC \
    StreamEncoder* self = Nan::ObjectWrap::Unwrap<StreamEncoder>(info.Holder()); \
    FLAC__StreamEncoder* enc = self->enc;

#define FLAC_SETTER_METHOD(type, v8Type, fn, jsFn) \
NAN_METHOD(StreamEncoder:: jsFn) { \
    UNWRAP_FLAC \
    auto inputMaybe = _JOIN(v8Type, FromJs)<type>(info[0]); \
    if(inputMaybe.IsNothing()) { \
        Nan::ThrowTypeError("Expected type to be " #v8Type ); \
        return; \
    } \
    type input = (type) inputMaybe.FromJust(); \
    FLAC__bool output = _JOIN(FLAC__stream_encoder_set_, fn)(enc, input); \
    info.GetReturnValue().Set(Nan::New(bool(output))); \
}

#define FLAC_GETTER_METHOD(type, v8Type, fn, jsFn) \
NAN_METHOD(StreamEncoder:: jsFn) { \
    UNWRAP_FLAC \
    type output = _JOIN(FLAC__stream_encoder_get_, fn)(enc); \
    info.GetReturnValue().Set(_JOIN(v8Type, ToJs)(output)); \
}

#define CHECK_ASYNC_IS_NULL \
if(self->async != nullptr) { Nan::ThrowError("Multiple calls to some methods of the Encoder are not allowed"); return; }

namespace flac_bindings {

    extern Library* libFlac;

    FLAC_SETTER_METHOD(long, number, ogg_serial_number, setOggSerialNumber);
    FLAC_GETTER_METHOD(FLAC__bool, boolean, verify, getVerify);
    FLAC_SETTER_METHOD(FLAC__bool, boolean, verify, setVerify);
    FLAC_GETTER_METHOD(FLAC__bool, boolean, streamable_subset, getStreamableSubset);
    FLAC_SETTER_METHOD(FLAC__bool, boolean, streamable_subset, setStreamableSubset);
    FLAC_GETTER_METHOD(unsigned, number, channels, getChannels);
    FLAC_SETTER_METHOD(unsigned, number, channels, setChannels);
    FLAC_GETTER_METHOD(unsigned, number, bits_per_sample, getBitsPerSample);
    FLAC_SETTER_METHOD(unsigned, number, bits_per_sample, setBitsPerSample);
    FLAC_GETTER_METHOD(unsigned, number, sample_rate, getSampleRate);
    FLAC_SETTER_METHOD(unsigned, number, sample_rate, setSampleRate);
    FLAC_SETTER_METHOD(unsigned, number, compression_level, setCompressionLevel);
    FLAC_GETTER_METHOD(unsigned, number, blocksize, getBlocksize);
    FLAC_SETTER_METHOD(unsigned, number, blocksize, setBlocksize);
    FLAC_GETTER_METHOD(FLAC__bool, boolean, do_mid_side_stereo, getDoMidSideStereo);
    FLAC_SETTER_METHOD(FLAC__bool, boolean, do_mid_side_stereo, setDoMidSideStereo);
    FLAC_GETTER_METHOD(FLAC__bool, boolean, loose_mid_side_stereo, getLooseMidSideStereo);
    FLAC_SETTER_METHOD(FLAC__bool, boolean, loose_mid_side_stereo, setLooseMidSideStereo);
    FLAC_GETTER_METHOD(unsigned, number, max_lpc_order, getMaxLpcOrder);
    FLAC_SETTER_METHOD(unsigned, number, max_lpc_order, setMaxLpcOrder);
    FLAC_GETTER_METHOD(unsigned, number, qlp_coeff_precision, getQlpCoeffPrecision);
    FLAC_SETTER_METHOD(unsigned, number, qlp_coeff_precision, setQlpCoeffPrecision);
    FLAC_GETTER_METHOD(FLAC__bool, boolean, do_qlp_coeff_prec_search, getDoQlpCoeffPrecSearch);
    FLAC_SETTER_METHOD(FLAC__bool, boolean, do_qlp_coeff_prec_search, setDoQlpCoeffPrecSearch);
    FLAC_GETTER_METHOD(FLAC__bool, boolean, do_escape_coding, getDoEscapeCoding);
    FLAC_SETTER_METHOD(FLAC__bool, boolean, do_escape_coding, setDoEscapeCoding);
    FLAC_GETTER_METHOD(FLAC__bool, boolean, do_exhaustive_model_search, getDoExhaustiveModelSearch);
    FLAC_SETTER_METHOD(FLAC__bool, boolean, do_exhaustive_model_search, setDoExhaustiveModelSearch);
    FLAC_GETTER_METHOD(unsigned, number, min_residual_partition_order, getMinResidualPartitionOrder);
    FLAC_SETTER_METHOD(unsigned, number, min_residual_partition_order, setMinResidualPartitionOrder);
    FLAC_GETTER_METHOD(unsigned, number, max_residual_partition_order, getMaxResidualPartitionOrder);
    FLAC_SETTER_METHOD(unsigned, number, max_residual_partition_order, setMaxResidualPartitionOrder);
    FLAC_GETTER_METHOD(unsigned, number, rice_parameter_search_dist, getRiceParameterSearchDist);
    FLAC_SETTER_METHOD(unsigned, number, rice_parameter_search_dist, setRiceParameterSearchDist);
    FLAC_GETTER_METHOD(uint64_t, number, total_samples_estimate, getTotalSamplesEstimate);
    FLAC_SETTER_METHOD(uint64_t, number, total_samples_estimate, setTotalSamplesEstimate);
    FLAC_GETTER_METHOD(int, number, state, getState);
    FLAC_GETTER_METHOD(int, number, verify_decoder_state, getVerifyDecoderState);

    NAN_METHOD(StreamEncoder::create) {
        if(throwIfNotConstructorCall(info)) return;
        FLAC__StreamEncoder* enc = FLAC__stream_encoder_new();
        if(enc != nullptr) {
            StreamEncoder* self = new StreamEncoder;
            self->enc = enc;
            self->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
        } else {
            Nan::ThrowError("Could not allocate memory for encoder");
        }
    }

    NAN_METHOD(StreamEncoder::initStream) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        WARN_SYNC_FUNCTION("Encoder.initStream");

        if(info[0]->IsFunction()) self->writeCbk.reset(new Nan::Callback(info[0].As<Function>()));
        if(info[1]->IsFunction()) self->seekCbk.reset(new Nan::Callback(info[1].As<Function>()));
        if(info[2]->IsFunction()) self->tellCbk.reset(new Nan::Callback(info[2].As<Function>()));
        if(info[3]->IsFunction()) self->metadataCbk.reset(new Nan::Callback(info[3].As<Function>()));

        self->async = new Nan::AsyncResource("flac:encoder:initStream");
        int ret = FLAC__stream_encoder_init_stream(enc,
            !self->writeCbk ? nullptr : encoder_write_callback,
            !self->seekCbk ? nullptr : encoder_seek_callback,
            !self->tellCbk ? nullptr : encoder_tell_callback,
            !self->metadataCbk ? nullptr : encoder_metadata_callback,
            self);

        info.GetReturnValue().Set(Nan::New(ret));
        delete self->async;
        self->async = nullptr;
    }

    NAN_METHOD(StreamEncoder::initOggStream) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        WARN_SYNC_FUNCTION("Encoder.initOggStream");

        if(info[0]->IsFunction()) self->readCbk.reset(new Nan::Callback(info[0].As<Function>()));
        if(info[1]->IsFunction()) self->writeCbk.reset(new Nan::Callback(info[1].As<Function>()));
        if(info[2]->IsFunction()) self->seekCbk.reset(new Nan::Callback(info[2].As<Function>()));
        if(info[3]->IsFunction()) self->tellCbk.reset(new Nan::Callback(info[3].As<Function>()));
        if(info[4]->IsFunction()) self->metadataCbk.reset(new Nan::Callback(info[4].As<Function>()));

        self->async = new Nan::AsyncResource("flac:encoder:initOggStream");
        int ret = FLAC__stream_encoder_init_ogg_stream(enc,
            !self->readCbk ? nullptr : encoder_read_callback,
            !self->writeCbk ? nullptr : encoder_write_callback,
            !self->seekCbk ? nullptr : encoder_seek_callback,
            !self->tellCbk ? nullptr : encoder_tell_callback,
            !self->metadataCbk ? nullptr : encoder_metadata_callback,
            self);

        info.GetReturnValue().Set(Nan::New(ret));
        delete self->async;
        self->async = nullptr;
    }

    NAN_METHOD(StreamEncoder::initFile) {
        UNWRAP_FLAC
        WARN_SYNC_FUNCTION("Encoder.initFile");
        if(!info[0]->IsString()) {
            Nan::ThrowError("Expected first argument to be a string");
            return;
        }

        if(info[1]->IsFunction()) self->progressCbk.reset(new Nan::Callback(info[1].As<Function>()));
        Nan::Utf8String str(info[0]);

        int ret = FLAC__stream_encoder_init_file(enc, *str, info[1]->IsFunction() ? encoder_progress_callback : nullptr, self);
        info.GetReturnValue().Set(Nan::New(ret));
    }

    NAN_METHOD(StreamEncoder::initOggFile) {
        UNWRAP_FLAC
        WARN_SYNC_FUNCTION("Encoder.initOggFile");
        if(!info[0]->IsString()) {
            Nan::ThrowError("Expected first argument to be a string");
            return;
        }

        if(info[1]->IsFunction()) self->progressCbk.reset(new Nan::Callback(info[1].As<Function>()));
        Nan::Utf8String str(info[0]);

        int ret = FLAC__stream_encoder_init_ogg_file(enc, *str, info[1]->IsFunction() ? encoder_progress_callback : nullptr, self);
        info.GetReturnValue().Set(Nan::New(ret));
    }

    NAN_METHOD(StreamEncoder::finish) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        WARN_SYNC_FUNCTION("Encoder.finish");
        self->async = new Nan::AsyncResource("flac:encoder:finish");
        FLAC__bool ret = FLAC__stream_encoder_finish(enc);
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        delete self->async;
        self->async = nullptr;
    }

    NAN_METHOD(StreamEncoder::process) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        WARN_SYNC_FUNCTION("Encoder.process");
        assertThrowing(FLAC__stream_encoder_get_state(self->enc) == 0, "The encoder must be in OK state");
        if(info[0].IsEmpty() || !info[0]->IsArray()) {
            Nan::ThrowTypeError("Expected first argument to be an Array");
            return;
        }

        Local<Array> buffers = info[0].As<Array>();
        auto channels = FLAC__stream_encoder_get_channels(self->enc);
        if(buffers->Length() < channels) {
            std::string err = "The array must have " + std::to_string(channels) + " buffers";
            Nan::ThrowError(err.c_str());
            return;
        }

        auto samples = numberFromJs<uint32_t>(info[1]);
        if(samples.IsNothing()) {
            Nan::ThrowTypeError("Expected second argument to be a number");
            return;
        }

        int32_t** _buffers = new int32_t*[buffers->Length()];
        for(uint32_t i = 0; i < buffers->Length(); i++) {
            Local<Value> buff = Nan::Get(buffers, i).ToLocalChecked();
            _buffers[i] = UnwrapPointer<int32_t>(buff);
            if(_buffers[i] == nullptr) {
                std::string err = "Expected element at " + std::to_string(i) + " to be a Buffer";
                Nan::ThrowTypeError(err.c_str());
                delete[] _buffers;
                return;
            }

            size_t buffLen = node::Buffer::Length(buff);
            if(buffLen < samples.FromJust() * channels * 4) {
                std::string errorMessage = "Buffer at position " + std::to_string(i) + " has not enough bytes: " +
                    "expected " + std::to_string(samples.FromJust() * channels * 4) + " bytes (" +
                    std::to_string(samples.FromJust()) + " [int32_t] samples * " + std::to_string(channels) +
                    " channels * 4 bytes per sample) but got " + std::to_string(node::Buffer::Length(buff)) + " bytes";
                Nan::ThrowError(errorMessage.c_str());
                delete[] _buffers;
                return;
            }
        }

        self->async = new Nan::AsyncResource("flac:encoder:process");
        FLAC__bool ret = FLAC__stream_encoder_process(enc, _buffers, samples.FromJust());
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        delete[] _buffers;
        delete self->async;
        self->async = nullptr;
    }

    NAN_METHOD(StreamEncoder::processInterleaved) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        WARN_SYNC_FUNCTION("Encoder.processInterleaved");
        assertThrowing(FLAC__stream_encoder_get_state(self->enc) == 0, "The encoder must be in OK state");
        const int32_t* buffer = UnwrapPointer<const int32_t>(info[0]);
        auto samples = numberFromJs<uint32_t>(info[1]);
        unsigned c = FLAC__stream_encoder_get_channels(enc);

        if(buffer == nullptr) {
            Nan::ThrowTypeError("Expected first argument to be a Buffer");
            return;
        } else if(samples.IsNothing()) {
            if(!info[1]->IsUndefined() && !info[1]->IsNull()) {
                Nan::ThrowTypeError("Expected second argument to be a number");
                return;
            } else {
                samples = Nan::Just<uint32_t>(node::Buffer::Length(info[0]) / c / 4);
            }
        }

        if(node::Buffer::Length(info[0]) < samples.FromJust() * c * 4) {
            std::string errorMessage = "Buffer has not enough bytes: expected " + std::to_string(samples.FromJust() * c * 4) +
                " bytes (" + std::to_string(samples.FromJust()) + " [int32_t] samples * " + std::to_string(c) +
                " channels * 4 bytes per sample) but got " + std::to_string(node::Buffer::Length(info[0])) + " bytes";
            Nan::ThrowError(errorMessage.c_str());
            return;
        }

        self->async = new Nan::AsyncResource("flac:encoder:processInterleaved");
        FLAC__bool ret = FLAC__stream_encoder_process_interleaved(enc, buffer, samples.FromJust());
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        delete self->async;
        self->async = nullptr;
    }

    NAN_METHOD(StreamEncoder::setMetadata) {
        UNWRAP_FLAC
        Local<Array> metadata = info[0].As<Array>();
        uint32_t num_blocks = metadata->Length();
        FLAC__StreamMetadata** metadatas = new FLAC__StreamMetadata*[num_blocks];

        for(uint32_t i = 0; i < num_blocks; i++) {
            auto maybeMetadata = Nan::Get(metadata, i);
            if(maybeMetadata.IsEmpty() || !maybeMetadata.ToLocalChecked()->IsObject()) {
                auto errorMessage = "Expected element at position " + std::to_string(i) + " to be an object";
                Nan::ThrowTypeError(errorMessage.c_str());
                return;
            }

            metadatas[i] = jsToStruct<FLAC__StreamMetadata>(maybeMetadata.ToLocalChecked().As<Object>());
            if(metadatas[i] == nullptr) {
                auto errorMessage = "Expected element at position " + std::to_string(i) + " to be a Metadata object";
                Nan::ThrowTypeError(errorMessage.c_str());
                return;
            }
        }

        FLAC__bool ret = FLAC__stream_encoder_set_metadata(enc, metadatas, num_blocks);
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        delete[] metadatas;
    }

    NAN_METHOD(StreamEncoder::setApodization) {
        UNWRAP_FLAC
        Nan::Utf8String str(info[0]);

        FLAC__bool ret = FLAC__stream_encoder_set_apodization(enc, *str);
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
    }

    NAN_METHOD(StreamEncoder::getResolvedStateString) {
        UNWRAP_FLAC
        info.GetReturnValue().Set(Nan::New(FLAC__stream_encoder_get_resolved_state_string(enc)).ToLocalChecked());
    }

    NAN_METHOD(StreamEncoder::getVerifyDecoderErrorStats) {
        UNWRAP_FLAC
        uint64_t absolute_sample;
        unsigned frame_number, channel, sample;
        int32_t expected, got;
        Local<Object> obj = Nan::New<Object>();

        FLAC__stream_encoder_get_verify_decoder_error_stats(enc, &absolute_sample, &frame_number, &channel, &sample, &expected, &got);

        Nan::Set(obj, Nan::New("absoluteSample").ToLocalChecked(), numberToJs(absolute_sample));
        Nan::Set(obj, Nan::New("frameNumber").ToLocalChecked(), numberToJs(frame_number));
        Nan::Set(obj, Nan::New("channel").ToLocalChecked(), numberToJs(channel));
        Nan::Set(obj, Nan::New("sample").ToLocalChecked(), numberToJs(sample));
        Nan::Set(obj, Nan::New("expected").ToLocalChecked(), numberToJs(expected));
        Nan::Set(obj, Nan::New("got").ToLocalChecked(), numberToJs(got));
        info.GetReturnValue().Set(obj);
    }

    NAN_METHOD(StreamEncoder::finishAsync) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        (void) enc;

        AsyncEncoderWorkBase* w = AsyncEncoderWork::forFinish(self, newCallback(info[0]));
        if(w == nullptr) return; //Exception thrown inside the work "constructor"
        info.GetReturnValue().Set(w->getReturnValue());
        w->SaveToPersistent("this", info.This());
        AsyncQueueWorker(w);
    }

    NAN_METHOD(StreamEncoder::processAsync) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        (void) enc;

        AsyncEncoderWorkBase* w = AsyncEncoderWork::forProcess(info[0], info[1], self, newCallback(info[2]));
        if(w == nullptr) return; //Exception thrown inside the work "constructor"
        info.GetReturnValue().Set(w->getReturnValue());
        w->SaveToPersistent("this", info.This());
        AsyncQueueWorker(w);
    }

    NAN_METHOD(StreamEncoder::processInterleavedAsync) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        (void) enc;

        Local<Value> buffers = info[0];
        Local<Value> samples = info[1]->IsFunction() ? static_cast<Local<Value>>(Nan::Undefined()) : info[1];
        Local<Value> callback = info[2]->IsFunction() ? info[2] : info[1];
        AsyncEncoderWorkBase* w = AsyncEncoderWork::forProcessInterleaved(buffers, samples, self, newCallback(callback));
        if(w == nullptr) return; //Exception thrown inside the work "constructor"
        info.GetReturnValue().Set(w->getReturnValue());
        w->SaveToPersistent("this", info.This());
        AsyncQueueWorker(w);
    }

    NAN_METHOD(StreamEncoder::initStreamAsync) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        (void) enc;

        if(info[0]->IsFunction()) self->writeCbk.reset(new Nan::Callback(info[0].As<Function>()));
        if(info[1]->IsFunction()) self->seekCbk.reset(new Nan::Callback(info[1].As<Function>()));
        if(info[2]->IsFunction()) self->tellCbk.reset(new Nan::Callback(info[2].As<Function>()));
        if(info[3]->IsFunction()) self->metadataCbk.reset(new Nan::Callback(info[3].As<Function>()));

        AsyncEncoderWorkBase* w = AsyncEncoderWork::forInitStream(self, newCallback(info[4]));
        if(w == nullptr) return;
        info.GetReturnValue().Set(w->getReturnValue());
        w->SaveToPersistent("this", info.This());
        AsyncQueueWorker(w);
    }

    NAN_METHOD(StreamEncoder::initOggStreamAsync) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        (void) enc;

        if(info[0]->IsFunction()) self->readCbk.reset(new Nan::Callback(info[0].As<Function>()));
        if(info[1]->IsFunction()) self->writeCbk.reset(new Nan::Callback(info[1].As<Function>()));
        if(info[2]->IsFunction()) self->seekCbk.reset(new Nan::Callback(info[2].As<Function>()));
        if(info[3]->IsFunction()) self->tellCbk.reset(new Nan::Callback(info[3].As<Function>()));
        if(info[4]->IsFunction()) self->metadataCbk.reset(new Nan::Callback(info[4].As<Function>()));

        AsyncEncoderWorkBase* w = AsyncEncoderWork::forInitOggStream(self, newCallback(info[5]));
        if(w == nullptr) return;
        info.GetReturnValue().Set(w->getReturnValue());
        w->SaveToPersistent("this", info.This());
        AsyncQueueWorker(w);
    }

    NAN_METHOD(StreamEncoder::initFileAsync) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        (void) enc;

        if(info[1]->IsFunction()) self->progressCbk.reset(new Nan::Callback(info[1].As<Function>()));

        AsyncEncoderWorkBase* w = AsyncEncoderWork::forInitFile(info[0], self, newCallback(info[1]));
        if(w == nullptr) return;
        info.GetReturnValue().Set(w->getReturnValue());
        w->SaveToPersistent("this", info.This());
        AsyncQueueWorker(w);
    }

    NAN_METHOD(StreamEncoder::initOggFileAsync) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        (void) enc;

        if(info[1]->IsFunction()) self->progressCbk.reset(new Nan::Callback(info[1].As<Function>()));

        AsyncEncoderWorkBase* w = AsyncEncoderWork::forInitOggFile(info[0], self, newCallback(info[1]));
        if(w == nullptr) return;
        info.GetReturnValue().Set(w->getReturnValue());
        w->SaveToPersistent("this", info.This());
        AsyncQueueWorker(w);
    }


    FlacEnumDefineReturnType StreamEncoder::createStateEnum() {
        Local<Object> obj1 = Nan::New<Object>();
        Local<Object> obj2 = Nan::New<Object>();
        flacEnum_defineValue(obj1, obj2, "OK", 0);
        flacEnum_defineValue(obj1, obj2, "UNINITIALIZED", 1);
        flacEnum_defineValue(obj1, obj2, "OGG_ERROR", 2);
        flacEnum_defineValue(obj1, obj2, "VERIFY_DECODER_ERROR", 3);
        flacEnum_defineValue(obj1, obj2, "VERIFY_MISMATCH_IN_AUDIO_DATA", 4);
        flacEnum_defineValue(obj1, obj2, "CLIENT_ERROR", 5);
        flacEnum_defineValue(obj1, obj2, "IO_ERROR", 6);
        flacEnum_defineValue(obj1, obj2, "FRAMING_ERROR", 7);
        flacEnum_defineValue(obj1, obj2, "MEMORY_ALLOCATION_ERROR", 8);
        return std::make_tuple(obj1, obj2);
    }

    FlacEnumDefineReturnType StreamEncoder::createInitStatusEnum() {
        Local<Object> obj1 = Nan::New<Object>();
        Local<Object> obj2 = Nan::New<Object>();
        flacEnum_defineValue(obj1, obj2, "OK", 0);
        flacEnum_defineValue(obj1, obj2, "ENCODER_ERROR", 1);
        flacEnum_defineValue(obj1, obj2, "UNSUPPORTED_CONTAINER", 2);
        flacEnum_defineValue(obj1, obj2, "INVALID_CALLBACKS", 3);
        flacEnum_defineValue(obj1, obj2, "INVALID_NUMBER_OF_CHANNELS", 4);
        flacEnum_defineValue(obj1, obj2, "INVALID_BITS_PER_SAMPLE", 5);
        flacEnum_defineValue(obj1, obj2, "INVALID_SAMPLE_RATE", 6);
        flacEnum_defineValue(obj1, obj2, "INVALID_BLOCK_SIZE", 7);
        flacEnum_defineValue(obj1, obj2, "INVALID_MAX_LPC_ORDER", 8);
        flacEnum_defineValue(obj1, obj2, "INVALID_QLP_COEFF_PRECISION", 9);
        flacEnum_defineValue(obj1, obj2, "BLOCK_SIZE_TOO_SMALL_FOR_LPC_ORDER", 10);
        flacEnum_defineValue(obj1, obj2, "NOT_STREAMABLE", 11);
        flacEnum_defineValue(obj1, obj2, "INVALID_METADATA", 12);
        flacEnum_defineValue(obj1, obj2, "ALREADY_INITIALIZED", 13);
        return std::make_tuple(obj1, obj2);
    }

    FlacEnumDefineReturnType StreamEncoder::createReadStatusEnum() {
        Local<Object> obj1 = Nan::New<Object>();
        Local<Object> obj2 = Nan::New<Object>();
        flacEnum_defineValue(obj1, obj2, "CONTINUE", 0);
        flacEnum_defineValue(obj1, obj2, "END_OF_STREAM", 1);
        flacEnum_defineValue(obj1, obj2, "ABORT", 2);
        flacEnum_defineValue(obj1, obj2, "UNSUPPORTED", 3);
        return std::make_tuple(obj1, obj2);
    }

    FlacEnumDefineReturnType StreamEncoder::createWriteStatusEnum() {
        Local<Object> obj1 = Nan::New<Object>();
        Local<Object> obj2 = Nan::New<Object>();
        flacEnum_defineValue(obj1, obj2, "CONTINUE", 0);
        flacEnum_defineValue(obj1, obj2, "FATAL_ERROR", 1);
        return std::make_tuple(obj1, obj2);
    }

    FlacEnumDefineReturnType StreamEncoder::createSeekStatusEnum() {
        Local<Object> obj1 = Nan::New<Object>();
        Local<Object> obj2 = Nan::New<Object>();
        flacEnum_defineValue(obj1, obj2, "OK", 0);
        flacEnum_defineValue(obj1, obj2, "ERROR", 1);
        flacEnum_defineValue(obj1, obj2, "UNSUPPORTED", 2);
        return std::make_tuple(obj1, obj2);
    }

    FlacEnumDefineReturnType StreamEncoder::createTellStatusEnum() {
        Local<Object> obj1 = Nan::New<Object>();
        Local<Object> obj2 = Nan::New<Object>();
        flacEnum_defineValue(obj1, obj2, "OK", 0);
        flacEnum_defineValue(obj1, obj2, "ERROR", 1);
        flacEnum_defineValue(obj1, obj2, "UNSUPPORTED", 2);
        return std::make_tuple(obj1, obj2);
    }

    StreamEncoder::StreamEncoder() {}

    StreamEncoder::~StreamEncoder() {
        FLAC__stream_encoder_delete(enc);
    }

    NAN_MODULE_INIT(StreamEncoder::initEncoder) {
        Local<FunctionTemplate> obj = Nan::New<FunctionTemplate>(create);
        obj->SetClassName(Nan::New("StreamEncoder").ToLocalChecked());
        obj->InstanceTemplate()->SetInternalFieldCount(1);

        #define loadFunction(fn) \
        _JOIN(FLAC__stream_encoder_, fn) =  libFlac->getSymbolAddress<_JOIN2(FLAC__stream_encoder_, fn, _t)>("FLAC__stream_encoder_" #fn); \
        if(_JOIN(FLAC__stream_encoder_, fn) == nullptr) printf("%s\n", libFlac->getLastError().c_str());

        #define setMethod(fn, jsFn) \
        Nan::SetPrototypeMethod(obj, #jsFn, jsFn); \
        loadFunction(fn)

        setMethod(set_ogg_serial_number, setOggSerialNumber);
        setMethod(set_verify, setVerify);
        setMethod(set_streamable_subset, setStreamableSubset);
        setMethod(set_channels, setChannels);
        setMethod(set_bits_per_sample, setBitsPerSample);
        setMethod(set_sample_rate, setSampleRate);
        setMethod(set_compression_level, setCompressionLevel);
        setMethod(set_blocksize, setBlocksize);
        setMethod(set_do_mid_side_stereo, setDoMidSideStereo);
        setMethod(set_loose_mid_side_stereo, setLooseMidSideStereo);
        setMethod(set_apodization, setApodization);
        setMethod(set_max_lpc_order, setMaxLpcOrder);
        setMethod(set_qlp_coeff_precision, setQlpCoeffPrecision);
        setMethod(set_do_qlp_coeff_prec_search, setDoQlpCoeffPrecSearch);
        setMethod(set_do_escape_coding, setDoEscapeCoding);
        setMethod(set_do_exhaustive_model_search, setDoExhaustiveModelSearch);
        setMethod(set_min_residual_partition_order, setMinResidualPartitionOrder);
        setMethod(set_max_residual_partition_order, setMaxResidualPartitionOrder);
        setMethod(set_rice_parameter_search_dist, setRiceParameterSearchDist);
        setMethod(set_total_samples_estimate, setTotalSamplesEstimate);
        setMethod(set_metadata, setMetadata);
        setMethod(get_state, getState);
        setMethod(get_verify_decoder_state, getVerifyDecoderState);
        setMethod(get_resolved_state_string, getResolvedStateString);
        setMethod(get_verify_decoder_error_stats, getVerifyDecoderErrorStats);
        setMethod(get_verify, getVerify);
        setMethod(get_streamable_subset, getStreamableSubset);
        setMethod(get_channels, getChannels);
        setMethod(get_bits_per_sample, getBitsPerSample);
        setMethod(get_sample_rate, getSampleRate);
        setMethod(get_blocksize, getBlocksize);
        setMethod(get_do_mid_side_stereo, getDoMidSideStereo);
        setMethod(get_loose_mid_side_stereo, getLooseMidSideStereo);
        setMethod(get_max_lpc_order, getMaxLpcOrder);
        setMethod(get_qlp_coeff_precision, getQlpCoeffPrecision);
        setMethod(get_do_qlp_coeff_prec_search, getDoQlpCoeffPrecSearch);
        setMethod(get_do_escape_coding, getDoEscapeCoding);
        setMethod(get_do_exhaustive_model_search, getDoExhaustiveModelSearch);
        setMethod(get_min_residual_partition_order, getMinResidualPartitionOrder);
        setMethod(get_max_residual_partition_order, getMaxResidualPartitionOrder);
        setMethod(get_rice_parameter_search_dist, getRiceParameterSearchDist);
        setMethod(get_total_samples_estimate, getTotalSamplesEstimate);
        setMethod(init_stream, initStream);
        setMethod(init_ogg_stream, initOggStream);
        setMethod(init_file, initFile);
        setMethod(init_ogg_file, initOggFile);
        setMethod(finish, finish);
        setMethod(process, process);
        setMethod(process_interleaved, processInterleaved);
        loadFunction(new);
        loadFunction(delete);

        Nan::SetPrototypeMethod(obj, "finishAsync", finishAsync);
        Nan::SetPrototypeMethod(obj, "processAsync", processAsync);
        Nan::SetPrototypeMethod(obj, "processInterleavedAsync", processInterleavedAsync);
        Nan::SetPrototypeMethod(obj, "initStreamAsync", initStreamAsync);
        Nan::SetPrototypeMethod(obj, "initOggStreamAsync", initOggStreamAsync);
        Nan::SetPrototypeMethod(obj, "initFileAsync", initFileAsync);
        Nan::SetPrototypeMethod(obj, "initOggFileAsync", initOggFileAsync);

        Local<Function> functionClass = Nan::GetFunction(obj).ToLocalChecked();

        flacEnum_declareInObject(functionClass, State, createStateEnum());
        flacEnum_declareInObject(functionClass, InitStatus, createInitStatusEnum());
        flacEnum_declareInObject(functionClass, ReadStatus, createReadStatusEnum());
        flacEnum_declareInObject(functionClass, WriteStatus, createWriteStatusEnum());
        flacEnum_declareInObject(functionClass, SeekStatus, createSeekStatusEnum());
        flacEnum_declareInObject(functionClass, TellStatus, createTellStatusEnum());

        Nan::Set(target, Nan::New("Encoder").ToLocalChecked(), functionClass);
    }

};


static int doAsyncWork(flac_bindings::StreamEncoder* enc, flac_bindings::EncoderWorkRequest &ewr, int errorReturnValue) {
    int returnValue = errorReturnValue;
    ewr.returnValue = &returnValue;

    enc->asyncExecutionContext->sendProgress(ewr);
    ewr.waitForWorkDone();

    return returnValue;
}

int encoder_read_callback(const FLAC__StreamEncoder* enc, char buffer[], size_t* bytes, void* data) {
    flac_bindings::StreamEncoder* cbks = (flac_bindings::StreamEncoder*) data;
    if(cbks->asyncExecutionContext) {
        using namespace flac_bindings;
        EncoderWorkRequest ewr(EncoderWorkRequest::Type::Read);
        ewr.buffer = buffer;
        ewr.bytes = bytes;
        return doAsyncWork(cbks, ewr, 2);
    }

    Nan::HandleScope scope;
    Handle<Value> args[] = {
        WrapPointer(buffer, *bytes).ToLocalChecked()
    };

    Nan::TryCatch tc;
    tc.SetVerbose(true);
    auto ret = (*cbks->readCbk)(cbks->async, 1, args);
    if(tc.HasCaught()) {
        tc.ReThrow();
        return 2;
    }
    if(ret.IsEmpty() || !ret.ToLocalChecked()->IsObject()) {
        Nan::ThrowError("Read callback did not return an object");
        *bytes = 0;
        return 2;
    } else {
        Local<Object> retJust = ret.ToLocalChecked().As<Object>();
        Local<Value> bytes2 = Nan::Get(retJust, Nan::New("bytes").ToLocalChecked()).ToLocalChecked();
        Local<Value> returnValue = Nan::Get(retJust, Nan::New("returnValue").ToLocalChecked()).ToLocalChecked();
        auto maybeBytes2 = numberFromJs<uint64_t>(bytes2);
        if(maybeBytes2.IsNothing()) {
            Nan::ThrowTypeError("Expected bytes to be number or bigint");
            return 1;
        }

        *bytes = (size_t) maybeBytes2.FromJust();
        return numberFromJs<int32_t>(returnValue).FromMaybe(0);
    }
}

int encoder_write_callback(const FLAC__StreamEncoder* enc, const char buffer[], size_t bytes, unsigned samples, unsigned frame, void* data) {
    flac_bindings::StreamEncoder* cbks = (flac_bindings::StreamEncoder*) data;
    if(cbks->asyncExecutionContext) {
        using namespace flac_bindings;
        EncoderWorkRequest ewr(EncoderWorkRequest::Type::Write);
        ewr.constBuffer = buffer;
        ewr.bytes = &bytes;
        ewr.samples = samples;
        ewr.frame = frame;
        return doAsyncWork(cbks, ewr, 1);
    }

    Nan::HandleScope scope;
    Handle<Value> args[] = {
        WrapPointer(buffer, bytes).ToLocalChecked(),
        numberToJs(samples),
        numberToJs(frame)
    };

    Nan::TryCatch tc;
    tc.SetVerbose(true);
    Local<Value> ret = (*cbks->writeCbk)(cbks->async, 3, args).ToLocalChecked();
    if(tc.HasCaught()) {
        tc.ReThrow();
        return 2;
    }
    int32_t _b = numberFromJs<int32_t>(ret).FromMaybe(0);
    return _b;
}

int encoder_seek_callback(const FLAC__StreamEncoder* enc, uint64_t offset, void* data) {
    flac_bindings::StreamEncoder* cbks = (flac_bindings::StreamEncoder*) data;
    if(cbks->asyncExecutionContext) {
        using namespace flac_bindings;
        EncoderWorkRequest ewr(EncoderWorkRequest::Type::Seek);
        ewr.offset = &offset;
        return doAsyncWork(cbks, ewr, 1);
    }

    Nan::HandleScope scope;
    Handle<Value> args[] = {
        numberToJs(offset)
    };

    Nan::TryCatch tc;tc.SetVerbose(true);
    Local<Value> ret = (*cbks->seekCbk)(cbks->async, 1, args).ToLocalChecked();
    if(tc.HasCaught()) {
        tc.ReThrow();
        return 1;
    }
    return numberFromJs<int>(ret).FromMaybe(0);
}

int encoder_tell_callback(const FLAC__StreamEncoder* enc, uint64_t* offset, void* data) {
    flac_bindings::StreamEncoder* cbks = (flac_bindings::StreamEncoder*) data;
    if(cbks->asyncExecutionContext) {
        using namespace flac_bindings;
        EncoderWorkRequest ewr(EncoderWorkRequest::Type::Tell);
        ewr.offset = offset;
        return doAsyncWork(cbks, ewr, 1);
    }

    Nan::HandleScope scope;
    Nan::TryCatch tc;
    tc.SetVerbose(true);
    auto ret = (*cbks->tellCbk)(cbks->async, 0, nullptr);
    if(tc.HasCaught()) {
        tc.ReThrow();
        return 1;
    }
    if(ret.IsEmpty() || !ret.ToLocalChecked()->IsObject()) {
        Nan::ThrowError("Tell callback did not return an object");
        *offset = 0;
        return 1;
    } else {
        Local<Object> retJust = ret.ToLocalChecked().As<Object>();
        Local<Value> offset2 = Nan::Get(retJust, Nan::New("offset").ToLocalChecked()).ToLocalChecked();
        Local<Value> returnValue = Nan::Get(retJust, Nan::New("returnValue").ToLocalChecked()).ToLocalChecked();
        auto maybeOffset2 = numberFromJs<uint64_t>(offset2);
        if(maybeOffset2.IsNothing()) {
            Nan::ThrowTypeError("Expected offset to be number or bigint");
            return 1;
        }

        *offset = maybeOffset2.FromJust();
        return numberFromJs<int32_t>(returnValue).FromMaybe(0);
    }
}

void encoder_metadata_callback(const FLAC__StreamEncoder* enc, const FLAC__StreamMetadata* metadata, void* data) {
    flac_bindings::StreamEncoder* cbks = (flac_bindings::StreamEncoder*) data;
    if(cbks->asyncExecutionContext) {
        using namespace flac_bindings;
        EncoderWorkRequest ewr(EncoderWorkRequest::Type::Metadata);
        ewr.metadata = metadata;
        doAsyncWork(cbks, ewr, 0);
        return;
    }

    Nan::HandleScope scope;
    Handle<Value> args[] = {
        flac_bindings::structToJs(metadata)
    };

    Nan::TryCatch tc;
    tc.SetVerbose(true);
    (*cbks->metadataCbk)(cbks->async, 1, args);
    if(tc.HasCaught()) {
        tc.ReThrow();
    }
}

void encoder_progress_callback(const FLAC__StreamEncoder* enc, uint64_t bytes_written, uint64_t samples_written, unsigned frames_written, unsigned total_frames_estimate, void* data) {
    flac_bindings::StreamEncoder* cbks = (flac_bindings::StreamEncoder*) data;
    if(cbks->asyncExecutionContext) {
        using namespace flac_bindings;
        EncoderWorkRequest ewr(EncoderWorkRequest::Type::Progress);
        ewr.progress.bytesWritten = bytes_written;
        ewr.progress.samplesWritten = samples_written;
        ewr.progress.framesWritten = frames_written;
        ewr.progress.totalFramesEstimate = total_frames_estimate;
        doAsyncWork(cbks, ewr, 0);
        return;
    }

    Nan::HandleScope scope;
    Handle<Value> args[] = {
        numberToJs(bytes_written),
        numberToJs(samples_written),
        numberToJs(frames_written),
        numberToJs(total_frames_estimate)
    };

    Nan::TryCatch tc;
    tc.SetVerbose(true);
    (*cbks->progressCbk)(cbks->async, 4, args);
    if(tc.HasCaught()) {
        tc.ReThrow();
    }
}
