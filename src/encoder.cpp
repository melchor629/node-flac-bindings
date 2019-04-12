#include <memory>
#include <nan.h>
#include "dl.hpp"

using namespace v8;
using namespace node;
#include "pointer.hpp"
#include "format.h"
#include "defs.hpp"
#include "mappings/mappings.hpp"

#define _JOIN(a, b) a##b
#define _JOIN2(a,b,c) a##b##c

typedef int (*FLAC__StreamEncoderReadCallback)(const FLAC__StreamEncoder *encoder, char buffer[], size_t *bytes, void *client_data);
typedef int (*FLAC__StreamEncoderWriteCallback)(const FLAC__StreamEncoder *encoder, const char buffer[], size_t bytes, unsigned samples, unsigned current_frame, void *client_data);
typedef int (*FLAC__StreamEncoderSeekCallback)(const FLAC__StreamEncoder *encoder, uint64_t absolute_byte_offset, void *client_data);
typedef int (*FLAC__StreamEncoderTellCallback)(const FLAC__StreamEncoder *encoder, uint64_t *absolute_byte_offset, void *client_data);
typedef void(*FLAC__StreamEncoderMetadataCallback)(const FLAC__StreamEncoder *encoder, const FLAC__StreamMetadata *metadata, void *client_data);
typedef void(*FLAC__StreamEncoderProgressCallback)(const FLAC__StreamEncoder *encoder, uint64_t bytes_written, uint64_t samples_written, unsigned frames_written, unsigned total_frames_estimate, void *client_data);

static int read_callback(const FLAC__StreamEncoder*, char[], size_t*, void*);
static int write_callback(const FLAC__StreamEncoder*, const char[], size_t, unsigned, unsigned, void*);
static int seek_callback(const FLAC__StreamEncoder*, uint64_t, void*);
static int tell_callback(const FLAC__StreamEncoder*, uint64_t*, void*);
static void metadata_callback(const FLAC__StreamEncoder*, const FLAC__StreamMetadata*, void*);
static void progress_callback(const FLAC__StreamEncoder*, uint64_t, uint64_t, unsigned, unsigned, void*);

#define UNWRAP_FLAC \
    StreamEncoder* self = Nan::ObjectWrap::Unwrap<StreamEncoder>(info.Holder()); \
    FLAC__StreamEncoder* enc = self->enc;

#define FLAC_FUNC(returnType, fn, ...) \
    typedef returnType (*_JOIN2(FLAC__stream_encoder_, fn, _t))(__VA_ARGS__); \
    static _JOIN2(FLAC__stream_encoder_, fn, _t) _JOIN(FLAC__stream_encoder_, fn);

#define FLAC_GETTER(type, fn) FLAC_FUNC(type, _JOIN(get_, fn), const FLAC__StreamEncoder*);

#define FLAC_SETTER(type, fn) FLAC_FUNC(FLAC__bool, _JOIN(set_, fn), FLAC__StreamEncoder*, type)

#define FLAC_GETTER_SETTER(type, fn) \
FLAC_GETTER(type, fn); \
FLAC_SETTER(type, fn);

