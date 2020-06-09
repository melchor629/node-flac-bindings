#include <numeric>
#include <FLAC/format.h>
#include "../utils/converters.hpp"
#include "../utils/js_utils.hpp"

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
            PropertyDescriptor::Value("channelAssignment", numberToJs(env, header.channel_assignment), attrs),
            PropertyDescriptor::Value("bitsPerSample", numberToJs(env, header.bits_per_sample), attrs),
            PropertyDescriptor::Value("crc", numberToJs(env, header.crc), attrs),
        });

        if(header.number_type == FLAC__FrameNumberType::FLAC__FRAME_NUMBER_TYPE_FRAME_NUMBER) {
            obj.DefineProperty(PropertyDescriptor::Value("frameNumber", numberToJs(env, header.number.frame_number), attrs));
        } else {
            obj.DefineProperty(PropertyDescriptor::Value("sampleNumber", numberToJs(env, header.number.sample_number), attrs));
        }

        return scope.Escape(objectFreeze(obj)).As<Object>();
    }

    Object subframeToJs(const Env& env, const FLAC__Subframe& subframe, const FLAC__FrameHeader& header) {
        EscapableHandleScope scope(env);
        auto obj = Object::New(env);
        auto attrs = napi_property_attributes::napi_enumerable;

        obj.DefineProperties({
            PropertyDescriptor::Value("type", numberToJs(env, subframe.type), attrs),
            PropertyDescriptor::Value("wastedBits", numberToJs(env, subframe.wasted_bits)),
        });

        switch(subframe.type) {
            case FLAC__SUBFRAME_TYPE_CONSTANT:
                obj.DefineProperty(PropertyDescriptor::Value("value", numberToJs(env, subframe.data.constant.value), attrs));
                break;

            case FLAC__SUBFRAME_TYPE_FIXED:
                obj.DefineProperties({
                    //TODO entropyCodingMethod
                    PropertyDescriptor::Value("order", numberToJs(env, subframe.data.fixed.order)),
                    PropertyDescriptor::Value("warmup", arrayToJs(env, subframe.data.fixed.warmup)),
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
                    //TODO entropyCodingMethod
                    PropertyDescriptor::Value("order", numberToJs(env, subframe.data.lpc.order), attrs),
                    PropertyDescriptor::Value("qlpCoeffPrecision", numberToJs(env, subframe.data.lpc.qlp_coeff_precision), attrs),
                    PropertyDescriptor::Value("quantizationLevel", numberToJs(env, subframe.data.lpc.quantization_level), attrs),
                    PropertyDescriptor::Value("wamup", arrayToJs(env, subframe.data.lpc.warmup), attrs),
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
                obj.DefineProperty(PropertyDescriptor::Value(
                    "data",
                    Buffer<int32_t>::Copy(
                        env,
                        subframe.data.verbatim.data,
                        header.blocksize * header.bits_per_sample
                    )
                ));
                break;
        }

        return scope.Escape(objectFreeze(obj)).As<Object>();
    }

    Object frameFooterToJs(const Env& env, const FLAC__FrameFooter& footer) {
        EscapableHandleScope scope(env);
        auto obj = Object::New(env);
        obj.DefineProperty(PropertyDescriptor::Value("crc", numberToJs(env, footer.crc), napi_enumerable));
        return scope.Escape(objectFreeze(obj)).As<Object>();
    }

    Object frameToJs(const Env& env, const FLAC__Frame* frame) {
        EscapableHandleScope scope(env);
        auto obj = Object::New(env);
        obj["header"] = frameHeaderToJs(env, frame->header);
        obj["subframes"] = arrayToJs(env, std::accumulate(
            frame->subframes,
            frame->subframes + FLAC__MAX_CHANNELS,
            std::vector<Value>(),
            [&env, &frame] (std::vector<Value> l, auto p) {
                l.push_back(subframeToJs(env, p, frame->header));
                return l;
            }
        ));
        obj["footer"] = frameFooterToJs(env, frame->footer);
        return scope.Escape(objectFreeze(obj)).As<Object>();
    }

}
