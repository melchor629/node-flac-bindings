#pragma once

#include "../utils/async.hpp"
#include "../utils/converters.hpp"
#include "../utils/enum.hpp"
#include "../utils/pointer.hpp"
#include <FLAC/stream_decoder.h>
#include <variant>

namespace flac_bindings {

  using namespace Napi;

  struct DecoderWorkRequest {
    struct Read {
      FLAC__byte* buffer;
      size_t& bytes;
      FLAC__StreamDecoderReadStatus returnValue = FLAC__STREAM_DECODER_READ_STATUS_ABORT;

      Read(FLAC__byte* buffer, size_t* bytes): buffer(buffer), bytes(*bytes) {}
    };

    struct Seek {
      uint64_t offset;
      FLAC__StreamDecoderSeekStatus returnValue = FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;
    };

    struct Tell {
      uint64_t& offset;
      FLAC__StreamDecoderTellStatus returnValue = FLAC__STREAM_DECODER_TELL_STATUS_ERROR;

      Tell(uint64_t* offset): offset(*offset) {}
    };

    struct Length {
      uint64_t& length;
      FLAC__StreamDecoderLengthStatus returnValue = FLAC__STREAM_DECODER_LENGTH_STATUS_ERROR;

      Length(uint64_t* length): length(*length) {}
    };

    struct Eof {
      FLAC__bool returnValue = true;
    };

    struct Write {
      const FLAC__Frame* frame;
      const int32_t* const* samples;
      FLAC__StreamDecoderWriteStatus returnValue = FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
    };

    struct Metadata {
      const FLAC__StreamMetadata* metadata;
    };

    struct Error {
      FLAC__StreamDecoderErrorStatus error = FLAC__STREAM_DECODER_ERROR_STATUS_LOST_SYNC;
    };

    std::variant<Read, Seek, Tell, Length, Eof, Write, Metadata, Error> data;

    DecoderWorkRequest(const DecoderWorkRequest& req): data(req.data) {}
    DecoderWorkRequest(
      const std::variant<Read, Seek, Tell, Length, Eof, Write, Metadata, Error>& data):
        data(data) {}
  };

  typedef AsyncBackgroundTask<
    std::variant<int, uint64_t, FLAC__StreamDecoderInitStatus>,
    DecoderWorkRequest>
    AsyncDecoderWorkBase;

  struct DecoderWorkContext {
    FunctionReference readCbk, seekCbk, tellCbk, lengthCbk, eofCbk, writeCbk, metadataCbk, errorCbk;
    std::atomic_bool workInProgress = false;
    AsyncDecoderWorkBase::ExecutionProgress* asyncExecutionProgress = nullptr;
    FLAC__StreamDecoder* dec;
    enum ExecutionMode {
      Sync,
      Async,
    } mode;

