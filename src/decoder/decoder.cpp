#include <memory>
#include <nan.h>
#include "../utils/dl.hpp"

using namespace v8;
using namespace node;
#define DECODER_IMPL
#include "decoder.hpp"
#include "../utils/pointer.hpp"
#include "../mappings/mappings.hpp"

#define _JOIN(a, b) a##b
#define _JOIN2(a,b,c) a##b##c


#define UNWRAP_FLAC \
    StreamDecoder* self = Nan::ObjectWrap::Unwrap<StreamDecoder>(info.Holder()); \
    FLAC__StreamDecoder* dec = self->dec;

#define FLAC_GETTER_METHOD(type, v8Type, fn, jsFn) \
NAN_METHOD(StreamDecoder:: jsFn) { \
    UNWRAP_FLAC \
    type output = _JOIN(FLAC__stream_decoder_get_, fn)(dec); \
    info.GetReturnValue().Set(_JOIN(v8Type, ToJs)(output)); \
}

#define FLAC_SETTER_METHOD(type, v8Type, fn, jsFn) \
NAN_METHOD(StreamDecoder:: jsFn) { \
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


namespace flac_bindings {

    extern Library* libFlac;

    FLAC_SETTER_METHOD(long, number, ogg_serial_number, setOggSerialNumber);
    FLAC_GETTER_METHOD(FLAC__bool, boolean, md5_checking, getMd5Checking);
    FLAC_SETTER_METHOD(FLAC__bool, boolean, md5_checking, setMd5Checking);
    FLAC_SETTER_METHOD(FLAC__MetadataType, number, metadata_respond, setMetadataRespond);
    FLAC_SETTER_METHOD(FLAC__MetadataType, number, metadata_ignore, setMetadataIgnore);
    FLAC_GETTER_METHOD(uint64_t, number, total_samples, getTotalSamples);
    FLAC_GETTER_METHOD(unsigned, number, channels, getChannels);
    FLAC_GETTER_METHOD(FLAC__ChannelAssignment, number, channel_assignment, getChannelAssignment);
    FLAC_GETTER_METHOD(unsigned, number, bits_per_sample, getBitsPerSample);
    FLAC_GETTER_METHOD(unsigned, number, sample_rate, getSampleRate);
    FLAC_GETTER_METHOD(unsigned, number, blocksize, getBlocksize);

    NAN_METHOD(StreamDecoder::create) {
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

    NAN_METHOD(StreamDecoder::initStream) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        WARN_SYNC_FUNCTION("Decoder.initStream");

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
            !self->readCbk ? nullptr : decoder_read_callback,
            !self->seekCbk ? nullptr : decoder_seek_callback,
            !self->tellCbk ? nullptr : decoder_tell_callback,
            !self->lengthCbk ? nullptr : decoder_length_callback,
            !self->eofCbk ? nullptr : decoder_eof_callback,
            !self->writeCbk ? nullptr : decoder_write_callback,
            !self->metadataCbk ? nullptr : decoder_metadata_callback,
            !self->errorCbk ? nullptr : decoder_error_callback,
            self
        );
        info.GetReturnValue().Set(Nan::New(ret));
        delete self->async;
        self->async = nullptr;
    }

    NAN_METHOD(StreamDecoder::initOggStream) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        WARN_SYNC_FUNCTION("Decoder.initOggStream");

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
            !self->readCbk ? nullptr : decoder_read_callback,
            !self->seekCbk ? nullptr : decoder_seek_callback,
            !self->tellCbk ? nullptr : decoder_tell_callback,
            !self->lengthCbk ? nullptr : decoder_length_callback,
            !self->eofCbk ? nullptr : decoder_eof_callback,
            !self->writeCbk ? nullptr : decoder_write_callback,
            !self->metadataCbk ? nullptr : decoder_metadata_callback,
            !self->errorCbk ? nullptr : decoder_error_callback,
            self
        );
        info.GetReturnValue().Set(Nan::New(ret));
        delete self->async;
        self->async = nullptr;
    }

    NAN_METHOD(StreamDecoder::initFile) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        WARN_SYNC_FUNCTION("Decoder.initFile");

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
            !self->writeCbk ? nullptr : decoder_write_callback,
            !self->metadataCbk ? nullptr : decoder_metadata_callback,
            !self->errorCbk ? nullptr : decoder_error_callback,
            self);
        info.GetReturnValue().Set(Nan::New(ret));
        delete self->async;
        self->async = nullptr;
    }

    NAN_METHOD(StreamDecoder::initOggFile) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        WARN_SYNC_FUNCTION("initOggFile");

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
            !self->writeCbk ? nullptr : decoder_write_callback,
            !self->metadataCbk ? nullptr : decoder_metadata_callback,
            !self->errorCbk ? nullptr : decoder_error_callback,
            self);
        info.GetReturnValue().Set(Nan::New(ret));
        delete self->async;
        self->async = nullptr;
    }

    NAN_METHOD(StreamDecoder::finish) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        WARN_SYNC_FUNCTION("Decoder.finish");
        self->async = new Nan::AsyncResource("flac:decoder:finish");
        bool returnValue = FLAC__stream_decoder_finish(dec);
        info.GetReturnValue().Set(Nan::New<Boolean>(returnValue));
        delete self->async;
        self->async = nullptr;
    }

    NAN_METHOD(StreamDecoder::flush) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        WARN_SYNC_FUNCTION("Decoder.flush");
        self->async = new Nan::AsyncResource("flac:decoder:flush");
        bool returnValue = FLAC__stream_decoder_flush(dec);
        info.GetReturnValue().Set(Nan::New<Boolean>(returnValue));
        delete self->async;
        self->async = nullptr;
    }

    NAN_METHOD(StreamDecoder::reset) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        self->async = new Nan::AsyncResource("flac:decoder:initStream");
        bool returnValue = FLAC__stream_decoder_reset(dec);
        info.GetReturnValue().Set(Nan::New<Boolean>(returnValue));
        delete self->async;
        self->async = nullptr;
    }

    NAN_METHOD(StreamDecoder::processSingle) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        WARN_SYNC_FUNCTION("Decoder.processSingle");
        self->async = new Nan::AsyncResource("flac:decoder:processSingle");
        bool returnValue = FLAC__stream_decoder_process_single(dec);
        info.GetReturnValue().Set(Nan::New<Boolean>(returnValue));
        delete self->async;
        self->async = nullptr;
    }

    NAN_METHOD(StreamDecoder::processUntilEndOfMetadata) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        WARN_SYNC_FUNCTION("Decoder.processUntilEndOfMetadata");
        self->async = new Nan::AsyncResource("flac:decoder:processUntilEndOfMetadata");
        bool returnValue = FLAC__stream_decoder_process_until_end_of_metadata(dec);
        info.GetReturnValue().Set(Nan::New<Boolean>(returnValue));
        delete self->async;
        self->async = nullptr;
    }

    NAN_METHOD(StreamDecoder::processUntilEndOfStream) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        WARN_SYNC_FUNCTION("Decoder.processUntilEndOfStream");
        self->async = new Nan::AsyncResource("flac:decoder:processUntilEndOfStream");
        bool returnValue = FLAC__stream_decoder_process_until_end_of_stream(dec);
        info.GetReturnValue().Set(Nan::New<Boolean>(returnValue));
        delete self->async;
        self->async = nullptr;
    }

    NAN_METHOD(StreamDecoder::skipSingleFrame) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        WARN_SYNC_FUNCTION("Decoder.skipSingleFrame");
        self->async = new Nan::AsyncResource("flac:decoder:skipSingleFrame");
        bool returnValue = FLAC__stream_decoder_skip_single_frame(dec);
        info.GetReturnValue().Set(Nan::New<Boolean>(returnValue));
        delete self->async;
        self->async = nullptr;
    }

    NAN_METHOD(StreamDecoder::seekAbsolute) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        WARN_SYNC_FUNCTION("Decoder.seekAbsolute");
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

    NAN_METHOD(StreamDecoder::setMetadataRespondApplication) {
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

    NAN_METHOD(StreamDecoder::setMetadataRespondAll) {
        UNWRAP_FLAC
        FLAC__bool ret = FLAC__stream_decoder_set_metadata_respond_all(dec);
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
    }

    NAN_METHOD(StreamDecoder::setMetadataIgnoreApplication) {
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

    NAN_METHOD(StreamDecoder::setMetadataIgnoreAll) {
        UNWRAP_FLAC
        FLAC__bool ret = FLAC__stream_decoder_set_metadata_ignore_all(dec);
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
    }

    NAN_METHOD(StreamDecoder::getState) {
        UNWRAP_FLAC
        int state = FLAC__stream_decoder_get_state(dec);
        info.GetReturnValue().Set(Nan::New(state));
    }

    NAN_METHOD(StreamDecoder::getResolvedStateString) {
        UNWRAP_FLAC
        const char* stateString = FLAC__stream_decoder_get_resolved_state_string(dec);
        info.GetReturnValue().Set(Nan::New(stateString).ToLocalChecked());
    }

    NAN_METHOD(StreamDecoder::getDecodePosition) {
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


    NAN_METHOD(StreamDecoder::finishAsync) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        (void) dec;
        AsyncDecoderWorkBase* w;
        if(info[0]->IsFunction()) {
            w = AsyncDecoderWork::forFinish(self, new Nan::Callback(info[0].template As<Function>()));
        } else {
            w = AsyncDecoderWork::forFinish(self);
            info.GetReturnValue().Set(((PromisifiedAsyncDecoderWork*) w)->getPromise());
        }
        w->SaveToPersistent("this", info.This());
        AsyncQueueWorker(w);
    }

    NAN_METHOD(StreamDecoder::flushAsync) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        (void) dec;
        AsyncDecoderWorkBase* w;
        if(info[0]->IsFunction()) {
            w = AsyncDecoderWork::forFlush(self, new Nan::Callback(info[0].template As<Function>()));
        } else {
            w = AsyncDecoderWork::forFlush(self);
            info.GetReturnValue().Set(((PromisifiedAsyncDecoderWork*) w)->getPromise());
        }
        w->SaveToPersistent("this", info.This());
        AsyncQueueWorker(w);
    }

    NAN_METHOD(StreamDecoder::processSingleAsync) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        (void) dec;
        AsyncDecoderWorkBase* w;
        if(info[0]->IsFunction()) {
            w = AsyncDecoderWork::forProcessSingle(self, new Nan::Callback(info[0].template As<Function>()));
        } else {
            w = AsyncDecoderWork::forProcessSingle(self);
            info.GetReturnValue().Set(((PromisifiedAsyncDecoderWork*) w)->getPromise());
        }
        w->SaveToPersistent("this", info.This());
        AsyncQueueWorker(w);
    }

    NAN_METHOD(StreamDecoder::processUntilEndOfMetadataAsync) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        (void) dec;
        AsyncDecoderWorkBase* w;
        if(info[0]->IsFunction()) {
            w = AsyncDecoderWork::forProcessUntilEndOfMetadata(self, new Nan::Callback(info[0].template As<Function>()));
        } else {
            w = AsyncDecoderWork::forProcessUntilEndOfMetadata(self);
            info.GetReturnValue().Set(((PromisifiedAsyncDecoderWork*) w)->getPromise());
        }
        w->SaveToPersistent("this", info.This());
        AsyncQueueWorker(w);
    }


    NAN_METHOD(StreamDecoder::processUntilEndOfStreamAsync) {
        UNWRAP_FLAC (void) dec;
        CHECK_ASYNC_IS_NULL
        AsyncDecoderWorkBase* w;
        if(info[0]->IsFunction()) {
            w = AsyncDecoderWork::forProcessUntilEndOfStream(self, new Nan::Callback(info[0].template As<Function>()));
        } else {
            w = AsyncDecoderWork::forProcessUntilEndOfStream(self);
            info.GetReturnValue().Set(((PromisifiedAsyncDecoderWork*) w)->getPromise());
        }
        w->SaveToPersistent("this", info.This());
        AsyncQueueWorker(w);
    }

    NAN_METHOD(StreamDecoder::skipSingleFrameAsync) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        (void) dec;
        AsyncDecoderWorkBase* w;
        if(info[0]->IsFunction()) {
            w = AsyncDecoderWork::forSkipSingleFrame(self, new Nan::Callback(info[0].template As<Function>()));
        } else {
            w = AsyncDecoderWork::forSkipSingleFrame(self);
            info.GetReturnValue().Set(((PromisifiedAsyncDecoderWork*) w)->getPromise());
        }
        w->SaveToPersistent("this", info.This());
        AsyncQueueWorker(w);
    }

    NAN_METHOD(StreamDecoder::seekAbsoluteAsync) {
        UNWRAP_FLAC
        CHECK_ASYNC_IS_NULL
        (void) dec;

        auto maybeOffset = numberFromJs<uint64_t>(info[0]);
        if(maybeOffset.IsNothing()) {
            Nan::ThrowTypeError("Expected first argument to be number or bigint");
            return;
        }

        uint16_t p = maybeOffset.FromJust();
        AsyncDecoderWorkBase* w;
        if(info[1]->IsFunction()) {
            w = AsyncDecoderWork::forSeekAbsolute(p, self, new Nan::Callback(info[1].template As<Function>()));
        } else {
            w = AsyncDecoderWork::forSeekAbsolute(p, self);
            info.GetReturnValue().Set(((PromisifiedAsyncDecoderWork*) w)->getPromise());
        }
        w->SaveToPersistent("this", info.This());
        AsyncQueueWorker(w);
    }

    NAN_METHOD(StreamDecoder::initStreamAsync) {
        UNWRAP_FLAC;
        CHECK_ASYNC_IS_NULL
        (void) dec;

        if(info[0]->IsFunction()) self->readCbk.reset(new Nan::Callback(info[0].As<Function>()));
        if(info[1]->IsFunction()) self->seekCbk.reset(new Nan::Callback(info[1].As<Function>()));
        if(info[2]->IsFunction()) self->tellCbk.reset(new Nan::Callback(info[2].As<Function>()));
        if(info[3]->IsFunction()) self->lengthCbk.reset(new Nan::Callback(info[3].As<Function>()));
        if(info[4]->IsFunction()) self->eofCbk.reset(new Nan::Callback(info[4].As<Function>()));
        if(info[5]->IsFunction()) self->writeCbk.reset(new Nan::Callback(info[5].As<Function>()));
        if(info[6]->IsFunction()) self->metadataCbk.reset(new Nan::Callback(info[6].As<Function>()));
        if(info[7]->IsFunction()) self->errorCbk.reset(new Nan::Callback(info[7].As<Function>()));

        AsyncDecoderWorkBase* w;
        if(info[8]->IsFunction()) {
            w = AsyncDecoderWork::forInitStream(self, new Nan::Callback(info[8].template As<Function>()));
        } else {
            w = AsyncDecoderWork::forInitStream(self);
            info.GetReturnValue().Set(((PromisifiedAsyncDecoderWork*) w)->getPromise());
        }
        w->SaveToPersistent("this", info.This());
        AsyncQueueWorker(w);
    }

    NAN_METHOD(StreamDecoder::initOggStreamAsync) {
        UNWRAP_FLAC;
        CHECK_ASYNC_IS_NULL
        (void) dec;

        if(info[0]->IsFunction()) self->readCbk.reset(new Nan::Callback(info[0].As<Function>()));
        if(info[1]->IsFunction()) self->seekCbk.reset(new Nan::Callback(info[1].As<Function>()));
        if(info[2]->IsFunction()) self->tellCbk.reset(new Nan::Callback(info[2].As<Function>()));
        if(info[3]->IsFunction()) self->lengthCbk.reset(new Nan::Callback(info[3].As<Function>()));
        if(info[4]->IsFunction()) self->eofCbk.reset(new Nan::Callback(info[4].As<Function>()));
        if(info[5]->IsFunction()) self->writeCbk.reset(new Nan::Callback(info[5].As<Function>()));
        if(info[6]->IsFunction()) self->metadataCbk.reset(new Nan::Callback(info[6].As<Function>()));
        if(info[7]->IsFunction()) self->errorCbk.reset(new Nan::Callback(info[7].As<Function>()));

        AsyncDecoderWorkBase* w;
        if(info[8]->IsFunction()) {
            w = AsyncDecoderWork::forInitOggStream(self, new Nan::Callback(info[8].template As<Function>()));
        } else {
            w = AsyncDecoderWork::forInitOggStream(self);
            info.GetReturnValue().Set(((PromisifiedAsyncDecoderWork*) w)->getPromise());
        }
        w->SaveToPersistent("this", info.This());
        AsyncQueueWorker(w);
    }

    NAN_METHOD(StreamDecoder::initFileAsync) {
        UNWRAP_FLAC;
        CHECK_ASYNC_IS_NULL
        (void) dec;

        if(!info[0]->IsString()) {
            Nan::ThrowTypeError("Expected first argument to be string");
            return;
        }

        Local<String> fileNameJs = info[0].As<String>();
        if(info[1]->IsFunction()) self->writeCbk.reset(new Nan::Callback(info[1].As<Function>()));
        if(info[2]->IsFunction()) self->metadataCbk.reset(new Nan::Callback(info[2].As<Function>()));
        if(info[3]->IsFunction()) self->errorCbk.reset(new Nan::Callback(info[3].As<Function>()));
        Nan::Utf8String fileName(fileNameJs);

        AsyncDecoderWorkBase* w;
        if(info[4]->IsFunction()) {
            w = AsyncDecoderWork::forInitFile(*fileName, self, new Nan::Callback(info[4].template As<Function>()));
        } else {
            w = AsyncDecoderWork::forInitFile(*fileName, self);
            info.GetReturnValue().Set(((PromisifiedAsyncDecoderWork*) w)->getPromise());
        }
        w->SaveToPersistent("this", info.This());
        AsyncQueueWorker(w);
    }

    NAN_METHOD(StreamDecoder::initOggFileAsync) {
        UNWRAP_FLAC;
        CHECK_ASYNC_IS_NULL
        (void) dec;

        if(!info[0]->IsString()) {
            Nan::ThrowTypeError("Expected first argument to be string");
            return;
        }

        Local<String> fileNameJs = info[0].As<String>();
        if(info[1]->IsFunction()) self->writeCbk.reset(new Nan::Callback(info[1].As<Function>()));
        if(info[2]->IsFunction()) self->metadataCbk.reset(new Nan::Callback(info[2].As<Function>()));
        if(info[3]->IsFunction()) self->errorCbk.reset(new Nan::Callback(info[3].As<Function>()));
        Nan::Utf8String fileName(fileNameJs);

        AsyncDecoderWorkBase* w;
        if(info[4]->IsFunction()) {
            w = AsyncDecoderWork::forInitOggFile(*fileName, self, new Nan::Callback(info[4].template As<Function>()));
        } else {
            w = AsyncDecoderWork::forInitOggFile(*fileName, self);
            info.GetReturnValue().Set(((PromisifiedAsyncDecoderWork*) w)->getPromise());
        }
        w->SaveToPersistent("this", info.This());
        AsyncQueueWorker(w);
    }



    FlacEnumDefineReturnType StreamDecoder::createStateEnum() {
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

    FlacEnumDefineReturnType StreamDecoder::createInitStatusEnum() {
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

    FlacEnumDefineReturnType StreamDecoder::createReadStatusEnum() {
        Local<Object> obj1 = Nan::New<Object>();
        Local<Object> obj2 = Nan::New<Object>();
        flacEnum_defineValue(obj1, obj2, "CONTINUE", 0);
        flacEnum_defineValue(obj1, obj2, "END_OF_STREAM", 1);
        flacEnum_defineValue(obj1, obj2, "ABORT", 2);
        return std::make_tuple(obj1, obj2);
    }

    FlacEnumDefineReturnType StreamDecoder::createSeekStatusEnum() {
        Local<Object> obj1 = Nan::New<Object>();
        Local<Object> obj2 = Nan::New<Object>();
        flacEnum_defineValue(obj1, obj2, "OK", 0);
        flacEnum_defineValue(obj1, obj2, "ERROR", 1);
        flacEnum_defineValue(obj1, obj2, "UNSUPPORTED", 2);
        return std::make_tuple(obj1, obj2);
    }

    FlacEnumDefineReturnType StreamDecoder::createTellStatusEnum() {
        Local<Object> obj1 = Nan::New<Object>();
        Local<Object> obj2 = Nan::New<Object>();
        flacEnum_defineValue(obj1, obj2, "OK", 0);
        flacEnum_defineValue(obj1, obj2, "ERROR", 1);
        flacEnum_defineValue(obj1, obj2, "UNSUPPORTED", 2);
        return std::make_tuple(obj1, obj2);
    }

    FlacEnumDefineReturnType StreamDecoder::createLengthStatusEnum() {
        Local<Object> obj1 = Nan::New<Object>();
        Local<Object> obj2 = Nan::New<Object>();
        flacEnum_defineValue(obj1, obj2, "OK", 0);
        flacEnum_defineValue(obj1, obj2, "ERROR", 1);
        flacEnum_defineValue(obj1, obj2, "UNSUPPORTED", 2);
        return std::make_tuple(obj1, obj2);
    }

    FlacEnumDefineReturnType StreamDecoder::createWriteStatusEnum() {
        Local<Object> obj1 = Nan::New<Object>();
        Local<Object> obj2 = Nan::New<Object>();
        flacEnum_defineValue(obj1, obj2, "CONTINUE", 0);
        flacEnum_defineValue(obj1, obj2, "ABORT", 1);
        flacEnum_defineValue(obj1, obj2, "UNSUPPORTED", 2);
        return std::make_tuple(obj1, obj2);
    }

    FlacEnumDefineReturnType StreamDecoder::createErrorStatusEnum() {
        Local<Object> obj1 = Nan::New<Object>();
        Local<Object> obj2 = Nan::New<Object>();
        flacEnum_defineValue(obj1, obj2, "LOST_SYNC", 0);
        flacEnum_defineValue(obj1, obj2, "BAD_HEADER", 1);
        flacEnum_defineValue(obj1, obj2, "FRAME_CRC_MISMATCH", 2);
        flacEnum_defineValue(obj1, obj2, "UNPARSEABLE_STREAM", 3);
        return std::make_tuple(obj1, obj2);
    }


    StreamDecoder::~StreamDecoder() {
        FLAC__stream_decoder_delete(dec);
    }

    NAN_MODULE_INIT(StreamDecoder::initDecoder) {
        Local<FunctionTemplate> obj = Nan::New<FunctionTemplate>(create);
        obj->SetClassName(Nan::New("StreamDecoder").ToLocalChecked());
        obj->InstanceTemplate()->SetInternalFieldCount(1);

        #define loadFunction(fn) \
        _JOIN(FLAC__stream_decoder_, fn) = libFlac->getSymbolAddress<_JOIN2(FLAC__stream_decoder_, fn, _t)>("FLAC__stream_decoder_" #fn); \
        if(_JOIN(FLAC__stream_decoder_, fn) == nullptr) printf("%s\n", libFlac->getLastError().c_str());

        #define setMethod(fn, jsFn) \
        Nan::SetPrototypeMethod(obj, #jsFn, jsFn); \
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

        Nan::SetPrototypeMethod(obj, "finishAsync", finishAsync);
        Nan::SetPrototypeMethod(obj, "flushAsync", flushAsync);
        Nan::SetPrototypeMethod(obj, "processSingleAsync", processSingleAsync);
        Nan::SetPrototypeMethod(obj, "processUntilEndOfMetadataAsync", processUntilEndOfMetadataAsync);
        Nan::SetPrototypeMethod(obj, "processUntilEndOfStreamAsync", processUntilEndOfStreamAsync);
        Nan::SetPrototypeMethod(obj, "skipSingleFrameAsync", skipSingleFrameAsync);
        Nan::SetPrototypeMethod(obj, "seekAbsoluteAsync", seekAbsoluteAsync);
        Nan::SetPrototypeMethod(obj, "initStreamAsync", initStreamAsync);
        Nan::SetPrototypeMethod(obj, "initOggStreamAsync", initOggStreamAsync);
        Nan::SetPrototypeMethod(obj, "initFileAsync", initFileAsync);
        Nan::SetPrototypeMethod(obj, "initOggFileAsync", initOggFileAsync);

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


static int doAsyncWork(flac_bindings::StreamDecoder* dec, flac_bindings::DecoderWorkRequest &dwr, int errorReturnValue) {
    int returnValue = errorReturnValue;
    dwr.returnValue = &returnValue;

    dec->progress->Send(&dwr, 1);
    dwr.waitForWorkDone();

    return returnValue;
}

int decoder_read_callback(const FLAC__StreamDecoder* dec, FLAC__byte buffer[], size_t* bytes, void* data) {
    flac_bindings::StreamDecoder* cbks = (flac_bindings::StreamDecoder*) data;
    if(cbks->progress) {
        using namespace flac_bindings;
        DecoderWorkRequest dwr(DecoderWorkRequest::Type::Read);
        dwr.buffer = buffer;
        dwr.bytes = bytes;
        return doAsyncWork(cbks, dwr, 2);
    }

    Nan::HandleScope scope;
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

int decoder_seek_callback(const FLAC__StreamDecoder* dec, uint64_t offset, void* data) {
    flac_bindings::StreamDecoder* cbks = (flac_bindings::StreamDecoder*) data;
    if(cbks->progress) {
        using namespace flac_bindings;
        DecoderWorkRequest dwr(DecoderWorkRequest::Type::Seek);
        dwr.offset = &offset;
        return doAsyncWork(cbks, dwr, 1);
    }

    Nan::HandleScope scope;
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

int decoder_tell_callback(const FLAC__StreamDecoder* dec, uint64_t* offset, void* data) {
    flac_bindings::StreamDecoder* cbks = (flac_bindings::StreamDecoder*) data;
    if(cbks->progress) {
        using namespace flac_bindings;
        DecoderWorkRequest dwr(DecoderWorkRequest::Type::Tell);
        dwr.offset = offset;
        return doAsyncWork(cbks, dwr, 1);
    }

    Nan::HandleScope scope;
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

int decoder_length_callback(const FLAC__StreamDecoder* dec, uint64_t* length, void* data) {
    flac_bindings::StreamDecoder* cbks = (flac_bindings::StreamDecoder*) data;
    if(cbks->progress) {
        using namespace flac_bindings;
        DecoderWorkRequest dwr(DecoderWorkRequest::Type::Read);
        dwr.offset = length;
        return doAsyncWork(cbks, dwr, 1);
    }

    Nan::HandleScope scope;
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

FLAC__bool decoder_eof_callback(const FLAC__StreamDecoder* dec, void* data) {
    flac_bindings::StreamDecoder* cbks = (flac_bindings::StreamDecoder*) data;
    if(cbks->progress) {
        using namespace flac_bindings;
        DecoderWorkRequest dwr(DecoderWorkRequest::Type::Eof);
        return doAsyncWork(cbks, dwr, true);
    }

    Nan::HandleScope scope;
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

int decoder_write_callback(const FLAC__StreamDecoder* dec, const FLAC__Frame* frame, const int32_t *const buffer[], void* data) {
    flac_bindings::StreamDecoder* cbks = (flac_bindings::StreamDecoder*) data;
    if(cbks->progress) {
        using namespace flac_bindings;
        DecoderWorkRequest dwr(DecoderWorkRequest::Type::Write);
        dwr.frame = frame;
        dwr.samples = buffer;
        return doAsyncWork(cbks, dwr, 1);
    }

    Nan::HandleScope scope;
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

void decoder_metadata_callback(const FLAC__StreamDecoder* dec, const FLAC__StreamMetadata* metadata, void* data) {
    flac_bindings::StreamDecoder* cbks = (flac_bindings::StreamDecoder*) data;
    if(cbks->progress) {
        using namespace flac_bindings;
        DecoderWorkRequest dwr(DecoderWorkRequest::Type::Metadata);
        dwr.metadata = metadata;
        doAsyncWork(cbks, dwr, 0);
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

void decoder_error_callback(const FLAC__StreamDecoder* dec, int error, void* data) {
    flac_bindings::StreamDecoder* cbks = (flac_bindings::StreamDecoder*) data;
    if(cbks->progress) {
        using namespace flac_bindings;
        DecoderWorkRequest dwr(DecoderWorkRequest::Type::Error);
        dwr.errorCode = error;
        doAsyncWork(cbks, dwr, 0);
        return;
    }

    Nan::HandleScope scope;
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
