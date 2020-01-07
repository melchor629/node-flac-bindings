#pragma once

#include <stdint.h>
#include <nan.h>

#include "../flac/format.h"
#include "../flac/decoder.hpp"
#include "../utils/async.hpp"
#include "../utils/defs.hpp"

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

    typedef AsyncBackgroundTask<bool, DecoderWorkRequest*> AsyncDecoderWorkBase;

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
        static NAN_METHOD(initStreamAsync);
        static NAN_METHOD(initOggStreamAsync);
        static NAN_METHOD(initFileAsync);
        static NAN_METHOD(initOggFileAsync);

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
        Nan::AsyncResource* async = nullptr;
        AsyncDecoderWorkBase::ExecutionContext* asyncExecutionContext = nullptr;
        void* dec = nullptr;

        ~StreamDecoder();
        static NAN_MODULE_INIT(initDecoder);

    };

#ifdef MAKE_FRIENDS
    static void decoderDoWork(const StreamDecoder* dec, AsyncDecoderWorkBase::ExecutionContext &w, DecoderWorkRequest* const* data);
#endif
    class AsyncDecoderWork: public AsyncDecoderWorkBase {
        AsyncDecoderWork(
            std::function<bool(AsyncDecoderWorkBase::ExecutionContext &)> function,
            const char* name,
            StreamDecoder* dec
        );

        inline Nan::AsyncResource* getAsyncResource() const { return this->async_resource; }

#ifdef MAKE_FRIENDS
        friend void decoderDoWork(const StreamDecoder* dec, AsyncDecoderWorkBase::ExecutionContext &w, DecoderWorkRequest* const* data);
#endif
    public:
        static AsyncDecoderWorkBase* forFinish(StreamDecoder* dec);
        static AsyncDecoderWorkBase* forFlush(StreamDecoder* dec);
        static AsyncDecoderWorkBase* forProcessSingle(StreamDecoder* dec);
        static AsyncDecoderWorkBase* forProcessUntilEndOfMetadata(StreamDecoder* dec);
        static AsyncDecoderWorkBase* forProcessUntilEndOfStream(StreamDecoder* dec);
        static AsyncDecoderWorkBase* forSkipSingleFrame(StreamDecoder* dec);
        static AsyncDecoderWorkBase* forSeekAbsolute(uint64_t value, StreamDecoder* dec);
        static AsyncDecoderWorkBase* forInitStream(StreamDecoder* dec);
        static AsyncDecoderWorkBase* forInitOggStream(StreamDecoder* dec);
        static AsyncDecoderWorkBase* forInitFile(const std::string &filePath, StreamDecoder* dec);
        static AsyncDecoderWorkBase* forInitOggFile(const std::string &filePath, StreamDecoder* dec);
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
