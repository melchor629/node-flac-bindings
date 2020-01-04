#include "nan.h"
#include "../utils/dl.hpp"

using namespace v8;
using namespace node;
#include "../utils/pointer.hpp"
#include "../format/format.h"
#include "../utils/defs.hpp"
#include "../mappings/mappings.hpp"

namespace flac_bindings {
    extern Library* libFlac;
}

#define _JOIN(a, b) a##b

extern "C" {
    const char *FLAC__VERSION_STRING;
    const char *FLAC__VENDOR_STRING;
    const char * const* FLAC__EntropyCodingMethodTypeString;
    const char * const* FLAC__SubframeTypeString;
    const char * const* FLAC__ChannelAssignmentString;
    const char * const* FLAC__FrameNumberTypeString;
    const char * const* FLAC__MetadataTypeString;
    const char * const* FLAC__StreamMetadata_Picture_TypeString;

    FLAC__bool FLAC__format_sample_rate_is_valid(unsigned sample_rate) {
        static FLAC__bool (*func)(unsigned) = NULL;
        if(func == NULL) {
            func = flac_bindings::libFlac->getSymbolAddress<FLAC__bool (*)(unsigned)>("FLAC__format_sample_rate_is_valid");
            if(func == NULL) printf("%s\n", flac_bindings::libFlac->getLastError().c_str());
        }
        if(func != NULL) return func(sample_rate);
        return false;
    }

    FLAC__bool FLAC__format_blocksize_is_subset(unsigned blocksize, unsigned sample_rate) {
        static FLAC__bool (*func)(unsigned, unsigned) = NULL;
        if(func == NULL) {
            func = flac_bindings::libFlac->getSymbolAddress<FLAC__bool (*)(unsigned, unsigned)>("FLAC__format_blocksize_is_subset");
            if(func == NULL) printf("%s\n", flac_bindings::libFlac->getLastError().c_str());
        }
        if(func != NULL) return func(blocksize, sample_rate);
        return false;
    }

    FLAC__bool FLAC__format_sample_rate_is_subset(unsigned sample_rate) {
        static FLAC__bool (*func)(unsigned) = NULL;
        if(func == NULL) {
            func = flac_bindings::libFlac->getSymbolAddress<FLAC__bool (*)(unsigned)>("FLAC__format_sample_rate_is_subset");
            if(func == NULL) printf("%s\n", flac_bindings::libFlac->getLastError().c_str());
        }
        if(func != NULL) return func(sample_rate);
        return false;
    }

    FLAC__bool FLAC__format_vorbiscomment_entry_name_is_legal(const char *name) {
        static FLAC__bool (*func)(const char*) = NULL;
        if(func == NULL) {
            func = flac_bindings::libFlac->getSymbolAddress<FLAC__bool (*)(const char*)>("FLAC__format_vorbiscomment_entry_name_is_legal");
            if(func == NULL) printf("%s\n", flac_bindings::libFlac->getLastError().c_str());
        }
        if(func != NULL) return func(name);
        return false;
    }

    FLAC__bool FLAC__format_vorbiscomment_entry_value_is_legal(const FLAC__byte *value, unsigned length) {
        static FLAC__bool (*func)(const FLAC__byte*, unsigned) = NULL;
        if(func == NULL) {
            func = flac_bindings::libFlac->getSymbolAddress<FLAC__bool (*)(const FLAC__byte*, unsigned)>("FLAC__format_vorbiscomment_entry_value_is_legal");
            if(func == NULL) printf("%s\n", flac_bindings::libFlac->getLastError().c_str());
        }
        if(func != NULL) return func(value, length);
        return false;
    }

    FLAC__bool FLAC__format_vorbiscomment_entry_is_legal(const FLAC__byte *entry, unsigned length) {
        static FLAC__bool (*func)(const FLAC__byte*, unsigned) = NULL;
        if(func == NULL) {
            func = flac_bindings::libFlac->getSymbolAddress<FLAC__bool (*)(const FLAC__byte*, unsigned)>("FLAC__format_vorbiscomment_entry_is_legal");
            if(func == NULL) printf("%s\n", flac_bindings::libFlac->getLastError().c_str());
        }
        if(func != NULL) return func(entry, length);
        return false;
    }

    FLAC__bool FLAC__format_seektable_is_legal(const FLAC__StreamMetadata_SeekTable *seek_table) {
        static FLAC__bool (*func)(const FLAC__StreamMetadata_SeekTable*) = NULL;
        if(func == NULL) {
            func = flac_bindings::libFlac->getSymbolAddress<FLAC__bool (*)(const FLAC__StreamMetadata_SeekTable*)>("FLAC__format_seektable_is_legal");
            if(func == NULL) printf("%s\n", flac_bindings::libFlac->getLastError().c_str());
        }
        if(func != NULL) return func(seek_table);
        return false;
    }

    unsigned FLAC__format_seektable_sort(FLAC__StreamMetadata_SeekTable *seek_table) {
        static FLAC__bool (*func)(FLAC__StreamMetadata_SeekTable*) = NULL;
        if(func == NULL) {
            func = flac_bindings::libFlac->getSymbolAddress<FLAC__bool (*)(FLAC__StreamMetadata_SeekTable*)>("FLAC__format_seektable_sort");
            if(func == NULL) printf("%s\n", flac_bindings::libFlac->getLastError().c_str());
        }
        if(func != NULL) return func(seek_table);
        return false;
    }

    FLAC__bool FLAC__format_cuesheet_is_legal(const FLAC__StreamMetadata_CueSheet *cue_sheet, FLAC__bool check_cd_da_subset, const char **violation) {
        static FLAC__bool (*func)(const FLAC__StreamMetadata_CueSheet*,FLAC__bool, const char**) = NULL;
        if(func == NULL) {
            func = flac_bindings::libFlac->getSymbolAddress<FLAC__bool (*)(const FLAC__StreamMetadata_CueSheet*,FLAC__bool, const char**)>("FLAC__format_cuesheet_is_legal");
            if(func == NULL) printf("%s\n", flac_bindings::libFlac->getLastError().c_str());
        }
        if(func != NULL) return func(cue_sheet, check_cd_da_subset, violation);
        return false;
    }

    FLAC__bool FLAC__format_picture_is_legal(const FLAC__StreamMetadata_Picture *picture, const char **violation) {
        static FLAC__bool (*func)(const FLAC__StreamMetadata_Picture*, const char**) = NULL;
        if(func == NULL) {
            func = flac_bindings::libFlac->getSymbolAddress<FLAC__bool (*)(const FLAC__StreamMetadata_Picture*, const char**)>("FLAC__format_picture_is_legal");
            if(func == NULL) printf("%s\n", flac_bindings::libFlac->getLastError().c_str());
        }
        if(func != NULL) return func(picture, violation);
        return false;
    }

}


