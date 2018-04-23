#include <nan.h>
#include "dl.hpp"

using namespace v8;
using namespace node;
#include "pointer.hpp"
#include "format.h"

#define _JOIN(a, b) a##b
#define _JOIN2(a,b,c) a##b##c

typedef int (*FLAC__StreamEncoderReadCallback)(const FLAC__StreamEncoder *encoder, char buffer[], size_t *bytes, void *client_data);
typedef int (*FLAC__StreamEncoderWriteCallback)(const FLAC__StreamEncoder *encoder, const char buffer[], size_t bytes, unsigned samples, unsigned current_frame, void *client_data);
typedef int (*FLAC__StreamEncoderSeekCallback)(const FLAC__StreamEncoder *encoder, uint64_t absolute_byte_offset, void *client_data);
typedef int (*FLAC__StreamEncoderTellCallback)(const FLAC__StreamEncoder *encoder, uint64_t *absolute_byte_offset, void *client_data);
typedef void(*FLAC__StreamEncoderMetadataCallback)(const FLAC__StreamEncoder *encoder, const FLAC__StreamMetadata *metadata, void *client_data);
typedef void(*FLAC__StreamEncoderProgressCallback)(const FLAC__StreamEncoder *encoder, uint64_t bytes_written, uint64_t samples_written, unsigned frames_written, unsigned total_frames_estimate, void *client_data);

struct flac_encoding_callbacks {
    Nan::Persistent<Function> readCbk, writeCbk, seekCbk, tellCbk, metadataCbk, progressCbk;
    Nan::AsyncResource async;

    flac_encoding_callbacks(): async("flac:encoder") {}
};

static int read_callback(const FLAC__StreamEncoder*, char[], size_t*, void*);
static int write_callback(const FLAC__StreamEncoder*, const char[], size_t, unsigned, unsigned, void*);
static int seek_callback(const FLAC__StreamEncoder*, uint64_t, void*);
static int tell_callback(const FLAC__StreamEncoder*, uint64_t*, void*);
static void metadata_callback(const FLAC__StreamEncoder*, const FLAC__StreamMetadata*, void*);
static void progress_callback(const FLAC__StreamEncoder*, uint64_t, uint64_t, unsigned, unsigned, void*);

#define UNWRAP_FLAC \
    if(info[0]->IsUndefined() || info[0]->IsNull()) Nan::ThrowError("Calling FLAC function without the encoder object"); \
    FLAC__StreamEncoder* enc = UnwrapPointer<FLAC__StreamEncoder>(info[0]);

#define FLAC_FUNC(returnType, fn, ...) \
    typedef returnType (*_JOIN2(FLAC__stream_encoder_, fn, _t))(__VA_ARGS__); \
    static _JOIN2(FLAC__stream_encoder_, fn, _t) _JOIN(FLAC__stream_encoder_, fn);

#define FLAC_GETTER(type, v8Type, fn) \
extern "C" { \
    FLAC_FUNC(type, _JOIN(get_, fn), const FLAC__StreamEncoder*); \
} \
NAN_METHOD(_JOIN(node_FLAC__stream_encoder_get_, fn)) { \
    UNWRAP_FLAC \
    type output = _JOIN(FLAC__stream_encoder_get_, fn)(enc); \
    info.GetReturnValue().Set(Nan::New<v8Type>(output)); \
}

#define FLAC_SETTER(type, v8Type, fn) \
extern "C" { \
    FLAC_FUNC(FLAC__bool, _JOIN(set_, fn), FLAC__StreamEncoder*, type) \
} \
NAN_METHOD(_JOIN(node_FLAC__stream_encoder_set_, fn)) { \
    UNWRAP_FLAC \
    type input = (type) info[1]->_JOIN(v8Type, Value)(); \
    FLAC__bool output = _JOIN(FLAC__stream_encoder_set_, fn)(enc, input); \
    info.GetReturnValue().Set(Nan::New(bool(output))); \
}

#define FLAC_GETTER_SETTER(type, v8Type, fn) \
FLAC_GETTER(type, v8Type, fn); \
FLAC_SETTER(type, v8Type, fn);


