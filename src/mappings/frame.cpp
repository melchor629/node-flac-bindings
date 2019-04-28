#include "mappings.hpp"

namespace flac_bindings {

    using namespace node;

    template<typename T>
    static inline void defineOwnProperty(const Local<Object> &obj, const char* name, const Local<T> &value) {
        Nan::DefineOwnProperty(
            obj,
            Nan::New(name).ToLocalChecked(),
            value,
            (PropertyAttribute) (PropertyAttribute::ReadOnly | PropertyAttribute::DontDelete)
        );
    }

    template<
        typename T,
        typename std::enable_if_t<
            std::is_integral<T>::value || std::is_enum<T>::value,
            int
        > = 0
    >
    static inline void defineOwnProperty(const Local<Object> &obj, const char* name, T value) {
        defineOwnProperty(obj, name, numberToJs(value));
    }

    template<typename T>
    static Local<Array> getWarmup(const T &p) {
        Local<Array> warmup = Nan::New<Array>();
        for(uint32_t u = 0; u < p.order; u++)
            Nan::Set(warmup, u, Nan::New(p.warmup[u]));
        return warmup;
    }

    template<typename T>
    static Local<Array> getQlpCoeff(const T &p) {
        Local<Array> qlpCoeff = Nan::New<Array>();
        for(uint32_t u = 0; u < p.order; u++)
            Nan::Set(qlpCoeff, u, Nan::New(p.qlp_coeff[u]));
        return qlpCoeff;
    }

    template<>
    Local<Object> structToJs(const FLAC__Frame* i, bool deleteHint) {
        Local<Object> obj = Nan::New<Object>();
        Local<Object> header = Nan::New<Object>();
        Local<Array> subframes = Nan::New<Array>();
        Local<Object> footer = Nan::New<Object>();

        defineOwnProperty(footer, "crc", i->footer.crc);

        defineOwnProperty(header, "blocksize", i->header.blocksize);
        defineOwnProperty(header, "sampleRate", i->header.sample_rate);
        defineOwnProperty(header, "channels", i->header.channels);
        defineOwnProperty<int>(header, "channelAssignment", i->header.channel_assignment);
        defineOwnProperty(header, "bitsPerSample", i->header.bits_per_sample);
        defineOwnProperty(header, "crc", i->header.crc);
        if(i->header.number_type == FLAC__FRAME_NUMBER_TYPE_FRAME_NUMBER) {
            defineOwnProperty(header, "frameNumber", i->header.number.frame_number);
        } else {
            defineOwnProperty(header, "sampleNumber", i->header.number.sample_number);
        }

        for(uint32_t o = 0; o < FLAC__MAX_CHANNELS; o++) {
            Local<Object> subframe = Nan::New<Object>();
            defineOwnProperty<int>(subframe, "type", i->subframes[o].type);
            defineOwnProperty(subframe, "wastedBits", i->subframes[o].wasted_bits);
            switch(i->subframes[o].type) {
                case FLAC__SUBFRAME_TYPE_CONSTANT:
                    defineOwnProperty(subframe, "value", i->subframes[o].data.constant.value);
                    break;

                case FLAC__SUBFRAME_TYPE_VERBATIM: {
                    Nan::MaybeLocal<Object> data = WrapPointer(
                        i->subframes[o].data.verbatim.data,
                        i->header.blocksize * i->header.bits_per_sample * sizeof(int32_t)
                    );
                    defineOwnProperty(subframe, "data", data.ToLocalChecked());
                    break;
                }

                case FLAC__SUBFRAME_TYPE_FIXED: {
                    Nan::MaybeLocal<Object> residual = WrapPointer(
                        i->subframes[o].data.fixed.residual,
                        (i->header.blocksize - i->subframes[o].data.fixed.order) * sizeof(int32_t)
                    );
                    //defineOwnProperty(subframe, "entropyCodingMethod", i->subframes[o].data.fixed.entropy_coding_method);
                    defineOwnProperty(subframe, "order", i->subframes[o].data.fixed.order);
                    defineOwnProperty(subframe, "warmup", getWarmup(i->subframes[o].data.fixed));
                    defineOwnProperty(subframe, "residual", residual.ToLocalChecked());
                    break;
                }

                case FLAC__SUBFRAME_TYPE_LPC: {
                    Nan::MaybeLocal<Object> residual = WrapPointer(
                        i->subframes[o].data.lpc.residual,
                        (i->header.blocksize - i->subframes[o].data.lpc.order) * sizeof(int32_t)
                    );
                    //defineOwnProperty(subframe, "entropyCodingMethod", i->subframes[o].data.lpc.entropy_coding_method);
                    defineOwnProperty(subframe, "order", i->subframes[o].data.lpc.order);
                    defineOwnProperty(subframe, "qlpCoeffPrecision", i->subframes[o].data.lpc.qlp_coeff_precision);
                    defineOwnProperty(subframe, "quantizationLevel", i->subframes[o].data.lpc.quantization_level);
                    defineOwnProperty(subframe, "warmup", getWarmup(i->subframes[o].data.lpc));
                    defineOwnProperty(subframe, "qlpCoeff", getQlpCoeff(i->subframes[o].data.lpc));
                    defineOwnProperty(subframe, "residual", residual.ToLocalChecked());
                    break;
                }

                default: break;
            }

            Nan::Set(subframes, o, subframe);
        }

        defineOwnProperty(obj, "header", header);
        defineOwnProperty(obj, "subframes", subframes);
        defineOwnProperty(obj, "footer", footer);
        return obj;
    }

}
