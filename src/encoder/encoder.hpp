#pragma once

#include <FLAC/stream_encoder.h>
#include "../utils/async.hpp"
#include "../utils/converters.hpp"
#include "../utils/enum.hpp"

namespace flac_bindings {

    using namespace Napi;

    struct EncoderWorkRequest {
        enum Type { Read, Write, Seek, Tell, Metadata, Progress } type;
        int returnValue = 0;
        FLAC__byte* buffer = nullptr;
        size_t* bytes = nullptr;
        const FLAC__byte* constBuffer = nullptr;
        unsigned samples = 0;
        unsigned frame = 0;
        uint64_t* offset = nullptr;
        const FLAC__StreamMetadata* metadata = nullptr;
        struct {
            uint64_t bytesWritten = 0;
            uint64_t samplesWritten = 0;
            unsigned framesWritten = 0;
            unsigned totalFramesEstimate = 0;
        } progress;

        EncoderWorkRequest();
        EncoderWorkRequest(const EncoderWorkRequest &);
        explicit EncoderWorkRequest(EncoderWorkRequest::Type type);
    };

    typedef AsyncBackgroundTask<int, EncoderWorkRequest*> AsyncEncoderWorkBase;

    struct EncoderWorkContext {
        FunctionReference readCbk, writeCbk, seekCbk, tellCbk, metadataCbk, progressCbk;
        class StreamEncoder* enc;

        EncoderWorkContext(StreamEncoder* enc): enc(enc) {}
        virtual ~EncoderWorkContext() {
            if(!readCbk.IsEmpty()) readCbk.Unref();
            if(!writeCbk.IsEmpty()) writeCbk.Unref();
            if(!seekCbk.IsEmpty()) seekCbk.Unref();
            if(!tellCbk.IsEmpty()) tellCbk.Unref();
            if(!metadataCbk.IsEmpty()) metadataCbk.Unref();
            if(!progressCbk.IsEmpty()) progressCbk.Unref();
        }
    };

    class StreamEncoder: public ObjectWrap<StreamEncoder> {

        friend class AsyncEncoderWork;

        void setOggSerialNumber(const CallbackInfo&);
        Napi::Value getVerify(const CallbackInfo&);
        void setVerify(const CallbackInfo&, const Napi::Value&);
        Napi::Value getStreamableSubset(const CallbackInfo&);
        void setStreamableSubset(const CallbackInfo&, const Napi::Value&);
        Napi::Value getChannels(const CallbackInfo&);
        void setChannels(const CallbackInfo&, const Napi::Value&);
        Napi::Value getBitsPerSample(const CallbackInfo&);
        void setBitsPerSample(const CallbackInfo&, const Napi::Value&);
        Napi::Value getSampleRate(const CallbackInfo&);
        void setSampleRate(const CallbackInfo&, const Napi::Value&);
        void setCompressionLevel(const CallbackInfo&);
        Napi::Value getBlocksize(const CallbackInfo&);
        void setBlocksize(const CallbackInfo&, const Napi::Value&);
        Napi::Value getDoMidSideStereo(const CallbackInfo&);
        void setDoMidSideStereo(const CallbackInfo&, const Napi::Value&);
        Napi::Value getLooseMidSideStereo(const CallbackInfo&);
        void setLooseMidSideStereo(const CallbackInfo&, const Napi::Value&);
        Napi::Value getMaxLpcOrder(const CallbackInfo&);
        void setMaxLpcOrder(const CallbackInfo&, const Napi::Value&);
        Napi::Value getQlpCoeffPrecision(const CallbackInfo&);
        void setQlpCoeffPrecision(const CallbackInfo&, const Napi::Value&);
        Napi::Value getDoQlpCoeffPrecSearch(const CallbackInfo&);
        void setDoQlpCoeffPrecSearch(const CallbackInfo&, const Napi::Value&);
        Napi::Value getDoEscapeCoding(const CallbackInfo&);
        void setDoEscapeCoding(const CallbackInfo&, const Napi::Value&);
        Napi::Value getDoExhaustiveModelSearch(const CallbackInfo&);
        void setDoExhaustiveModelSearch(const CallbackInfo&, const Napi::Value&);
        Napi::Value getMinResidualPartitionOrder(const CallbackInfo&);
        void setMinResidualPartitionOrder(const CallbackInfo&, const Napi::Value&);
        Napi::Value getMaxResidualPartitionOrder(const CallbackInfo&);
        void setMaxResidualPartitionOrder(const CallbackInfo&, const Napi::Value&);
        Napi::Value getRiceParameterSearchDist(const CallbackInfo&);
        void setRiceParameterSearchDist(const CallbackInfo&, const Napi::Value&);
        Napi::Value getTotalSamplesEstimate(const CallbackInfo&);
        void setTotalSamplesEstimate(const CallbackInfo&, const Napi::Value&);
        void setMetadata(const CallbackInfo&);
        void setApodization(const CallbackInfo&);
        Napi::Value getState(const CallbackInfo&);
        Napi::Value getVerifyDecoderState(const CallbackInfo&);
        Napi::Value getResolvedStateString(const CallbackInfo&);
        Napi::Value getVerifyDecoderErrorStats(const CallbackInfo&);

