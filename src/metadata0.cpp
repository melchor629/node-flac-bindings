#include <nan.h>
#include "dl.hpp"

using namespace v8;
using namespace node;
#include "pointer.hpp"
#include "format.h"

#define _JOIN(a, b) a##b
#define _JOIN2(a,b,c) a##b##c

#define metadataFunction(ret, name, ...) \
typedef ret (*_JOIN2(FLAC__metadata_, name, _t))(__VA_ARGS__); \
static _JOIN2(FLAC__metadata_, name, _t) _JOIN(FLAC__metadata_, name);

extern "C" {
    metadataFunction(FLAC__bool, get_streaminfo, const char *filename, FLAC__StreamMetadata *streaminfo);
    metadataFunction(FLAC__bool, get_tags, const char *filename, FLAC__StreamMetadata **tags);
    metadataFunction(FLAC__bool, get_cuesheet, const char *filename, FLAC__StreamMetadata **cuesheet);
    metadataFunction(FLAC__bool, get_picture, const char *filename, FLAC__StreamMetadata **picture, FLAC__StreamMetadata_Picture_Type type, const char *mime_type, const FLAC__byte *description, unsigned max_width, unsigned max_height, unsigned max_depth, unsigned max_colors);
}

namespace flac_bindings {

    extern Library* libFlac;

    NAN_METHOD(node_FLAC__metadata_get_streaminfo) {
        Nan::Utf8String filename(info[0]);
        FLAC__StreamMetadata metadata;
        FLAC__bool ret = FLAC__metadata_get_streaminfo(*filename, &metadata);
        if(ret) {
            info.GetReturnValue().Set(structToJs(&metadata));
        } else {
            info.GetReturnValue().Set(Nan::New<Boolean>(false));
        }
    }

    NAN_METHOD(node_FLAC__metadata_get_tags) {
        Nan::Utf8String filename(info[0]);
        FLAC__StreamMetadata* metadatas;
        FLAC__bool ret = FLAC__metadata_get_tags(*filename, &metadatas);
        if(ret) {
            Local<Object> m = structToJs(metadatas);
            Nan::Set(m, Nan::New("_m").ToLocalChecked(), WrapPointer(metadatas).ToLocalChecked());
            info.GetReturnValue().Set(m);
        } else {
            info.GetReturnValue().Set(Nan::New<Boolean>(false));
        }
    }

    NAN_METHOD(node_FLAC__metadata_get_cuesheet) {
        Nan::Utf8String filename(info[0]);
        FLAC__StreamMetadata* metadatas;
        FLAC__bool ret = FLAC__metadata_get_cuesheet(*filename, &metadatas);
        if(ret) {
            Local<Object> m = structToJs(metadatas);
            Nan::Set(m, Nan::New("_m").ToLocalChecked(), WrapPointer(metadatas).ToLocalChecked());
            info.GetReturnValue().Set(m);
        } else {
            info.GetReturnValue().Set(Nan::New<Boolean>(false));
        }
    }

    NAN_METHOD(node_FLAC__metadata_get_picture) {
        FLAC__StreamMetadata* picture;
        Nan::Utf8String filename(info[0]);
        FLAC__StreamMetadata_Picture_Type type = (FLAC__StreamMetadata_Picture_Type) Nan::To<unsigned>(info[1]).FromJust();
        Nan::Utf8String mime_type(info[2]);
        Nan::Utf8String description(info[3]);
        unsigned max_width = Nan::To<unsigned>(info[4]).FromJust();
        unsigned max_height = Nan::To<unsigned>(info[5]).FromJust();
        unsigned max_depth = Nan::To<unsigned>(info[6]).FromJust();
        unsigned max_colors = Nan::To<unsigned>(info[7]).FromJust();
        FLAC__bool ret = FLAC__metadata_get_picture(*filename, &picture, type, *mime_type, (FLAC__byte*) *description, max_width, max_height, max_depth, max_colors);

        if(ret) {
            Local<Object> m = structToJs(picture);
            Nan::Set(m, Nan::New("_m").ToLocalChecked(), WrapPointer(picture).ToLocalChecked());
            info.GetReturnValue().Set(m);
        } else {
            info.GetReturnValue().Set(Nan::New<Boolean>(false));
        }
    }

    NAN_MODULE_INIT(initMetadata0) {
        Local<Object> obj = Nan::New<Object>();
        #define setMethod(fn) \
        Nan::SetMethod(obj, #fn, _JOIN(node_FLAC__metadata_, fn)); \
        _JOIN(FLAC__metadata_, fn) = libFlac->getSymbolAddress<_JOIN2(FLAC__metadata_, fn, _t)>("FLAC__metadata_" #fn); \
        if(_JOIN(FLAC__metadata_, fn) == nullptr) printf("%s\n", libFlac->getLastError().c_str());

        setMethod(get_streaminfo);
        setMethod(get_tags);
        setMethod(get_cuesheet);
        setMethod(get_picture);

        Nan::Set(target, Nan::New("metadata0").ToLocalChecked(), obj);
    }

}
