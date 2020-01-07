#pragma once

#include <stdint.h>
#include <mutex>
#include <condition_variable>
#include <nan.h>

#include "../flac/format.h"
#include "../flac/encoder.hpp"
#include "../utils/async.hpp"
#include "../utils/defs.hpp"

namespace flac_bindings {
    struct EncoderWorkRequest: SyncronizableWorkRequest {
        enum Type { Read, Write, Seek, Tell, Metadata, Progress } type;
        int* returnValue = nullptr;
        char* buffer = nullptr;
        size_t* bytes = nullptr;
        const char* constBuffer = nullptr;
        unsigned samples = 0;
        unsigned frame = 0;
        uint64_t* offset = nullptr;
        const FLAC__StreamMetadata* metadata = nullptr;
        struct {
            uint64_t bytesWritten;
            uint64_t samplesWritten;
            unsigned framesWritten;
            unsigned totalFramesEstimate;
        } progress;

        EncoderWorkRequest();
        EncoderWorkRequest(const EncoderWorkRequest &);
        explicit EncoderWorkRequest(EncoderWorkRequest::Type type);
    };

    typedef AsyncBackgroundTask<bool, EncoderWorkRequest*> AsyncEncoderWorkBase;

    class StreamEncoder: public Nan::ObjectWrap {
        static NAN_METHOD(setOggSerialNumber);
        static NAN_METHOD(getVerify);
        static NAN_METHOD(setVerify);
        static NAN_METHOD(getStreamableSubset);
        static NAN_METHOD(setStreamableSubset);
        static NAN_METHOD(getChannels);
        static NAN_METHOD(setChannels);
        static NAN_METHOD(getBitsPerSample);
        static NAN_METHOD(setBitsPerSample);
        static NAN_METHOD(getSampleRate);
        static NAN_METHOD(setSampleRate);
        static NAN_METHOD(setCompressionLevel);
        static NAN_METHOD(getBlocksize);
        static NAN_METHOD(setBlocksize);
        static NAN_METHOD(getDoMidSideStereo);
        static NAN_METHOD(setDoMidSideStereo);
        static NAN_METHOD(getLooseMidSideStereo);
        static NAN_METHOD(setLooseMidSideStereo);
        static NAN_METHOD(getMaxLpcOrder);
        static NAN_METHOD(setMaxLpcOrder);
        static NAN_METHOD(getQlpCoeffPrecision);
        static NAN_METHOD(setQlpCoeffPrecision);
        static NAN_METHOD(getDoQlpCoeffPrecSearch);
        static NAN_METHOD(setDoQlpCoeffPrecSearch);
        static NAN_METHOD(getDoEscapeCoding);
        static NAN_METHOD(setDoEscapeCoding);
        static NAN_METHOD(getDoExhaustiveModelSearch);
        static NAN_METHOD(setDoExhaustiveModelSearch);
        static NAN_METHOD(getMinResidualPartitionOrder);
        static NAN_METHOD(setMinResidualPartitionOrder);
        static NAN_METHOD(getMaxResidualPartitionOrder);
        static NAN_METHOD(setMaxResidualPartitionOrder);
        static NAN_METHOD(getRiceParameterSearchDist);
        static NAN_METHOD(setRiceParameterSearchDist);
        static NAN_METHOD(getTotalSamplesEstimate);
        static NAN_METHOD(setTotalSamplesEstimate);
        static NAN_METHOD(getState);
        static NAN_METHOD(getVerifyDecoderState);
        static NAN_METHOD(create);
        static NAN_METHOD(initStream);
        static NAN_METHOD(initOggStream);
        static NAN_METHOD(initFile);
        static NAN_METHOD(initOggFile);
        static NAN_METHOD(finish);
        static NAN_METHOD(process);
        static NAN_METHOD(processInterleaved);
        static NAN_METHOD(setMetadata);
        static NAN_METHOD(setApodization);
        static NAN_METHOD(getResolvedStateString);
        static NAN_METHOD(getVerifyDecoderErrorStats);
        static NAN_METHOD(finishAsync);
        static NAN_METHOD(processAsync);
        static NAN_METHOD(processInterleavedAsync);
        static NAN_METHOD(initStreamAsync);
        static NAN_METHOD(initOggStreamAsync);
        static NAN_METHOD(initFileAsync);
        static NAN_METHOD(initOggFileAsync);
        static FlacEnumDefineReturnType createStateEnum();
        static FlacEnumDefineReturnType createInitStatusEnum();
        static FlacEnumDefineReturnType createReadStatusEnum();
        static FlacEnumDefineReturnType createWriteStatusEnum();
        static FlacEnumDefineReturnType createSeekStatusEnum();
        static FlacEnumDefineReturnType createTellStatusEnum();

    public:
        static NAN_MODULE_INIT(initEncoder);

        StreamEncoder();
        ~StreamEncoder();

        std::shared_ptr<Nan::Callback> readCbk, writeCbk, seekCbk, tellCbk, metadataCbk, progressCbk;
        Nan::AsyncResource* async = nullptr;
        FLAC__StreamEncoder* enc = nullptr;
        AsyncEncoderWorkBase::ExecutionContext* asyncExecutionContext = nullptr;
    };

#ifdef MAKE_FRIENDS
    static void encoderDoWork(const StreamEncoder* dec, AsyncEncoderWorkBase::ExecutionContext& w, EncoderWorkRequest* const* data);
#endif
    class AsyncEncoderWork: public AsyncEncoderWorkBase {
        AsyncEncoderWork(
            std::function<bool(AsyncEncoderWorkBase::ExecutionContext &)> function,
            const char* name,
            StreamEncoder* enc
        );

        inline Nan::AsyncResource* getAsyncResource() const { return this->async_resource; }

#ifdef MAKE_FRIENDS
        friend void encoderDoWork(const StreamEncoder* dec, AsyncEncoderWorkBase::ExecutionContext& w, EncoderWorkRequest* const* data);
#endif
    public:
        static AsyncEncoderWorkBase* forFinish(StreamEncoder* enc);
        static AsyncEncoderWorkBase* forProcess(v8::Local<v8::Value> buffers, v8::Local<v8::Value> samples, StreamEncoder* enc);
        static AsyncEncoderWorkBase* forProcessInterleaved(v8::Local<v8::Value> buffer, v8::Local<v8::Value> samples, StreamEncoder* enc);
        static AsyncEncoderWorkBase* forInitStream(StreamEncoder* enc);
        static AsyncEncoderWorkBase* forInitOggStream(StreamEncoder* enc);
        static AsyncEncoderWorkBase* forInitFile(v8::Local<v8::Value> path, StreamEncoder* enc);
        static AsyncEncoderWorkBase* forInitOggFile(v8::Local<v8::Value> path, StreamEncoder* enc);
    };

}

int encoder_read_callback(const FLAC__StreamEncoder*, char[], size_t*, void*);
int encoder_write_callback(const FLAC__StreamEncoder*, const char[], size_t, unsigned, unsigned, void*);
int encoder_seek_callback(const FLAC__StreamEncoder*, uint64_t, void*);
int encoder_tell_callback(const FLAC__StreamEncoder*, uint64_t*, void*);
void encoder_metadata_callback(const FLAC__StreamEncoder*, const FLAC__StreamMetadata*, void*);
void encoder_progress_callback(const FLAC__StreamEncoder*, uint64_t, uint64_t, unsigned, unsigned, void*);