FLAC_SETTER(long, Number, ogg_serial_number);
FLAC_GETTER_SETTER(FLAC__bool, Boolean, verify);
FLAC_GETTER_SETTER(FLAC__bool, Boolean, streamable_subset);
FLAC_GETTER_SETTER(unsigned, Number, channels);
FLAC_GETTER_SETTER(unsigned, Number, bits_per_sample);
FLAC_GETTER_SETTER(unsigned, Number, sample_rate);
FLAC_SETTER(unsigned, Number, compression_level);
FLAC_GETTER_SETTER(unsigned, Number, blocksize);
FLAC_GETTER_SETTER(FLAC__bool, Boolean, do_mid_side_stereo);
FLAC_GETTER_SETTER(FLAC__bool, Boolean, loose_mid_side_stereo);
FLAC_GETTER_SETTER(unsigned, Number, max_lpc_order);
FLAC_GETTER_SETTER(unsigned, Number, qlp_coeff_precision);
FLAC_GETTER_SETTER(FLAC__bool, Boolean, do_qlp_coeff_prec_search);
FLAC_GETTER_SETTER(FLAC__bool, Boolean, do_escape_coding);
FLAC_GETTER_SETTER(FLAC__bool, Boolean, do_exhaustive_model_search);
FLAC_GETTER_SETTER(unsigned, Number, min_residual_partition_order);
FLAC_GETTER_SETTER(unsigned, Number, max_residual_partition_order);
FLAC_GETTER_SETTER(unsigned, Number, rice_parameter_search_dist);
FLAC_GETTER_SETTER(uint64_t, Number, total_samples_estimate);
FLAC_GETTER(int, Number, state);
FLAC_GETTER(int, Number, verify_decoder_state);

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
    const char* const* FLAC__StreamEncoderStateString;
    const char* const* FLAC__StreamEncoderInitStatusString;
    const char* const* FLAC__StreamEncoderReadStatusString;
    const char* const* FLAC__StreamEncoderWriteStatusString;
    const char* const* FLAC__StreamEncoderSeekStatusString;
    const char* const* FLAC__StreamEncoderTellStatusString;
}

namespace flac_bindings {

    extern Library* libFlac;
    static std::vector<flac_encoding_callbacks*> objectsToDelete;

    NAN_METHOD(node_FLAC__stream_encoder_new) {
        FLAC__StreamEncoder* enc = FLAC__stream_encoder_new();
        if(enc != nullptr) {
            info.GetReturnValue().Set(WrapPointer(enc).ToLocalChecked());
        } else {
            info.GetReturnValue().SetNull();
        }
    }

    NAN_METHOD(node_FLAC__stream_encoder_delete) {
        UNWRAP_FLAC
        FLAC__stream_encoder_delete(enc);
    }

    NAN_METHOD(node_FLAC__stream_encoder_init_stream) {
        UNWRAP_FLAC

        flac_encoding_callbacks* req = new flac_encoding_callbacks;
        if(info[1]->IsFunction()) req->writeCbk.Reset(info[1].As<Function>());
        if(info[2]->IsFunction()) req->seekCbk.Reset(info[2].As<Function>());
        if(info[3]->IsFunction()) req->tellCbk.Reset(info[3].As<Function>());
        if(info[4]->IsFunction()) req->metadataCbk.Reset(info[4].As<Function>());
        int ret = FLAC__stream_encoder_init_stream(enc,
            req->writeCbk.IsEmpty() ? nullptr : write_callback,
            req->seekCbk.IsEmpty() ? nullptr : seek_callback,
            req->tellCbk.IsEmpty() ? nullptr : tell_callback,
            req->metadataCbk.IsEmpty() ? nullptr : metadata_callback,
            req);

        info.GetReturnValue().Set(Nan::New(ret));
        objectsToDelete.push_back(req);
    }

