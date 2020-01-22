#pragma once

#include "../flac/format.h"
#include "../utils/async.hpp"
#include "../utils/converters.hpp"
#include "../utils/enum.hpp"

namespace flac_bindings {

    using namespace Napi;

    struct DecoderWorkRequest {
        enum Type { Read, Seek, Tell, Length, Eof, Write, Metadata, Error } type;
        uint8_t* buffer = nullptr;
        size_t* bytes = nullptr;
        uint64_t* offset = nullptr;
        const FLAC__Frame* frame = nullptr;
        const int32_t *const* samples = nullptr;
        const FLAC__StreamMetadata* metadata = nullptr;
        int errorCode = 0;
        int returnValue = 0;

        DecoderWorkRequest();
        DecoderWorkRequest(const DecoderWorkRequest &);
        explicit DecoderWorkRequest(DecoderWorkRequest::Type type);
    };

    typedef AsyncBackgroundTask<int, DecoderWorkRequest*> AsyncDecoderWorkBase;

    struct DecoderWorkContext {
        FunctionReference readCbk, seekCbk, tellCbk, lengthCbk, eofCbk, writeCbk, metadataCbk, errorCbk;
        class StreamDecoder* dec;

        DecoderWorkContext(StreamDecoder* dec): dec(dec) {}
    };

    class StreamDecoder: public ObjectWrap<StreamDecoder> {

        friend class AsyncDecoderWork;

        void setOggSerialNumber(const CallbackInfo&);
        Napi::Value getMd5Checking(const CallbackInfo&);
        void setMd5Checking(const CallbackInfo&);
        void setMetadataRespond(const CallbackInfo&);
        void setMetadataIgnore(const CallbackInfo&);
        Napi::Value getTotalSamples(const CallbackInfo&);
        Napi::Value getChannels(const CallbackInfo&);
        Napi::Value getChannelAssignment(const CallbackInfo&);
        Napi::Value getBitsPerSample(const CallbackInfo&);
        Napi::Value getSampleRate(const CallbackInfo&);
        Napi::Value getBlocksize(const CallbackInfo&);

        Napi::Value initStream(const CallbackInfo&);
        Napi::Value initOggStream(const CallbackInfo&);
        Napi::Value initFile(const CallbackInfo&);
        Napi::Value initOggFile(const CallbackInfo&);
        Napi::Value finish(const CallbackInfo&);
        Napi::Value flush(const CallbackInfo&);
        Napi::Value reset(const CallbackInfo&);
        Napi::Value processSingle(const CallbackInfo&);
        Napi::Value processUntilEndOfMetadata(const CallbackInfo&);
        Napi::Value processUntilEndOfStream(const CallbackInfo&);
        Napi::Value skipSingleFrame(const CallbackInfo&);
        Napi::Value seekAbsolute(const CallbackInfo&);
        void setMetadataRespondApplication(const CallbackInfo&);
        void setMetadataRespondAll(const CallbackInfo&);
        void setMetadataIgnoreApplication(const CallbackInfo&);
        void setMetadataIgnoreAll(const CallbackInfo&);
        Napi::Value getState(const CallbackInfo&);
        Napi::Value getResolvedStateString(const CallbackInfo&);
        Napi::Value getDecodePosition(const CallbackInfo&);

        Napi::Value finishAsync(const CallbackInfo&);
        Napi::Value flushAsync(const CallbackInfo&);
        Napi::Value processSingleAsync(const CallbackInfo&);
        Napi::Value processUntilEndOfMetadataAsync(const CallbackInfo&);
        Napi::Value processUntilEndOfStreamAsync(const CallbackInfo&);
        Napi::Value skipSingleFrameAsync(const CallbackInfo&);
        Napi::Value seekAbsoluteAsync(const CallbackInfo&);
        Napi::Value initStreamAsync(const CallbackInfo&);
        Napi::Value initOggStreamAsync(const CallbackInfo&);
        Napi::Value initFileAsync(const CallbackInfo&);
        Napi::Value initOggFileAsync(const CallbackInfo&);

