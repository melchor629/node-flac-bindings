#include <vector>
#include <nan.h>
#include "dl.hpp"

using namespace v8;
using namespace node;
#include "pointer.hpp"
#include "format.h"

#define _JOIN(a, b) a##b
#define _JOIN2(a,b,c) a##b##c

struct flac_decoding_callbacks {
    Nan::Persistent<Function> readCbk, seekCbk, tellCbk, lengthCbk, eofCbk, writeCbk, metadataCbk, errorCbk;
    Nan::AsyncResource async;

    flac_decoding_callbacks(): async("flac:decoding") {}
};

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
    if(info[0]->IsUndefined() || info[0]->IsNull()) Nan::ThrowError("Calling FLAC function without the decoder object"); \
    FLAC__StreamDecoder* dec = UnwrapPointer<FLAC__StreamDecoder>(info[0]);

#define FLAC_FUNC(returnType, fn, ...) \
    typedef returnType (*_JOIN2(FLAC__stream_decoder_, fn, _t))(__VA_ARGS__); \
    static _JOIN2(FLAC__stream_decoder_, fn, _t) _JOIN(FLAC__stream_decoder_, fn);

#define FLAC_GETTER(type, v8Type, fn) \
extern "C" { \
    FLAC_FUNC(type, _JOIN(get_, fn), const FLAC__StreamDecoder*); \
} \
NAN_METHOD(_JOIN(node_FLAC__stream_decoder_get_, fn)) { \
    UNWRAP_FLAC \
    type output = _JOIN(FLAC__stream_decoder_get_, fn)(dec); \
    info.GetReturnValue().Set(Nan::New<v8Type>(output)); \
}

#define FLAC_SETTER(type, v8Type, fn) \
extern "C" { \
    FLAC_FUNC(FLAC__bool, _JOIN(set_, fn), FLAC__StreamDecoder*, type) \
} \
NAN_METHOD(_JOIN(node_FLAC__stream_decoder_set_, fn)) { \
    UNWRAP_FLAC \
    type input = (type) info[1]->_JOIN(v8Type, Value)(); \
    FLAC__bool output = _JOIN(FLAC__stream_decoder_set_, fn)(dec, input); \
    info.GetReturnValue().Set(Nan::New(bool(output))); \
}

#define FLAC_GETTER_SETTER(type, v8Type, fn) \
FLAC_GETTER(type, v8Type, fn); \
FLAC_SETTER(type, v8Type, fn);

FLAC_SETTER(long, Number, ogg_serial_number);
FLAC_GETTER_SETTER(FLAC__bool, Boolean, md5_checking);
#ifndef _MSC_VER
FLAC_SETTER(FLAC__MetadataType, Number, metadata_respond);
FLAC_SETTER(FLAC__MetadataType, Number, metadata_ignore);
#else
FLAC_SETTER(int, Number, metadata_respond);
FLAC_SETTER(int, Number, metadata_ignore);
#endif
FLAC_GETTER(uint64_t, Number, total_samples);
FLAC_GETTER(unsigned, Number, channels);
FLAC_GETTER(FLAC__ChannelAssignment, Number, channel_assignment);
FLAC_GETTER(unsigned, Number, bits_per_sample);
FLAC_GETTER(unsigned, Number, sample_rate);
FLAC_GETTER(unsigned, Number, blocksize);

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

    static const char* const* FLAC__StreamDecoderStateString;
    static const char* const* FLAC__StreamDecoderInitStatusString;
    static const char* const* FLAC__StreamDecoderReadStatusString;
    static const char* const* FLAC__StreamDecoderSeekStatusString;
    static const char* const* FLAC__StreamDecoderTellStatusString;
    static const char* const* FLAC__StreamDecoderLengthStatusString;
    static const char* const* FLAC__StreamDecoderWriteStatusString;
    static const char* const* FLAC__StreamDecoderErrorStatusString;
}

namespace flac_bindings {

    extern Library* libFlac;
    static std::vector<flac_decoding_callbacks*> objectsToDelete;

    NAN_METHOD(node_FLAC__stream_decoder_new) {
        FLAC__StreamDecoder* dec = FLAC__stream_decoder_new();
        if(dec != nullptr) {
            info.GetReturnValue().Set(WrapPointer(dec).ToLocalChecked());
        } else {
            info.GetReturnValue().SetNull();
        }
    }

    NAN_METHOD(node_FLAC__stream_decoder_delete) {
        UNWRAP_FLAC
        FLAC__stream_decoder_delete(dec);
    }

