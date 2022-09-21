#include "../utils/converters.hpp"
#include "../utils/js_utils.hpp"
#include <FLAC/format.h>
#include <numeric>

namespace flac_bindings {

  using namespace Napi;

  Object frameHeaderToJs(const Env& env, const FLAC__FrameHeader& header) {
    EscapableHandleScope scope(env);
    auto obj = Object::New(env);
    auto attrs = napi_property_attributes::napi_enumerable;

    obj.DefineProperties({
      PropertyDescriptor::Value("blocksize", numberToJs(env, header.blocksize), attrs),
      PropertyDescriptor::Value("sampleRate", numberToJs(env, header.sample_rate), attrs),
      PropertyDescriptor::Value("channels", numberToJs(env, header.channels), attrs),
      PropertyDescriptor::Value(
        "channelAssignment",
        numberToJs(env, header.channel_assignment),
        attrs),
      PropertyDescriptor::Value("bitsPerSample", numberToJs(env, header.bits_per_sample), attrs),
      PropertyDescriptor::Value("crc", numberToJs(env, header.crc), attrs),
    });

    if (header.number_type == FLAC__FrameNumberType::FLAC__FRAME_NUMBER_TYPE_FRAME_NUMBER) {
      obj.DefineProperty(PropertyDescriptor::Value(
        "frameNumber",
        numberToJs(env, header.number.frame_number),
        attrs));
    } else {
      obj.DefineProperty(PropertyDescriptor::Value(
        "sampleNumber",
        numberToJs(env, header.number.sample_number),
        attrs));
    }

    return scope.Escape(objectFreeze(obj)).As<Object>();
  }

  Object
    subframeToJs(const Env& env, const FLAC__Subframe& subframe, const FLAC__FrameHeader& header) {
    EscapableHandleScope scope(env);
    auto obj = Object::New(env);
    auto attrs = napi_property_attributes::napi_enumerable;

    obj.DefineProperties({
      PropertyDescriptor::Value("type", numberToJs(env, subframe.type), attrs),
      PropertyDescriptor::Value("wastedBits", numberToJs(env, subframe.wasted_bits), attrs),
    });

    switch (subframe.type) {
      case FLAC__SUBFRAME_TYPE_CONSTANT:
        obj.DefineProperty(
          PropertyDescriptor::Value("value", numberToJs(env, subframe.data.constant.value), attrs));
        break;

      case FLAC__SUBFRAME_TYPE_FIXED:
        obj.DefineProperties({
          // TODO entropyCodingMethod
          PropertyDescriptor::Value("order", numberToJs(env, subframe.data.fixed.order), attrs),
          PropertyDescriptor::Value("warmup", arrayToJs(env, subframe.data.fixed.warmup), attrs),
          /*PropertyDescriptor::Value(
              "residual",
              Buffer<int32_t>::Copy(
                  env,
                  subframe.data.fixed.residual,
                  (header.blocksize - subframe.data.fixed.order)
              ),
              attrs
          ),*/
        });
        break;

      case FLAC__SUBFRAME_TYPE_LPC:
        obj.DefineProperties({
          // TODO entropyCodingMethod
          PropertyDescriptor::Value("order", numberToJs(env, subframe.data.lpc.order), attrs),
          PropertyDescriptor::Value(
            "qlpCoeffPrecision",
            numberToJs(env, subframe.data.lpc.qlp_coeff_precision),
            attrs),
          PropertyDescriptor::Value(
            "quantizationLevel",
            numberToJs(env, subframe.data.lpc.quantization_level),
            attrs),
          PropertyDescriptor::Value("warmup", arrayToJs(env, subframe.data.lpc.warmup), attrs),
          PropertyDescriptor::Value("qlpCoeff", arrayToJs(env, subframe.data.lpc.qlp_coeff), attrs),
          /*PropertyDescriptor::Value(
              "residual",
              Buffer<int32_t>::Copy(
                  env,
                  subframe.data.fixed.residual,
                  header.blocksize - subframe.data.fixed.order
              ),
              attrs
          ),*/
        });
        break;

      case FLAC__SUBFRAME_TYPE_VERBATIM:
#if FLAC_API_VERSION_CURRENT >= 12
        if (subframe.data.verbatim.data_type == FLAC__VERBATIM_SUBFRAME_DATA_TYPE_INT32) {
          obj.DefineProperty(PropertyDescriptor::Value(
            "data",
            Buffer<int32_t>::New(
              env,
              const_cast<int32_t*>(subframe.data.verbatim.data.int32),
              (size_t) header.blocksize * header.bits_per_sample / sizeof(int32_t)),
            attrs));
        } else if (subframe.data.verbatim.data_type == FLAC__VERBATIM_SUBFRAME_DATA_TYPE_INT64) {
          obj.DefineProperty(PropertyDescriptor::Value(
            "data",
            Buffer<int64_t>::New(
              env,
              const_cast<int64_t*>(subframe.data.verbatim.data.int64),
              (size_t) header.blocksize * header.bits_per_sample / sizeof(int32_t)),
            attrs));
        } else {
          obj.DefineProperty(PropertyDescriptor::Value("data", env.Null(), attrs));
        }
#else
        obj.DefineProperty(PropertyDescriptor::Value(
          "data",
          Buffer<int32_t>::New(
            env,
            (int32_t*) subframe.data.verbatim.data,
            (size_t) header.blocksize * header.bits_per_sample / sizeof(int32_t)),
          attrs));
#endif

        break;
    }

    return scope.Escape(objectFreeze(obj)).As<Object>();
  }

  Array subframesToJs(const Env& env, const FLAC__Frame* frame) {
    EscapableHandleScope scope(env);
    auto array = Array::New(env, frame->header.channels);
    for (auto i = 0u; i < frame->header.channels; i++) {
      array[i] = subframeToJs(env, frame->subframes[i], frame->header);
    }

    return scope.Escape(array).As<Array>();
  }

  Object frameFooterToJs(const Env& env, const FLAC__FrameFooter& footer) {
    EscapableHandleScope scope(env);
    auto obj = Object::New(env);
    obj.DefineProperty(
      PropertyDescriptor::Value("crc", numberToJs(env, footer.crc), napi_enumerable));
    return scope.Escape(objectFreeze(obj)).As<Object>();
  }

  Object frameToJs(const Env& env, const FLAC__Frame* frame) {
    EscapableHandleScope scope(env);
    auto obj = Object::New(env);
    auto attrs = napi_property_attributes::napi_enumerable;
    obj.DefineProperties({
      PropertyDescriptor::Value("header", frameHeaderToJs(env, frame->header), attrs),
      PropertyDescriptor::Value("subframes", subframesToJs(env, frame), attrs),
      PropertyDescriptor::Value("footer", frameFooterToJs(env, frame->footer), attrs),
    });
    return scope.Escape(objectFreeze(obj)).As<Object>();
  }

}