    DecoderWorkContext(FLAC__StreamDecoder* decoder, ExecutionMode mode):
        dec(decoder), mode(mode) {}
    virtual ~DecoderWorkContext() {
      if (!readCbk.IsEmpty())
        readCbk.Unref();
      if (!seekCbk.IsEmpty())
        seekCbk.Unref();
      if (!tellCbk.IsEmpty())
        tellCbk.Unref();
      if (!lengthCbk.IsEmpty())
        lengthCbk.Unref();
      if (!eofCbk.IsEmpty())
        eofCbk.Unref();
      if (!writeCbk.IsEmpty())
        writeCbk.Unref();
      if (!metadataCbk.IsEmpty())
        metadataCbk.Unref();
      if (!errorCbk.IsEmpty())
        errorCbk.Unref();
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

  class StreamDecoderBuilder: public ObjectWrap<StreamDecoderBuilder> {
    friend class AsyncDecoderWork;
    friend class StreamDecoder;

    Napi::Value getState(const CallbackInfo&);
    Napi::Value getMd5Checking(const CallbackInfo&);

    Napi::Value setOggSerialNumber(const CallbackInfo&);
    Napi::Value setMd5Checking(const CallbackInfo&);
    Napi::Value setMetadataRespond(const CallbackInfo&);
    Napi::Value setMetadataRespondAll(const CallbackInfo&);
    Napi::Value setMetadataRespondApplication(const CallbackInfo&);
    Napi::Value setMetadataIgnore(const CallbackInfo&);
    Napi::Value setMetadataIgnoreAll(const CallbackInfo&);
    Napi::Value setMetadataIgnoreApplication(const CallbackInfo&);

    Napi::Value buildWithStream(const CallbackInfo&);
    Napi::Value buildWithOggStream(const CallbackInfo&);
    Napi::Value buildWithFile(const CallbackInfo&);
    Napi::Value buildWithOggFile(const CallbackInfo&);

    Napi::Value buildWithStreamAsync(const CallbackInfo&);
    Napi::Value buildWithOggStreamAsync(const CallbackInfo&);
    Napi::Value buildWithFileAsync(const CallbackInfo&);
    Napi::Value buildWithOggFileAsync(const CallbackInfo&);

    Napi::Value createDecoder(Napi::Env, Napi::Value, std::shared_ptr<DecoderWorkContext>);
    void checkInitStatus(Napi::Env env, FLAC__StreamDecoderInitStatus status);
    void checkIfBuilt(Napi::Env env);

    FLAC__StreamDecoder* dec = nullptr;
    std::atomic_bool workInProgress = false;

  public:
    static Function init(Napi::Env, FlacAddon&);

    StreamDecoderBuilder(const CallbackInfo&);
    ~StreamDecoderBuilder();
  };

  class StreamDecoder: public ObjectWrap<StreamDecoder> {
    friend class StreamDecoderBuilder;
    friend class AsyncDecoderWork;

    Napi::Value getMd5Checking(const CallbackInfo&);
    Napi::Value getTotalSamples(const CallbackInfo&);
    Napi::Value getChannels(const CallbackInfo&);
    Napi::Value getChannelAssignment(const CallbackInfo&);
    Napi::Value getBitsPerSample(const CallbackInfo&);
    Napi::Value getSampleRate(const CallbackInfo&);
    Napi::Value getBlocksize(const CallbackInfo&);

    Napi::Value finish(const CallbackInfo&);
    Napi::Value flush(const CallbackInfo&);
    Napi::Value reset(const CallbackInfo&);
    Napi::Value processSingle(const CallbackInfo&);
    Napi::Value processUntilEndOfMetadata(const CallbackInfo&);
    Napi::Value processUntilEndOfStream(const CallbackInfo&);
    Napi::Value skipSingleFrame(const CallbackInfo&);
    Napi::Value seekAbsolute(const CallbackInfo&);
    Napi::Value getDecodePosition(const CallbackInfo&);

    Napi::Value getState(const CallbackInfo&);
    Napi::Value getResolvedStateString(const CallbackInfo&);

    Napi::Value finishAsync(const CallbackInfo&);
    Napi::Value flushAsync(const CallbackInfo&);
    Napi::Value processSingleAsync(const CallbackInfo&);
    Napi::Value processUntilEndOfMetadataAsync(const CallbackInfo&);
    Napi::Value processUntilEndOfStreamAsync(const CallbackInfo&);
    Napi::Value skipSingleFrameAsync(const CallbackInfo&);
    Napi::Value seekAbsoluteAsync(const CallbackInfo&);
    Napi::Value getDecodePositionAsync(const CallbackInfo&);

    inline void checkPendingAsyncWork(
      const Napi::Env& env,
      std::optional<DecoderWorkContext::ExecutionMode> mode = std::nullopt) {
      this->ctx->runLocked([this, &env, &mode]() {
        if (this->ctx->workInProgress.load()) {
          throw Error::New(env, "There is still an operation running on this object");
        }

        if (mode != std::nullopt && this->ctx->mode != mode) {
          auto modeString =
            (this->ctx->mode == DecoderWorkContext::ExecutionMode::Sync ? "synchronous"
                                                                        : "asynchronous");
          throw Error::New(
            env,
            "This method cannot be called when Decoder has been created using "s + modeString
              + " method"s);
        }
      });
    }

    Promise enqueueWork(AsyncDecoderWorkBase*);

    static c_enum::DefineReturnType createStateEnum(const Napi::Env&);
    static c_enum::DefineReturnType createInitStatusEnum(const Napi::Env&);
    static c_enum::DefineReturnType createReadStatusEnum(const Napi::Env&);
    static c_enum::DefineReturnType createSeekStatusEnum(const Napi::Env&);
    static c_enum::DefineReturnType createTellStatusEnum(const Napi::Env&);
    static c_enum::DefineReturnType createLengthStatusEnum(const Napi::Env&);
    static c_enum::DefineReturnType createWriteStatusEnum(const Napi::Env&);
    static c_enum::DefineReturnType createErrorStatusEnum(const Napi::Env&);

    static FLAC__StreamDecoderReadStatus
      readCallback(const FLAC__StreamDecoder*, FLAC__byte[], size_t*, void*);
    static FLAC__StreamDecoderSeekStatus seekCallback(const FLAC__StreamDecoder*, uint64_t, void*);
    static FLAC__StreamDecoderTellStatus tellCallback(const FLAC__StreamDecoder*, uint64_t*, void*);
    static FLAC__StreamDecoderLengthStatus
      lengthCallback(const FLAC__StreamDecoder*, uint64_t*, void*);
    static FLAC__bool eofCallback(const FLAC__StreamDecoder*, void*);
    static FLAC__StreamDecoderWriteStatus
      writeCallback(const FLAC__StreamDecoder*, const FLAC__Frame*, const int32_t* const[], void*);
    static void metadataCallback(const FLAC__StreamDecoder*, const FLAC__StreamMetadata*, void*);
    static void errorCallback(const FLAC__StreamDecoder*, FLAC__StreamDecoderErrorStatus, void*);

    FLAC__StreamDecoder* dec = nullptr;
    std::shared_ptr<DecoderWorkContext> ctx;
    ObjectReference builder;

  public:
    static Function init(Napi::Env env, FlacAddon& addon);

    StreamDecoder(const CallbackInfo&);
    ~StreamDecoder();
  };

  class AsyncDecoderWork: public AsyncDecoderWorkBase {

    typedef std::initializer_list<Napi::Value> StoreList;

    AsyncDecoderWork(
      const StoreList&,
      std::function<ValueType()>,
      const char*,
      DecoderWorkContext*,
      std::function<Napi::Value(const Napi::Env&, ValueType)>);

    void onProgress(
      const DecoderWorkContext*,
      Napi::Env&,
      ExecutionProgress&,
      const std::shared_ptr<DecoderWorkRequest>&);

    static FunctionCallback decorate(DecoderWorkContext*, const std::function<ValueType()>&);

    static FLAC__StreamDecoderReadStatus
      readCallback(const FLAC__StreamDecoder*, FLAC__byte[], size_t*, void*);
    static FLAC__StreamDecoderSeekStatus seekCallback(const FLAC__StreamDecoder*, uint64_t, void*);
    static FLAC__StreamDecoderTellStatus tellCallback(const FLAC__StreamDecoder*, uint64_t*, void*);
    static FLAC__StreamDecoderLengthStatus
      lengthCallback(const FLAC__StreamDecoder*, uint64_t*, void*);
    static FLAC__bool eofCallback(const FLAC__StreamDecoder*, void*);
    static FLAC__StreamDecoderWriteStatus
      writeCallback(const FLAC__StreamDecoder*, const FLAC__Frame*, const int32_t* const[], void*);
    static void metadataCallback(const FLAC__StreamDecoder*, const FLAC__StreamMetadata*, void*);
    static void errorCallback(const FLAC__StreamDecoder*, FLAC__StreamDecoderErrorStatus, void*);

    pointer::BufferReference<FLAC__byte> readSharedBufferRef;
    pointer::BufferReference<int32_t> writeSharedBufferRefs[FLAC__MAX_CHANNELS];

  public:
    static AsyncDecoderWork* forFinish(const StoreList&, StreamDecoder&);
    static AsyncDecoderWork* forFlush(const StoreList&, DecoderWorkContext*);
    static AsyncDecoderWork* forProcessSingle(const StoreList&, DecoderWorkContext*);
    static AsyncDecoderWork* forProcessUntilEndOfMetadata(const StoreList&, DecoderWorkContext*);
    static AsyncDecoderWork* forProcessUntilEndOfStream(const StoreList&, DecoderWorkContext*);
    static AsyncDecoderWork* forSkipSingleFrame(const StoreList&, DecoderWorkContext*);
    static AsyncDecoderWork* forSeekAbsolute(const StoreList&, uint64_t, DecoderWorkContext*);
    static AsyncDecoderWork*
      forInitStream(const StoreList&, std::shared_ptr<DecoderWorkContext>, StreamDecoderBuilder&);
    static AsyncDecoderWork* forInitOggStream(
      const StoreList&,
      std::shared_ptr<DecoderWorkContext>,
      StreamDecoderBuilder&);
    static AsyncDecoderWork* forInitFile(
      const StoreList&,
      const std::string&,
      std::shared_ptr<DecoderWorkContext>,
      StreamDecoderBuilder&);
    static AsyncDecoderWork* forInitOggFile(
      const StoreList&,
      const std::string&,
      std::shared_ptr<DecoderWorkContext>,
      StreamDecoderBuilder&);
    static AsyncDecoderWork* forGetDecoderPosition(const StoreList&, DecoderWorkContext*);
  };

}
