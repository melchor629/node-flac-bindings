#include "converters.hpp"
#include "js_utils.hpp"
#include "pointer.hpp"

namespace flac_bindings {

  static int64_t readI32(const void* buff) {
    return *((const int32_t*) buff);
  }

  static void writeI32(void* buff, int64_t value) {
    *((int32_t*) buff) = value;
  }

  static int64_t readI24(const void* buff) {
    const auto tmp = (const uint8_t*) buff;
    const uint64_t val = tmp[0] | (tmp[1] << 8) | (tmp[2] << 16);
    return val | (val & 0x800000) * 0x1FE;
  }

  static void writeI24(void* buff, int64_t value) {
    auto tmp = (uint8_t*) buff;
    tmp[0] = value & 0xFF;
    tmp[1] = (value >> 8) & 0xFF;
    tmp[2] = (value >> 16) & 0xFF;
  }

  static int64_t readI16(const void* buff) {
    return *((const int16_t*) buff);
  }

  static void writeI16(void* buff, int64_t value) {
    *((int16_t*) buff) = value;
  }

  static int64_t readI8(const void* buff) {
    return *((const int8_t*) buff);
  }

  static void writeI8(void* buff, int64_t value) {
    *((int8_t*) buff) = value;
  }

  typedef int64_t (*BufferReader)(const void* buff);
  static inline BufferReader getReader(uint64_t inBps, Napi::Env env) {
    switch (inBps) {
      case 1:
        return readI8;
      case 2:
        return readI16;
      case 3:
        return readI24;
      case 4:
        return readI32;
      default:
        throw Napi::Error::New(env, "Unsupported "s + std::to_string(inBps) + " bits per sample"s);
    }
  }

  typedef void (*BufferWriter)(void* buff, int64_t);
  static inline BufferWriter getWriter(uint64_t outBps, Napi::Env env) {
    switch (outBps) {
      case 1:
        return writeI8;
      case 2:
        return writeI16;
      case 3:
        return writeI24;
      case 4:
        return writeI32;
      default:
        throw Napi::Error::New(env, "Unsupported "s + std::to_string(outBps) + " bits per sample"s);
    }
  }

  static Napi::Value zipAudio(const Napi::CallbackInfo& info) {
    using namespace Napi;
    EscapableHandleScope scope(info.Env());

    if (!info[0].IsObject()) {
      throw TypeError::New(info.Env(), "Expected first argument to be object");
    }

    auto obj = info[0].As<Object>();
    auto samples = numberFromJs<uint64_t>(obj.Get("samples"));
    auto inBps = maybeNumberFromJs<uint64_t>(obj.Get("inBps")).value_or(4);
    auto outBps = maybeNumberFromJs<uint64_t>(obj.Get("outBps")).value_or(4);
    auto buffers = arrayFromJs<char*>(obj.Get("buffers"), [&samples, &inBps](auto val) {
      if (!val.IsBuffer()) {
        throw TypeError::New(val.Env(), "Expected value in buffers to be a Buffer");
      }

      auto buffer = val.template As<Buffer<char>>();
      if (buffer.ByteLength() < samples * inBps) {
        throw RangeError::New(
          val.Env(),
          "Buffer has size "s + std::to_string(buffer.ByteLength())
            + " but expected to be at least "s + std::to_string(samples * inBps));
      }

      return buffer.Data();
    });
    auto channels = buffers.size();

    if (samples == 0 || inBps == 0 || outBps == 0 || channels == 0) {
      throw Error::New(info.Env(), "Invalid value for one of the given properties");
    }

    // NOTE: if outBps > inBps means that after one sample copy, there is some bytes that need
    //       to be cleared up. To be faster, all memory is cleared when allocated.
    //       the reader and writer functions are here to properly read and write ints.
    auto reader = getReader(inBps, info.Env());
    auto writer = getWriter(outBps, info.Env());
    uint64_t outputBufferSize = samples * outBps * channels;
    char* outputBuffer =
      (char*) (outBps <= inBps ? malloc(outputBufferSize) : calloc(outputBufferSize, 1));
    for (uint64_t sample = 0; sample < samples; sample++) {
      for (uint64_t channel = 0; channel < channels; channel++) {
        char* out = outputBuffer + (sample * channels + channel) * outBps;
        char* in = buffers[channel] + sample * inBps;
        auto value = reader(in);
        writer(out, value);
      }
    }

    return scope.Escape(
      Buffer<char>::New(info.Env(), outputBuffer, outputBufferSize, [](auto, auto data) {
        free(data);
      }));
  }