    NAN_METHOD(node_FLAC__stream_decoder_init_stream) {
        UNWRAP_FLAC

        flac_decoding_callbacks* cbks = new flac_decoding_callbacks;
        if(info[1]->IsFunction()) cbks->readCbk.Reset(info[1].As<Function>());
        if(info[2]->IsFunction()) cbks->seekCbk.Reset(info[2].As<Function>());
        if(info[3]->IsFunction()) cbks->tellCbk.Reset(info[3].As<Function>());
        if(info[4]->IsFunction()) cbks->lengthCbk.Reset(info[4].As<Function>());
        if(info[5]->IsFunction()) cbks->eofCbk.Reset(info[5].As<Function>());
        if(info[6]->IsFunction()) cbks->writeCbk.Reset(info[6].As<Function>());
        if(info[7]->IsFunction()) cbks->metadataCbk.Reset(info[7].As<Function>());
        if(info[8]->IsFunction()) cbks->errorCbk.Reset(info[8].As<Function>());

        int ret = FLAC__stream_decoder_init_stream(dec,
            cbks->readCbk.IsEmpty() ? nullptr : read_callback,
            cbks->seekCbk.IsEmpty() ? nullptr : seek_callback,
            cbks->tellCbk.IsEmpty() ? nullptr : tell_callback,
            cbks->lengthCbk.IsEmpty() ? nullptr : length_callback,
            cbks->eofCbk.IsEmpty() ? nullptr : eof_callback,
            cbks->writeCbk.IsEmpty() ? nullptr : write_callback,
            cbks->metadataCbk.IsEmpty() ? nullptr : metadata_callback,
            cbks->errorCbk.IsEmpty() ? nullptr : error_callback,
            cbks
        );
        info.GetReturnValue().Set(Nan::New(ret));
        objectsToDelete.push_back(cbks);
    }

    NAN_METHOD(node_FLAC__stream_decoder_init_ogg_stream) {
        UNWRAP_FLAC

        flac_decoding_callbacks* cbks = new flac_decoding_callbacks;
        if(info[1]->IsFunction()) cbks->readCbk.Reset(info[1].As<Function>());
        if(info[2]->IsFunction()) cbks->seekCbk.Reset(info[2].As<Function>());
        if(info[3]->IsFunction()) cbks->tellCbk.Reset(info[3].As<Function>());
        if(info[4]->IsFunction()) cbks->lengthCbk.Reset(info[4].As<Function>());
        if(info[5]->IsFunction()) cbks->eofCbk.Reset(info[5].As<Function>());
        if(info[6]->IsFunction()) cbks->writeCbk.Reset(info[6].As<Function>());
        if(info[7]->IsFunction()) cbks->metadataCbk.Reset(info[7].As<Function>());
        if(info[8]->IsFunction()) cbks->errorCbk.Reset(info[8].As<Function>());

        int ret = FLAC__stream_decoder_init_ogg_stream(dec,
            cbks->readCbk.IsEmpty() ? nullptr : read_callback,
            cbks->seekCbk.IsEmpty() ? nullptr : seek_callback,
            cbks->tellCbk.IsEmpty() ? nullptr : tell_callback,
            cbks->lengthCbk.IsEmpty() ? nullptr : length_callback,
            cbks->eofCbk.IsEmpty() ? nullptr : eof_callback,
            cbks->writeCbk.IsEmpty() ? nullptr : write_callback,
            cbks->metadataCbk.IsEmpty() ? nullptr : metadata_callback,
            cbks->errorCbk.IsEmpty() ? nullptr : error_callback,
            cbks
        );
        info.GetReturnValue().Set(Nan::New(ret));
        objectsToDelete.push_back(cbks);
    }

    NAN_METHOD(node_FLAC__stream_decoder_init_file) {
        UNWRAP_FLAC

        flac_decoding_callbacks* cbks = new flac_decoding_callbacks;
        Local<String> fileNameJs = info[1].As<String>();
        if(info[2]->IsFunction()) cbks->writeCbk.Reset(info[2].As<Function>());
        if(info[3]->IsFunction()) cbks->metadataCbk.Reset(info[3].As<Function>());
        if(info[4]->IsFunction()) cbks->errorCbk.Reset(info[4].As<Function>());
        Nan::Utf8String fileName(fileNameJs);

        int ret = FLAC__stream_decoder_init_file(dec, *fileName,
            cbks->writeCbk.IsEmpty() ? nullptr : write_callback,
            cbks->metadataCbk.IsEmpty() ? nullptr : metadata_callback,
            cbks->errorCbk.IsEmpty() ? nullptr : error_callback,
            cbks);
        info.GetReturnValue().Set(Nan::New(ret));
        objectsToDelete.push_back(cbks);
    }

