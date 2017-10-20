#include "nan.h"
#include "dl.hpp"

using namespace v8;
using namespace node;
#include "pointer.hpp"
#include "format.h"

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
        unsigned sampleRate = Nan::To<unsigned>(info[0]).FromJust();
        FLAC__bool ret = FLAC__format_sample_rate_is_valid(sampleRate);
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
    }

    NAN_METHOD(node_FLAC__format_blocksize_is_subset) {
        unsigned blocksize = Nan::To<unsigned>(info[1]).FromJust();
        unsigned sampleRate = Nan::To<unsigned>(info[1]).FromJust();
        FLAC__bool ret = FLAC__format_blocksize_is_subset(blocksize, sampleRate);
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
    }

    NAN_METHOD(node_FLAC__format_sample_rate_is_subset) {
        unsigned sampleRate = Nan::To<unsigned>(info[0]).FromJust();
        FLAC__bool ret = FLAC__format_sample_rate_is_subset(sampleRate);
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
    }

    NAN_METHOD(node_FLAC__format_vorbiscomment_entry_name_is_legal) {
        Local<String> jsStr = info[0]->ToString();
        char* str = new char[jsStr->Utf8Length() + 1];
        jsStr->WriteUtf8(str);
        str[jsStr->Utf8Length()] = '\0';
        FLAC__bool ret = FLAC__format_vorbiscomment_entry_name_is_legal(str);
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        delete[] str;
    }

    NAN_METHOD(node_FLAC__format_vorbiscomment_entry_value_is_legal) {
        Local<String> jsStr = Nan::To<String>(info[0]).ToLocalChecked();
        char* str = new char[jsStr->Utf8Length() + 1];
        jsStr->WriteUtf8(str);
        str[jsStr->Utf8Length()] = '\0';
        FLAC__bool ret = FLAC__format_vorbiscomment_entry_value_is_legal((uint8_t*) str, jsStr->Utf8Length());
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        delete[] str;
    }

    NAN_METHOD(node_FLAC__format_vorbiscomment_entry_is_legal) {
        Local<String> jsStr = Nan::To<String>(info[0]).ToLocalChecked();
        char* str = new char[jsStr->Utf8Length() + 1];
        jsStr->WriteUtf8(str);
        str[jsStr->Utf8Length()] = '\0';
        FLAC__bool ret = FLAC__format_vorbiscomment_entry_is_legal((uint8_t*) str, jsStr->Utf8Length());
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        delete[] str;
    }

    NAN_METHOD(node_FLAC__format_seektable_is_legal) {
        FLAC__StreamMetadata_SeekTable* table = fromjs<FLAC__StreamMetadata_SeekTable>(info[0]);
        FLAC__bool ret = FLAC__format_seektable_is_legal(table);
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
    }

    NAN_METHOD(node_FLAC__format_seektable_sort) {
        FLAC__StreamMetadata_SeekTable* table = fromjs<FLAC__StreamMetadata_SeekTable>(info[0]);
        FLAC__format_seektable_sort(table);
        info.GetReturnValue().Set(structToJs(table));
    }

    NAN_METHOD(node_FLAC__format_cuesheet_is_legal) {
        FLAC__StreamMetadata_CueSheet* cue = fromjs<FLAC__StreamMetadata_CueSheet>(info[0]);
        bool check = Nan::To<int>(info[1].As<Boolean>()).FromJust();
        const char* violation = NULL;
        FLAC__bool ret = FLAC__format_cuesheet_is_legal(cue, check, &violation);
        if(ret) {
            info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        } else {
            info.GetReturnValue().Set(Nan::New(violation).ToLocalChecked());
        }
    }

    NAN_METHOD(node_FLAC__format_picture_is_legal) {
        FLAC__StreamMetadata_Picture* picture = fromjs<FLAC__StreamMetadata_Picture>(info[0]);
        const char* violation = NULL;
        FLAC__bool ret = FLAC__format_picture_is_legal(picture, &violation);
        if(ret) {
            info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        } else {
            info.GetReturnValue().Set(Nan::New(violation).ToLocalChecked());
        }
    }

    NAN_PROPERTY_GETTER(MetadataType) {
        char* propertyName = new char[property->Utf8Length() + 1];
        property->WriteUtf8(propertyName);
        propertyName[property->Utf8Length()] = '\0';
        std::string PropertyName(propertyName);

        if(PropertyName == "STREAMINFO") info.GetReturnValue().Set(Nan::New(FLAC__METADATA_TYPE_STREAMINFO));
        else if(PropertyName == "PADDING") info.GetReturnValue().Set(Nan::New(FLAC__METADATA_TYPE_PADDING));
        else if(PropertyName == "APPLICATION") info.GetReturnValue().Set(Nan::New(FLAC__METADATA_TYPE_APPLICATION));
        else if(PropertyName == "SEEKTABLE") info.GetReturnValue().Set(Nan::New(FLAC__METADATA_TYPE_SEEKTABLE));
        else if(PropertyName == "VORBIS_COMMENT") info.GetReturnValue().Set(Nan::New(FLAC__METADATA_TYPE_VORBIS_COMMENT));
        else if(PropertyName == "CUESHEET") info.GetReturnValue().Set(Nan::New(FLAC__METADATA_TYPE_CUESHEET));
        else if(PropertyName == "PICTURE") info.GetReturnValue().Set(Nan::New(FLAC__METADATA_TYPE_PICTURE));
        else if(PropertyName == "UNDEFINED") info.GetReturnValue().Set(Nan::New(FLAC__METADATA_TYPE_UNDEFINED));
        else info.GetReturnValue().SetUndefined();
    }

    NAN_PROPERTY_GETTER(EntropyCodingMethodType) {
        Nan::Utf8String propertyName(property);
        std::string PropertyName(*propertyName);

        if(PropertyName == "PARTITIONED_RICE") info.GetReturnValue().Set(Nan::New(FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE));
        else if(PropertyName == "PARTITIONED_RICE2") info.GetReturnValue().Set(Nan::New(FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE2));
        else info.GetReturnValue().SetUndefined();
    }

    NAN_PROPERTY_GETTER(SubframeType) {
        Nan::Utf8String propertyName(property);
        std::string PropertyName(*propertyName);

        if(PropertyName == "CONSTANT") info.GetReturnValue().Set(Nan::New(FLAC__SUBFRAME_TYPE_CONSTANT));
        else if(PropertyName == "VERBATIM") info.GetReturnValue().Set(Nan::New(FLAC__SUBFRAME_TYPE_VERBATIM));
        else if(PropertyName == "FIXED") info.GetReturnValue().Set(Nan::New(FLAC__SUBFRAME_TYPE_FIXED));
        else if(PropertyName == "LPC") info.GetReturnValue().Set(Nan::New(FLAC__SUBFRAME_TYPE_LPC));
        else info.GetReturnValue().SetUndefined();
    }

    NAN_PROPERTY_GETTER(ChannelAssignment) {
        Nan::Utf8String propertyName(property);
        std::string PropertyName(*propertyName);

        if(PropertyName == "INDEPENDENT") info.GetReturnValue().Set(Nan::New(FLAC__CHANNEL_ASSIGNMENT_INDEPENDENT));
        else if(PropertyName == "LEFT_SIDE") info.GetReturnValue().Set(Nan::New(FLAC__CHANNEL_ASSIGNMENT_LEFT_SIDE));
        else if(PropertyName == "RIGHT_SIDE") info.GetReturnValue().Set(Nan::New(FLAC__CHANNEL_ASSIGNMENT_RIGHT_SIDE));
        else if(PropertyName == "MID_SIDE") info.GetReturnValue().Set(Nan::New(FLAC__CHANNEL_ASSIGNMENT_MID_SIDE));
        else info.GetReturnValue().SetUndefined();
    }

    NAN_PROPERTY_GETTER(FrameNumberType) {
        Nan::Utf8String propertyName(property);
        std::string PropertyName(*propertyName);

        if(PropertyName == "FRAME_NUMBER") info.GetReturnValue().Set(Nan::New(FLAC__FRAME_NUMBER_TYPE_FRAME_NUMBER));
        else if(PropertyName == "SAMPLE_NUMBER") info.GetReturnValue().Set(Nan::New(FLAC__FRAME_NUMBER_TYPE_SAMPLE_NUMBER));
        else info.GetReturnValue().SetUndefined();
    }

    NAN_PROPERTY_GETTER(StreamMetadata_Picture_Type) {
        Nan::Utf8String propertyName(property);
        std::string PropertyName(*propertyName);

        if(PropertyName == "OTHER") info.GetReturnValue().Set(Nan::New(FLAC__STREAM_METADATA_PICTURE_TYPE_OTHER));
        else if(PropertyName == "FILE_ICON_STANDARD") info.GetReturnValue().Set(Nan::New(FLAC__STREAM_METADATA_PICTURE_TYPE_FILE_ICON_STANDARD));
        else if(PropertyName == "FILE_ICON") info.GetReturnValue().Set(Nan::New(FLAC__STREAM_METADATA_PICTURE_TYPE_FILE_ICON));
        else if(PropertyName == "FRONT_COVER") info.GetReturnValue().Set(Nan::New(FLAC__STREAM_METADATA_PICTURE_TYPE_FRONT_COVER));
        else if(PropertyName == "BACK_COVER") info.GetReturnValue().Set(Nan::New(FLAC__STREAM_METADATA_PICTURE_TYPE_BACK_COVER));
        else if(PropertyName == "LEAFLET_PAGE") info.GetReturnValue().Set(Nan::New(FLAC__STREAM_METADATA_PICTURE_TYPE_LEAFLET_PAGE));
        else if(PropertyName == "MEDIA") info.GetReturnValue().Set(Nan::New(FLAC__STREAM_METADATA_PICTURE_TYPE_MEDIA));
        else if(PropertyName == "LEAD_ARTIST") info.GetReturnValue().Set(Nan::New(FLAC__STREAM_METADATA_PICTURE_TYPE_LEAD_ARTIST));
        else if(PropertyName == "ARTIST") info.GetReturnValue().Set(Nan::New(FLAC__STREAM_METADATA_PICTURE_TYPE_ARTIST));
        else if(PropertyName == "CONDUCTOR") info.GetReturnValue().Set(Nan::New(FLAC__STREAM_METADATA_PICTURE_TYPE_CONDUCTOR));
        else if(PropertyName == "BAND") info.GetReturnValue().Set(Nan::New(FLAC__STREAM_METADATA_PICTURE_TYPE_BAND));
        else if(PropertyName == "COMPOSER") info.GetReturnValue().Set(Nan::New(FLAC__STREAM_METADATA_PICTURE_TYPE_COMPOSER));
        else if(PropertyName == "LYRICIST") info.GetReturnValue().Set(Nan::New(FLAC__STREAM_METADATA_PICTURE_TYPE_LYRICIST));
        else if(PropertyName == "RECORDING_LOCATION") info.GetReturnValue().Set(Nan::New(FLAC__STREAM_METADATA_PICTURE_TYPE_RECORDING_LOCATION));
        else if(PropertyName == "DURING_RECORDING") info.GetReturnValue().Set(Nan::New(FLAC__STREAM_METADATA_PICTURE_TYPE_DURING_RECORDING));
        else if(PropertyName == "DURING_PERFORMANCE") info.GetReturnValue().Set(Nan::New(FLAC__STREAM_METADATA_PICTURE_TYPE_DURING_PERFORMANCE));
        else if(PropertyName == "VIDEO_SCREEN_CAPTURE") info.GetReturnValue().Set(Nan::New(FLAC__STREAM_METADATA_PICTURE_TYPE_VIDEO_SCREEN_CAPTURE));
        else if(PropertyName == "FISH") info.GetReturnValue().Set(Nan::New(FLAC__STREAM_METADATA_PICTURE_TYPE_FISH));
        else if(PropertyName == "ILLUSTRATION") info.GetReturnValue().Set(Nan::New(FLAC__STREAM_METADATA_PICTURE_TYPE_ILLUSTRATION));
        else if(PropertyName == "BAND_LOGOTYPE") info.GetReturnValue().Set(Nan::New(FLAC__STREAM_METADATA_PICTURE_TYPE_BAND_LOGOTYPE));
        else if(PropertyName == "PUBLISHER_LOGOTYPE") info.GetReturnValue().Set(Nan::New(FLAC__STREAM_METADATA_PICTURE_TYPE_PUBLISHER_LOGOTYPE));
        else if(PropertyName == "UNDEFINED") info.GetReturnValue().Set(Nan::New(FLAC__STREAM_METADATA_PICTURE_TYPE_UNDEFINED));
        else info.GetReturnValue().SetUndefined();
    }

    NAN_INDEX_GETTER(MetadataTypeString) {
        if(index < 8 || index == FLAC__MAX_METADATA_TYPE_CODE) {
            info.GetReturnValue().Set(Nan::New(FLAC__MetadataTypeString[index]).ToLocalChecked());
        } else {
            info.GetReturnValue().SetNull();
        }
    }

    NAN_INDEX_ENUMERATOR(MetadataTypeString) {
        Local<Array> array = Nan::New<Array>();
        for(int i = 0; i < 8; i++) Nan::Set(array, i, Nan::New(i));
        info.GetReturnValue().Set(array);
    }

    NAN_INDEX_GETTER(EntropyCodingMethodTypeString) {
        if(index < 2) {
            info.GetReturnValue().Set(Nan::New(FLAC__EntropyCodingMethodTypeString[index]).ToLocalChecked());
        } else {
            info.GetReturnValue().SetNull();
        }
    }

    NAN_INDEX_ENUMERATOR(EntropyCodingMethodTypeString) {
        Local<Array> array = Nan::New<Array>();
        for(int i = 0; i < 2; i++) Nan::Set(array, i, Nan::New(i));
        info.GetReturnValue().Set(array);
    }

    NAN_INDEX_GETTER(SubframeTypeString) {
        if(index < 4) {
            info.GetReturnValue().Set(Nan::New(FLAC__SubframeTypeString[index]).ToLocalChecked());
        } else {
            info.GetReturnValue().SetNull();
        }
    }

    NAN_INDEX_ENUMERATOR(SubframeTypeString) {
        Local<Array> array = Nan::New<Array>();
        for(int i = 0; i < 4; i++) Nan::Set(array, i, Nan::New(i));
        info.GetReturnValue().Set(array);
    }

    NAN_INDEX_GETTER(ChannelAssignmentString) {
        if(index < 4) {
            info.GetReturnValue().Set(Nan::New(FLAC__ChannelAssignmentString[index]).ToLocalChecked());
        } else {
            info.GetReturnValue().SetNull();
        }
    }

    NAN_INDEX_ENUMERATOR(ChannelAssignmentString) {
        Local<Array> array = Nan::New<Array>();
        for(int i = 0; i < 94; i++) Nan::Set(array, i, Nan::New(i));
        info.GetReturnValue().Set(array);
    }

    NAN_INDEX_GETTER(FrameNumberTypeString) {
        if(index < 2) {
            info.GetReturnValue().Set(Nan::New(FLAC__FrameNumberTypeString[index]).ToLocalChecked());
        } else {
            info.GetReturnValue().SetNull();
        }
    }

    NAN_INDEX_ENUMERATOR(FrameNumberTypeString) {
        Local<Array> array = Nan::New<Array>();
        for(int i = 0; i < 2; i++) Nan::Set(array, i, Nan::New(i));
        info.GetReturnValue().Set(array);
    }

    NAN_INDEX_GETTER(StreamMetadata_Picture_TypeString) {
        if(index < 22) {
            info.GetReturnValue().Set(Nan::New(FLAC__StreamMetadata_Picture_TypeString[index]).ToLocalChecked());
        } else {
            info.GetReturnValue().SetNull();
        }
    }

    NAN_INDEX_ENUMERATOR(StreamMetadata_Picture_TypeString) {
        Local<Array> array = Nan::New<Array>();
        for(int i = 0; i < 22; i++) Nan::Set(array, i, Nan::New(i));
        info.GetReturnValue().Set(array);
    }

    NAN_MODULE_INIT(initFormat) {
        Local<Object> obj = Nan::New<Object>();

        FLAC__VERSION_STRING = *libFlac->getSymbolAddress<const char**>("FLAC__VERSION_STRING");
        if(FLAC__VERSION_STRING == nullptr) printf("%s\n", libFlac->getLastError().c_str());
        FLAC__VENDOR_STRING = *libFlac->getSymbolAddress<const char**>("FLAC__VENDOR_STRING");
        if(FLAC__VENDOR_STRING == nullptr) printf("%s\n", libFlac->getLastError().c_str());

        Nan::Set(obj, Nan::New("FLAC__VERSION_STRING").ToLocalChecked(), Nan::New<String>(FLAC__VERSION_STRING).ToLocalChecked());
        Nan::Set(obj, Nan::New("FLAC__VENDOR_STRING").ToLocalChecked(), Nan::New<String>(FLAC__VENDOR_STRING).ToLocalChecked());
        Nan::SetMethod(obj, "sample_rate_is_valid", node_FLAC__format_sample_rate_is_valid);
        Nan::SetMethod(obj, "blocksize_is_subset", node_FLAC__format_blocksize_is_subset);
        Nan::SetMethod(obj, "sample_rate_is_subset", node_FLAC__format_sample_rate_is_subset);
        Nan::SetMethod(obj, "vorbiscomment_entry_name_is_legal", node_FLAC__format_vorbiscomment_entry_name_is_legal);
        Nan::SetMethod(obj, "vorbiscomment_entry_value_is_legal", node_FLAC__format_vorbiscomment_entry_value_is_legal);
        Nan::SetMethod(obj, "vorbiscomment_entry_is_legal", node_FLAC__format_vorbiscomment_entry_is_legal);
        Nan::SetMethod(obj, "seektable_is_legal", node_FLAC__format_seektable_is_legal);
        Nan::SetMethod(obj, "seektable_sort", node_FLAC__format_seektable_sort);
        Nan::SetMethod(obj, "cuesheet_is_legal", node_FLAC__format_cuesheet_is_legal);
        Nan::SetMethod(obj, "picture_is_legal", node_FLAC__format_picture_is_legal);

        #define propertyGetter(func) \
        Local<ObjectTemplate> _JOIN(func, Var) = Nan::New<ObjectTemplate>(); \
        Nan::SetNamedPropertyHandler(_JOIN(func, Var), func); \
        Nan::Set(obj, Nan::New(#func).ToLocalChecked(), Nan::NewInstance(_JOIN(func, Var)).ToLocalChecked());

        propertyGetter(MetadataType);
        propertyGetter(EntropyCodingMethodType);
        propertyGetter(SubframeType);
        propertyGetter(ChannelAssignment);
        propertyGetter(FrameNumberType);
        propertyGetter(StreamMetadata_Picture_Type);

        #define indexGetter(func) \
        _JOIN(FLAC__, func) = libFlac->getSymbolAddress<const char* const*>("FLAC__" #func); \
        Local<ObjectTemplate> _JOIN(func, _template) = Nan::New<ObjectTemplate>(); \
        Nan::SetIndexedPropertyHandler(_JOIN(func, _template), func, nullptr, nullptr, nullptr, func); \
        Nan::Set(obj, Nan::New(#func).ToLocalChecked(), Nan::NewInstance(_JOIN(func, _template)).ToLocalChecked());

        indexGetter(MetadataTypeString);
        indexGetter(EntropyCodingMethodTypeString);
        indexGetter(SubframeTypeString);
        indexGetter(ChannelAssignmentString);
        indexGetter(FrameNumberTypeString);
        indexGetter(StreamMetadata_Picture_TypeString);

        Nan::Set(target, Nan::New("format").ToLocalChecked(), obj);
    }
}