namespace flac_bindings {

    NAN_METHOD(node_FLAC__format_sample_rate_is_valid) {
        auto maybeSampleRate = numberFromJs<unsigned>(info[0]);
        if(maybeSampleRate.IsJust()) {
            unsigned sampleRate = maybeSampleRate.FromJust();
            FLAC__bool ret = FLAC__format_sample_rate_is_valid(sampleRate);
            info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        } else {
            info.GetReturnValue().Set(Nan::False());
        }
    }

    NAN_METHOD(node_FLAC__format_blocksize_is_subset) {
        auto maybeBlockSize = numberFromJs<unsigned>(info[0]);
        auto maybeSampleRate = numberFromJs<unsigned>(info[1]);
        if(maybeSampleRate.IsJust() && maybeBlockSize.IsJust()) {
            unsigned blocksize = maybeBlockSize.FromJust();
            unsigned sampleRate = maybeSampleRate.FromJust();
            FLAC__bool ret = FLAC__format_blocksize_is_subset(blocksize, sampleRate);
            info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        } else {
            info.GetReturnValue().Set(Nan::False());
        }
    }

    NAN_METHOD(node_FLAC__format_sample_rate_is_subset) {
        auto maybeSampleRate = numberFromJs<unsigned>(info[0]);
        if(maybeSampleRate.IsJust()) {
            unsigned sampleRate = maybeSampleRate.FromJust();
            FLAC__bool ret = FLAC__format_sample_rate_is_subset(sampleRate);
            info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        } else {
            info.GetReturnValue().Set(Nan::False());
        }
    }