    NAN_METHOD(node_FLAC__stream_decoder_init_ogg_file) {
        UNWRAP_FLAC

        flac_decoding_callbacks* cbks = new flac_decoding_callbacks;
        Local<String> fileNameJs = info[1].As<String>();
        if(info[2]->IsFunction()) cbks->writeCbk.Reset(info[2].As<Function>());
        if(info[3]->IsFunction()) cbks->metadataCbk.Reset(info[3].As<Function>());
        if(info[4]->IsFunction()) cbks->errorCbk.Reset(info[4].As<Function>());
        Nan::Utf8String fileName(fileNameJs);

        int ret = FLAC__stream_decoder_init_ogg_file(dec, *fileName,
            cbks->writeCbk.IsEmpty() ? nullptr : write_callback,
            cbks->metadataCbk.IsEmpty() ? nullptr : metadata_callback,
            cbks->errorCbk.IsEmpty() ? nullptr : error_callback,
            cbks);
        info.GetReturnValue().Set(Nan::New(ret));
        objectsToDelete.push_back(cbks);
    }

    NAN_METHOD(node_FLAC__stream_decoder_finish) {
        UNWRAP_FLAC
        bool returnValue = FLAC__stream_decoder_finish(dec);
        info.GetReturnValue().Set(Nan::New<Boolean>(returnValue));
    }

    NAN_METHOD(node_FLAC__stream_decoder_flush) {
        UNWRAP_FLAC
        bool returnValue = FLAC__stream_decoder_flush(dec);
        info.GetReturnValue().Set(Nan::New<Boolean>(returnValue));
    }

    NAN_METHOD(node_FLAC__stream_decoder_reset) {
        UNWRAP_FLAC
        bool returnValue = FLAC__stream_decoder_reset(dec);
        info.GetReturnValue().Set(Nan::New<Boolean>(returnValue));
    }

    NAN_METHOD(node_FLAC__stream_decoder_process_single) {
        UNWRAP_FLAC
        bool returnValue = FLAC__stream_decoder_process_single(dec);
        info.GetReturnValue().Set(Nan::New<Boolean>(returnValue));
    }

    NAN_METHOD(node_FLAC__stream_decoder_process_until_end_of_metadata) {
        UNWRAP_FLAC
        bool returnValue = FLAC__stream_decoder_process_until_end_of_metadata(dec);
        info.GetReturnValue().Set(Nan::New<Boolean>(returnValue));
    }

    NAN_METHOD(node_FLAC__stream_decoder_process_until_end_of_stream) {
        UNWRAP_FLAC
        bool returnValue = FLAC__stream_decoder_process_until_end_of_stream(dec);
        info.GetReturnValue().Set(Nan::New<Boolean>(returnValue));
    }

    NAN_METHOD(node_FLAC__stream_decoder_skip_single_frame) {
        UNWRAP_FLAC
        bool returnValue = FLAC__stream_decoder_skip_single_frame(dec);
        info.GetReturnValue().Set(Nan::New<Boolean>(returnValue));
    }

    NAN_METHOD(node_FLAC__stream_decoder_seek_absolute) {
        UNWRAP_FLAC
        Local<Number> number = info[1].As<Number>();
        bool returnValue = FLAC__stream_decoder_seek_absolute(dec, Nan::To<int64_t>(number).FromJust());
        info.GetReturnValue().Set(Nan::New<Boolean>(returnValue));
    }

    NAN_METHOD(node_FLAC__stream_decoder_set_metadata_respond_application) {
        UNWRAP_FLAC
        Local<Array> idArray = info[1].As<Array>();
        FLAC__byte id[] = {
            (FLAC__byte) Nan::To<uint32_t>(Nan::Get(idArray, 0).ToLocalChecked()).FromJust(),
            (FLAC__byte) Nan::To<uint32_t>(Nan::Get(idArray, 1).ToLocalChecked()).FromJust(),
            (FLAC__byte) Nan::To<uint32_t>(Nan::Get(idArray, 2).ToLocalChecked()).FromJust(),
            (FLAC__byte) Nan::To<uint32_t>(Nan::Get(idArray, 3).ToLocalChecked()).FromJust(),
        };

        FLAC__bool ret = FLAC__stream_decoder_set_metadata_respond_application(dec, id);
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
    }

    NAN_METHOD(node_FLAC__stream_decoder_set_metadata_respond_all) {
        UNWRAP_FLAC
        FLAC__bool ret = FLAC__stream_decoder_set_metadata_respond_all(dec);
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
    }