    NAN_METHOD(node_FLAC__stream_encoder_init_ogg_stream) {
        UNWRAP_FLAC
        flac_encoding_callbacks* req = new flac_encoding_callbacks;
        if(info[1]->IsFunction()) req->readCbk.Reset(info[1].As<Function>());
        if(info[2]->IsFunction()) req->writeCbk.Reset(info[2].As<Function>());
        if(info[3]->IsFunction()) req->seekCbk.Reset(info[3].As<Function>());
        if(info[4]->IsFunction()) req->tellCbk.Reset(info[4].As<Function>());
        req->metadataCbk.Reset(info[5].As<Function>());
        int ret = FLAC__stream_encoder_init_ogg_stream(enc,
            req->readCbk.IsEmpty() ? nullptr : read_callback,
            req->writeCbk.IsEmpty() ? nullptr : write_callback,
            req->seekCbk.IsEmpty() ? nullptr : seek_callback,
            req->tellCbk.IsEmpty() ? nullptr : tell_callback,
            req->metadataCbk.IsEmpty() ? nullptr : metadata_callback,
            req);

        info.GetReturnValue().Set(Nan::New(ret));
        objectsToDelete.push_back(req);
    }

    NAN_METHOD(node_FLAC__stream_encoder_init_file) {
        UNWRAP_FLAC
        if(!info[1]->IsString()) {
            Nan::ThrowError("Second argument has to be a String");
        }

        flac_encoding_callbacks* req = new flac_encoding_callbacks;
        if(info[2]->IsFunction()) req->progressCbk.Reset(info[2].As<Function>());
        Nan::Utf8String str(info[1]);

        int ret = FLAC__stream_encoder_init_file(enc, *str, info[2]->IsFunction() ? progress_callback : nullptr, req);
        info.GetReturnValue().Set(Nan::New(ret));
        objectsToDelete.push_back(req);
    }

    NAN_METHOD(node_FLAC__stream_encoder_init_ogg_file) {
        UNWRAP_FLAC
        if(!info[1]->IsString()) {
            Nan::ThrowError("Second argument has to be a String");
        }

        flac_encoding_callbacks* req = new flac_encoding_callbacks;
        if(info[2]->IsFunction()) req->progressCbk.Reset(info[2].As<Function>());
        Nan::Utf8String str(info[1]);

        int ret = FLAC__stream_encoder_init_ogg_file(enc, *str, info[2]->IsFunction() ? progress_callback : nullptr, req);
        info.GetReturnValue().Set(Nan::New(ret));
        objectsToDelete.push_back(req);
    }

    NAN_METHOD(node_FLAC__stream_encoder_finish) {
        UNWRAP_FLAC
        FLAC__bool ret = FLAC__stream_encoder_finish(enc);
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
    }

    NAN_METHOD(node_FLAC__stream_encoder_process) {
        UNWRAP_FLAC
        Local<Array> buffers = info[1].As<Array>();
        int32_t** _buffers = new int32_t*[buffers->Length()];
        for(uint32_t i = 0; i < buffers->Length(); i++) {
            _buffers[i] = UnwrapPointer<int32_t>(Nan::Get(buffers, i).ToLocalChecked());
        }
        uint32_t samples = Nan::To<uint32_t>(info[2].As<Number>()).FromJust();
        FLAC__bool ret = FLAC__stream_encoder_process(enc, _buffers, samples);
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
    }

    NAN_METHOD(node_FLAC__stream_encoder_process_interleaved) {
        UNWRAP_FLAC
        const int32_t* buffer = UnwrapPointer<const int32_t>(info[1]->ToObject());
        uint32_t samples = Nan::To<uint32_t>(info[2].As<Number>()).FromJust();
        FLAC__bool ret = FLAC__stream_encoder_process_interleaved(enc, buffer, samples);
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
    }

    NAN_INDEX_GETTER(node_FLAC__StreamEncoderStateString) {
        if(index < 9) {
            info.GetReturnValue().Set(Nan::New(FLAC__StreamEncoderStateString[index]).ToLocalChecked());
        } else {
            info.GetReturnValue().SetNull();
        }
    }

    NAN_INDEX_ENUMERATOR(node_FLAC__StreamEncoderStateString) {
        Local<Array> array = Nan::New<Array>();
        for(int i = 0; i < 9; i++) Nan::Set(array, i, Nan::New(i));
        info.GetReturnValue().Set(array);
    }

    NAN_INDEX_GETTER(node_FLAC__StreamEncoderInitStatusString) {
        if(index < 14) {
            info.GetReturnValue().Set(Nan::New(FLAC__StreamEncoderInitStatusString[index]).ToLocalChecked());
        } else {
            info.GetReturnValue().SetNull();
        }
    }

