#include "defs.hpp"

namespace flac_bindings {

    template<>
    void structToJs(const FLAC__Frame* i, Local<Object> &obj) {
        Local<Object> header = Nan::New<Object>();
        Local<Array> subframes = Nan::New<Array>();
        Local<Object> footer = Nan::New<Object>();

        Nan::Set(footer, Nan::New("crc").ToLocalChecked(), Nan::New(i->footer.crc));
        Nan::Set(header, Nan::New("blocksize").ToLocalChecked(), Nan::New(i->header.blocksize));
        Nan::Set(header, Nan::New("sampleRate").ToLocalChecked(), Nan::New(i->header.sample_rate));
        Nan::Set(header, Nan::New("channels").ToLocalChecked(), Nan::New(i->header.channels));
        Nan::Set(header, Nan::New("channelAssignment").ToLocalChecked(), Nan::New(i->header.channel_assignment));
        Nan::Set(header, Nan::New("bitsPerSample").ToLocalChecked(), Nan::New(i->header.bits_per_sample));
        Nan::Set(header, Nan::New("crc").ToLocalChecked(), Nan::New(i->header.crc));
        if(i->header.number_type == FLAC__FRAME_NUMBER_TYPE_FRAME_NUMBER) {
            Nan::Set(header, Nan::New("frameNumber").ToLocalChecked(), Nan::New(i->header.number.frame_number));
        } else {
            Nan::Set(header, Nan::New("sampleNumber").ToLocalChecked(), Nan::New<Number>(i->header.number.sample_number));
        }

        for(uint32_t o = 0; o < FLAC__MAX_CHANNELS; o++) {
            Local<Object> subframe = Nan::New<Object>();
            Nan::Set(subframe, Nan::New("type").ToLocalChecked(), Nan::New(i->subframes[o].type));
            Nan::Set(subframe, Nan::New("wastedBits").ToLocalChecked(), Nan::New(i->subframes[o].wasted_bits));
            switch(i->subframes[o].type) {
                case FLAC__SUBFRAME_TYPE_CONSTANT:
                    Nan::Set(subframe, Nan::New("value").ToLocalChecked(), Nan::New(i->subframes[o].data.constant.value));
                    break;
                case FLAC__SUBFRAME_TYPE_VERBATIM: {
                    Nan::MaybeLocal<Object> data = WrapPointer(i->subframes[o].data.verbatim.data,
                        i->header.blocksize * i->header.bits_per_sample * sizeof(int32_t));
                    Nan::Set(subframe, Nan::New("data").ToLocalChecked(), data.ToLocalChecked());
                    break;
                }
                case FLAC__SUBFRAME_TYPE_FIXED: {
                    Nan::MaybeLocal<Object> residual = WrapPointer(i->subframes[o].data.fixed.residual,
                        (i->header.blocksize - i->subframes[o].data.fixed.order) * sizeof(int32_t));
                    Local<Array> warmup = Nan::New<Array>();
                    for(uint32_t u = 0; u < i->subframes[o].data.fixed.order; u++)
                        Nan::Set(warmup, u, Nan::New(i->subframes[o].data.fixed.warmup[u]));
                    //Nan::Set(subframe, Nan::New("entropyCodingMethod").ToLocalChecked(), Nan::New<Number>(i->subframes[o].data.fixed.entropy_coding_method));
                    Nan::Set(subframe, Nan::New("order").ToLocalChecked(), Nan::New<Number>(i->subframes[o].data.fixed.order));
                    Nan::Set(subframe, Nan::New("warmup").ToLocalChecked(), warmup);
                    Nan::Set(subframe, Nan::New("residual").ToLocalChecked(), residual.ToLocalChecked());
                    break;
                }
                case FLAC__SUBFRAME_TYPE_LPC: {
                    Nan::MaybeLocal<Object> residual = WrapPointer(i->subframes[o].data.lpc.residual,
                        (i->header.blocksize - i->subframes[o].data.lpc.order) * sizeof(int32_t));
                    Local<Array> warmup = Nan::New<Array>(), qlpCoeff = Nan::New<Array>();
                    for(uint32_t u = 0; u < i->subframes[o].data.lpc.order; u++)
                        Nan::Set(warmup, u, Nan::New(i->subframes[o].data.lpc.warmup[u]));
                    for(uint32_t u = 0; u < i->subframes[o].data.lpc.order * i->subframes[o].data.lpc.qlp_coeff_precision; u++)
                        Nan::Set(qlpCoeff, u, Nan::New(i->subframes[o].data.lpc.qlp_coeff[u]));
                    //Nan::Set(subframe, Nan::New("entropyCodingMethod").ToLocalChecked(), Nan::New<Number>(i->subframes[o].data.lpc.entropy_coding_method));
                    Nan::Set(subframe, Nan::New("order").ToLocalChecked(), Nan::New<Number>(i->subframes[o].data.lpc.order));
                    Nan::Set(subframe, Nan::New("qlpCoeffPrecision").ToLocalChecked(), Nan::New<Number>(i->subframes[o].data.lpc.qlp_coeff_precision));
                    Nan::Set(subframe, Nan::New("quantizationLevel").ToLocalChecked(), Nan::New<Number>(i->subframes[o].data.lpc.quantization_level));
                    Nan::Set(subframe, Nan::New("warmup").ToLocalChecked(), warmup);
                    Nan::Set(subframe, Nan::New("qlpCoeff").ToLocalChecked(), qlpCoeff);
                    Nan::Set(subframe, Nan::New("residual").ToLocalChecked(), residual.ToLocalChecked());
                    break;
                }
                default: break;
            }

            Nan::Set(subframes, o, subframe);
        }

        Nan::Set(obj, Nan::New("header").ToLocalChecked(), header);
        Nan::Set(obj, Nan::New("subframes").ToLocalChecked(), subframes);
        Nan::Set(obj, Nan::New("footer").ToLocalChecked(), footer);
    }

}