  static Napi::Value unzipAudio(const Napi::CallbackInfo& info) {
    using namespace Napi;
    EscapableHandleScope scope(info.Env());

    if (!info[0].IsObject()) {
      throw TypeError::New(info.Env(), "Expected first argument to be object");
    }

    auto obj = info[0].As<Object>();
    auto inBps = maybeNumberFromJs<uint64_t>(obj.Get("inBps")).value_or(4);
    auto outBps = maybeNumberFromJs<uint64_t>(obj.Get("outBps")).value_or(4);
    auto channels = maybeNumberFromJs<uint64_t>(obj.Get("channels")).value_or(2);

    auto bufferPair = pointer::fromBuffer<char>(obj.Get("buffer"));
    auto buffer = std::get<0>(bufferPair);
    uint64_t samples = maybeNumberFromJs<uint64_t>(obj.Get("samples"))
                         .value_or(std::get<1>(bufferPair) / inBps / channels);

    if (inBps == 0 || outBps == 0 || samples == 0 || channels == 0) {
      throw Error::New(info.Env(), "Invalid value for one of the given properties");
    }

    if (std::get<1>(bufferPair) < samples * inBps * channels) {
      throw RangeError::New(
        info.Env(),
        "Buffer has size "s + std::to_string(std::get<1>(bufferPair))
          + " but expected to be at least "s + std::to_string(samples * inBps * channels));
    }

    // NOTE: see above function note about the outputBuffer
    auto reader = getReader(inBps, info.Env());
    auto writer = getWriter(outBps, info.Env());
    uint64_t outputBufferSize = samples * outBps;
    std::vector<char*> outputBuffers(channels);
    for (uint64_t channel = 0; channel < channels; channel += 1) {
      outputBuffers[channel] =
        (char*) (outBps <= inBps ? malloc(outputBufferSize) : calloc(outputBufferSize, 1));
    }
    for (uint64_t sample = 0; sample < samples; sample++) {
      for (uint64_t channel = 0; channel < channels; channel += 1) {
        char* out = outputBuffers[channel] + sample * outBps;
        char* in = buffer + (sample * channels + channel) * inBps;
        auto value = reader(in);
        writer(out, value);
      }
    }

    auto array = Napi::Array::New(info.Env(), channels);
    for (uint64_t channel = 0; channel < channels; channel += 1) {
      array[channel] = Buffer<char>::New(
        info.Env(),
        outputBuffers[channel],
        outputBufferSize,
        [](auto, auto data) { free(data); });
    }
    return scope.Escape(array);
  }

  static Napi::Value convertSampleFormat(const Napi::CallbackInfo& info) {
    using namespace Napi;
    EscapableHandleScope scope(info.Env());

    if (!info[0].IsObject()) {
      throw TypeError::New(info.Env(), "Expected first argument to be object");
    }

    auto obj = info[0].As<Object>();
    auto inBps = maybeNumberFromJs<uint64_t>(obj.Get("inBps")).value_or(4);
    auto outBps = maybeNumberFromJs<uint64_t>(obj.Get("outBps")).value_or(4);

    if (inBps == outBps) {
      return scope.Escape(obj.Get("buffer"));
    }

    auto bufferPair = pointer::fromBuffer<char>(obj.Get("buffer"));
    auto buffer = std::get<0>(bufferPair);
    uint64_t samples =
      maybeNumberFromJs<uint64_t>(obj.Get("samples")).value_or(std::get<1>(bufferPair) / inBps);

    if (inBps == 0 || outBps == 0 || samples == 0) {
      throw Error::New(info.Env(), "Invalid value for one of the given properties");
    }

    if (std::get<1>(bufferPair) < samples * inBps) {
      throw RangeError::New(
        info.Env(),
        "Buffer has size "s + std::to_string(std::get<1>(bufferPair))
          + " but expected to be at least "s + std::to_string(samples * inBps));
    }

    // NOTE: see above function note about the outputBuffer
    auto reader = getReader(inBps, info.Env());
    auto writer = getWriter(outBps, info.Env());
    uint64_t outputBufferSize = samples * outBps;
    char* outputBuffer =
      (char*) (outBps <= inBps ? malloc(outputBufferSize) : calloc(outputBufferSize, 1));
    for (uint64_t sample = 0; sample < samples; sample++) {
      char* out = outputBuffer + sample * outBps;
      char* in = buffer + sample * inBps;
      auto value = reader(in);
      writer(out, value);
    }

    return scope.Escape(
      Buffer<char>::New(info.Env(), outputBuffer, outputBufferSize, [](auto, auto data) {
        free(data);
      }));
  }

  Napi::Object initFns(Napi::Env env) {
    using namespace Napi;
    EscapableHandleScope scope(env);

    auto obj = Object::New(env);
    obj.DefineProperties({
      PropertyDescriptor::Function(env, obj, "zipAudio", zipAudio, napi_enumerable),
      PropertyDescriptor::Function(env, obj, "unzipAudio", unzipAudio, napi_enumerable),
      PropertyDescriptor::Function(
        env,
        obj,
        "convertSampleFormat",
        convertSampleFormat,
        napi_enumerable),
    });

    return scope.Escape(objectFreeze(obj)).As<Object>();
  }

}