    NAN_INDEX_ENUMERATOR(node_FLAC__StreamEncoderInitStatusString) {
        Local<Array> array = Nan::New<Array>();
        for(int i = 0; i < 14; i++) Nan::Set(array, i, Nan::New(i));
        info.GetReturnValue().Set(array);
    }

    NAN_INDEX_GETTER(node_FLAC__StreamEncoderReadStatusString) {
        if(index < 4) {
            info.GetReturnValue().Set(Nan::New(FLAC__StreamEncoderReadStatusString[index]).ToLocalChecked());
        } else {
            info.GetReturnValue().SetNull();
        }
    }

    NAN_INDEX_ENUMERATOR(node_FLAC__StreamEncoderReadStatusString) {
        Local<Array> array = Nan::New<Array>();
        for(int i = 0; i < 4; i++) Nan::Set(array, i, Nan::New(i));
        info.GetReturnValue().Set(array);
    }

    NAN_INDEX_GETTER(node_FLAC__StreamEncoderWriteStatusString) {
        if(index < 2) {
            info.GetReturnValue().Set(Nan::New(FLAC__StreamEncoderWriteStatusString[index]).ToLocalChecked());
        } else {
            info.GetReturnValue().SetNull();
        }
    }

    NAN_INDEX_ENUMERATOR(node_FLAC__StreamEncoderWriteStatusString) {
        Local<Array> array = Nan::New<Array>();
        for(int i = 0; i < 2; i++) Nan::Set(array, i, Nan::New(i));
        info.GetReturnValue().Set(array);
    }

    NAN_INDEX_GETTER(node_FLAC__StreamEncoderSeekStatusString) {
        if(index < 3) {
            info.GetReturnValue().Set(Nan::New(FLAC__StreamEncoderSeekStatusString[index]).ToLocalChecked());
        } else {
            info.GetReturnValue().SetNull();
        }
    }

    NAN_INDEX_ENUMERATOR(node_FLAC__StreamEncoderSeekStatusString) {
        Local<Array> array = Nan::New<Array>();
        for(int i = 0; i < 3; i++) Nan::Set(array, i, Nan::New(i));
        info.GetReturnValue().Set(array);
    }

    NAN_INDEX_GETTER(node_FLAC__StreamEncoderTellStatusString) {
        if(index < 3) {
            info.GetReturnValue().Set(Nan::New(FLAC__StreamEncoderTellStatusString[index]).ToLocalChecked());
        } else {
            info.GetReturnValue().SetNull();
        }
    }

    NAN_INDEX_ENUMERATOR(node_FLAC__StreamEncoderTellStatusString) {
        Local<Array> array = Nan::New<Array>();
        for(int i = 0; i < 3; i++) Nan::Set(array, i, Nan::New(i));
        info.GetReturnValue().Set(array);
    }

    NAN_METHOD(node_FLAC__stream_encoder_set_metadata) {
        UNWRAP_FLAC
        Local<Array> metadata = info[1].As<Array>();
        uint32_t num_blocks = metadata->Length();
        FLAC__StreamMetadata** metadatas = new FLAC__StreamMetadata*[num_blocks];

        for(uint32_t i = 0; i < num_blocks; i++) {
            metadatas[i] = fromjs<FLAC__StreamMetadata>(Nan::Get(metadata, i).ToLocalChecked().As<Object>());
        }

        FLAC__bool ret = FLAC__stream_encoder_set_metadata(enc, metadatas, num_blocks);
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        delete[] metadatas;
    }

    NAN_METHOD(node_FLAC__stream_encoder_set_apodization) {
        UNWRAP_FLAC
        Nan::Utf8String str(info[1]);

        FLAC__bool ret = FLAC__stream_encoder_set_apodization(enc, *str);
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
    }

    NAN_METHOD(node_FLAC__stream_encoder_get_resolved_state_string) {
        UNWRAP_FLAC
        info.GetReturnValue().Set(Nan::New(FLAC__stream_encoder_get_resolved_state_string(enc)).ToLocalChecked());
    }