    NAN_METHOD(node_FLAC__stream_decoder_set_metadata_ignore_application) {
        UNWRAP_FLAC
        Local<Array> idArray = info[1].As<Array>();
        FLAC__byte id[] = {
            (FLAC__byte) Nan::To<uint32_t>(Nan::Get(idArray, 0).ToLocalChecked()).FromJust(),
            (FLAC__byte) Nan::To<uint32_t>(Nan::Get(idArray, 1).ToLocalChecked()).FromJust(),
            (FLAC__byte) Nan::To<uint32_t>(Nan::Get(idArray, 2).ToLocalChecked()).FromJust(),
            (FLAC__byte) Nan::To<uint32_t>(Nan::Get(idArray, 3).ToLocalChecked()).FromJust(),
        };

        FLAC__bool ret = FLAC__stream_decoder_set_metadata_ignore_application(dec, id);
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
    }

    NAN_METHOD(node_FLAC__stream_decoder_set_metadata_ignore_all) {
        UNWRAP_FLAC
        FLAC__bool ret = FLAC__stream_decoder_set_metadata_ignore_all(dec);
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
    }

    NAN_METHOD(node_FLAC__stream_decoder_get_state) {
        UNWRAP_FLAC
        int state = FLAC__stream_decoder_get_state(dec);
        info.GetReturnValue().Set(Nan::New(state));
    }

    NAN_METHOD(node_FLAC__stream_decoder_get_resolved_state_string) {
        UNWRAP_FLAC
        const char* stateString = FLAC__stream_decoder_get_resolved_state_string(dec);
        info.GetReturnValue().Set(Nan::New(stateString).ToLocalChecked());
    }

    NAN_METHOD(node_FLAC__stream_decoder_get_decode_position) {
        UNWRAP_FLAC
        uint64_t pos;
        FLAC__bool ret = FLAC__stream_decoder_get_decode_position(dec, &pos);
        if(ret) {
            info.GetReturnValue().Set(Nan::New<Number>(pos));
        } else {
            info.GetReturnValue().SetNull();
        }
    }

    NAN_PROPERTY_GETTER(DecState) {
        Nan::Utf8String propertyName(property);
        std::string PropertyName(*propertyName);

        if(PropertyName == "SEARCH_FOR_METADATA") info.GetReturnValue().Set(0);
        else if(PropertyName == "READ_METADATA") info.GetReturnValue().Set(1);
        else if(PropertyName == "SEARCH_FOR_FRAME_SYNC") info.GetReturnValue().Set(2);
        else if(PropertyName == "READ_FRAME") info.GetReturnValue().Set(3);
        else if(PropertyName == "END_OF_STREAM") info.GetReturnValue().Set(4);
        else if(PropertyName == "OGG_ERROR") info.GetReturnValue().Set(5);
        else if(PropertyName == "SEEK_ERROR") info.GetReturnValue().Set(6);
        else if(PropertyName == "DECODER_ABORTED") info.GetReturnValue().Set(7);
        else if(PropertyName == "MEMORY_ALLOCATION_ERROR") info.GetReturnValue().Set(8);
        else if(PropertyName == "UNINITIALIZED") info.GetReturnValue().Set(9);
        else info.GetReturnValue().SetUndefined();
    }

    NAN_PROPERTY_GETTER(DecInitStatus) {
        Nan::Utf8String propertyName(property);
        std::string PropertyName(*propertyName);

        if(PropertyName == "OK") info.GetReturnValue().Set(0);
        else if(PropertyName == "UNSUPPORTED_CONTAINER") info.GetReturnValue().Set(1);
        else if(PropertyName == "INVALID_CALLBACKS") info.GetReturnValue().Set(2);
        else if(PropertyName == "MEMORY_ALLOCATION_ERROR") info.GetReturnValue().Set(3);
        else if(PropertyName == "ERROR_OPENING_FILE") info.GetReturnValue().Set(4);
        else if(PropertyName == "AKREADY_INITIALIZED") info.GetReturnValue().Set(5);
        else info.GetReturnValue().SetUndefined();
    }

    NAN_PROPERTY_GETTER(DecReadStatus) {
        Nan::Utf8String propertyName(property);
        std::string PropertyName(*propertyName);

        if(PropertyName == "CONTINUE") info.GetReturnValue().Set(0);
        else if(PropertyName == "END_OF_STREAM") info.GetReturnValue().Set(1);
        else if(PropertyName == "ABORT") info.GetReturnValue().Set(2);
        else info.GetReturnValue().SetUndefined();
    }

    NAN_PROPERTY_GETTER(DecSeekStatus) {
        Nan::Utf8String propertyName(property);
        std::string PropertyName(*propertyName);

        if(PropertyName == "OK") info.GetReturnValue().Set(0);
        else if(PropertyName == "ERROR") info.GetReturnValue().Set(1);
        else if(PropertyName == "UNSUPPORTED") info.GetReturnValue().Set(2);
        else info.GetReturnValue().SetUndefined();
    }

