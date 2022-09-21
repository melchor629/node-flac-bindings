#pragma once

#include "../utils/async.hpp"
#include "../utils/converters.hpp"
#include "../utils/enum.hpp"
#include "../utils/pointer.hpp"
#include <FLAC/stream_encoder.h>
#include <variant>

namespace flac_bindings {

  using namespace Napi;

  struct EncoderWorkRequest {
    struct Read {
      FLAC__byte* buffer;
      size_t& bytes;
      FLAC__StreamEncoderReadStatus returnValue = FLAC__STREAM_ENCODER_READ_STATUS_ABORT;

      Read(FLAC__byte* buffer, size_t* bytes): buffer(buffer), bytes(*bytes) {}
    };

    struct Write {
      const FLAC__byte* buffer;
      size_t bytes;
      unsigned samples;
      unsigned frame;
      FLAC__StreamEncoderWriteStatus returnValue = FLAC__STREAM_ENCODER_WRITE_STATUS_FATAL_ERROR;

      Write(const FLAC__byte* buffer, size_t bytes, unsigned samples, unsigned frame):
          buffer(buffer), bytes(bytes), samples(samples), frame(frame) {}
    };

    struct Seek {
      uint64_t offset;
      FLAC__StreamEncoderSeekStatus returnValue = FLAC__STREAM_ENCODER_SEEK_STATUS_ERROR;

      Seek(uint64_t offset): offset(offset) {}
    };

    struct Tell {
      uint64_t& offset;
      FLAC__StreamEncoderTellStatus returnValue = FLAC__STREAM_ENCODER_TELL_STATUS_ERROR;

      Tell(uint64_t* offset): offset(*offset) {}
    };

    struct Metadata {
      const FLAC__StreamMetadata* metadata;

      Metadata(const FLAC__StreamMetadata* metadata): metadata(metadata) {}
    };

    struct Progress {
      uint64_t bytesWritten;
      uint64_t samplesWritten;
      unsigned framesWritten;
      unsigned totalFramesEstimate;

      Progress(
        uint64_t bytesWritten,
        uint64_t samplesWritten,
        unsigned framesWritten,
        unsigned totalFramesEstimate):
          bytesWritten(bytesWritten),
          samplesWritten(samplesWritten), framesWritten(framesWritten),
          totalFramesEstimate(totalFramesEstimate) {}
    };

    std::variant<Read, Write, Seek, Tell, Metadata, Progress> data;

    EncoderWorkRequest(const EncoderWorkRequest& other): data(other.data) {}
    EncoderWorkRequest(const std::variant<Read, Write, Seek, Tell, Metadata, Progress>& data):
        data(data) {}
  };

  typedef AsyncBackgroundTask<int, EncoderWorkRequest> AsyncEncoderWorkBase;

  struct EncoderWorkContext {
    FunctionReference readCbk, writeCbk, seekCbk, tellCbk, metadataCbk, progressCbk;
    std::atomic_bool workInProgress = false;
    AsyncEncoderWorkBase::ExecutionProgress* asyncExecutionProgress = nullptr;
    FLAC__StreamEncoder* enc;
    enum ExecutionMode {
      Sync,
      Async,
    } mode;

    EncoderWorkContext(FLAC__StreamEncoder* encoder, ExecutionMode mode):
        enc(encoder), mode(mode) {}
    virtual ~EncoderWorkContext() {
      if (!readCbk.IsEmpty())
        readCbk.Unref();
      if (!writeCbk.IsEmpty())
        writeCbk.Unref();
      if (!seekCbk.IsEmpty())
        seekCbk.Unref();
      if (!tellCbk.IsEmpty())
        tellCbk.Unref();
      if (!metadataCbk.IsEmpty())
        metadataCbk.Unref();
      if (!progressCbk.IsEmpty())
        progressCbk.Unref();
    }

    inline void runLocked(const std::function<void()>& funcBody) {
      std::lock_guard<std::mutex> lockGuard(this->mutex);
      funcBody();
    }

    template<typename ReturnType>
    inline ReturnType runLocked(const std::function<ReturnType()>& funcBody) {
      std::lock_guard<std::mutex> lockGuard(this->mutex);
      return funcBody();
    }

  private:
    std::mutex mutex;
  };

  class StreamEncoderBuilder: public ObjectWrap<StreamEncoderBuilder> {
    friend class AsyncEncoderWork;
    friend class StreamEncoder;

    Napi::Value getState(const CallbackInfo&);
    Napi::Value getVerify(const CallbackInfo&);
    Napi::Value getStreamableSubset(const CallbackInfo&);
    Napi::Value getChannels(const CallbackInfo&);
    Napi::Value getBitsPerSample(const CallbackInfo&);
    Napi::Value getSampleRate(const CallbackInfo&);
    Napi::Value getBlocksize(const CallbackInfo&);
    Napi::Value getDoMidSideStereo(const CallbackInfo&);
    Napi::Value getLooseMidSideStereo(const CallbackInfo&);
    Napi::Value getMaxLpcOrder(const CallbackInfo&);
    Napi::Value getQlpCoeffPrecision(const CallbackInfo&);
    Napi::Value getDoQlpCoeffPrecSearch(const CallbackInfo&);
    Napi::Value getDoEscapeCoding(const CallbackInfo&);
    Napi::Value getDoExhaustiveModelSearch(const CallbackInfo&);
    Napi::Value getMinResidualPartitionOrder(const CallbackInfo&);
    Napi::Value getMaxResidualPartitionOrder(const CallbackInfo&);
    Napi::Value getRiceParameterSearchDist(const CallbackInfo&);
    Napi::Value getLimitMinBitrate(const CallbackInfo&);
    Napi::Value getTotalSamplesEstimate(const CallbackInfo&);