    NAN_METHOD(node_FLAC__stream_encoder_get_verify_decoder_error_stats) {
        UNWRAP_FLAC
        uint64_t absolute_sample;
        unsigned frame_number, channel, sample;
        int32_t expected, got;
        Local<Object> obj = Nan::New<Object>();

        FLAC__stream_encoder_get_verify_decoder_error_stats(enc, &absolute_sample, &frame_number, &channel, &sample, &expected, &got);

        Nan::Set(obj, Nan::New("absoluteSample").ToLocalChecked(), Nan::New<Number>(absolute_sample));
        Nan::Set(obj, Nan::New("frameNumber").ToLocalChecked(), Nan::New<Number>(frame_number));
        Nan::Set(obj, Nan::New("channel").ToLocalChecked(), Nan::New<Number>(channel));
        Nan::Set(obj, Nan::New("sample").ToLocalChecked(), Nan::New<Number>(sample));
        Nan::Set(obj, Nan::New("expected").ToLocalChecked(), Nan::New<Number>(expected));
        Nan::Set(obj, Nan::New("got").ToLocalChecked(), Nan::New<Number>(got));
        info.GetReturnValue().Set(obj);
    }

    NAN_PROPERTY_GETTER(State) {
        Nan::Utf8String propertyName(property);
        std::string PropertyName(*propertyName);

        if(PropertyName == "OK") info.GetReturnValue().Set(Nan::New(0));
        else if(PropertyName == "UNINITIALIZED") info.GetReturnValue().Set(Nan::New(1));
        else if(PropertyName == "OGG_ERROR") info.GetReturnValue().Set(Nan::New(2));
        else if(PropertyName == "VERIFY_DECODER_ERROR") info.GetReturnValue().Set(Nan::New(3));
        else if(PropertyName == "VERIFY_MISMATCH_IN_AUDIO_DATA") info.GetReturnValue().Set(Nan::New(4));
        else if(PropertyName == "CLIENT_ERROR") info.GetReturnValue().Set(Nan::New(5));
        else if(PropertyName == "IO_ERROR") info.GetReturnValue().Set(Nan::New(6));
        else if(PropertyName == "FRAMING_ERROR") info.GetReturnValue().Set(Nan::New(7));
        else if(PropertyName == "MEMORY_ALLOCATION_ERROR") info.GetReturnValue().Set(Nan::New(8));
        else info.GetReturnValue().SetUndefined();
    }

    NAN_PROPERTY_GETTER(InitStatus) {
        Nan::Utf8String propertyName(property);
        std::string PropertyName(*propertyName);

        if(PropertyName == "OK") info.GetReturnValue().Set(Nan::New(0));
        else if(PropertyName == "ENCODER_ERROR") info.GetReturnValue().Set(Nan::New(1));
        else if(PropertyName == "UNSUPPORTED_CONTAINER") info.GetReturnValue().Set(Nan::New(2));
        else if(PropertyName == "INVALID_CALLBACKS") info.GetReturnValue().Set(Nan::New(3));
        else if(PropertyName == "INVALID_NUMBER_OF_CHANNELS") info.GetReturnValue().Set(Nan::New(4));
        else if(PropertyName == "INVALID_BITS_PER_SAMPLE") info.GetReturnValue().Set(Nan::New(5));
        else if(PropertyName == "INVALID_SAMPLE_RATE") info.GetReturnValue().Set(Nan::New(6));
        else if(PropertyName == "INVALID_BLOCK_SIZE") info.GetReturnValue().Set(Nan::New(7));
        else if(PropertyName == "INVALID_MAX_LPC_ORDER") info.GetReturnValue().Set(Nan::New(8));
        else if(PropertyName == "INVALID_QLP_COEFF_PRECISION") info.GetReturnValue().Set(Nan::New(9));
        else if(PropertyName == "BLOCK_SIZE_TOO_SMALL_FOR_LPC_ORDER") info.GetReturnValue().Set(Nan::New(10));
        else if(PropertyName == "NOT_STREAMABLE") info.GetReturnValue().Set(Nan::New(11));
        else if(PropertyName == "INVALID_METADATA") info.GetReturnValue().Set(Nan::New(12));
        else if(PropertyName == "ALREADY_INITIALIZED") info.GetReturnValue().Set(Nan::New(13));
        else info.GetReturnValue().SetUndefined();
    }