    NAN_PROPERTY_GETTER(DecTellStatus) {
        Nan::Utf8String propertyName(property);
        std::string PropertyName(*propertyName);

        if(PropertyName == "OK") info.GetReturnValue().Set(0);
        else if(PropertyName == "ERROR") info.GetReturnValue().Set(1);
        else if(PropertyName == "UNSUPPORTED") info.GetReturnValue().Set(2);
        else info.GetReturnValue().SetUndefined();
    }

    NAN_PROPERTY_GETTER(DecLengthStatus) {
        Nan::Utf8String propertyName(property);
        std::string PropertyName(*propertyName);

        if(PropertyName == "OK") info.GetReturnValue().Set(0);
        else if(PropertyName == "ERROR") info.GetReturnValue().Set(1);
        else if(PropertyName == "UNSUPPORTED") info.GetReturnValue().Set(2);
        else info.GetReturnValue().SetUndefined();
    }

    NAN_PROPERTY_GETTER(DecWriteStatus) {
        Nan::Utf8String propertyName(property);
        std::string PropertyName(*propertyName);

        if(PropertyName == "CONTINUE") info.GetReturnValue().Set(0);
        else if(PropertyName == "ABORT") info.GetReturnValue().Set(1);
        else info.GetReturnValue().SetUndefined();
    }

    NAN_PROPERTY_GETTER(DecErrorStatus) {
        Nan::Utf8String propertyName(property);
        std::string PropertyName(*propertyName);

        if(PropertyName == "LOST_SYNC") info.GetReturnValue().Set(0);
        else if(PropertyName == "BAD_HEADER") info.GetReturnValue().Set(1);
        else if(PropertyName == "FRAME_CRC_MISMATCH") info.GetReturnValue().Set(2);
        else if(PropertyName == "UNPARSEABLE_STREAM") info.GetReturnValue().Set(3);
        else info.GetReturnValue().SetUndefined();
    }

    NAN_INDEX_GETTER(node_FLAC__StreamDecoderStateString) {
        if(index < 10) {
            info.GetReturnValue().Set(Nan::New(FLAC__StreamDecoderStateString[index]).ToLocalChecked());
        } else {
            info.GetReturnValue().SetNull();
        }
    }

    NAN_INDEX_ENUMERATOR(node_FLAC__StreamDecoderStateString) {
        Local<Array> array = Nan::New<Array>();
        for(int i = 0; i < 10; i++) Nan::Set(array, i, Nan::New(i));
        info.GetReturnValue().Set(array);
    }

    NAN_INDEX_GETTER(node_FLAC__StreamDecoderInitStatusString) {
        if(index < 6) {
            info.GetReturnValue().Set(Nan::New(FLAC__StreamDecoderInitStatusString[index]).ToLocalChecked());
        } else {
            info.GetReturnValue().SetNull();
        }
    }

    NAN_INDEX_ENUMERATOR(node_FLAC__StreamDecoderInitStatusString) {
        Local<Array> array = Nan::New<Array>();
        for(int i = 0; i < 6; i++) Nan::Set(array, i, Nan::New(i));
        info.GetReturnValue().Set(array);
    }

    NAN_INDEX_GETTER(node_FLAC__StreamDecoderReadStatusString) {
        if(index < 3) {
            info.GetReturnValue().Set(Nan::New(FLAC__StreamDecoderReadStatusString[index]).ToLocalChecked());
        } else {
            info.GetReturnValue().SetNull();
        }
    }

    NAN_INDEX_ENUMERATOR(node_FLAC__StreamDecoderReadStatusString) {
        Local<Array> array = Nan::New<Array>();
        for(int i = 0; i < 3; i++) Nan::Set(array, i, Nan::New(i));
        info.GetReturnValue().Set(array);
    }

    NAN_INDEX_GETTER(node_FLAC__StreamDecoderSeekStatusString) {
        if(index < 3) {
            info.GetReturnValue().Set(Nan::New(FLAC__StreamDecoderSeekStatusString[index]).ToLocalChecked());
        } else {
            info.GetReturnValue().SetNull();
        }
    }

    NAN_INDEX_ENUMERATOR(node_FLAC__StreamDecoderSeekStatusString) {
        Local<Array> array = Nan::New<Array>();
        for(int i = 0; i < 3; i++) Nan::Set(array, i, Nan::New(i));
        info.GetReturnValue().Set(array);
    }

    NAN_INDEX_GETTER(node_FLAC__StreamDecoderTellStatusString) {
        if(index < 3) {
            info.GetReturnValue().Set(Nan::New(FLAC__StreamDecoderTellStatusString[index]).ToLocalChecked());
        } else {
            info.GetReturnValue().SetNull();
        }
    }