        Napi::Value initStream(const CallbackInfo&);
        Napi::Value initOggStream(const CallbackInfo&);
        Napi::Value initFile(const CallbackInfo&);
        Napi::Value initOggFile(const CallbackInfo&);
        Napi::Value finish(const CallbackInfo&);
        Napi::Value process(const CallbackInfo&);
        Napi::Value processInterleaved(const CallbackInfo&);

        Napi::Value finishAsync(const CallbackInfo&);
        Napi::Value processAsync(const CallbackInfo&);
        Napi::Value processInterleavedAsync(const CallbackInfo&);
        Napi::Value initStreamAsync(const CallbackInfo&);
        Napi::Value initOggStreamAsync(const CallbackInfo&);
        Napi::Value initFileAsync(const CallbackInfo&);
        Napi::Value initOggFileAsync(const CallbackInfo&);

        inline void checkPendingAsyncWork(const Napi::Env& env) {
            std::lock_guard<std::mutex> lockGuard(this->mutex);
            if(busy) {
                throw Error::New(env, "There is still an operation running on this object");
            }
        }

        inline void runLocked(const std::function<void()>& funcBody) {
            std::lock_guard<std::mutex> lockGuard(this->mutex);
            funcBody();
        }

        void checkIsInitialized(const Napi::Env&);
        void checkIsNotInitialized(const Napi::Env&);
        Promise enqueueWork(AsyncEncoderWorkBase*);
        template<typename EnumType>
        static EnumType doAsyncWork(EncoderWorkContext* ctx, EncoderWorkRequest* req, EnumType defaultReturnValue);

        static c_enum::DefineReturnType createStateEnum(const Napi::Env&);
        static c_enum::DefineReturnType createInitStatusEnum(const Napi::Env&);
        static c_enum::DefineReturnType createReadStatusEnum(const Napi::Env&);
        static c_enum::DefineReturnType createWriteStatusEnum(const Napi::Env&);
        static c_enum::DefineReturnType createSeekStatusEnum(const Napi::Env&);
        static c_enum::DefineReturnType createTellStatusEnum(const Napi::Env&);

        static FLAC__StreamEncoderReadStatus readCallback(const FLAC__StreamEncoder*, FLAC__byte[], size_t*, void*);
        static FLAC__StreamEncoderWriteStatus writeCallback(const FLAC__StreamEncoder*, const FLAC__byte[], size_t, unsigned, unsigned, void*);
        static FLAC__StreamEncoderSeekStatus seekCallback(const FLAC__StreamEncoder*, uint64_t, void*);
        static FLAC__StreamEncoderTellStatus tellCallback(const FLAC__StreamEncoder*, uint64_t*, void*);
        static void metadataCallback(const FLAC__StreamEncoder*, const FLAC__StreamMetadata*, void*);
        static void progressCallback(const FLAC__StreamEncoder*, uint64_t, uint64_t, unsigned, unsigned, void*);

        FLAC__StreamEncoder* enc = nullptr;
        volatile bool busy = false;
        AsyncEncoderWorkBase::ExecutionProgress* asyncExecutionProgress = nullptr;
        std::shared_ptr<EncoderWorkContext> ctx;
        ObjectReference metadataArrayRef;
        std::mutex mutex;

    public:

        static Function init(Napi::Env env, FlacAddon& addon);

        StreamEncoder(const CallbackInfo&);
        ~StreamEncoder();

    };

    class AsyncEncoderWork: public AsyncEncoderWorkBase {

        typedef std::initializer_list<Napi::Value> StoreList;

        AsyncEncoderWork(
            const StoreList&,
            std::function<int()>,
            const char*,
            EncoderWorkContext*,
            std::function<Napi::Value(const Napi::Env&, int)> = booleanToJs<int>
        );

        void onProgress(
            const EncoderWorkContext*,
            Napi::Env&,
            ExecutionProgress&,
            EncoderWorkRequest* const*,
            size_t
        );

        static FunctionCallback decorate(EncoderWorkContext*, const std::function<int()>&);

        pointer::BufferReference<FLAC__byte> sharedBufferRef;

    public:
        static AsyncEncoderWork* forFinish(const StoreList&, EncoderWorkContext* ctx);
        static AsyncEncoderWork* forProcess(const StoreList&, const std::vector<int32_t*>& buffers, uint64_t samples, EncoderWorkContext* ctx);
        static AsyncEncoderWork* forProcessInterleaved(const StoreList&, int32_t* buffer, uint64_t samples, EncoderWorkContext* ctx);
        static AsyncEncoderWork* forInitStream(const StoreList&, EncoderWorkContext* ctx);
        static AsyncEncoderWork* forInitOggStream(const StoreList&, EncoderWorkContext* ctx);
        static AsyncEncoderWork* forInitFile(const StoreList&, const std::string& path, EncoderWorkContext* ctx);
        static AsyncEncoderWork* forInitOggFile(const StoreList&, const std::string& path, EncoderWorkContext* ctx);

    };

}