    NAN_METHOD(node_FLAC__format_vorbiscomment_entry_name_is_legal) {
        MaybeLocal<String> jsStrMaybe = Nan::To<v8::String>(info[0]);
        if(jsStrMaybe.IsEmpty() || !info[0]->IsString()) {
            info.GetReturnValue().Set(Nan::False());
            return;
        }

        Local<String> jsStr = jsStrMaybe.ToLocalChecked();
        Nan::Utf8String str(jsStr);
        FLAC__bool ret = FLAC__format_vorbiscomment_entry_name_is_legal(*str);
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
    }

    NAN_METHOD(node_FLAC__format_vorbiscomment_entry_value_is_legal) {
        MaybeLocal<String> jsStrMaybe = Nan::To<v8::String>(info[0]);
        if(jsStrMaybe.IsEmpty() || !info[0]->IsString()) {
            info.GetReturnValue().Set(Nan::False());
            return;
        }

        Local<String> jsStr = jsStrMaybe.ToLocalChecked();
        Nan::Utf8String str(jsStr);
        FLAC__bool ret = FLAC__format_vorbiscomment_entry_value_is_legal((uint8_t*) *str, str.length());
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
    }

    NAN_METHOD(node_FLAC__format_vorbiscomment_entry_is_legal) {
        MaybeLocal<String> jsStrMaybe = Nan::To<v8::String>(info[0]);
        if(jsStrMaybe.IsEmpty() || !info[0]->IsString()) {
            info.GetReturnValue().Set(Nan::False());
            return;
        }

        Local<String> jsStr = jsStrMaybe.ToLocalChecked();
        Nan::Utf8String str(jsStr);
        FLAC__bool ret = FLAC__format_vorbiscomment_entry_is_legal((uint8_t*) *str, str.length());
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
    }

    NAN_METHOD(node_FLAC__format_seektable_is_legal) {
        FLAC__StreamMetadata* table = jsToStruct<FLAC__StreamMetadata>(info[0]);
        if(table == nullptr) return;
        if(table->type != FLAC__METADATA_TYPE_SEEKTABLE) {
            Nan::ThrowTypeError("Metadata is not of type SeekTable");
            return;
        }

        FLAC__bool ret = FLAC__format_seektable_is_legal(&table->data.seek_table);
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
    }

    NAN_METHOD(node_FLAC__format_seektable_sort) {
        FLAC__StreamMetadata* table = jsToStruct<FLAC__StreamMetadata>(info[0]);
        if(table == nullptr) return;
        if(table->type != FLAC__METADATA_TYPE_SEEKTABLE) {
            Nan::ThrowTypeError("Metadata is not of type SeekTable");
            return;
        }

        unsigned numberOfPlaceholdersConvertedToTemplates = FLAC__format_seektable_sort(&table->data.seek_table);
        info.GetReturnValue().Set(numberToJs(numberOfPlaceholdersConvertedToTemplates));
    }

    NAN_METHOD(node_FLAC__format_cuesheet_is_legal) {
        FLAC__StreamMetadata* cue = jsToStruct<FLAC__StreamMetadata>(info[0]);
        if(cue == nullptr) return;
        if(cue->type != FLAC__METADATA_TYPE_CUESHEET) {
            Nan::ThrowTypeError("Metadata is not of type CueSheet");
            return;
        }

        bool check = Nan::To<int>(info[1].As<Boolean>()).FromJust();
        const char* violation = NULL;
        FLAC__bool ret = FLAC__format_cuesheet_is_legal(&cue->data.cue_sheet, check, &violation);
        if(ret) {
            info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        } else {
            info.GetReturnValue().Set(Nan::New(violation).ToLocalChecked());
        }
    }

