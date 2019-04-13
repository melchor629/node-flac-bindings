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

typedef int(*FLAC__StreamDecoderReadCallback)(const FLAC__StreamDecoder*, FLAC__byte [], size_t*, void*);
typedef int(*FLAC__StreamDecoderSeekCallback)(const FLAC__StreamDecoder*, uint64_t, void*);
typedef int(*FLAC__StreamDecoderTellCallback)(const FLAC__StreamDecoder*, uint64_t*, void*);
typedef int(*FLAC__StreamDecoderLengthCallback)(const FLAC__StreamDecoder*, uint64_t*, void*);
typedef FLAC__bool(*FLAC__StreamDecoderEofCallback)(const FLAC__StreamDecoder*, void*);
typedef int(*FLAC__StreamDecoderWriteCallback)(const FLAC__StreamDecoder*, const FLAC__Frame*, const int32_t *const [], void*);
typedef void(*FLAC__StreamDecoderMetadataCallback)(const FLAC__StreamDecoder*, const FLAC__StreamMetadata*, void*);
typedef void(*FLAC__StreamDecoderErrorCallback)(const FLAC__StreamDecoder*, int, void*);

static int read_callback(const FLAC__StreamDecoder*, FLAC__byte [], size_t*, void*);
static int seek_callback(const FLAC__StreamDecoder*, uint64_t, void*);
static int tell_callback(const FLAC__StreamDecoder*, uint64_t*, void*);
static int length_callback(const FLAC__StreamDecoder*, uint64_t*, void*);
static FLAC__bool eof_callback(const FLAC__StreamDecoder*, void*);
static int write_callback(const FLAC__StreamDecoder*, const FLAC__Frame*, const int32_t *const [], void*);
static void metadata_callback(const FLAC__StreamDecoder*, const FLAC__StreamMetadata*, void*);
static void error_callback(const FLAC__StreamDecoder*, int, void*);

#define UNWRAP_FLAC \
    StreamDecoder* self = Nan::ObjectWrap::Unwrap<StreamDecoder>(info.Holder()); \
    FLAC__StreamDecoder* dec = self->dec;

#define FLAC_FUNC(returnType, fn, ...) \
    typedef returnType (*_JOIN2(FLAC__stream_decoder_, fn, _t))(__VA_ARGS__); \
    static _JOIN2(FLAC__stream_decoder_, fn, _t) _JOIN(FLAC__stream_decoder_, fn);

#define FLAC_GETTER(type, fn) FLAC_FUNC(type, _JOIN(get_, fn), const FLAC__StreamDecoder*);

#define FLAC_SETTER(type, fn) FLAC_FUNC(FLAC__bool, _JOIN(set_, fn), FLAC__StreamDecoder*, type)

#define FLAC_GETTER_SETTER(type, fn) \
FLAC_GETTER(type, fn); \
FLAC_SETTER(type, fn);

#define FLAC_GETTER_METHOD(type, v8Type, fn) \
NAN_METHOD(_JOIN(node_FLAC__stream_decoder_get_, fn)) { \
    UNWRAP_FLAC \
    type output = _JOIN(FLAC__stream_decoder_get_, fn)(dec); \
    info.GetReturnValue().Set(_JOIN(v8Type, ToJs)(output)); \
}