    Napi::Value setOggSerialNumber(const CallbackInfo&);
    Napi::Value setVerify(const CallbackInfo&);
    Napi::Value setStreamableSubset(const CallbackInfo&);
    Napi::Value setChannels(const CallbackInfo&);
    Napi::Value setBitsPerSample(const CallbackInfo&);
    Napi::Value setSampleRate(const CallbackInfo&);
    Napi::Value setCompressionLevel(const CallbackInfo&);
    Napi::Value setBlocksize(const CallbackInfo&);
    Napi::Value setDoMidSideStereo(const CallbackInfo&);
    Napi::Value setLooseMidSideStereo(const CallbackInfo&);
    Napi::Value setMaxLpcOrder(const CallbackInfo&);
    Napi::Value setQlpCoeffPrecision(const CallbackInfo&);
    Napi::Value setDoQlpCoeffPrecSearch(const CallbackInfo&);
    Napi::Value setDoEscapeCoding(const CallbackInfo&);
    Napi::Value setDoExhaustiveModelSearch(const CallbackInfo&);
    Napi::Value setMinResidualPartitionOrder(const CallbackInfo&);
    Napi::Value setMaxResidualPartitionOrder(const CallbackInfo&);
    Napi::Value setRiceParameterSearchDist(const CallbackInfo&);
    Napi::Value setTotalSamplesEstimate(const CallbackInfo&);
    Napi::Value setMetadata(const CallbackInfo&);
    Napi::Value setApodization(const CallbackInfo&);
    Napi::Value setLimitMinBitrate(const CallbackInfo&);

    Napi::Value buildWithStream(const CallbackInfo&);
    Napi::Value buildWithOggStream(const CallbackInfo&);
    Napi::Value buildWithFile(const CallbackInfo&);
    Napi::Value buildWithOggFile(const CallbackInfo&);

    Napi::Value buildWithStreamAsync(const CallbackInfo&);
    Napi::Value buildWithOggStreamAsync(const CallbackInfo&);
    Napi::Value buildWithFileAsync(const CallbackInfo&);
    Napi::Value buildWithOggFileAsync(const CallbackInfo&);

    Napi::Value createEncoder(Napi::Env, Napi::Value, std::shared_ptr<EncoderWorkContext>);
    void checkInitStatus(Napi::Env env, FLAC__StreamEncoderInitStatus status);
    void checkIfBuilt(Napi::Env env);

    FLAC__StreamEncoder* enc = nullptr;
    std::atomic_bool workInProgress = false;

  public:
    static Function init(Napi::Env env, FlacAddon& addon);

    StreamEncoderBuilder(const CallbackInfo&);
    ~StreamEncoderBuilder();
  };

  class StreamEncoder: public ObjectWrap<StreamEncoder> {
    friend class AsyncEncoderWork;
    friend class StreamEncoderBuilder;

    Napi::Value getVerify(const CallbackInfo&);
    Napi::Value getStreamableSubset(const CallbackInfo&);
    Napi::Value getChannels(const CallbackInfo&);
    Napi::Value getBitsPerSample(const CallbackInfo&);
    Napi::Value getSampleRate(const CallbackInfo&);
    Napi::Value getBlocksize(const CallbackInfo&);
    Napi::Value getDoMidSideStereo(const CallbackInfo&);
    Napi::Value getLooseMidSideStereo(const CallbackInfo&);
    Napi::Value getMaxLpcOrder(const CallbackInfo&);
    Napi::Value getQlpCoeffPrecision(const CallbackInfo&);
    Napi::Value getDoQlpCoeffPrecSearch(const CallbackInfo&);
    Napi::Value getDoEscapeCoding(const CallbackInfo&);
    Napi::Value getDoExhaustiveModelSearch(const CallbackInfo&);
    Napi::Value getMinResidualPartitionOrder(const CallbackInfo&);
    Napi::Value getMaxResidualPartitionOrder(const CallbackInfo&);
    Napi::Value getRiceParameterSearchDist(const CallbackInfo&);
    Napi::Value getLimitMinBitrate(const CallbackInfo&);
    Napi::Value getTotalSamplesEstimate(const CallbackInfo&);
    Napi::Value getState(const CallbackInfo&);
    Napi::Value getVerifyDecoderState(const CallbackInfo&);
    Napi::Value getResolvedStateString(const CallbackInfo&);
    Napi::Value getVerifyDecoderErrorStats(const CallbackInfo&);

