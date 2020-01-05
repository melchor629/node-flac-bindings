#pragma once

#include <stdint.h>
#include <mutex>
#include <condition_variable>
#include <nan.h>

#include "../utils/async.hpp"
#include "../format/format.h"
#include "../utils/defs.hpp"


#define _JOIN(a, b) a##b
#define _JOIN2(a,b,c) a##b##c

#ifdef ENCODER_IMPL
#define _extern
#else
#define _extern extern
#endif

#define FLAC_FUNC(returnType, fn, ...) \
    typedef returnType (*_JOIN2(FLAC__stream_encoder_, fn, _t))(__VA_ARGS__); \
    _extern _JOIN2(FLAC__stream_encoder_, fn, _t) _JOIN(FLAC__stream_encoder_, fn);

#define FLAC_GETTER(type, fn) FLAC_FUNC(type, _JOIN(get_, fn), const FLAC__StreamEncoder*);

#define FLAC_SETTER(type, fn) FLAC_FUNC(FLAC__bool, _JOIN(set_, fn), FLAC__StreamEncoder*, type)

#define FLAC_GETTER_SETTER(type, fn) \
FLAC_GETTER(type, fn); \
FLAC_SETTER(type, fn);

extern "C" {
    typedef int (*FLAC__StreamEncoderReadCallback)(const FLAC__StreamEncoder *encoder, char buffer[], size_t *bytes, void *client_data);
    typedef int (*FLAC__StreamEncoderWriteCallback)(const FLAC__StreamEncoder *encoder, const char buffer[], size_t bytes, unsigned samples, unsigned current_frame, void *client_data);
    typedef int (*FLAC__StreamEncoderSeekCallback)(const FLAC__StreamEncoder *encoder, uint64_t absolute_byte_offset, void *client_data);
    typedef int (*FLAC__StreamEncoderTellCallback)(const FLAC__StreamEncoder *encoder, uint64_t *absolute_byte_offset, void *client_data);
    typedef void(*FLAC__StreamEncoderMetadataCallback)(const FLAC__StreamEncoder *encoder, const FLAC__StreamMetadata *metadata, void *client_data);
    typedef void(*FLAC__StreamEncoderProgressCallback)(const FLAC__StreamEncoder *encoder, uint64_t bytes_written, uint64_t samples_written, unsigned frames_written, unsigned total_frames_estimate, void *client_data);

    FLAC_FUNC(FLAC__StreamEncoder*, new, void);
    FLAC_FUNC(void, delete, FLAC__StreamEncoder*);
    FLAC_FUNC(int, init_stream, FLAC__StreamEncoder*, FLAC__StreamEncoderWriteCallback, FLAC__StreamEncoderSeekCallback, FLAC__StreamEncoderTellCallback, FLAC__StreamEncoderMetadataCallback, void*);
    FLAC_FUNC(int, init_ogg_stream, FLAC__StreamEncoder*, FLAC__StreamEncoderReadCallback, FLAC__StreamEncoderWriteCallback, FLAC__StreamEncoderSeekCallback, FLAC__StreamEncoderTellCallback, FLAC__StreamEncoderMetadataCallback, void*);
    FLAC_FUNC(int, init_file, FLAC__StreamEncoder*, const char*, FLAC__StreamEncoderProgressCallback, void*);
    FLAC_FUNC(int, init_ogg_file, FLAC__StreamEncoder*, const char*, FLAC__StreamEncoderProgressCallback, void*);
    FLAC_FUNC(FLAC__bool, finish, FLAC__StreamEncoder*);
    FLAC_FUNC(FLAC__bool, process, FLAC__StreamEncoder*, const int32_t* const [], unsigned);
    FLAC_FUNC(FLAC__bool, process_interleaved, FLAC__StreamEncoder*, const int32_t [], unsigned);
    FLAC_FUNC(FLAC__bool, set_apodization, FLAC__StreamEncoder*, const char*);
    FLAC_FUNC(const char*, get_resolved_state_string, const FLAC__StreamEncoder*);
    FLAC_FUNC(void, get_verify_decoder_error_stats, const FLAC__StreamEncoder*, uint64_t*, unsigned*, unsigned*, unsigned*, int32_t*, int32_t*);
    FLAC_FUNC(FLAC__bool, set_metadata, FLAC__StreamEncoder*, FLAC__StreamMetadata**, unsigned);

    FLAC_SETTER(long, ogg_serial_number);
    FLAC_GETTER_SETTER(FLAC__bool, verify);
    FLAC_GETTER_SETTER(FLAC__bool, streamable_subset);
    FLAC_GETTER_SETTER(unsigned, channels);
    FLAC_GETTER_SETTER(unsigned, bits_per_sample);
    FLAC_GETTER_SETTER(unsigned, sample_rate);
    FLAC_SETTER(unsigned, compression_level);
    FLAC_GETTER_SETTER(unsigned, blocksize);
    FLAC_GETTER_SETTER(FLAC__bool, do_mid_side_stereo);
    FLAC_GETTER_SETTER(FLAC__bool, loose_mid_side_stereo);
    FLAC_GETTER_SETTER(unsigned, max_lpc_order);
    FLAC_GETTER_SETTER(unsigned, qlp_coeff_precision);
    FLAC_GETTER_SETTER(FLAC__bool, do_qlp_coeff_prec_search);
    FLAC_GETTER_SETTER(FLAC__bool, do_escape_coding);
    FLAC_GETTER_SETTER(FLAC__bool, do_exhaustive_model_search);
    FLAC_GETTER_SETTER(unsigned, min_residual_partition_order);
    FLAC_GETTER_SETTER(unsigned, max_residual_partition_order);
    FLAC_GETTER_SETTER(unsigned, rice_parameter_search_dist);
    FLAC_GETTER_SETTER(uint64_t, total_samples_estimate);
    FLAC_GETTER(int, state);
    FLAC_GETTER(int, verify_decoder_state);
}

#undef _JOIN
#undef _JOIN2
#undef FLAC_FUNC
#undef FLAC_GETTER
#undef FLAC_SETTER
#undef FLAC_GETTER_SETTER

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