        inline void checkPendingAsyncWork(const Napi::Env& env) {
            if(asyncContext != nullptr) {
                throw Error::New(env, "There is still an operation running on this object");
            }
        }

        void checkIsInitialized(const Napi::Env&);
        void checkIsNotInitialized(const Napi::Env&);
        Promise enqueueWork(AsyncDecoderWorkBase*);
        static int doAsyncWork(DecoderWorkContext* ctx, DecoderWorkRequest* req, int defaultReturnValue = 0);

        static c_enum::DefineReturnType createStateEnum(const Napi::Env&);
        static c_enum::DefineReturnType createInitStatusEnum(const Napi::Env&);
        static c_enum::DefineReturnType createReadStatusEnum(const Napi::Env&);
        static c_enum::DefineReturnType createSeekStatusEnum(const Napi::Env&);
        static c_enum::DefineReturnType createTellStatusEnum(const Napi::Env&);
        static c_enum::DefineReturnType createLengthStatusEnum(const Napi::Env&);
        static c_enum::DefineReturnType createWriteStatusEnum(const Napi::Env&);
        static c_enum::DefineReturnType createErrorStatusEnum(const Napi::Env&);

        static int readCallback(const FLAC__StreamDecoder*, FLAC__byte [], size_t*, void*);
        static int seekCallback(const FLAC__StreamDecoder*, uint64_t, void*);
        static int tellCallback(const FLAC__StreamDecoder*, uint64_t*, void*);
        static int lengthCallback(const FLAC__StreamDecoder*, uint64_t*, void*);
        static FLAC__bool eofCallback(const FLAC__StreamDecoder*, void*);
        static int writeCallback(const FLAC__StreamDecoder*, const FLAC__Frame*, const int32_t *const [], void*);
        static void metadataCallback(const FLAC__StreamDecoder*, const FLAC__StreamMetadata*, void*);
        static void errorCallback(const FLAC__StreamDecoder*, int, void*);

        static FunctionReference constructor;

        FLAC__StreamDecoder* dec = nullptr;
        AsyncContext* asyncContext = nullptr;
        AsyncDecoderWorkBase::ExecutionProgress* asyncExecutionProgress = nullptr;
        std::shared_ptr<DecoderWorkContext> ctx;

    public:

        static Function init(const Napi::Env& env);

        StreamDecoder(const CallbackInfo&);
        ~StreamDecoder();

    };

    class AsyncDecoderWork: public AsyncDecoderWorkBase {

        AsyncDecoderWork(
            const Object&,
            std::function<int()>,
            const char*,
            DecoderWorkContext*,
            std::function<Napi::Value(const Napi::Env&, int)> = booleanToJs<int>
        );

        void onProgress(
            const DecoderWorkContext*,
            Napi::Env&,
            ExecutionProgress&,
            DecoderWorkRequest* const*,
            size_t
        );

        static FunctionCallback decorate(DecoderWorkContext*, const std::function<int()>&);

    public:
        static AsyncDecoderWork* forFinish(const Object&, DecoderWorkContext*);
        static AsyncDecoderWork* forFlush(const Object&, DecoderWorkContext*);
        static AsyncDecoderWork* forProcessSingle(const Object&, DecoderWorkContext*);
        static AsyncDecoderWork* forProcessUntilEndOfMetadata(const Object&, DecoderWorkContext*);
        static AsyncDecoderWork* forProcessUntilEndOfStream(const Object&, DecoderWorkContext*);
        static AsyncDecoderWork* forSkipSingleFrame(const Object&, DecoderWorkContext*);
        static AsyncDecoderWork* forSeekAbsolute(const Object&, uint64_t, DecoderWorkContext*);
        static AsyncDecoderWork* forInitStream(const Object&, DecoderWorkContext*);
        static AsyncDecoderWork* forInitOggStream(const Object&, DecoderWorkContext*);
        static AsyncDecoderWork* forInitFile(const Object&, const std::string&, DecoderWorkContext*);
        static AsyncDecoderWork* forInitOggFile(const Object&, const std::string&, DecoderWorkContext*);

    };

}