#define FLAC_SETTER_METHOD(type, v8Type, fn) \
NAN_METHOD(_JOIN(node_FLAC__stream_encoder_set_, fn)) { \
    UNWRAP_FLAC \
    Nan::Maybe<type> inputMaybe = Nan::To<type>(info[0]); \
    if(inputMaybe.IsNothing() || !info[0]->Is##v8Type ()) { \
        Nan::ThrowTypeError("Expected type to be " #v8Type ); \
        return; \
    } \
    type input = (type) inputMaybe.FromJust(); \
    FLAC__bool output = _JOIN(FLAC__stream_encoder_set_, fn)(enc, input); \
    info.GetReturnValue().Set(Nan::New(bool(output))); \
}

#define FLAC_GETTER_METHOD(type, v8Type, fn) \
NAN_METHOD(_JOIN(node_FLAC__stream_encoder_get_, fn)) { \
    UNWRAP_FLAC \
    type output = _JOIN(FLAC__stream_encoder_get_, fn)(enc); \
    info.GetReturnValue().Set(Nan::New<v8Type>(output)); \
}

#define CHECK_ASYNC_IS_NULL \
if(self->async != nullptr) { Nan::ThrowError("Multiple calls to some methods of the Encoder are not allowed"); return; }

extern "C" {
    FLAC_FUNC(FLAC__StreamEncoder*, new, void);
    FLAC_FUNC(void, delete, FLAC__StreamEncoder*);
    FLAC_FUNC(int, init_stream, FLAC__StreamEncoder*, FLAC__StreamEncoderWriteCallback, FLAC__StreamEncoderSeekCallback, FLAC__StreamEncoderTellCallback, FLAC__StreamEncoderMetadataCallback, void*);
    FLAC_FUNC(int, init_ogg_stream, FLAC__StreamEncoder*, FLAC__StreamEncoderReadCallback, FLAC__StreamEncoderWriteCallback, FLAC__StreamEncoderSeekCallback, FLAC__StreamEncoderTellCallback, FLAC__StreamEncoderMetadataCallback, void*);
    FLAC_FUNC(int, init_file, FLAC__StreamEncoder*, const char*, FLAC__StreamEncoderProgressCallback, void*);
    FLAC_FUNC(int, init_ogg_file, FLAC__StreamEncoder*, const char*, FLAC__StreamEncoderProgressCallback, void*);
    FLAC_FUNC(FLAC__bool, finish, FLAC__StreamEncoder*);
    FLAC_FUNC(FLAC__bool, process, FLAC__StreamEncoder*, const int32_t* const [], unsigned);
    FLAC_FUNC(FLAC__bool, process_interleaved, FLAC__StreamEncoder*, const int32_t [], unsigned);
    FLAC_FUNC(FLAC__bool, set_apodization, FLAC__StreamEncoder*, const char*);
    FLAC_FUNC(const char*, get_resolved_state_string, const FLAC__StreamEncoder*);
    FLAC_FUNC(void, get_verify_decoder_error_stats, const FLAC__StreamEncoder*, uint64_t*, unsigned*, unsigned*, unsigned*, int32_t*, int32_t*);
    FLAC_FUNC(FLAC__bool, set_metadata, FLAC__StreamEncoder*, FLAC__StreamMetadata**, unsigned);

    FLAC_SETTER(long, ogg_serial_number);
    FLAC_GETTER_SETTER(FLAC__bool, verify);
    FLAC_GETTER_SETTER(FLAC__bool, streamable_subset);
    FLAC_GETTER_SETTER(unsigned, channels);
    FLAC_GETTER_SETTER(unsigned, bits_per_sample);
    FLAC_GETTER_SETTER(unsigned, sample_rate);
    FLAC_SETTER(unsigned, compression_level);
    FLAC_GETTER_SETTER(unsigned, blocksize);
    FLAC_GETTER_SETTER(FLAC__bool, do_mid_side_stereo);
    FLAC_GETTER_SETTER(FLAC__bool, loose_mid_side_stereo);
    FLAC_GETTER_SETTER(unsigned, max_lpc_order);
    FLAC_GETTER_SETTER(unsigned, qlp_coeff_precision);
    FLAC_GETTER_SETTER(FLAC__bool, do_qlp_coeff_prec_search);
    FLAC_GETTER_SETTER(FLAC__bool, do_escape_coding);
    FLAC_GETTER_SETTER(FLAC__bool, do_exhaustive_model_search);
    FLAC_GETTER_SETTER(unsigned, min_residual_partition_order);
    FLAC_GETTER_SETTER(unsigned, max_residual_partition_order);
    FLAC_GETTER_SETTER(unsigned, rice_parameter_search_dist);
    FLAC_GETTER_SETTER(uint64_t, total_samples_estimate);
    FLAC_GETTER(int, state);
    FLAC_GETTER(int, verify_decoder_state);
}

namespace flac_bindings {

    extern Library* libFlac;

    class StreamEncoder: public Nan::ObjectWrap {
    public:

        StreamEncoder();
        ~StreamEncoder();

        std::shared_ptr<Nan::Callback> readCbk, writeCbk, seekCbk, tellCbk, metadataCbk, progressCbk;
        Nan::AsyncResource* async = nullptr;
        FLAC__StreamEncoder* enc = nullptr;

    private:

        static FLAC_SETTER_METHOD(long, Number, ogg_serial_number);
        static FLAC_GETTER_METHOD(FLAC__bool, Boolean, verify);
        static FLAC_SETTER_METHOD(FLAC__bool, Boolean, verify);
        static FLAC_GETTER_METHOD(FLAC__bool, Boolean, streamable_subset);
        static FLAC_SETTER_METHOD(FLAC__bool, Boolean, streamable_subset);
        static FLAC_GETTER_METHOD(unsigned, Number, channels);
        static FLAC_SETTER_METHOD(unsigned, Number, channels);
        static FLAC_GETTER_METHOD(unsigned, Number, bits_per_sample);
        static FLAC_SETTER_METHOD(unsigned, Number, bits_per_sample);
        static FLAC_GETTER_METHOD(unsigned, Number, sample_rate);
        static FLAC_SETTER_METHOD(unsigned, Number, sample_rate);
        static FLAC_SETTER_METHOD(unsigned, Number, compression_level);
        static FLAC_GETTER_METHOD(unsigned, Number, blocksize);
        static FLAC_SETTER_METHOD(unsigned, Number, blocksize);
        static FLAC_GETTER_METHOD(FLAC__bool, Boolean, do_mid_side_stereo);
        static FLAC_SETTER_METHOD(FLAC__bool, Boolean, do_mid_side_stereo);
        static FLAC_GETTER_METHOD(FLAC__bool, Boolean, loose_mid_side_stereo);
        static FLAC_SETTER_METHOD(FLAC__bool, Boolean, loose_mid_side_stereo);
        static FLAC_GETTER_METHOD(unsigned, Number, max_lpc_order);
        static FLAC_SETTER_METHOD(unsigned, Number, max_lpc_order);
        static FLAC_GETTER_METHOD(unsigned, Number, qlp_coeff_precision);
        static FLAC_SETTER_METHOD(unsigned, Number, qlp_coeff_precision);
        static FLAC_GETTER_METHOD(FLAC__bool, Boolean, do_qlp_coeff_prec_search);
        static FLAC_SETTER_METHOD(FLAC__bool, Boolean, do_qlp_coeff_prec_search);
        static FLAC_GETTER_METHOD(FLAC__bool, Boolean, do_escape_coding);
        static FLAC_SETTER_METHOD(FLAC__bool, Boolean, do_escape_coding);
        static FLAC_GETTER_METHOD(FLAC__bool, Boolean, do_exhaustive_model_search);
        static FLAC_SETTER_METHOD(FLAC__bool, Boolean, do_exhaustive_model_search);
        static FLAC_GETTER_METHOD(unsigned, Number, min_residual_partition_order);
        static FLAC_SETTER_METHOD(unsigned, Number, min_residual_partition_order);
        static FLAC_GETTER_METHOD(unsigned, Number, max_residual_partition_order);
        static FLAC_SETTER_METHOD(unsigned, Number, max_residual_partition_order);
        static FLAC_GETTER_METHOD(unsigned, Number, rice_parameter_search_dist);
        static FLAC_SETTER_METHOD(unsigned, Number, rice_parameter_search_dist);
        static FLAC_GETTER_METHOD(uint64_t, Number, total_samples_estimate);
        static FLAC_SETTER_METHOD(uint64_t, Number, total_samples_estimate);
        static FLAC_GETTER_METHOD(int, Number, state);
        static FLAC_GETTER_METHOD(int, Number, verify_decoder_state);

        static NAN_METHOD(node_FLAC__stream_encoder_new) {
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

        static NAN_METHOD(node_FLAC__stream_encoder_init_stream) {
            UNWRAP_FLAC
            CHECK_ASYNC_IS_NULL

            if(info[0]->IsFunction()) self->writeCbk.reset(new Nan::Callback(info[0].As<Function>()));
            if(info[1]->IsFunction()) self->seekCbk.reset(new Nan::Callback(info[1].As<Function>()));
            if(info[2]->IsFunction()) self->tellCbk.reset(new Nan::Callback(info[2].As<Function>()));
            if(info[3]->IsFunction()) self->metadataCbk.reset(new Nan::Callback(info[3].As<Function>()));

            self->async = new Nan::AsyncResource("flac:encoder:initStream");
            int ret = FLAC__stream_encoder_init_stream(enc,
                !self->writeCbk ? nullptr : write_callback,
                !self->seekCbk ? nullptr : seek_callback,
                !self->tellCbk ? nullptr : tell_callback,
                !self->metadataCbk ? nullptr : metadata_callback,
                self);

            info.GetReturnValue().Set(Nan::New(ret));
            delete self->async;
            self->async = nullptr;
        }

        static NAN_METHOD(node_FLAC__stream_encoder_init_ogg_stream) {
            UNWRAP_FLAC
            CHECK_ASYNC_IS_NULL

            if(info[0]->IsFunction()) self->readCbk.reset(new Nan::Callback(info[0].As<Function>()));
            if(info[1]->IsFunction()) self->writeCbk.reset(new Nan::Callback(info[1].As<Function>()));
            if(info[2]->IsFunction()) self->seekCbk.reset(new Nan::Callback(info[2].As<Function>()));
            if(info[3]->IsFunction()) self->tellCbk.reset(new Nan::Callback(info[3].As<Function>()));
            if(info[4]->IsFunction()) self->metadataCbk.reset(new Nan::Callback(info[4].As<Function>()));

            self->async = new Nan::AsyncResource("flac:encoder:initOggStream");
            int ret = FLAC__stream_encoder_init_ogg_stream(enc,
                !self->readCbk ? nullptr : read_callback,
                !self->writeCbk ? nullptr : write_callback,
                !self->seekCbk ? nullptr : seek_callback,
                !self->tellCbk ? nullptr : tell_callback,
                !self->metadataCbk ? nullptr : metadata_callback,
                self);

            info.GetReturnValue().Set(Nan::New(ret));
            delete self->async;
            self->async = nullptr;
        }

        static NAN_METHOD(node_FLAC__stream_encoder_init_file) {
            UNWRAP_FLAC
            if(!info[0]->IsString()) {
                Nan::ThrowError("Expected first argument to be a string");
                return;
            }

            if(info[1]->IsFunction()) self->progressCbk.reset(new Nan::Callback(info[1].As<Function>()));
            Nan::Utf8String str(info[0]);

            int ret = FLAC__stream_encoder_init_file(enc, *str, info[1]->IsFunction() ? progress_callback : nullptr, self);
            info.GetReturnValue().Set(Nan::New(ret));
        }

        static NAN_METHOD(node_FLAC__stream_encoder_init_ogg_file) {
            UNWRAP_FLAC
            if(!info[0]->IsString()) {
                Nan::ThrowError("Expected first argument to be a string");
                return;
            }

            if(info[1]->IsFunction()) self->progressCbk.reset(new Nan::Callback(info[1].As<Function>()));
            Nan::Utf8String str(info[0]);

            int ret = FLAC__stream_encoder_init_ogg_file(enc, *str, info[1]->IsFunction() ? progress_callback : nullptr, self);
            info.GetReturnValue().Set(Nan::New(ret));
        }

        static NAN_METHOD(node_FLAC__stream_encoder_finish) {
            UNWRAP_FLAC
            CHECK_ASYNC_IS_NULL
            self->async = new Nan::AsyncResource("flac:encoder:finish");
            FLAC__bool ret = FLAC__stream_encoder_finish(enc);
            info.GetReturnValue().Set(Nan::New<Boolean>(ret));
            delete self->async;
            self->async = nullptr;
        }

        static NAN_METHOD(node_FLAC__stream_encoder_process) {
            UNWRAP_FLAC
            CHECK_ASYNC_IS_NULL
            if(info[0].IsEmpty() || !info[0]->IsArray()) {
                Nan::ThrowTypeError("Expected first argument to be an Array");
                return;
            }

            Local<Array> buffers = info[0].As<Array>();
            int32_t** _buffers = new int32_t*[buffers->Length()];
            for(uint32_t i = 0; i < buffers->Length(); i++) {
                _buffers[i] = UnwrapPointer<int32_t>(Nan::Get(buffers, i).ToLocalChecked());
                if(_buffers[i] == nullptr) {
                    std::string err = "Expected element at " + std::to_string(i) + " to be a Buffer";
                    Nan::ThrowTypeError(err.c_str());
                    return;
                }
            }

            auto samples = numberFromJs<uint32_t>(info[1]);
            if(samples.IsNothing()) {
                Nan::ThrowTypeError("Expected second argument to be a number");
                return;
            }

            self->async = new Nan::AsyncResource("flac:encoder:process");
            FLAC__bool ret = FLAC__stream_encoder_process(enc, _buffers, samples.FromJust());
            info.GetReturnValue().Set(Nan::New<Boolean>(ret));
            delete self->async;
            self->async = nullptr;
        }

        static NAN_METHOD(node_FLAC__stream_encoder_process_interleaved) {
            UNWRAP_FLAC
            CHECK_ASYNC_IS_NULL
            const int32_t* buffer = UnwrapPointer<const int32_t>(info[0]);
            auto samples = numberFromJs<uint32_t>(info[1]);

            if(buffer == nullptr) {
                Nan::ThrowTypeError("Expected first argument to be a Buffer");
                return;
            } else if(samples.IsNothing()) {
                Nan::ThrowTypeError("Expected second argument to be a number");
                return;
            }

            self->async = new Nan::AsyncResource("flac:encoder:processInterleaved");
            FLAC__bool ret = FLAC__stream_encoder_process_interleaved(enc, buffer, samples.FromJust());
            info.GetReturnValue().Set(Nan::New<Boolean>(ret));
            delete self->async;
            self->async = nullptr;
        }

        static NAN_METHOD(node_FLAC__stream_encoder_set_metadata) {
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

        static NAN_METHOD(node_FLAC__stream_encoder_set_apodization) {
            UNWRAP_FLAC
            Nan::Utf8String str(info[0]);

            FLAC__bool ret = FLAC__stream_encoder_set_apodization(enc, *str);
            info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        }

        static NAN_METHOD(node_FLAC__stream_encoder_get_resolved_state_string) {
            UNWRAP_FLAC
            info.GetReturnValue().Set(Nan::New(FLAC__stream_encoder_get_resolved_state_string(enc)).ToLocalChecked());
        }

        static NAN_METHOD(node_FLAC__stream_encoder_get_verify_decoder_error_stats) {
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

        static FlacEnumDefineReturnType createStateEnum() {
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

        static FlacEnumDefineReturnType createInitStatusEnum() {
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

        static FlacEnumDefineReturnType createReadStatusEnum() {
            Local<Object> obj1 = Nan::New<Object>();
            Local<Object> obj2 = Nan::New<Object>();
            flacEnum_defineValue(obj1, obj2, "CONTINUE", 0);
            flacEnum_defineValue(obj1, obj2, "END_OF_STREAM", 1);
            flacEnum_defineValue(obj1, obj2, "ABORT", 2);
            flacEnum_defineValue(obj1, obj2, "UNSUPPORTED", 3);
            return std::make_tuple(obj1, obj2);
        }

        static FlacEnumDefineReturnType createWriteStatusEnum() {
            Local<Object> obj1 = Nan::New<Object>();
            Local<Object> obj2 = Nan::New<Object>();
            flacEnum_defineValue(obj1, obj2, "CONTINUE", 0);
            flacEnum_defineValue(obj1, obj2, "FATAL_ERROR", 1);
            return std::make_tuple(obj1, obj2);
        }

        static FlacEnumDefineReturnType createSeekStatusEnum() {
            Local<Object> obj1 = Nan::New<Object>();
            Local<Object> obj2 = Nan::New<Object>();
            flacEnum_defineValue(obj1, obj2, "OK", 0);
            flacEnum_defineValue(obj1, obj2, "ERROR", 1);
            flacEnum_defineValue(obj1, obj2, "UNSUPPORTED", 2);
            return std::make_tuple(obj1, obj2);
        }

        static FlacEnumDefineReturnType createTellStatusEnum() {
            Local<Object> obj1 = Nan::New<Object>();
            Local<Object> obj2 = Nan::New<Object>();
            flacEnum_defineValue(obj1, obj2, "OK", 0);
            flacEnum_defineValue(obj1, obj2, "ERROR", 1);
            flacEnum_defineValue(obj1, obj2, "UNSUPPORTED", 2);
            return std::make_tuple(obj1, obj2);
        }

    public:

        static NAN_MODULE_INIT(initEncoder);

    };

    StreamEncoder::StreamEncoder() {}

    StreamEncoder::~StreamEncoder() {
        FLAC__stream_encoder_delete(enc);
    }

    NAN_MODULE_INIT(StreamEncoder::initEncoder) {
        Local<FunctionTemplate> obj = Nan::New<FunctionTemplate>(node_FLAC__stream_encoder_new);
        obj->SetClassName(Nan::New("StreamEncoder").ToLocalChecked());
        obj->InstanceTemplate()->SetInternalFieldCount(1);

        #define loadFunction(fn) \
        _JOIN(FLAC__stream_encoder_, fn) =  libFlac->getSymbolAddress<_JOIN2(FLAC__stream_encoder_, fn, _t)>("FLAC__stream_encoder_" #fn); \
        if(_JOIN(FLAC__stream_encoder_, fn) == nullptr) printf("%s\n", libFlac->getLastError().c_str());

        #define setMethod(fn, jsFn) \
        Nan::SetPrototypeMethod(obj, #jsFn, _JOIN(node_FLAC__stream_encoder_, fn)); \
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

static int read_callback(const FLAC__StreamEncoder* enc, char buffer[], size_t* bytes, void* data) {
    Nan::HandleScope scope;
    flac_bindings::StreamEncoder* cbks = (flac_bindings::StreamEncoder*) data;
    Handle<Value> args[] = {
        WrapPointer(buffer, *bytes).ToLocalChecked(),
        numberToJs(*bytes)
    };

    Nan::TryCatch tc;
    tc.SetVerbose(true);
    auto ret = (*cbks->readCbk)(cbks->async, 2, args);
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

static int write_callback(const FLAC__StreamEncoder* enc, const char buffer[], size_t bytes, unsigned samples, unsigned frame, void* data) {
    Nan::HandleScope scope;
    flac_bindings::StreamEncoder* cbks = (flac_bindings::StreamEncoder*) data;
    Handle<Value> args[] = {
        WrapPointer(buffer, bytes).ToLocalChecked(),
        numberToJs(bytes),
        numberToJs(samples),
        numberToJs(frame)
    };

    Nan::TryCatch tc;
    tc.SetVerbose(true);
    Local<Value> ret = (*cbks->writeCbk)(cbks->async, 4, args).ToLocalChecked();
    if(tc.HasCaught()) {
        tc.ReThrow();
        return 2;
    }
    int32_t _b = numberFromJs<int32_t>(ret).FromMaybe(0);
    return _b;
}

static int seek_callback(const FLAC__StreamEncoder* enc, uint64_t offset, void* data) {
    Nan::HandleScope scope;
    flac_bindings::StreamEncoder* cbks = (flac_bindings::StreamEncoder*) data;
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

static int tell_callback(const FLAC__StreamEncoder* enc, uint64_t* offset, void* data) {
    Nan::HandleScope scope;
    flac_bindings::StreamEncoder* cbks = (flac_bindings::StreamEncoder*) data;
    Handle<Value> args[1] = {
        numberToJs(*offset)
    };

    Nan::TryCatch tc;
    tc.SetVerbose(true);
    auto ret = (*cbks->tellCbk)(cbks->async, 1, args);
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

static void metadata_callback(const FLAC__StreamEncoder* enc, const FLAC__StreamMetadata* metadata, void* data) {
    Nan::HandleScope scope;
    flac_bindings::StreamEncoder* cbks = (flac_bindings::StreamEncoder*) data;
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

static void progress_callback(const FLAC__StreamEncoder* enc, uint64_t bytes_written, uint64_t samples_written, unsigned frames_written, unsigned total_frames_estimate, void* data) {
    Nan::HandleScope scope;
    flac_bindings::StreamEncoder* cbks = (flac_bindings::StreamEncoder*) data;
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