    NAN_METHOD(node_FLAC__format_picture_is_legal) {
        FLAC__StreamMetadata* picture = jsToStruct<FLAC__StreamMetadata>(info[0]);
        if(picture == nullptr) return;
        if(picture->type != FLAC__METADATA_TYPE_PICTURE) {
            Nan::ThrowTypeError("Metadata is not of type Picture");
            return;
        }

        const char* violation = NULL;
        FLAC__bool ret = FLAC__format_picture_is_legal(&picture->data.picture, &violation);
        if(ret) {
            info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        } else {
            info.GetReturnValue().Set(Nan::New(violation).ToLocalChecked());
        }
    }

    FlacEnumDefineReturnType createMetadataTypeEnum() {
        Local<Object> obj1 = Nan::New<Object>();
        Local<Object> obj2 = Nan::New<Object>();
        flacEnum_defineValue(obj1, obj2, "STREAMINFO", FLAC__METADATA_TYPE_STREAMINFO);
        flacEnum_defineValue(obj1, obj2, "PADDING", FLAC__METADATA_TYPE_PADDING);
        flacEnum_defineValue(obj1, obj2, "APPLICATION", FLAC__METADATA_TYPE_APPLICATION);
        flacEnum_defineValue(obj1, obj2, "SEEKTABLE", FLAC__METADATA_TYPE_SEEKTABLE);
        flacEnum_defineValue(obj1, obj2, "VORBIS_COMMENT", FLAC__METADATA_TYPE_VORBIS_COMMENT);
        flacEnum_defineValue(obj1, obj2, "CUESHEET", FLAC__METADATA_TYPE_CUESHEET);
        flacEnum_defineValue(obj1, obj2, "PICTURE", FLAC__METADATA_TYPE_PICTURE);
        flacEnum_defineValue(obj1, obj2, "UNDEFINED", FLAC__METADATA_TYPE_UNDEFINED);
        flacEnum_defineValue(obj1, obj2, "MAX_METADATA_TYPE", FLAC__MAX_METADATA_TYPE);
        return std::make_tuple(obj1, obj2);
    }

    FlacEnumDefineReturnType createEntropyCodingMethodTypeEnum() {
        Local<Object> obj1 = Nan::New<Object>();
        Local<Object> obj2 = Nan::New<Object>();
        flacEnum_defineValue(obj1, obj2, "PARTITIONED_RICE", FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE);
        flacEnum_defineValue(obj1, obj2, "PARTITIONED_RICE2", FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE2);
        return std::make_tuple(obj1, obj2);
    }

    FlacEnumDefineReturnType createSubframeTypeEnum() {
        Local<Object> obj1 = Nan::New<Object>();
        Local<Object> obj2 = Nan::New<Object>();
        flacEnum_defineValue(obj1, obj2, "CONSTANT", FLAC__SUBFRAME_TYPE_CONSTANT);
        flacEnum_defineValue(obj1, obj2, "VERBATIM", FLAC__SUBFRAME_TYPE_VERBATIM);
        flacEnum_defineValue(obj1, obj2, "FIXED", FLAC__SUBFRAME_TYPE_FIXED);
        flacEnum_defineValue(obj1, obj2, "LPC", FLAC__SUBFRAME_TYPE_LPC);
        return std::make_tuple(obj1, obj2);
    }