    Napi::Value finish(const CallbackInfo&);
    Napi::Value process(const CallbackInfo&);
    Napi::Value processInterleaved(const CallbackInfo&);

    Napi::Value finishAsync(const CallbackInfo&);
    Napi::Value processAsync(const CallbackInfo&);
    Napi::Value processInterleavedAsync(const CallbackInfo&);

    inline void checkPendingAsyncWork(
      const Napi::Env& env,
      std::optional<EncoderWorkContext::ExecutionMode> mode = std::nullopt) {
      this->ctx->runLocked([this, &env, &mode]() {
        if (this->ctx->workInProgress.load()) {
          throw Error::New(env, "There is still an operation running on this object");
        }

        if (mode != std::nullopt && this->ctx->mode != mode) {
          auto modeString =
            (this->ctx->mode == EncoderWorkContext::ExecutionMode::Sync ? "synchronous"
                                                                        : "asynchronous");
          throw Error::New(
            env,
            "This method cannot be called when Encoder has been created using "s + modeString
              + " method"s);
        }
      });
    }

    Promise enqueueWork(AsyncEncoderWorkBase*);

    static c_enum::DefineReturnType createStateEnum(const Napi::Env&);
    static c_enum::DefineReturnType createInitStatusEnum(const Napi::Env&);
    static c_enum::DefineReturnType createReadStatusEnum(const Napi::Env&);
    static c_enum::DefineReturnType createWriteStatusEnum(const Napi::Env&);
    static c_enum::DefineReturnType createSeekStatusEnum(const Napi::Env&);
    static c_enum::DefineReturnType createTellStatusEnum(const Napi::Env&);

    static FLAC__StreamEncoderReadStatus
      readCallback(const FLAC__StreamEncoder*, FLAC__byte[], size_t*, void*);
    static FLAC__StreamEncoderWriteStatus writeCallback(
      const FLAC__StreamEncoder*,
      const FLAC__byte[],
      size_t,
      unsigned,
      unsigned,
      void*);
    static FLAC__StreamEncoderSeekStatus seekCallback(const FLAC__StreamEncoder*, uint64_t, void*);
    static FLAC__StreamEncoderTellStatus tellCallback(const FLAC__StreamEncoder*, uint64_t*, void*);
    static void metadataCallback(const FLAC__StreamEncoder*, const FLAC__StreamMetadata*, void*);
    static void
      progressCallback(const FLAC__StreamEncoder*, uint64_t, uint64_t, unsigned, unsigned, void*);

    FLAC__StreamEncoder* enc = nullptr;
    std::shared_ptr<EncoderWorkContext> ctx;
    Napi::ObjectReference builder;

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
      std::function<Napi::Value(const Napi::Env&, int)> = booleanToJs<int>);

    void onProgress(
      const EncoderWorkContext*,
      Napi::Env&,
      ExecutionProgress&,
      const std::shared_ptr<EncoderWorkRequest>&);

    static FunctionCallback decorate(EncoderWorkContext*, const std::function<int()>&);

    static FLAC__StreamEncoderReadStatus
      readCallback(const FLAC__StreamEncoder*, FLAC__byte[], size_t*, void*);
    static FLAC__StreamEncoderWriteStatus writeCallback(
      const FLAC__StreamEncoder*,
      const FLAC__byte[],
      size_t,
      unsigned,
      unsigned,
      void*);
    static FLAC__StreamEncoderSeekStatus seekCallback(const FLAC__StreamEncoder*, uint64_t, void*);
    static FLAC__StreamEncoderTellStatus tellCallback(const FLAC__StreamEncoder*, uint64_t*, void*);
    static void metadataCallback(const FLAC__StreamEncoder*, const FLAC__StreamMetadata*, void*);
    static void
      progressCallback(const FLAC__StreamEncoder*, uint64_t, uint64_t, unsigned, unsigned, void*);

    pointer::BufferReference<FLAC__byte> sharedBufferRef;

  public:
    static AsyncEncoderWork* forFinish(const StoreList&, StreamEncoder& encoder);
    static AsyncEncoderWork* forProcess(
      const StoreList&,
      const std::vector<int32_t*>& buffers,
      uint64_t samples,
      EncoderWorkContext* ctx);
    static AsyncEncoderWork* forProcessInterleaved(
      const StoreList&,
      int32_t* buffer,
      uint64_t samples,
      EncoderWorkContext* ctx);
    static AsyncEncoderWork* forInitStream(
      const StoreList&,
      std::shared_ptr<EncoderWorkContext> ctx,
      StreamEncoderBuilder&);
    static AsyncEncoderWork* forInitOggStream(
      const StoreList&,
      std::shared_ptr<EncoderWorkContext> ctx,
      StreamEncoderBuilder&);
    static AsyncEncoderWork* forInitFile(
      const StoreList&,
      const std::string& path,
      std::shared_ptr<EncoderWorkContext> ctx,
      StreamEncoderBuilder&);
    static AsyncEncoderWork* forInitOggFile(
      const StoreList&,
      const std::string& path,
      std::shared_ptr<EncoderWorkContext> ctx,
      StreamEncoderBuilder&);
  };

}
