#include <nan.h>
#include "dl.hpp"

using namespace v8;
using namespace node;
#include "pointer.hpp"
#include "format.h"
#include "mappings/mappings.hpp"
#include "metadata.hpp"

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
        if(info[0].IsEmpty() || !info[0]->IsString()) {
            Nan::ThrowTypeError("Expected argument to be string");
            return;
        }

        Nan::Utf8String filename(info[0]);
        FLAC__StreamMetadata metadata;
        FLAC__bool ret = FLAC__metadata_get_streaminfo(*filename, &metadata);
        if(ret) {
            info.GetReturnValue().Set(structToJs(FLAC__metadata_object_clone(&metadata)));
        } else {
            info.GetReturnValue().Set(Nan::New<Boolean>(false));
        }
    }

    NAN_METHOD(node_FLAC__metadata_get_tags) {
        if(info[0].IsEmpty() || !info[0]->IsString()) {
            Nan::ThrowTypeError("Expected argument to be string");
            return;
        }

        Nan::Utf8String filename(info[0]);
        FLAC__StreamMetadata* metadatas;
        FLAC__bool ret = FLAC__metadata_get_tags(*filename, &metadatas);
        if(ret) {
            info.GetReturnValue().Set(structToJs(metadatas));
        } else {
            info.GetReturnValue().Set(Nan::New<Boolean>(false));
        }
    }

    NAN_METHOD(node_FLAC__metadata_get_cuesheet) {
        if(info[0].IsEmpty() || !info[0]->IsString()) {
            Nan::ThrowTypeError("Expected argument to be string");
            return;
        }

        Nan::Utf8String filename(info[0]);
        FLAC__StreamMetadata* metadatas;
        FLAC__bool ret = FLAC__metadata_get_cuesheet(*filename, &metadatas);
        if(ret) {
            info.GetReturnValue().Set(structToJs(metadatas));
        } else {
            info.GetReturnValue().Set(Nan::New<Boolean>(false));
        }
    }

    NAN_METHOD(node_FLAC__metadata_get_picture) {
        if(info[0].IsEmpty() || !info[0]->IsString()) {
            Nan::ThrowTypeError("Expected argument to be string");
            return;
        }

        FLAC__StreamMetadata* picture;
        Nan::Utf8String filename(info[0]);
        FLAC__StreamMetadata_Picture_Type type = (FLAC__StreamMetadata_Picture_Type) numberFromJs<int>(info[1]).FromMaybe(-1);
        Nan::Utf8String mime_type(info[2]);
        Nan::Utf8String description(info[3]);
        unsigned max_width = numberFromJs<unsigned>(info[4]).FromMaybe(-1);
        unsigned max_height = numberFromJs<unsigned>(info[5]).FromMaybe(-1);
        unsigned max_depth = numberFromJs<unsigned>(info[6]).FromMaybe(-1);
        unsigned max_colors = numberFromJs<unsigned>(info[7]).FromMaybe(-1);
        FLAC__bool ret = FLAC__metadata_get_picture(
            *filename,
            &picture,
            type,
            info[2].IsEmpty() || !info[2]->IsString() ? nullptr : *mime_type,
            info[3].IsEmpty() || !info[3]->IsString() ? nullptr : (FLAC__byte*) *description,
            max_width,
            max_height,
            max_depth,
            max_colors
        );

        if(ret) {
            info.GetReturnValue().Set(structToJs(picture));
        } else {
            info.GetReturnValue().Set(Nan::New<Boolean>(false));
        }
    }

    NAN_MODULE_INIT(initMetadata0) {
        Local<Object> obj = Nan::New<Object>();
        #define setMethod(fn, jsFunction) \
        Nan::SetMethod(obj, #jsFunction, _JOIN(node_FLAC__metadata_, fn)); \
        _JOIN(FLAC__metadata_, fn) = libFlac->getSymbolAddress<_JOIN2(FLAC__metadata_, fn, _t)>("FLAC__metadata_" #fn); \
        if(_JOIN(FLAC__metadata_, fn) == nullptr) printf("%s\n", libFlac->getLastError().c_str());

        setMethod(get_streaminfo, getStreaminfo);
        setMethod(get_tags, getTags);
        setMethod(get_cuesheet, getCuesheet);
        setMethod(get_picture, getPicture);

        Nan::Set(target, Nan::New("metadata0").ToLocalChecked(), obj);
    }

}