    NAN_PROPERTY_GETTER(ReadStatus) {
        Nan::Utf8String propertyName(property);
        std::string PropertyName(*propertyName);

        if(PropertyName == "CONTINUE") info.GetReturnValue().Set(Nan::New(0));
        else if(PropertyName == "END_OF_STREAM") info.GetReturnValue().Set(Nan::New(1));
        else if(PropertyName == "ABORT") info.GetReturnValue().Set(Nan::New(2));
        else if(PropertyName == "UNSUPPORTED") info.GetReturnValue().Set(Nan::New(3));
        else info.GetReturnValue().SetUndefined();
    }

    NAN_PROPERTY_GETTER(WriteStatus) {
        Nan::Utf8String propertyName(property);
        std::string PropertyName(*propertyName);

        if(PropertyName == "CONTINUE") info.GetReturnValue().Set(Nan::New(0));
        else if(PropertyName == "FATAL_ERROR") info.GetReturnValue().Set(Nan::New(1));
        else info.GetReturnValue().SetUndefined();
    }

    NAN_PROPERTY_GETTER(SeekStatus) {
        Nan::Utf8String propertyName(property);
        std::string PropertyName(*propertyName);

        if(PropertyName == "OK") info.GetReturnValue().Set(Nan::New(0));
        else if(PropertyName == "ERROR") info.GetReturnValue().Set(Nan::New(1));
        else if(PropertyName == "UNSUPPORTED") info.GetReturnValue().Set(Nan::New(2));
        else info.GetReturnValue().SetUndefined();
    }

    NAN_PROPERTY_GETTER(TellStatus) {
        Nan::Utf8String propertyName(property);
        std::string PropertyName(*propertyName);

        if(PropertyName == "OK") info.GetReturnValue().Set(Nan::New(0));
        else if(PropertyName == "ERROR") info.GetReturnValue().Set(Nan::New(1));
        else if(PropertyName == "UNSUPPORTED") info.GetReturnValue().Set(Nan::New(2));
        else info.GetReturnValue().SetUndefined();
    }