    NAN_INDEX_ENUMERATOR(node_FLAC__StreamDecoderTellStatusString) {
        Local<Array> array = Nan::New<Array>();
        for(int i = 0; i < 3; i++) Nan::Set(array, i, Nan::New(i));
        info.GetReturnValue().Set(array);
    }

    NAN_INDEX_GETTER(node_FLAC__StreamDecoderLengthStatusString) {
        if(index < 3) {
            info.GetReturnValue().Set(Nan::New(FLAC__StreamDecoderLengthStatusString[index]).ToLocalChecked());
        } else {
            info.GetReturnValue().SetNull();
        }
    }

    NAN_INDEX_ENUMERATOR(node_FLAC__StreamDecoderLengthStatusString) {
        Local<Array> array = Nan::New<Array>();
        for(int i = 0; i < 3; i++) Nan::Set(array, i, Nan::New(i));
        info.GetReturnValue().Set(array);
    }

    NAN_INDEX_GETTER(node_FLAC__StreamDecoderWriteStatusString) {
        if(index < 2) {
            info.GetReturnValue().Set(Nan::New(FLAC__StreamDecoderWriteStatusString[index]).ToLocalChecked());
        } else {
            info.GetReturnValue().SetNull();
        }
    }

    NAN_INDEX_ENUMERATOR(node_FLAC__StreamDecoderWriteStatusString) {
        Local<Array> array = Nan::New<Array>();
        for(int i = 0; i < 2; i++) Nan::Set(array, i, Nan::New(i));
        info.GetReturnValue().Set(array);
    }

    NAN_INDEX_GETTER(node_FLAC__StreamDecoderErrorStatusString) {
        if(index < 4) {
            info.GetReturnValue().Set(Nan::New(FLAC__StreamDecoderErrorStatusString[index]).ToLocalChecked());
        } else {
            info.GetReturnValue().SetNull();
        }
    }

    NAN_INDEX_ENUMERATOR(node_FLAC__StreamDecoderErrorStatusString) {
        Local<Array> array = Nan::New<Array>();
        for(int i = 0; i < 4; i++) Nan::Set(array, i, Nan::New(i));
        info.GetReturnValue().Set(array);
    }