    FlacEnumDefineReturnType createChannelAssignmentEnum() {
        Local<Object> obj1 = Nan::New<Object>();
        Local<Object> obj2 = Nan::New<Object>();
        flacEnum_defineValue(obj1, obj2, "INDEPENDENT", FLAC__CHANNEL_ASSIGNMENT_INDEPENDENT);
        flacEnum_defineValue(obj1, obj2, "LEFT_SIDE", FLAC__CHANNEL_ASSIGNMENT_LEFT_SIDE);
        flacEnum_defineValue(obj1, obj2, "RIGHT_SIDE", FLAC__CHANNEL_ASSIGNMENT_RIGHT_SIDE);
        flacEnum_defineValue(obj1, obj2, "MID_SIDE", FLAC__CHANNEL_ASSIGNMENT_MID_SIDE);
        return std::make_tuple(obj1, obj2);
    }

    FlacEnumDefineReturnType createFrameNumberTypeEnum() {
        Local<Object> obj1 = Nan::New<Object>();
        Local<Object> obj2 = Nan::New<Object>();
        flacEnum_defineValue(obj1, obj2, "FRAME_NUMBER", FLAC__FRAME_NUMBER_TYPE_FRAME_NUMBER);
        flacEnum_defineValue(obj1, obj2, "SAMPLE_NUMBER", FLAC__FRAME_NUMBER_TYPE_SAMPLE_NUMBER);
        return std::make_tuple(obj1, obj2);
    }

    FlacEnumDefineReturnType createPictureTypeEnum() {
        Local<Object> obj1 = Nan::New<Object>();
        Local<Object> obj2 = Nan::New<Object>();
        flacEnum_defineValue(obj1, obj2, "OTHER", FLAC__STREAM_METADATA_PICTURE_TYPE_OTHER);
        flacEnum_defineValue(obj1, obj2, "FILE_ICON_STANDARD", FLAC__STREAM_METADATA_PICTURE_TYPE_FILE_ICON_STANDARD);
        flacEnum_defineValue(obj1, obj2, "FILE_ICON", FLAC__STREAM_METADATA_PICTURE_TYPE_FILE_ICON);
        flacEnum_defineValue(obj1, obj2, "FRONT_COVER", FLAC__STREAM_METADATA_PICTURE_TYPE_FRONT_COVER);
        flacEnum_defineValue(obj1, obj2, "BACK_COVER", FLAC__STREAM_METADATA_PICTURE_TYPE_BACK_COVER);
        flacEnum_defineValue(obj1, obj2, "LEAFLET_PAGE", FLAC__STREAM_METADATA_PICTURE_TYPE_LEAFLET_PAGE);
        flacEnum_defineValue(obj1, obj2, "MEDIA", FLAC__STREAM_METADATA_PICTURE_TYPE_MEDIA);
        flacEnum_defineValue(obj1, obj2, "LEAD_ARTIST", FLAC__STREAM_METADATA_PICTURE_TYPE_LEAD_ARTIST);
        flacEnum_defineValue(obj1, obj2, "ARTIST", FLAC__STREAM_METADATA_PICTURE_TYPE_ARTIST);
        flacEnum_defineValue(obj1, obj2, "CONDUCTOR", FLAC__STREAM_METADATA_PICTURE_TYPE_CONDUCTOR);
        flacEnum_defineValue(obj1, obj2, "BAND", FLAC__STREAM_METADATA_PICTURE_TYPE_BAND);
        flacEnum_defineValue(obj1, obj2, "COMPOSER", FLAC__STREAM_METADATA_PICTURE_TYPE_COMPOSER);
        flacEnum_defineValue(obj1, obj2, "LYRICIST", FLAC__STREAM_METADATA_PICTURE_TYPE_LYRICIST);
        flacEnum_defineValue(obj1, obj2, "RECORDING_LOCATION", FLAC__STREAM_METADATA_PICTURE_TYPE_RECORDING_LOCATION);
        flacEnum_defineValue(obj1, obj2, "DURING_RECORDING", FLAC__STREAM_METADATA_PICTURE_TYPE_DURING_RECORDING);
        flacEnum_defineValue(obj1, obj2, "DURING_PERFORMANCE", FLAC__STREAM_METADATA_PICTURE_TYPE_DURING_PERFORMANCE);
        flacEnum_defineValue(obj1, obj2, "VIDEO_SCREEN_CAPTURE", FLAC__STREAM_METADATA_PICTURE_TYPE_VIDEO_SCREEN_CAPTURE);
        flacEnum_defineValue(obj1, obj2, "FISH", FLAC__STREAM_METADATA_PICTURE_TYPE_FISH);
        flacEnum_defineValue(obj1, obj2, "ILLUSTRATION", FLAC__STREAM_METADATA_PICTURE_TYPE_ILLUSTRATION);
        flacEnum_defineValue(obj1, obj2, "BAND_LOGOTYPE", FLAC__STREAM_METADATA_PICTURE_TYPE_BAND_LOGOTYPE);
        flacEnum_defineValue(obj1, obj2, "PUBLISHER_LOGOTYPE", FLAC__STREAM_METADATA_PICTURE_TYPE_PUBLISHER_LOGOTYPE);
        flacEnum_defineValue(obj1, obj2, "UNDEFINED", FLAC__STREAM_METADATA_PICTURE_TYPE_UNDEFINED);
        return std::make_tuple(obj1, obj2);
    }