    NAN_MODULE_INIT(initEncoder) {
        Local<Object> obj = Nan::New<Object>();
        #define setMethod(fn) \
        Nan::SetMethod(obj, #fn, _JOIN(node_FLAC__stream_encoder_, fn)); \
        _JOIN(FLAC__stream_encoder_, fn) =  libFlac->getSymbolAddress<_JOIN2(FLAC__stream_encoder_, fn, _t)>("FLAC__stream_encoder_" #fn); \
        if(_JOIN(FLAC__stream_encoder_, fn) == nullptr) printf("%s\n", libFlac->getLastError().c_str());

        setMethod(set_ogg_serial_number);
        setMethod(set_verify);
        setMethod(set_streamable_subset);
        setMethod(set_channels);
        setMethod(set_bits_per_sample);
        setMethod(set_sample_rate);
        setMethod(set_compression_level);
        setMethod(set_blocksize);
        setMethod(set_do_mid_side_stereo);
        setMethod(set_loose_mid_side_stereo);
        setMethod(set_apodization);
        setMethod(set_max_lpc_order);
        setMethod(set_qlp_coeff_precision);
        setMethod(set_do_qlp_coeff_prec_search);
        setMethod(set_do_escape_coding);
        setMethod(set_do_exhaustive_model_search);
        setMethod(set_min_residual_partition_order);
        setMethod(set_max_residual_partition_order);
        setMethod(set_rice_parameter_search_dist);
        setMethod(set_total_samples_estimate);
        setMethod(set_metadata);
        setMethod(get_state);
        setMethod(get_verify_decoder_state);
        setMethod(get_resolved_state_string);
        setMethod(get_verify_decoder_error_stats);
        setMethod(get_verify);
        setMethod(get_streamable_subset);
        setMethod(get_channels);
        setMethod(get_bits_per_sample);
        setMethod(get_sample_rate);
        setMethod(get_blocksize);
        setMethod(get_do_mid_side_stereo);
        setMethod(get_loose_mid_side_stereo);
        setMethod(get_max_lpc_order);
        setMethod(get_qlp_coeff_precision);
        setMethod(get_do_qlp_coeff_prec_search);
        setMethod(get_do_escape_coding);
        setMethod(get_do_exhaustive_model_search);
        setMethod(get_min_residual_partition_order);
        setMethod(get_max_residual_partition_order);
        setMethod(get_rice_parameter_search_dist);
        setMethod(get_total_samples_estimate);
        setMethod(init_stream);
        setMethod(init_ogg_stream);
        setMethod(init_file);
        setMethod(init_ogg_file);
        setMethod(finish);
        setMethod(process);
        setMethod(process_interleaved);
        setMethod(new);
        setMethod(delete);

        #define indexGetter(func) \
        _JOIN(FLAC__StreamEncoder, func) = libFlac->getSymbolAddress<const char* const*>("FLAC__StreamEncoder" #func); \
        Local<ObjectTemplate> _JOIN(func, _template) = Nan::New<ObjectTemplate>(); \
        Nan::SetIndexedPropertyHandler(_JOIN(func, _template), _JOIN(node_FLAC__StreamEncoder, func), nullptr, nullptr, nullptr, \
            _JOIN(node_FLAC__StreamEncoder, func)); \
        Nan::Set(obj, Nan::New(#func).ToLocalChecked(), Nan::NewInstance(_JOIN(func, _template)).ToLocalChecked());

        indexGetter(StateString);
        indexGetter(InitStatusString);
        indexGetter(ReadStatusString);
        indexGetter(WriteStatusString);
        indexGetter(SeekStatusString);
        indexGetter(TellStatusString);

        #define propertyGetter(func) \
        Local<ObjectTemplate> _JOIN(func, Var) = Nan::New<ObjectTemplate>(); \
        Nan::SetNamedPropertyHandler(_JOIN(func, Var), func); \
        Nan::Set(obj, Nan::New(#func).ToLocalChecked(), Nan::NewInstance(_JOIN(func, Var)).ToLocalChecked());

        propertyGetter(State);
        propertyGetter(InitStatus);
        propertyGetter(ReadStatus);
        propertyGetter(WriteStatus);
        propertyGetter(SeekStatus);
        propertyGetter(TellStatus);

        Nan::Set(target, Nan::New("encoder").ToLocalChecked(), obj);
    }

    void atExitEncoder() {
        for(auto it = objectsToDelete.begin(); it != objectsToDelete.end(); it++) {
            (*it)->readCbk.Reset();
            (*it)->writeCbk.Reset();
            (*it)->seekCbk.Reset();
            (*it)->tellCbk.Reset();
            (*it)->progressCbk.Reset();
            (*it)->metadataCbk.Reset();
            delete (*it);
        }
        objectsToDelete.clear();
    }
};

static int read_callback(const FLAC__StreamEncoder* enc, char buffer[], size_t* bytes, void* data) {
    Nan::HandleScope scope;
    flac_encoding_callbacks* cbks = (flac_encoding_callbacks*) data;
    Handle<Value> args[] = {
        WrapPointer(buffer, *bytes).ToLocalChecked(),
        Nan::New<Number>(*bytes)
    };

    Nan::TryCatch tc;tc.SetVerbose(true);
    Local<Function> func = Nan::New(cbks->readCbk);
    Local<Value> ret = cbks->async.runInAsyncScope(Nan::GetCurrentContext()->Global(), func, 2, args).ToLocalChecked();
    if(tc.HasCaught()) {
        tc.ReThrow();
        return 2;
    }
    if(ret.IsEmpty()) {
        *bytes = 0;
        return 2;
    } else {
        Local<Object> retJust = ret.As<Object>();
        Local<Value> bytes2 = Nan::Get(retJust, Nan::New("bytes").ToLocalChecked()).ToLocalChecked();
        Local<Value> returnValue = Nan::Get(retJust, Nan::New("returnValue").ToLocalChecked()).ToLocalChecked();
        *bytes = (size_t) Nan::To<int32_t>(bytes2).FromJust();
        return Nan::To<int32_t>(returnValue).FromJust();
    }
}

static int write_callback(const FLAC__StreamEncoder* enc, const char buffer[], size_t bytes, unsigned samples, unsigned frame, void* data) {
    Nan::HandleScope scope;
    flac_encoding_callbacks* cbks = (flac_encoding_callbacks*) data;
    Handle<Value> args[] = {
        WrapPointer(buffer, bytes).ToLocalChecked(),
        Nan::New<Number>(bytes),
        Nan::New<Number>(samples),
        Nan::New<Number>(frame)
    };

    Nan::TryCatch tc;tc.SetVerbose(true);
    Local<Function> func = Nan::New(cbks->writeCbk);
    Local<Value> ret = cbks->async.runInAsyncScope(Nan::GetCurrentContext()->Global(), func, 4, args).ToLocalChecked();
    if(tc.HasCaught()) {
        tc.ReThrow();
        return 2;
    }
    int32_t _b = Nan::To<int32_t>(ret).FromJust();
    return _b;
}

static int seek_callback(const FLAC__StreamEncoder* enc, uint64_t offset, void* data) {
    Nan::HandleScope scope;
    flac_encoding_callbacks* cbks = (flac_encoding_callbacks*) data;
    Handle<Value> args[] = {
        Nan::New<Number>(offset)
    };

    Nan::TryCatch tc;tc.SetVerbose(true);
    Local<Function> func = Nan::New(cbks->seekCbk);
    Local<Value> ret = cbks->async.runInAsyncScope(Nan::GetCurrentContext()->Global(), func, 1, args).ToLocalChecked();
    if(tc.HasCaught()) {
        tc.ReThrow();
        return 1;
    }
    return Nan::To<int>(ret).FromJust();
}

static int tell_callback(const FLAC__StreamEncoder* enc, uint64_t* offset, void* data) {
    Nan::HandleScope scope;
    flac_encoding_callbacks* cbks = (flac_encoding_callbacks*) data;
    Handle<Value> args[1] = {
        Nan::New<Number>(*offset)
    };

    Nan::TryCatch tc;tc.SetVerbose(true);
    Local<Function> func = Nan::New(cbks->tellCbk);
    Local<Value> ret = cbks->async.runInAsyncScope(Nan::GetCurrentContext()->Global(), func, 1, args).ToLocalChecked();
    if(tc.HasCaught()) {
        tc.ReThrow();
        return 1;
    }
    if(ret.IsEmpty()) {
        *offset = 0;
        return 1;
    } else {
        Local<Object> retJust = ret.As<Object>();
        Local<Value> offset2 = Nan::Get(retJust, Nan::New("offset").ToLocalChecked()).ToLocalChecked();
        Local<Value> returnValue = Nan::Get(retJust, Nan::New("returnValue").ToLocalChecked()).ToLocalChecked();
        *offset = (size_t) Nan::To<int64_t>(offset2).FromJust();
        return Nan::To<int32_t>(returnValue).FromJust();
    }
}

static void metadata_callback(const FLAC__StreamEncoder* enc, const FLAC__StreamMetadata* metadata, void* data) {
    Nan::HandleScope scope;
    flac_encoding_callbacks* cbks = (flac_encoding_callbacks*) data;
    Handle<Value> args[] = {
        flac_bindings::structToJs(metadata)
    };

    Nan::TryCatch tc;tc.SetVerbose(true);
    Local<Function> func = Nan::New(cbks->metadataCbk);
    cbks->async.runInAsyncScope(Nan::GetCurrentContext()->Global(), func, 1, args);
}

static void progress_callback(const FLAC__StreamEncoder* enc, uint64_t bytes_written, uint64_t samples_written, unsigned frames_written, unsigned total_frames_estimate, void* data) {
    Nan::HandleScope scope;
    flac_encoding_callbacks* cbks = (flac_encoding_callbacks*) data;
    Handle<Value> args[] = {
        Nan::New<Number>(bytes_written),
        Nan::New<Number>(samples_written),
        Nan::New<Number>(frames_written),
        Nan::New<Number>(total_frames_estimate)
    };

    Nan::TryCatch tc;tc.SetVerbose(true);
    Local<Function> func = Nan::New(cbks->progressCbk);
    cbks->async.runInAsyncScope(Nan::GetCurrentContext()->Global(), func, 4, args);
    if(tc.HasCaught()) {
        tc.ReThrow();
    }
}