    NAN_MODULE_INIT(initDecoder) {
        Local<Object> obj = Nan::New<Object>();
        #define setMethod(fn) \
        Nan::SetMethod(obj, #fn, _JOIN(node_FLAC__stream_decoder_, fn)); \
        _JOIN(FLAC__stream_decoder_, fn) = libFlac->getSymbolAddress<_JOIN2(FLAC__stream_decoder_, fn, _t)>("FLAC__stream_decoder_" #fn); \
        if(_JOIN(FLAC__stream_decoder_, fn) == nullptr) printf("%s\n", libFlac->getLastError().c_str());

        setMethod(new);
        setMethod(delete);
        setMethod(set_ogg_serial_number);
        setMethod(set_md5_checking);
        setMethod(set_metadata_respond);
        setMethod(set_metadata_respond_application);
        setMethod(set_metadata_respond_all);
        setMethod(set_metadata_ignore);
        setMethod(set_metadata_ignore_application);
        setMethod(set_metadata_ignore_all);
        setMethod(get_state);
        setMethod(get_resolved_state_string);
        setMethod(get_md5_checking);
        setMethod(get_total_samples);
        setMethod(get_channels);
        setMethod(get_channel_assignment);
        setMethod(get_bits_per_sample);
        setMethod(get_sample_rate);
        setMethod(get_blocksize);
        setMethod(get_decode_position);
        setMethod(init_stream);
        setMethod(init_ogg_stream);
        setMethod(init_file);
        setMethod(init_ogg_file);
        setMethod(finish);
        setMethod(flush);
        setMethod(reset);
        setMethod(process_single);
        setMethod(process_until_end_of_stream);
        setMethod(process_until_end_of_metadata);
        setMethod(skip_single_frame);
        setMethod(seek_absolute);

        #define propertyGetter(func) \
        Local<ObjectTemplate> _JOIN(func, Var) = Nan::New<ObjectTemplate>(); \
        Nan::SetNamedPropertyHandler(_JOIN(func, Var), _JOIN(Dec, func)); \
        Nan::Set(obj, Nan::New(#func).ToLocalChecked(), Nan::NewInstance(_JOIN(func, Var)).ToLocalChecked());

        propertyGetter(State);
        propertyGetter(InitStatus);
        propertyGetter(ReadStatus);
        propertyGetter(SeekStatus);
        propertyGetter(TellStatus);
        propertyGetter(LengthStatus);
        propertyGetter(WriteStatus);
        propertyGetter(ErrorStatus);

        #define indexGetter(func) \
        _JOIN(FLAC__StreamDecoder, func) = libFlac->getSymbolAddress<const char* const*>("FLAC__StreamDecoder" #func); \
        Local<ObjectTemplate> _JOIN(func, _template) = Nan::New<ObjectTemplate>(); \
        Nan::SetIndexedPropertyHandler(_JOIN(func, _template), _JOIN(node_FLAC__StreamDecoder, func), nullptr, nullptr, nullptr, \
            _JOIN(node_FLAC__StreamDecoder, func)); \
        Nan::Set(obj, Nan::New(#func).ToLocalChecked(), Nan::NewInstance(_JOIN(func, _template)).ToLocalChecked());

        indexGetter(StateString);
        indexGetter(InitStatusString);
        indexGetter(ReadStatusString);
        indexGetter(SeekStatusString);
        indexGetter(TellStatusString);
        indexGetter(LengthStatusString);
        indexGetter(WriteStatusString);
        indexGetter(ErrorStatusString);

        Nan::Set(target, Nan::New("decoder").ToLocalChecked(), obj);
    }

    void atExitDecoder() {
        for(auto it = objectsToDelete.begin(); it != objectsToDelete.end(); it++) {
            (*it)->readCbk.Reset();
            (*it)->writeCbk.Reset();
            (*it)->seekCbk.Reset();
            (*it)->tellCbk.Reset();
            (*it)->lengthCbk.Reset();
            (*it)->eofCbk.Reset();
            (*it)->metadataCbk.Reset();
            (*it)->errorCbk.Reset();
            delete (*it);
        }
        objectsToDelete.clear();
    }
}


static int read_callback(const FLAC__StreamDecoder* dec, FLAC__byte buffer[], size_t* bytes, void* data) {
    Nan::HandleScope scope;
    flac_decoding_callbacks* cbks = (flac_decoding_callbacks*) data;
    Handle<Value> args[] {
        WrapPointer(buffer, *bytes).ToLocalChecked()
    };

    Nan::TryCatch tc; tc.SetVerbose(true);
    Local<Function> func = Nan::New(cbks->readCbk);
    Local<Value> ret = cbks->async.runInAsyncScope(Nan::GetCurrentContext()->Global(), func, 1, args).ToLocalChecked();
    if(tc.HasCaught()) {
        tc.ReThrow();
        return 1;
    }
    if(!ret.IsEmpty()) {
        Local<Object> retJust = ret.As<Object>();
        Local<Value> bytes2 = Nan::Get(retJust, Nan::New("bytes").ToLocalChecked()).ToLocalChecked();
        Local<Value> returnValue = Nan::Get(retJust, Nan::New("returnValue").ToLocalChecked()).ToLocalChecked();
        *bytes = (size_t) Nan::To<int32_t>(bytes2).FromJust();
        return Nan::To<int32_t>(returnValue).FromJust();
    } else {
        printf("readCbk returned emtpy, to avoid errors will return END_OF_STREAM\n");
        return 1;
    }
}

static int seek_callback(const FLAC__StreamDecoder* dec, uint64_t offset, void* data) {
    Nan::HandleScope scope;
    flac_decoding_callbacks* cbks = (flac_decoding_callbacks*) data;
    Handle<Value> args[] {
        Nan::New<Number>(offset)
    };

    Nan::TryCatch tc; tc.SetVerbose(true);
    Local<Function> func = Nan::New(cbks->seekCbk);
    Local<Value> ret = cbks->async.runInAsyncScope(Nan::GetCurrentContext()->Global(), func, 1, args).ToLocalChecked();
    if(tc.HasCaught()) {
        tc.ReThrow();
        return 0;
    }
    if(ret.IsEmpty()) {
        return 0;
    } else {
        return Nan::To<int32_t>(ret).FromJust();
    }
}

static int tell_callback(const FLAC__StreamDecoder* dec, uint64_t* offset, void* data) {
    Nan::HandleScope scope;
    flac_decoding_callbacks* cbks = (flac_decoding_callbacks*) data;
    Handle<Value> args[] { Nan::Null() };

    Nan::TryCatch tc; tc.SetVerbose(true);
    Local<Function> func = Nan::New(cbks->tellCbk);
    Local<Value> ret = cbks->async.runInAsyncScope(Nan::GetCurrentContext()->Global(), func, 0, args).ToLocalChecked();
    if(tc.HasCaught()) {
        tc.ReThrow();
        return 1;
    }
    if(!ret.IsEmpty()) {
        Local<Object> retJust = ret.As<Object>();
        Local<Value> offset2 = Nan::Get(retJust, Nan::New("offset").ToLocalChecked()).ToLocalChecked();
        Local<Value> returnValue = Nan::Get(retJust, Nan::New("returnValue").ToLocalChecked()).ToLocalChecked();
        *offset = (size_t) Nan::To<int64_t>(offset2).FromJust();
        return Nan::To<int32_t>(returnValue).FromJust();
    } else {
        printf("tellCallback returned empty, to avoid errors will return ERROR\n");
        *offset = 0;
        return 1;
    }
}

static int length_callback(const FLAC__StreamDecoder* dec, uint64_t* length, void* data) {
    Nan::HandleScope scope;
    flac_decoding_callbacks* cbks = (flac_decoding_callbacks*) data;
    Handle<Value> args[] { Nan::Null() };

    Nan::TryCatch tc; tc.SetVerbose(true);
    Local<Function> func = Nan::New(cbks->lengthCbk);
    Local<Value> ret = cbks->async.runInAsyncScope(Nan::GetCurrentContext()->Global(), func, 0, args).ToLocalChecked();
    if(tc.HasCaught()) {
        tc.ReThrow();
        return 1;
    }
    if(!ret.IsEmpty()) {
        Local<Object> retJust = ret.As<Object>();
        Local<Value> length2 = Nan::Get(retJust, Nan::New("length").ToLocalChecked()).ToLocalChecked();
        Local<Value> returnValue = Nan::Get(retJust, Nan::New("returnValue").ToLocalChecked()).ToLocalChecked();
        *length = (size_t) Nan::To<int64_t>(length2).FromJust();
        return Nan::To<int32_t>(returnValue).FromJust();
    } else {
        printf("lengthCbk returned empty, to avoid errors will return ERROR\n");
        *length = 0;
        return 1;
    }
}

static FLAC__bool eof_callback(const FLAC__StreamDecoder* dec, void* data) {
    Nan::HandleScope scope;
    flac_decoding_callbacks* cbks = (flac_decoding_callbacks*) data;
    Handle<Value> args[] { Nan::Null() };

    Nan::TryCatch tc; tc.SetVerbose(true);
    Local<Function> func = Nan::New(cbks->eofCbk);
    Local<Value> ret = cbks->async.runInAsyncScope(Nan::GetCurrentContext()->Global(), func, 0, args).ToLocalChecked();
    if(tc.HasCaught()) {
        tc.ReThrow();
        return false;
    }
    if(ret.IsEmpty()) {
        return false;
    } else {
        return Nan::To<bool>(ret).FromMaybe(0);
    }
}

static int write_callback(const FLAC__StreamDecoder* dec, const FLAC__Frame* frame, const int32_t *const buffer[], void* data) {
    Nan::HandleScope scope;
    flac_decoding_callbacks* cbks = (flac_decoding_callbacks*) data;
    Local<Array> buffers = Nan::New<Array>();
    unsigned channels = FLAC__stream_decoder_get_channels(dec);
    for(uint32_t i = 0; i < channels; i++)
        Nan::Set(buffers, i, WrapPointer(buffer[i], frame->header.blocksize * sizeof(int32_t)).ToLocalChecked());

    Handle<Value> args[] {
        flac_bindings::structToJs(frame),
        buffers
    };

    Nan::TryCatch tc; tc.SetVerbose(true);
    Local<Function> func = Nan::New(cbks->writeCbk);
    Local<Value> ret = cbks->async.runInAsyncScope(Nan::GetCurrentContext()->Global(), func, 2, args).ToLocalChecked();
    if(tc.HasCaught()) {
        tc.ReThrow();
        return 2;
    }
    if(ret.IsEmpty()) {
        return 0;
    } else {
        return Nan::To<int32_t>(ret).FromMaybe(0);
    }
}

static void metadata_callback(const FLAC__StreamDecoder* dec, const FLAC__StreamMetadata* metadata, void* data) {
    Nan::HandleScope scope;
    flac_decoding_callbacks* cbks = (flac_decoding_callbacks*) data;
    Handle<Value> args[] = {
        flac_bindings::structToJs(metadata)
    };

    Nan::TryCatch tc; tc.SetVerbose(true);
    Local<Function> func = Nan::New(cbks->metadataCbk);
    cbks->async.runInAsyncScope(Nan::GetCurrentContext()->Global(), func, 1, args);
    if(tc.HasCaught()) {
        tc.ReThrow();
    }
}

static void error_callback(const FLAC__StreamDecoder* dec, int error, void* data) {
    Nan::HandleScope scope;
    flac_decoding_callbacks* cbks = (flac_decoding_callbacks*) data;
    Handle<Value> args[] = {
        Nan::New(error)
    };

    Nan::TryCatch tc; tc.SetVerbose(true);
    Local<Function> func = Nan::New(cbks->errorCbk);
    cbks->async.runInAsyncScope(Nan::GetCurrentContext()->Global(), func, 1, args);
    if(tc.HasCaught()) {
        tc.ReThrow();
    }
}
