#pragma once

#include <stdint.h>
#include <nan.h>

#include "../utils/async.hpp"
#include "../format/format.h"
#include "../utils/defs.hpp"

#define _JOIN(a, b) a##b
#define _JOIN2(a,b,c) a##b##c

#ifdef DECODER_IMPL
#define _extern
#else
#define _extern extern
#endif

#define FLAC_FUNC(returnType, fn, ...) \
    typedef returnType (*_JOIN2(FLAC__stream_decoder_, fn, _t))(__VA_ARGS__); \
    _extern _JOIN2(FLAC__stream_decoder_, fn, _t) _JOIN(FLAC__stream_decoder_, fn);

#define FLAC_GETTER(type, fn) FLAC_FUNC(type, _JOIN(get_, fn), const FLAC__StreamDecoder*);

#define FLAC_SETTER(type, fn) FLAC_FUNC(FLAC__bool, _JOIN(set_, fn), FLAC__StreamDecoder*, type)

#define FLAC_GETTER_SETTER(type, fn) \
FLAC_GETTER(type, fn); \
FLAC_SETTER(type, fn);

extern "C" {
    typedef int(*FLAC__StreamDecoderReadCallback)(const FLAC__StreamDecoder*, FLAC__byte [], size_t*, void*);
    typedef int(*FLAC__StreamDecoderSeekCallback)(const FLAC__StreamDecoder*, uint64_t, void*);
    typedef int(*FLAC__StreamDecoderTellCallback)(const FLAC__StreamDecoder*, uint64_t*, void*);
    typedef int(*FLAC__StreamDecoderLengthCallback)(const FLAC__StreamDecoder*, uint64_t*, void*);
    typedef FLAC__bool(*FLAC__StreamDecoderEofCallback)(const FLAC__StreamDecoder*, void*);
    typedef int(*FLAC__StreamDecoderWriteCallback)(const FLAC__StreamDecoder*, const FLAC__Frame*, const int32_t *const [], void*);
    typedef void(*FLAC__StreamDecoderMetadataCallback)(const FLAC__StreamDecoder*, const FLAC__StreamMetadata*, void*);
    typedef void(*FLAC__StreamDecoderErrorCallback)(const FLAC__StreamDecoder*, int, void*);

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

#undef _JOIN
#undef _JOIN2
#undef FLAC_FUNC
#undef FLAC_GETTER
#undef FLAC_SETTER
#undef FLAC_GETTER_SETTER

namespace flac_bindings {
    struct DecoderWorkRequest: SyncronizableWorkRequest {
        enum Type { Read, Seek, Tell, Length, Eof, Write, Metadata, Error } type;
        uint8_t* buffer = nullptr;
        size_t* bytes = nullptr;
        uint64_t* offset = nullptr;
        const FLAC__Frame* frame = nullptr;
        const int32_t *const* samples = nullptr;
        const FLAC__StreamMetadata* metadata = nullptr;
        int errorCode = 0;
        int* returnValue = nullptr;

        DecoderWorkRequest();
        DecoderWorkRequest(const DecoderWorkRequest &);
        explicit DecoderWorkRequest(DecoderWorkRequest::Type type);
    };

    typedef AsyncBackgroundTask<bool, DecoderWorkRequest> AsyncDecoderWorkBase;
    typedef PromisifiedAsyncBackgroundTask<bool, DecoderWorkRequest> PromisifiedAsyncDecoderWorkBase;

    class StreamDecoder: public Nan::ObjectWrap {

        static NAN_METHOD(setOggSerialNumber);
        static NAN_METHOD(getMd5Checking);
        static NAN_METHOD(setMd5Checking);
        static NAN_METHOD(setMetadataRespond);
        static NAN_METHOD(setMetadataIgnore);
        static NAN_METHOD(getTotalSamples);
        static NAN_METHOD(getChannels);
        static NAN_METHOD(getChannelAssignment);
        static NAN_METHOD(getBitsPerSample);
        static NAN_METHOD(getSampleRate);
        static NAN_METHOD(getBlocksize);

        static NAN_METHOD(create);
        static NAN_METHOD(initStream);
        static NAN_METHOD(initOggStream);
        static NAN_METHOD(initFile);
        static NAN_METHOD(initOggFile);
        static NAN_METHOD(finish);
        static NAN_METHOD(flush);
        static NAN_METHOD(reset);
        static NAN_METHOD(processSingle);
        static NAN_METHOD(processUntilEndOfMetadata);
        static NAN_METHOD(processUntilEndOfStream);
        static NAN_METHOD(skipSingleFrame);
        static NAN_METHOD(seekAbsolute);
        static NAN_METHOD(setMetadataRespondApplication);
        static NAN_METHOD(setMetadataRespondAll);
        static NAN_METHOD(setMetadataIgnoreApplication);
        static NAN_METHOD(setMetadataIgnoreAll);
        static NAN_METHOD(getState);
        static NAN_METHOD(getResolvedStateString);
        static NAN_METHOD(getDecodePosition);

        static NAN_METHOD(finishAsync);
        static NAN_METHOD(flushAsync);
        static NAN_METHOD(processSingleAsync);
        static NAN_METHOD(processUntilEndOfMetadataAsync);
        static NAN_METHOD(processUntilEndOfStreamAsync);
        static NAN_METHOD(skipSingleFrameAsync);
        static NAN_METHOD(seekAbsoluteAsync);

        static FlacEnumDefineReturnType createStateEnum();
        static FlacEnumDefineReturnType createInitStatusEnum();
        static FlacEnumDefineReturnType createReadStatusEnum();
        static FlacEnumDefineReturnType createSeekStatusEnum();
        static FlacEnumDefineReturnType createTellStatusEnum();
        static FlacEnumDefineReturnType createLengthStatusEnum();
        static FlacEnumDefineReturnType createWriteStatusEnum();
        static FlacEnumDefineReturnType createErrorStatusEnum();

    public:
        std::shared_ptr<Nan::Callback> readCbk, seekCbk, tellCbk, lengthCbk, eofCbk, writeCbk, metadataCbk, errorCbk;
        Nan::Persistent<v8::Value> lastExceptionProcessed;
        const AsyncDecoderWorkBase::ExecutionProgress *progress = nullptr;
        const AsyncDecoderWorkBase::RejectCallbacks* reject = nullptr;
        Nan::AsyncResource* async = nullptr;
        void* dec = nullptr;

        ~StreamDecoder();
        static NAN_MODULE_INIT(initDecoder);

    };

#ifdef MAKE_FRIENDS
    static void decoderDoWork(const StreamDecoder* dec, const AsyncDecoderWorkBase* w, const DecoderWorkRequest *data, size_t size);
#endif
    class AsyncDecoderWork: public AsyncDecoderWorkBase {
        AsyncDecoderWork(
            std::function<bool()> function,
            Nan::Callback* callback,
            const char* name,
            StreamDecoder* dec
        );

        inline Nan::AsyncResource* getAsyncResource() const { return this->async_resource; }

#ifdef MAKE_FRIENDS
        friend void decoderDoWork(const StreamDecoder* dec, const AsyncDecoderWorkBase* w, const DecoderWorkRequest *data, size_t size);
#endif
    public:
        static AsyncDecoderWork* forFinish(StreamDecoder* dec, Nan::Callback* cbk);
        static AsyncDecoderWork* forFlush(StreamDecoder* dec, Nan::Callback* cbk);
        static AsyncDecoderWork* forProcessSingle(StreamDecoder* dec, Nan::Callback* cbk);
        static AsyncDecoderWork* forProcessUntilEndOfMetadata(StreamDecoder* dec, Nan::Callback* cbk);
        static AsyncDecoderWork* forProcessUntilEndOfStream(StreamDecoder* dec, Nan::Callback* cbk);
        static AsyncDecoderWork* forSkipSingleFrame(StreamDecoder* dec, Nan::Callback* cbk);
        static AsyncDecoderWork* forSeekAbsolute(uint64_t value, StreamDecoder* dec, Nan::Callback* cbk);
    };

    class PromisifiedAsyncDecoderWork: public PromisifiedAsyncDecoderWorkBase {
        PromisifiedAsyncDecoderWork(
            std::function<bool()> function,
            const char* name,
            StreamDecoder* dec
        );

        inline Nan::AsyncResource* getAsyncResource() const { return this->async_resource; }

        friend void decoderDoWork(const StreamDecoder* dec, const AsyncDecoderWorkBase* w, const DecoderWorkRequest *data, size_t size);
    public:
        static PromisifiedAsyncDecoderWork* forFinish(StreamDecoder* dec);
        static PromisifiedAsyncDecoderWork* forFlush(StreamDecoder* dec);
        static PromisifiedAsyncDecoderWork* forProcessSingle(StreamDecoder* dec);
        static PromisifiedAsyncDecoderWork* forProcessUntilEndOfMetadata(StreamDecoder* dec);
        static PromisifiedAsyncDecoderWork* forProcessUntilEndOfStream(StreamDecoder* dec);
        static PromisifiedAsyncDecoderWork* forSkipSingleFrame(StreamDecoder* dec);
        static PromisifiedAsyncDecoderWork* forSeekAbsolute(uint64_t value, StreamDecoder* dec);
    };
}

int decoder_read_callback(const FLAC__StreamDecoder*, FLAC__byte [], size_t*, void*);
int decoder_seek_callback(const FLAC__StreamDecoder*, uint64_t, void*);
int decoder_tell_callback(const FLAC__StreamDecoder*, uint64_t*, void*);
int decoder_length_callback(const FLAC__StreamDecoder*, uint64_t*, void*);
FLAC__bool decoder_eof_callback(const FLAC__StreamDecoder*, void*);
int decoder_write_callback(const FLAC__StreamDecoder*, const FLAC__Frame*, const int32_t *const [], void*);
void decoder_metadata_callback(const FLAC__StreamDecoder*, const FLAC__StreamMetadata*, void*);
void decoder_error_callback(const FLAC__StreamDecoder*, int, void*);