#define FLAC_SETTER_METHOD(type, v8Type, fn) \
NAN_METHOD(_JOIN(node_FLAC__stream_decoder_set_, fn)) { \
    UNWRAP_FLAC \
    auto inputMaybe = _JOIN(v8Type, FromJs)<type>(info[0]); \
    if(inputMaybe.IsNothing()) { \
        Nan::ThrowTypeError("Expected type to be " #v8Type ); \
        return; \
    } \
    type input = (type) inputMaybe.FromJust(); \
    FLAC__bool output = _JOIN(FLAC__stream_decoder_set_, fn)(dec, input); \
    info.GetReturnValue().Set(Nan::New(bool(output))); \
}

#define CHECK_ASYNC_IS_NULL \
if(self->async != nullptr) { Nan::ThrowError("Multiple calls to some methods of the Decoder are not allowed"); return; }


extern "C" {
    FLAC_FUNC(FLAC__StreamDecoder*, new, void);
    FLAC_FUNC(void, delete, FLAC__StreamDecoder*);
    FLAC_FUNC(int, init_stream, FLAC__StreamDecoder*, FLAC__StreamDecoderReadCallback,
        FLAC__StreamDecoderSeekCallback, FLAC__StreamDecoderTellCallback,
        FLAC__StreamDecoderLengthCallback, FLAC__StreamDecoderEofCallback,
        FLAC__StreamDecoderWriteCallback, FLAC__StreamDecoderMetadataCallback,
        FLAC__StreamDecoderErrorCallback, void*);
    FLAC_FUNC(int, init_ogg_stream, FLAC__StreamDecoder*, FLAC__StreamDecoderReadCallback,
        FLAC__StreamDecoderSeekCallback, FLAC__StreamDecoderTellCallback,
        FLAC__StreamDecoderLengthCallback, FLAC__StreamDecoderEofCallback,
        FLAC__StreamDecoderWriteCallback, FLAC__StreamDecoderMetadataCallback,
        FLAC__StreamDecoderErrorCallback, void*);
    FLAC_FUNC(int, init_file, FLAC__StreamDecoder*, const char*, FLAC__StreamDecoderWriteCallback,
        FLAC__StreamDecoderMetadataCallback, FLAC__StreamDecoderErrorCallback, void*);
    FLAC_FUNC(int, init_ogg_file, FLAC__StreamDecoder*, const char*, FLAC__StreamDecoderWriteCallback,
        FLAC__StreamDecoderMetadataCallback, FLAC__StreamDecoderErrorCallback, void*);
    FLAC_FUNC(FLAC__bool, finish, FLAC__StreamDecoder*);
    FLAC_FUNC(FLAC__bool, flush, FLAC__StreamDecoder*);
    FLAC_FUNC(FLAC__bool, reset, FLAC__StreamDecoder*);
    FLAC_FUNC(FLAC__bool, process_single, FLAC__StreamDecoder*);
    FLAC_FUNC(FLAC__bool, process_until_end_of_metadata, FLAC__StreamDecoder*);
    FLAC_FUNC(FLAC__bool, process_until_end_of_stream, FLAC__StreamDecoder*);
    FLAC_FUNC(FLAC__bool, skip_single_frame, FLAC__StreamDecoder*);
    FLAC_FUNC(FLAC__bool, seek_absolute, FLAC__StreamDecoder*, uint64_t);
    FLAC_FUNC(FLAC__bool, set_metadata_respond_application, FLAC__StreamDecoder*, const FLAC__byte id[4]);
    FLAC_FUNC(FLAC__bool, set_metadata_respond_all, FLAC__StreamDecoder*);
    FLAC_FUNC(FLAC__bool, set_metadata_ignore_application, FLAC__StreamDecoder*, const FLAC__byte id[4]);
    FLAC_FUNC(FLAC__bool, set_metadata_ignore_all, FLAC__StreamDecoder*);
    FLAC_FUNC(int, get_state, const FLAC__StreamDecoder*);
    FLAC_FUNC(const char*, get_resolved_state_string, const FLAC__StreamDecoder*);
    FLAC_FUNC(FLAC__bool, get_decode_position, const FLAC__StreamDecoder*, uint64_t*);

    FLAC_SETTER(long, ogg_serial_number);
    FLAC_GETTER_SETTER(FLAC__bool, md5_checking);
    FLAC_SETTER(FLAC__MetadataType, metadata_respond);
    FLAC_SETTER(FLAC__MetadataType, metadata_ignore);
    FLAC_GETTER(uint64_t, total_samples);
    FLAC_GETTER(unsigned, channels);
    FLAC_GETTER(FLAC__ChannelAssignment, channel_assignment);
    FLAC_GETTER(unsigned, bits_per_sample);
    FLAC_GETTER(unsigned, sample_rate);
    FLAC_GETTER(unsigned, blocksize);
}

namespace flac_bindings {

    extern Library* libFlac;

    class StreamDecoder: public Nan::ObjectWrap {

    public:
        std::shared_ptr<Nan::Callback> readCbk, seekCbk, tellCbk, lengthCbk, eofCbk, writeCbk, metadataCbk, errorCbk;
        Nan::AsyncResource* async = nullptr;
        FLAC__StreamDecoder* dec = nullptr;

        ~StreamDecoder();

    private:

        static FLAC_SETTER_METHOD(long, number, ogg_serial_number);
        static FLAC_GETTER_METHOD(FLAC__bool, boolean, md5_checking);
        static FLAC_SETTER_METHOD(FLAC__bool, boolean, md5_checking);
        static FLAC_SETTER_METHOD(FLAC__MetadataType, number, metadata_respond);
        static FLAC_SETTER_METHOD(FLAC__MetadataType, number, metadata_ignore);
        static FLAC_GETTER_METHOD(uint64_t, number, total_samples);
        static FLAC_GETTER_METHOD(unsigned, number, channels);
        static FLAC_GETTER_METHOD(FLAC__ChannelAssignment, number, channel_assignment);
        static FLAC_GETTER_METHOD(unsigned, number, bits_per_sample);
        static FLAC_GETTER_METHOD(unsigned, number, sample_rate);
        static FLAC_GETTER_METHOD(unsigned, number, blocksize);

        static NAN_METHOD(node_FLAC__stream_decoder_new) {
            if(throwIfNotConstructorCall(info)) return;
            FLAC__StreamDecoder* dec = FLAC__stream_decoder_new();
            if(dec != nullptr) {
                StreamDecoder* obj = new StreamDecoder;
                obj->dec = dec;
                obj->Wrap(info.This());
                info.GetReturnValue().Set(info.This());
            } else {
                Nan::ThrowError("Could not allocate memory");
            }
        }

        static NAN_METHOD(node_FLAC__stream_decoder_init_stream) {
            UNWRAP_FLAC
            CHECK_ASYNC_IS_NULL

            if(info[0]->IsFunction()) self->readCbk.reset(new Nan::Callback(info[0].As<Function>()));
            if(info[1]->IsFunction()) self->seekCbk.reset(new Nan::Callback(info[1].As<Function>()));
            if(info[2]->IsFunction()) self->tellCbk.reset(new Nan::Callback(info[2].As<Function>()));
            if(info[3]->IsFunction()) self->lengthCbk.reset(new Nan::Callback(info[3].As<Function>()));
            if(info[4]->IsFunction()) self->eofCbk.reset(new Nan::Callback(info[4].As<Function>()));
            if(info[5]->IsFunction()) self->writeCbk.reset(new Nan::Callback(info[5].As<Function>()));
            if(info[6]->IsFunction()) self->metadataCbk.reset(new Nan::Callback(info[6].As<Function>()));
            if(info[7]->IsFunction()) self->errorCbk.reset(new Nan::Callback(info[7].As<Function>()));

            self->async = new Nan::AsyncResource("flac:decoder:initStream");
            int ret = FLAC__stream_decoder_init_stream(dec,
                !self->readCbk ? nullptr : read_callback,
                !self->seekCbk ? nullptr : seek_callback,
                !self->tellCbk ? nullptr : tell_callback,
                !self->lengthCbk ? nullptr : length_callback,
                !self->eofCbk ? nullptr : eof_callback,
                !self->writeCbk ? nullptr : write_callback,
                !self->metadataCbk ? nullptr : metadata_callback,
                !self->errorCbk ? nullptr : error_callback,
                self
            );
            info.GetReturnValue().Set(Nan::New(ret));
            delete self->async;
            self->async = nullptr;
        }

        static NAN_METHOD(node_FLAC__stream_decoder_init_ogg_stream) {
            UNWRAP_FLAC
            CHECK_ASYNC_IS_NULL

            if(info[0]->IsFunction()) self->readCbk.reset(new Nan::Callback(info[0].As<Function>()));
            if(info[1]->IsFunction()) self->seekCbk.reset(new Nan::Callback(info[1].As<Function>()));
            if(info[2]->IsFunction()) self->tellCbk.reset(new Nan::Callback(info[2].As<Function>()));
            if(info[3]->IsFunction()) self->lengthCbk.reset(new Nan::Callback(info[3].As<Function>()));
            if(info[4]->IsFunction()) self->eofCbk.reset(new Nan::Callback(info[4].As<Function>()));
            if(info[5]->IsFunction()) self->writeCbk.reset(new Nan::Callback(info[5].As<Function>()));
            if(info[6]->IsFunction()) self->metadataCbk.reset(new Nan::Callback(info[6].As<Function>()));
            if(info[7]->IsFunction()) self->errorCbk.reset(new Nan::Callback(info[7].As<Function>()));

            self->async = new Nan::AsyncResource("flac:decoder:initOggStream");
            int ret = FLAC__stream_decoder_init_ogg_stream(dec,
                !self->readCbk ? nullptr : read_callback,
                !self->seekCbk ? nullptr : seek_callback,
                !self->tellCbk ? nullptr : tell_callback,
                !self->lengthCbk ? nullptr : length_callback,
                !self->eofCbk ? nullptr : eof_callback,
                !self->writeCbk ? nullptr : write_callback,
                !self->metadataCbk ? nullptr : metadata_callback,
                !self->errorCbk ? nullptr : error_callback,
                self
            );
            info.GetReturnValue().Set(Nan::New(ret));
            delete self->async;
            self->async = nullptr;
        }

        static NAN_METHOD(node_FLAC__stream_decoder_init_file) {
            UNWRAP_FLAC
            CHECK_ASYNC_IS_NULL

            if(!info[0]->IsString()) {
                Nan::ThrowTypeError("Expected first argument to be string");
                return;
            }

            Local<String> fileNameJs = info[0].As<String>();
            if(info[1]->IsFunction()) self->writeCbk.reset(new Nan::Callback(info[1].As<Function>()));
            if(info[2]->IsFunction()) self->metadataCbk.reset(new Nan::Callback(info[2].As<Function>()));
            if(info[3]->IsFunction()) self->errorCbk.reset(new Nan::Callback(info[3].As<Function>()));
            Nan::Utf8String fileName(fileNameJs);

            self->async = new Nan::AsyncResource("flac:decoder:initFile");
            int ret = FLAC__stream_decoder_init_file(dec, *fileName,
                !self->writeCbk ? nullptr : write_callback,
                !self->metadataCbk ? nullptr : metadata_callback,
                !self->errorCbk ? nullptr : error_callback,
                self);
            info.GetReturnValue().Set(Nan::New(ret));
            delete self->async;
            self->async = nullptr;
        }

        static NAN_METHOD(node_FLAC__stream_decoder_init_ogg_file) {
            UNWRAP_FLAC
            CHECK_ASYNC_IS_NULL

            if(!info[0]->IsString()) {
                Nan::ThrowTypeError("Expected first argument to be string");
                return;
            }

            Local<String> fileNameJs = info[0].As<String>();
            if(info[1]->IsFunction()) self->writeCbk.reset(new Nan::Callback(info[1].As<Function>()));
            if(info[2]->IsFunction()) self->metadataCbk.reset(new Nan::Callback(info[2].As<Function>()));
            if(info[3]->IsFunction()) self->errorCbk.reset(new Nan::Callback(info[3].As<Function>()));
            Nan::Utf8String fileName(fileNameJs);

            self->async = new Nan::AsyncResource("flac:decoder:initOggFile");
            int ret = FLAC__stream_decoder_init_ogg_file(dec, *fileName,
                !self->writeCbk ? nullptr : write_callback,
                !self->metadataCbk ? nullptr : metadata_callback,
                !self->errorCbk ? nullptr : error_callback,
                self);
            info.GetReturnValue().Set(Nan::New(ret));
            delete self->async;
            self->async = nullptr;
        }

        static NAN_METHOD(node_FLAC__stream_decoder_finish) {
            UNWRAP_FLAC
            self->async = new Nan::AsyncResource("flac:decoder:finish");
            bool returnValue = FLAC__stream_decoder_finish(dec);
            info.GetReturnValue().Set(Nan::New<Boolean>(returnValue));
            delete self->async;
            self->async = nullptr;
        }

        static NAN_METHOD(node_FLAC__stream_decoder_flush) {
            UNWRAP_FLAC
            CHECK_ASYNC_IS_NULL
            self->async = new Nan::AsyncResource("flac:decoder:flush");
            bool returnValue = FLAC__stream_decoder_flush(dec);
            info.GetReturnValue().Set(Nan::New<Boolean>(returnValue));
            delete self->async;
            self->async = nullptr;
        }

        static NAN_METHOD(node_FLAC__stream_decoder_reset) {
            UNWRAP_FLAC
            CHECK_ASYNC_IS_NULL
            self->async = new Nan::AsyncResource("flac:decoder:initStream");
            bool returnValue = FLAC__stream_decoder_reset(dec);
            info.GetReturnValue().Set(Nan::New<Boolean>(returnValue));
            delete self->async;
            self->async = nullptr;
        }

        static NAN_METHOD(node_FLAC__stream_decoder_process_single) {
            UNWRAP_FLAC
            CHECK_ASYNC_IS_NULL
            self->async = new Nan::AsyncResource("flac:decoder:processSingle");
            bool returnValue = FLAC__stream_decoder_process_single(dec);
            info.GetReturnValue().Set(Nan::New<Boolean>(returnValue));
            delete self->async;
            self->async = nullptr;
        }

        static NAN_METHOD(node_FLAC__stream_decoder_process_until_end_of_metadata) {
            UNWRAP_FLAC
            CHECK_ASYNC_IS_NULL
            self->async = new Nan::AsyncResource("flac:decoder:processUntilEndOfMetadata");
            bool returnValue = FLAC__stream_decoder_process_until_end_of_metadata(dec);
            info.GetReturnValue().Set(Nan::New<Boolean>(returnValue));
            delete self->async;
            self->async = nullptr;
        }

        static NAN_METHOD(node_FLAC__stream_decoder_process_until_end_of_stream) {
            UNWRAP_FLAC
            CHECK_ASYNC_IS_NULL
            self->async = new Nan::AsyncResource("flac:decoder:processUntilEndOfStream");
            bool returnValue = FLAC__stream_decoder_process_until_end_of_stream(dec);
            info.GetReturnValue().Set(Nan::New<Boolean>(returnValue));
            delete self->async;
            self->async = nullptr;
        }

        static NAN_METHOD(node_FLAC__stream_decoder_skip_single_frame) {
            UNWRAP_FLAC
            CHECK_ASYNC_IS_NULL
            self->async = new Nan::AsyncResource("flac:decoder:skipSingleFrame");
            bool returnValue = FLAC__stream_decoder_skip_single_frame(dec);
            info.GetReturnValue().Set(Nan::New<Boolean>(returnValue));
            delete self->async;
            self->async = nullptr;
        }

        static NAN_METHOD(node_FLAC__stream_decoder_seek_absolute) {
            UNWRAP_FLAC
            CHECK_ASYNC_IS_NULL
            auto maybeOffset = numberFromJs<uint64_t>(info[0]);
            if(maybeOffset.IsNothing()) {
                Nan::ThrowTypeError("Expected first argument to be number or bigint");
                return;
            }

            self->async = new Nan::AsyncResource("flac:decoder:seekAbsolute");
            bool returnValue = FLAC__stream_decoder_seek_absolute(dec, maybeOffset.FromJust());
            info.GetReturnValue().Set(Nan::New<Boolean>(returnValue));
            delete self->async;
            self->async = nullptr;
        }

        static NAN_METHOD(node_FLAC__stream_decoder_set_metadata_respond_application) {
            UNWRAP_FLAC
            if(info[0].IsEmpty() || !Buffer::HasInstance(info[0])) {
                Nan::ThrowError("Parameter must be a Buffer");
                return;
            }

            Local<Object> canBeBuffer = info[0].As<Object>();
            FLAC__byte* id = (FLAC__byte*) node::Buffer::Data(canBeBuffer);
            if(node::Buffer::Length(canBeBuffer) < 4) {
                Nan::ThrowError("Buffer must have 4 bytes");
                return;
            }

            FLAC__bool ret = FLAC__stream_decoder_set_metadata_respond_application(dec, id);
            info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        }

        static NAN_METHOD(node_FLAC__stream_decoder_set_metadata_respond_all) {
            UNWRAP_FLAC
            FLAC__bool ret = FLAC__stream_decoder_set_metadata_respond_all(dec);
            info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        }

        static NAN_METHOD(node_FLAC__stream_decoder_set_metadata_ignore_application) {
            UNWRAP_FLAC
            if(info[0].IsEmpty() || !Buffer::HasInstance(info[0])) {
                Nan::ThrowError("Parameter must be a Buffer");
                return;
            }

            Local<Object> canBeBuffer = info[0].As<Object>();
            FLAC__byte* id = (FLAC__byte*) node::Buffer::Data(canBeBuffer);
            if(node::Buffer::Length(canBeBuffer) < 4) {
                Nan::ThrowError("Buffer must have 4 bytes");
                return;
            }

            FLAC__bool ret = FLAC__stream_decoder_set_metadata_ignore_application(dec, id);
            info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        }

        static NAN_METHOD(node_FLAC__stream_decoder_set_metadata_ignore_all) {
            UNWRAP_FLAC
            FLAC__bool ret = FLAC__stream_decoder_set_metadata_ignore_all(dec);
            info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        }

        static NAN_METHOD(node_FLAC__stream_decoder_get_state) {
            UNWRAP_FLAC
            int state = FLAC__stream_decoder_get_state(dec);
            info.GetReturnValue().Set(Nan::New(state));
        }

        static NAN_METHOD(node_FLAC__stream_decoder_get_resolved_state_string) {
            UNWRAP_FLAC
            const char* stateString = FLAC__stream_decoder_get_resolved_state_string(dec);
            info.GetReturnValue().Set(Nan::New(stateString).ToLocalChecked());
        }

        static NAN_METHOD(node_FLAC__stream_decoder_get_decode_position) {
            UNWRAP_FLAC
            CHECK_ASYNC_IS_NULL
            uint64_t pos;
            self->async = new Nan::AsyncResource("flac:decoder:decodePosition");
            FLAC__bool ret = FLAC__stream_decoder_get_decode_position(dec, &pos);
            if(ret) {
                info.GetReturnValue().Set(numberToJs(pos));
            } else {
                info.GetReturnValue().SetNull();
            }
            delete self->async;
            self->async = nullptr;
        }

        static FlacEnumDefineReturnType createStateEnum() {
            Local<Object> obj1 = Nan::New<Object>();
            Local<Object> obj2 = Nan::New<Object>();
            flacEnum_defineValue(obj1, obj2, "SEARCH_FOR_METADATA", 0);
            flacEnum_defineValue(obj1, obj2, "READ_METADATA", 1);
            flacEnum_defineValue(obj1, obj2, "SEARCH_FOR_FRAME_SYNC", 2);
            flacEnum_defineValue(obj1, obj2, "READ_FRAME", 3);
            flacEnum_defineValue(obj1, obj2, "END_OF_STREAM", 4);
            flacEnum_defineValue(obj1, obj2, "OGG_ERROR", 5);
            flacEnum_defineValue(obj1, obj2, "SEEK_ERROR", 6);
            flacEnum_defineValue(obj1, obj2, "DECODER_ABORTED", 7);
            flacEnum_defineValue(obj1, obj2, "MEMORY_ALLOCATION_ERROR", 8);
            flacEnum_defineValue(obj1, obj2, "UNINITIALIZED", 9);
            return std::make_tuple(obj1, obj2);
        }

        static FlacEnumDefineReturnType createInitStatusEnum() {
            Local<Object> obj1 = Nan::New<Object>();
            Local<Object> obj2 = Nan::New<Object>();
            flacEnum_defineValue(obj1, obj2, "OK", 0);
            flacEnum_defineValue(obj1, obj2, "UNSUPPORTED_CONTAINER", 1);
            flacEnum_defineValue(obj1, obj2, "INVALID_CALLBACKS", 2);
            flacEnum_defineValue(obj1, obj2, "MEMORY_ALLOCATION_ERROR", 3);
            flacEnum_defineValue(obj1, obj2, "ERROR_OPENING_FILE", 4);
            flacEnum_defineValue(obj1, obj2, "ALREADY_INITIALIZED", 5);
            return std::make_tuple(obj1, obj2);
        }

        static FlacEnumDefineReturnType createReadStatusEnum() {
            Local<Object> obj1 = Nan::New<Object>();
            Local<Object> obj2 = Nan::New<Object>();
            flacEnum_defineValue(obj1, obj2, "OK", 0);
            flacEnum_defineValue(obj1, obj2, "UNSUPPORTED_CONTAINER", 1);
            flacEnum_defineValue(obj1, obj2, "INVALID_CALLBACKS", 2);
            flacEnum_defineValue(obj1, obj2, "MEMORY_ALLOCATION_ERROR", 3);
            flacEnum_defineValue(obj1, obj2, "ERROR_OPENING_FILE", 4);
            flacEnum_defineValue(obj1, obj2, "ALREADY_INITIALIZED", 5);
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

        static FlacEnumDefineReturnType createLengthStatusEnum() {
            Local<Object> obj1 = Nan::New<Object>();
            Local<Object> obj2 = Nan::New<Object>();
            flacEnum_defineValue(obj1, obj2, "OK", 0);
            flacEnum_defineValue(obj1, obj2, "ERROR", 1);
            flacEnum_defineValue(obj1, obj2, "UNSUPPORTED", 2);
            return std::make_tuple(obj1, obj2);
        }

        static FlacEnumDefineReturnType createWriteStatusEnum() {
            Local<Object> obj1 = Nan::New<Object>();
            Local<Object> obj2 = Nan::New<Object>();
            flacEnum_defineValue(obj1, obj2, "CONTINUE", 0);
            flacEnum_defineValue(obj1, obj2, "ABORT", 1);
            flacEnum_defineValue(obj1, obj2, "UNSUPPORTED", 2);
            return std::make_tuple(obj1, obj2);
        }

        static FlacEnumDefineReturnType createErrorStatusEnum() {
            Local<Object> obj1 = Nan::New<Object>();
            Local<Object> obj2 = Nan::New<Object>();
            flacEnum_defineValue(obj1, obj2, "LOST_SYNC", 0);
            flacEnum_defineValue(obj1, obj2, "BAD_HEADER", 1);
            flacEnum_defineValue(obj1, obj2, "FRAME_CRC_MISMATCH", 2);
            flacEnum_defineValue(obj1, obj2, "UNPARSEABLE_STREAM", 3);
            return std::make_tuple(obj1, obj2);
        }

    public:

        static NAN_MODULE_INIT(initDecoder);

    };

    StreamDecoder::~StreamDecoder() {
        FLAC__stream_decoder_delete(dec);
    }

    NAN_MODULE_INIT(StreamDecoder::initDecoder) {
        Local<FunctionTemplate> obj = Nan::New<FunctionTemplate>(node_FLAC__stream_decoder_new);
        obj->SetClassName(Nan::New("StreamDecoder").ToLocalChecked());
        obj->InstanceTemplate()->SetInternalFieldCount(1);

        #define loadFunction(fn) \
        _JOIN(FLAC__stream_decoder_, fn) = libFlac->getSymbolAddress<_JOIN2(FLAC__stream_decoder_, fn, _t)>("FLAC__stream_decoder_" #fn); \
        if(_JOIN(FLAC__stream_decoder_, fn) == nullptr) printf("%s\n", libFlac->getLastError().c_str());

        #define setMethod(fn, jsFn) \
        Nan::SetPrototypeMethod(obj, #jsFn, _JOIN(node_FLAC__stream_decoder_, fn)); \
        loadFunction(fn)

        loadFunction(new);
        loadFunction(delete);
        setMethod(set_ogg_serial_number, setOggSerialNumber);
        setMethod(set_md5_checking, setMd5Checking);
        setMethod(set_metadata_respond, setMetadataRespond);
        setMethod(set_metadata_respond_application, setMetadataRespondApplication);
        setMethod(set_metadata_respond_all, setMetadataRespondAll);
        setMethod(set_metadata_ignore, setMetadataIgnore);
        setMethod(set_metadata_ignore_application, setMetadataIgnoreApplication);
        setMethod(set_metadata_ignore_all, setMetadataIgnoreAll);
        setMethod(get_state, getState);
        setMethod(get_resolved_state_string, getResolvedStateString);
        setMethod(get_md5_checking, getMd5Checking);
        setMethod(get_total_samples, getTotalSamples);
        setMethod(get_channels, getChannels);
        setMethod(get_channel_assignment, getChannelAssignment);
        setMethod(get_bits_per_sample, getBitsPerSample);
        setMethod(get_sample_rate, getSampleRate);
        setMethod(get_blocksize, getBlocksize);
        setMethod(get_decode_position, getDecodePosition);
        setMethod(init_stream, initStream);
        setMethod(init_ogg_stream, initOggStream);
        setMethod(init_file, initFile);
        setMethod(init_ogg_file, initOggFile);
        setMethod(finish, finish);
        setMethod(flush, flush);
        setMethod(reset, reset);
        setMethod(process_single, processSingle);
        setMethod(process_until_end_of_stream, processUntilEndOfStream);
        setMethod(process_until_end_of_metadata, processUntilEndOfMetadata);
        setMethod(skip_single_frame, skipSingleFrame);
        setMethod(seek_absolute, seekAbsolute);

        Local<Function> functionClass = Nan::GetFunction(obj).ToLocalChecked();

        flacEnum_declareInObject(functionClass, State, createStateEnum());
        flacEnum_declareInObject(functionClass, InitStatus, createInitStatusEnum());
        flacEnum_declareInObject(functionClass, ReadStatus, createReadStatusEnum());
        flacEnum_declareInObject(functionClass, SeekStatus, createSeekStatusEnum());
        flacEnum_declareInObject(functionClass, TellStatus, createTellStatusEnum());
        flacEnum_declareInObject(functionClass, LengthStatus, createLengthStatusEnum());
        flacEnum_declareInObject(functionClass, WriteStatus, createWriteStatusEnum());
        flacEnum_declareInObject(functionClass, ErrorStatus, createErrorStatusEnum());

        Nan::Set(target, Nan::New("Decoder").ToLocalChecked(), functionClass);
    }

}


static int read_callback(const FLAC__StreamDecoder* dec, FLAC__byte buffer[], size_t* bytes, void* data) {
    Nan::HandleScope scope;
    flac_bindings::StreamDecoder* cbks = (flac_bindings::StreamDecoder*) data;
    Handle<Value> args[] {
        WrapPointer(buffer, *bytes).ToLocalChecked()
    };

    Nan::TryCatch tc;
    tc.SetVerbose(true);
    auto ret = (*cbks->readCbk)(cbks->async, 1, args);
    if(tc.HasCaught()) {
        tc.ReThrow();
        return 1;
    }
    if(!ret.IsEmpty() && ret.ToLocalChecked()->IsObject()) {
        Local<Object> retJust = ret.ToLocalChecked().As<Object>();
        Local<Value> bytes2 = Nan::Get(retJust, Nan::New("bytes").ToLocalChecked()).ToLocalChecked();
        Local<Value> returnValue = Nan::Get(retJust, Nan::New("returnValue").ToLocalChecked()).ToLocalChecked();
        auto maybeBytes2 = numberFromJs<uint64_t>(bytes2);
        if(maybeBytes2.IsNothing()) {
            Nan::ThrowTypeError("Expected bytes to be number or bigint");
            return 1;
        }

        *bytes = maybeBytes2.FromJust();
        return numberFromJs<int32_t>(returnValue).FromMaybe(0);
    } else {
        Nan::ThrowError("Read callback did not return an object");
        printf("readCbk returned emtpy, to avoid errors will return END_OF_STREAM\n");
        return 1;
    }
}

static int seek_callback(const FLAC__StreamDecoder* dec, uint64_t offset, void* data) {
    Nan::HandleScope scope;
    flac_bindings::StreamDecoder* cbks = (flac_bindings::StreamDecoder*) data;
    Handle<Value> args[] {
        numberToJs(offset)
    };

    Nan::TryCatch tc;
    tc.SetVerbose(true);
    auto ret = (*cbks->seekCbk)(cbks->async, 1, args);
    if(tc.HasCaught()) {
        tc.ReThrow();
        return 1;
    }
    if(ret.IsEmpty()) {
        return 1;
    } else {
        return numberFromJs<int32_t>(ret.ToLocalChecked()).FromMaybe(0);
    }
}

static int tell_callback(const FLAC__StreamDecoder* dec, uint64_t* offset, void* data) {
    Nan::HandleScope scope;
    flac_bindings::StreamDecoder* cbks = (flac_bindings::StreamDecoder*) data;
    Handle<Value> args[] { numberToJs(*offset) };

    Nan::TryCatch tc;
    tc.SetVerbose(true);
    auto ret = (*cbks->tellCbk)(cbks->async, 1, args);
    if(tc.HasCaught()) {
        tc.ReThrow();
        return 1;
    }
    if(!ret.IsEmpty() && ret.ToLocalChecked()->IsObject()) {
        Local<Object> retJust = ret.ToLocalChecked().As<Object>();
        Local<Value> offset2 = Nan::Get(retJust, Nan::New("offset").ToLocalChecked()).ToLocalChecked();
        Local<Value> returnValue = Nan::Get(retJust, Nan::New("returnValue").ToLocalChecked()).ToLocalChecked();
        *offset = numberFromJs<uint64_t>(offset2).FromJust();
        return numberFromJs<int32_t>(returnValue).FromMaybe(0);
    } else {
        Nan::ThrowError("Tell callback did not return an object");
        printf("tellCallback returned empty, to avoid errors will return ERROR\n");
        *offset = 0;
        return 1;
    }
}

static int length_callback(const FLAC__StreamDecoder* dec, uint64_t* length, void* data) {
    Nan::HandleScope scope;
    flac_bindings::StreamDecoder* cbks = (flac_bindings::StreamDecoder*) data;
    Handle<Value> args[] { numberToJs(*length) };

    Nan::TryCatch tc;
    tc.SetVerbose(true);
    auto ret = (*cbks->lengthCbk)(cbks->async, 1, args);
    if(tc.HasCaught()) {
        tc.ReThrow();
        return 1;
    }
    if(!ret.IsEmpty() && ret.ToLocalChecked()->IsObject()) {
        Local<Object> retJust = ret.ToLocalChecked().As<Object>();
        Local<Value> length2 = Nan::Get(retJust, Nan::New("length").ToLocalChecked()).ToLocalChecked();
        Local<Value> returnValue = Nan::Get(retJust, Nan::New("returnValue").ToLocalChecked()).ToLocalChecked();
        auto maybeLength2 = numberFromJs<uint64_t>(length2);
        if(maybeLength2.IsNothing()) {
            Nan::ThrowTypeError("Expected length to be number or bigint");
            return 1;
        }

        *length = maybeLength2.FromJust();
        return numberFromJs<int32_t>(returnValue).FromMaybe(0);
    } else {
        Nan::ThrowError("Length callback did not return an object");
        printf("lengthCbk returned empty, to avoid further issues will return ERROR\n");
        *length = 0;
        return 1;
    }
}

static FLAC__bool eof_callback(const FLAC__StreamDecoder* dec, void* data) {
    Nan::HandleScope scope;
    flac_bindings::StreamDecoder* cbks = (flac_bindings::StreamDecoder*) data;
    Handle<Value> args[] { Nan::Null() };

    Nan::TryCatch tc;
    tc.SetVerbose(true);
    auto ret = (*cbks->eofCbk)(cbks->async, 0, args);
    if(tc.HasCaught()) {
        tc.ReThrow();
        return false;
    }
    if(ret.IsEmpty()) {
        return false;
    } else {
        return Nan::To<bool>(ret.ToLocalChecked()).FromMaybe(false);
    }
}

static int write_callback(const FLAC__StreamDecoder* dec, const FLAC__Frame* frame, const int32_t *const buffer[], void* data) {
    Nan::HandleScope scope;
    flac_bindings::StreamDecoder* cbks = (flac_bindings::StreamDecoder*) data;
    Local<Array> buffers = Nan::New<Array>();
    unsigned channels = FLAC__stream_decoder_get_channels(dec);
    for(uint32_t i = 0; i < channels; i++)
        Nan::Set(buffers, i, WrapPointer(buffer[i], frame->header.blocksize * sizeof(int32_t)).ToLocalChecked());

    Handle<Value> args[] {
        flac_bindings::structToJs(frame),
        buffers
    };

    Nan::TryCatch tc;
    tc.SetVerbose(true);
    auto ret = (*cbks->writeCbk)(cbks->async, 2, args);
    if(tc.HasCaught()) {
        tc.ReThrow();
        return 2;
    }
    if(ret.IsEmpty()) {
        return 2;
    } else {
        return numberFromJs<int32_t>(ret.ToLocalChecked()).FromMaybe(0);
    }
}

static void metadata_callback(const FLAC__StreamDecoder* dec, const FLAC__StreamMetadata* metadata, void* data) {
    Nan::HandleScope scope;
    flac_bindings::StreamDecoder* cbks = (flac_bindings::StreamDecoder*) data;
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

static void error_callback(const FLAC__StreamDecoder* dec, int error, void* data) {
    Nan::HandleScope scope;
    flac_bindings::StreamDecoder* cbks = (flac_bindings::StreamDecoder*) data;
    Handle<Value> args[] = {
        Nan::New(error)
    };

    Nan::TryCatch tc;
    tc.SetVerbose(true);
    (*cbks->errorCbk)(cbks->async, 1, args);
    if(tc.HasCaught()) {
        tc.ReThrow();
    }
}
