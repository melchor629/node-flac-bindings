#include <nan.h>

using namespace v8;
using namespace node;
#include "../flac/metadata.hpp"
#include "../utils/pointer.hpp"
#include "../flac/format.h"
#include "../mappings/mappings.hpp"

namespace flac_bindings {

    NAN_MODULE_INIT(initMetadataObjectMethods) {
        Local<Object> obj = Nan::New<Object>();

        Metadata::init(obj);
        StreamInfoMetadata::init(obj);
        PaddingMetadata::init(obj);
        ApplicationMetadata::init(obj);
        SeekTableMetadata::init(obj);
        SeekPoint::init(obj);
        VorbisCommentMetadata::init(obj);
        CueSheetMetadata::init(obj);
        CueSheetIndex::init(obj);
        CueSheetTrack::init(obj);
        PictureMetadata::init(obj);
        UnknownMetadata::init(obj);

        Nan::Set(target, Nan::New("metadata").ToLocalChecked(), obj);
    }

}
