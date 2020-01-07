#include <nan.h>

using namespace v8;
using namespace node;
#include "../utils/pointer.hpp"
#include "../flac/format.h"
#include "../flac/metadata0.hpp"
#include "../mappings/mappings.hpp"
#include "../flac/metadata.hpp"
#include "../utils/async.hpp"

#define _JOIN(a, b) a##b

namespace flac_bindings {

    static NAN_METHOD(getStreaminfo) {
        if(info[0].IsEmpty() || !info[0]->IsString()) {
            Nan::ThrowTypeError("Expected argument to be string");
            return;
        }

        Nan::Utf8String filename(info[0]);
        FLAC__StreamMetadata metadata;
        FLAC__bool ret = FLAC__metadata_get_streaminfo(*filename, &metadata);
        if(ret) {
            info.GetReturnValue().Set(structToJs(FLAC__metadata_object_clone(&metadata), true));
        } else {
            info.GetReturnValue().Set(Nan::New<Boolean>(false));
        }
    }

    static Local<Value> asyncImpl(const char* name, std::function<FLAC__StreamMetadata*()> impl) {
        Nan::EscapableHandleScope scope;
        auto* worker = new AsyncBackgroundTask<FLAC__StreamMetadata*>(
            [impl] (auto &c) {
                FLAC__StreamMetadata* metadata = impl();
                if(metadata != nullptr) {
                    c.resolve(metadata);
                } else {
                    c.reject("Could not read the block from the file");
                }
            },
            nullptr,
            name,
            [] (auto m) { return structToJs(m, true); }
        );

        AsyncQueueWorker(worker);
        return scope.Escape(worker->getReturnValue());
    }

    static NAN_METHOD(getStreaminfoAsync) {
        if(info[0].IsEmpty() || !info[0]->IsString()) {
            Nan::ThrowTypeError("Expected argument to be string");
            return;
        }

        Nan::Utf8String filename(info[0]);
        std::string fileName = *filename;
        info.GetReturnValue().Set(asyncImpl(
            "flac_bindings:metadata0:getStreaminfoAsync",
            [fileName] () {
                FLAC__StreamMetadata metadata;
                FLAC__bool ret = FLAC__metadata_get_streaminfo(fileName.c_str(), &metadata);
                return ret ? FLAC__metadata_object_clone(&metadata) : nullptr;
            }
        ));
    }

    static NAN_METHOD(getTags) {
        if(info[0].IsEmpty() || !info[0]->IsString()) {
            Nan::ThrowTypeError("Expected argument to be string");
            return;
        }

        Nan::Utf8String filename(info[0]);
        FLAC__StreamMetadata* metadatas;
        FLAC__bool ret = FLAC__metadata_get_tags(*filename, &metadatas);
        if(ret) {
            info.GetReturnValue().Set(structToJs(metadatas, true));
        } else {
            info.GetReturnValue().Set(Nan::New<Boolean>(false));
        }
    }

    static NAN_METHOD(getTagsAsync) {
        if(info[0].IsEmpty() || !info[0]->IsString()) {
            Nan::ThrowTypeError("Expected argument to be string");
            return;
        }

        Nan::Utf8String filename(info[0]);
        std::string fileName = *filename;
        info.GetReturnValue().Set(asyncImpl(
            "flac_bindings:metadata0:getTagsAsync",
            [fileName] () {
                FLAC__StreamMetadata* metadata = nullptr;
                FLAC__bool ret = FLAC__metadata_get_tags(fileName.c_str(), &metadata);
                return ret ? metadata : nullptr;
            }
        ));
    }

    static NAN_METHOD(getCuesheet) {
        if(info[0].IsEmpty() || !info[0]->IsString()) {
            Nan::ThrowTypeError("Expected argument to be string");
            return;
        }

        Nan::Utf8String filename(info[0]);
        FLAC__StreamMetadata* metadatas;
        FLAC__bool ret = FLAC__metadata_get_cuesheet(*filename, &metadatas);
        if(ret) {
            info.GetReturnValue().Set(structToJs(metadatas, true));
        } else {
            info.GetReturnValue().Set(Nan::New<Boolean>(false));
        }
    }

    static NAN_METHOD(getCuesheetAsync) {
        if(info[0].IsEmpty() || !info[0]->IsString()) {
            Nan::ThrowTypeError("Expected argument to be string");
            return;
        }

        Nan::Utf8String filename(info[0]);
        std::string fileName = *filename;
        info.GetReturnValue().Set(asyncImpl(
            "flac_bindings:metadata0:getCuesheetAsync",
            [fileName] () {
                FLAC__StreamMetadata* metadata = nullptr;
                FLAC__bool ret = FLAC__metadata_get_cuesheet(fileName.c_str(), &metadata);
                return ret ? metadata : nullptr;
            }
        ));
    }

    static NAN_METHOD(getPicture) {
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
            info.GetReturnValue().Set(structToJs(picture, true));
        } else {
            info.GetReturnValue().Set(Nan::New<Boolean>(false));
        }
    }

    static NAN_METHOD(getPictureAsync) {
        if(info[0].IsEmpty() || !info[0]->IsString()) {
            Nan::ThrowTypeError("Expected argument to be string");
            return;
        }

        Nan::Utf8String filename(info[0]);
        std::string fileName = *filename;
        FLAC__StreamMetadata_Picture_Type type = (FLAC__StreamMetadata_Picture_Type) numberFromJs<int>(info[1]).FromMaybe(-1);
        Nan::Utf8String mime_type(info[2]);
        std::string mimeType = info[2]->IsString() ? *mime_type : "";
        Nan::Utf8String description(info[3]);
        std::string descr = info[3]->IsString() ? *description : "";
        unsigned max_width = numberFromJs<unsigned>(info[4]).FromMaybe(-1);
        unsigned max_height = numberFromJs<unsigned>(info[5]).FromMaybe(-1);
        unsigned max_depth = numberFromJs<unsigned>(info[6]).FromMaybe(-1);
        unsigned max_colors = numberFromJs<unsigned>(info[7]).FromMaybe(-1);
        info.GetReturnValue().Set(asyncImpl(
            "flac_bindings:metadata0:getPictureAsync",
            [=] () {
                FLAC__StreamMetadata* metadata;
                FLAC__bool ret = FLAC__metadata_get_picture(
                    fileName.c_str(),
                    &metadata,
                    type,
                    mimeType.length() == 0 ? nullptr : mimeType.c_str(),
                    descr.length() == 0 ? nullptr : (FLAC__byte*) descr.c_str(),
                    max_width,
                    max_height,
                    max_depth,
                    max_colors
                );
                return ret ? metadata : nullptr;
            }
        ));
    }

    NAN_MODULE_INIT(initMetadata0) {
        Local<Object> obj = Nan::New<Object>();

        #define setMethod(fn, jsFunction) \
        Nan::SetMethod(obj, #jsFunction, jsFunction); \
        Nan::SetMethod(obj, #jsFunction "Async", _JOIN(jsFunction, Async));

        setMethod(get_streaminfo, getStreaminfo);
        setMethod(get_tags, getTags);
        setMethod(get_cuesheet, getCuesheet);
        setMethod(get_picture, getPicture);

        Nan::Set(target, Nan::New("metadata0").ToLocalChecked(), obj);
    }

}