    NAN_MODULE_INIT(initFormat) {
        Local<Object> obj = Nan::New<Object>();

        FLAC__VERSION_STRING = *libFlac->getSymbolAddress<const char**>("FLAC__VERSION_STRING");
        if(FLAC__VERSION_STRING == nullptr) printf("%s\n", libFlac->getLastError().c_str());
        FLAC__VENDOR_STRING = *libFlac->getSymbolAddress<const char**>("FLAC__VENDOR_STRING");
        if(FLAC__VENDOR_STRING == nullptr) printf("%s\n", libFlac->getLastError().c_str());

        Nan::Set(obj, Nan::New("FLAC__VERSION_STRING").ToLocalChecked(), Nan::New<String>(FLAC__VERSION_STRING).ToLocalChecked());
        Nan::Set(obj, Nan::New("FLAC__VENDOR_STRING").ToLocalChecked(), Nan::New<String>(FLAC__VENDOR_STRING).ToLocalChecked());
        Nan::SetMethod(obj, "sampleRateIsValid", node_FLAC__format_sample_rate_is_valid);
        Nan::SetMethod(obj, "blocksizeIsSubset", node_FLAC__format_blocksize_is_subset);
        Nan::SetMethod(obj, "sampleRateIsSubset", node_FLAC__format_sample_rate_is_subset);
        Nan::SetMethod(obj, "vorbiscommentEntryNameIsLegal", node_FLAC__format_vorbiscomment_entry_name_is_legal);
        Nan::SetMethod(obj, "vorbiscommentEntryValueIsLegal", node_FLAC__format_vorbiscomment_entry_value_is_legal);
        Nan::SetMethod(obj, "vorbiscommentEntryIsLegal", node_FLAC__format_vorbiscomment_entry_is_legal);
        Nan::SetMethod(obj, "seektableIsLegal", node_FLAC__format_seektable_is_legal);
        Nan::SetMethod(obj, "seektableSort", node_FLAC__format_seektable_sort);
        Nan::SetMethod(obj, "cuesheetIsLegal", node_FLAC__format_cuesheet_is_legal);
        Nan::SetMethod(obj, "pictureIsLegal", node_FLAC__format_picture_is_legal);

        flacEnum_declareInObject(obj, MetadataType, createMetadataTypeEnum());
        flacEnum_declareInObject(obj, EntropyCodingMethodType, createEntropyCodingMethodTypeEnum());
        flacEnum_declareInObject(obj, SubframeType, createSubframeTypeEnum());
        flacEnum_declareInObject(obj, ChannelAssignment, createChannelAssignmentEnum());
        flacEnum_declareInObject(obj, FrameNumberType, createFrameNumberTypeEnum());
        flacEnum_declareInObject(obj, PictureType, createPictureTypeEnum());

        Nan::Set(target, Nan::New("format").ToLocalChecked(), obj);
    }
}
