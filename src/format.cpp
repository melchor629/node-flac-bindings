#include "format.h"
#include "nan.h"
#include <dlfcn.h>

namespace flac_bindings {
extern void* libFlacHandle;
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
            const char* err = dlerror();
            func = (FLAC__bool (*)(unsigned)) dlsym(flac_bindings::libFlacHandle, "FLAC__format_sample_rate_is_valid");
            if((err = dlerror())) printf("%s\n", err);
        }
        if(func != NULL) return func(sample_rate);
        return false;
    }

    FLAC__bool FLAC__format_blocksize_is_subset(unsigned blocksize, unsigned sample_rate) {
        static FLAC__bool (*func)(unsigned, unsigned) = NULL;
        if(func == NULL) {
            const char* err = dlerror();
            func = (FLAC__bool (*)(unsigned, unsigned)) dlsym(flac_bindings::libFlacHandle, "FLAC__format_blocksize_is_subset");
            if((err = dlerror())) printf("%s\n", err);
        }
        if(func != NULL) return func(blocksize, sample_rate);
        return false;
    }

    FLAC__bool FLAC__format_sample_rate_is_subset(unsigned sample_rate) {
        static FLAC__bool (*func)(unsigned) = NULL;
        if(func == NULL) {
            const char* err = dlerror();
            func = (FLAC__bool (*)(unsigned)) dlsym(flac_bindings::libFlacHandle, "FLAC__format_sample_rate_is_subset");
            if((err = dlerror())) printf("%s\n", err);
        }
        if(func != NULL) return func(sample_rate);
        return false;
    }

    FLAC__bool FLAC__format_vorbiscomment_entry_name_is_legal(const char *name) {
        static FLAC__bool (*func)(const char*) = NULL;
        if(func == NULL) {
            const char* err = dlerror();
            func = (FLAC__bool (*)(const char*)) dlsym(flac_bindings::libFlacHandle, "FLAC__format_vorbiscomment_entry_name_is_legal");
            if((err = dlerror())) printf("%s\n", err);
        }
        if(func != NULL) return func(name);
        return false;
    }

    FLAC__bool FLAC__format_vorbiscomment_entry_value_is_legal(const FLAC__byte *value, unsigned length) {
        static FLAC__bool (*func)(const FLAC__byte*, unsigned) = NULL;
        if(func == NULL) {
            const char* err = dlerror();
            func = (FLAC__bool (*)(const FLAC__byte*, unsigned)) dlsym(flac_bindings::libFlacHandle, "FLAC__format_vorbiscomment_entry_value_is_legal");
            if((err = dlerror())) printf("%s\n", err);
        }
        if(func != NULL) return func(value, length);
        return false;
    }

    FLAC__bool FLAC__format_vorbiscomment_entry_is_legal(const FLAC__byte *entry, unsigned length) {
        static FLAC__bool (*func)(const FLAC__byte*, unsigned) = NULL;
        if(func == NULL) {
            const char* err = dlerror();
            func = (FLAC__bool (*)(const FLAC__byte*, unsigned)) dlsym(flac_bindings::libFlacHandle, "FLAC__format_vorbiscomment_entry_is_legal");
            if((err = dlerror())) printf("%s\n", err);
        }
        if(func != NULL) return func(entry, length);
        return false;
    }

    FLAC__bool FLAC__format_seektable_is_legal(const FLAC__StreamMetadata_SeekTable *seek_table) {
        static FLAC__bool (*func)(const FLAC__StreamMetadata_SeekTable*) = NULL;
        if(func == NULL) {
            const char* err = dlerror();
            func = (FLAC__bool (*)(const FLAC__StreamMetadata_SeekTable*)) dlsym(flac_bindings::libFlacHandle, "FLAC__format_seektable_is_legal");
            if((err = dlerror())) printf("%s\n", err);
        }
        if(func != NULL) return func(seek_table);
        return false;
    }

    unsigned FLAC__format_seektable_sort(FLAC__StreamMetadata_SeekTable *seek_table) {
        static FLAC__bool (*func)(FLAC__StreamMetadata_SeekTable*) = NULL;
        if(func == NULL) {
            const char* err = dlerror();
            func = (FLAC__bool (*)(FLAC__StreamMetadata_SeekTable*)) dlsym(flac_bindings::libFlacHandle, "FLAC__format_seektable_sort");
            if((err = dlerror())) printf("%s\n", err);
        }
        if(func != NULL) return func(seek_table);
        return false;
    }

    FLAC__bool FLAC__format_cuesheet_is_legal(const FLAC__StreamMetadata_CueSheet *cue_sheet, FLAC__bool check_cd_da_subset, const char **violation) {
        static FLAC__bool (*func)(const FLAC__StreamMetadata_CueSheet*,FLAC__bool, const char**) = NULL;
        if(func == NULL) {
            const char* err = dlerror();
            func = (FLAC__bool (*)(const FLAC__StreamMetadata_CueSheet*,FLAC__bool, const char**)) dlsym(flac_bindings::libFlacHandle, "FLAC__format_cuesheet_is_legal");
            if((err = dlerror())) printf("%s\n", err);
        }
        if(func != NULL) return func(cue_sheet, check_cd_da_subset, violation);
        return false;
    }

    FLAC__bool FLAC__format_picture_is_legal(const FLAC__StreamMetadata_Picture *picture, const char **violation) {
        static FLAC__bool (*func)(const FLAC__StreamMetadata_Picture*, const char**) = NULL;
        if(func == NULL) {
            const char* err = dlerror();
            func = (FLAC__bool (*)(const FLAC__StreamMetadata_Picture*, const char**)) dlsym(flac_bindings::libFlacHandle, "FLAC__format_picture_is_legal");
            if((err = dlerror())) printf("%s\n", err);
        }
        if(func != NULL) return func(picture, violation);
        return false;
    }

}


using namespace v8;
using namespace node;
#include "pointer.hpp"

namespace flac_bindings {

    static void no_free(char* a,void* b) { }

    template<typename T> Local<Object> structToJs(const T* i);
    template<typename T> void jsToStruct(const Local<Object> &obj, T* i);

    template<>
    Local<Object> structToJs(const FLAC__StreamMetadata_StreamInfo* i) {
        Nan::EscapableHandleScope scope;
        Local<Object> obj = Nan::New<Object>();
        Nan::Set(obj, Nan::New("min_blocksize").ToLocalChecked(), Nan::New(i->min_blocksize));
        Nan::Set(obj, Nan::New("max_blocksize").ToLocalChecked(), Nan::New(i->max_blocksize));
        Nan::Set(obj, Nan::New("min_framesize").ToLocalChecked(), Nan::New(i->min_framesize));
        Nan::Set(obj, Nan::New("max_framesize").ToLocalChecked(), Nan::New(i->max_framesize));
        Nan::Set(obj, Nan::New("channels").ToLocalChecked(), Nan::New(i->channels));
        Nan::Set(obj, Nan::New("bits_per_sample").ToLocalChecked(), Nan::New(i->bits_per_sample));
        Nan::Set(obj, Nan::New("sample_rate").ToLocalChecked(), Nan::New(i->sample_rate));
        Nan::Set(obj, Nan::New("total_samples").ToLocalChecked(), Nan::New<Number>(i->total_samples));

        Local<Array> md5sum = Nan::New<Array>(16);
        for(uint32_t o = 0; o < 16; o++) {
            Nan::Set(md5sum, o, Nan::New(i->md5sum[o]));
        }
        Nan::Set(obj, Nan::New("md5sum").ToLocalChecked(), md5sum);
        return scope.Escape(obj);
    }

    template<>
    void jsToStruct(const Local<Object> &obj, FLAC__StreamMetadata_StreamInfo* i) {
        i->min_blocksize = Nan::To<uint32_t>(Nan::Get(obj, Nan::New("min_blocksize").ToLocalChecked()).ToLocalChecked()).FromJust();
        i->max_blocksize = Nan::To<uint32_t>(Nan::Get(obj, Nan::New("max_blocksize").ToLocalChecked()).ToLocalChecked()).FromJust();
        i->min_framesize = Nan::To<uint32_t>(Nan::Get(obj, Nan::New("min_framesize").ToLocalChecked()).ToLocalChecked()).FromJust();
        i->max_framesize = Nan::To<uint32_t>(Nan::Get(obj, Nan::New("max_framesize").ToLocalChecked()).ToLocalChecked()).FromJust();
        i->channels = Nan::To<uint32_t>(Nan::Get(obj, Nan::New("channels").ToLocalChecked()).ToLocalChecked()).FromJust();
        i->bits_per_sample = Nan::To<uint32_t>(Nan::Get(obj, Nan::New("bits_per_sample").ToLocalChecked()).ToLocalChecked()).FromJust();
        i->sample_rate = Nan::To<uint32_t>(Nan::Get(obj, Nan::New("sample_rate").ToLocalChecked()).ToLocalChecked()).FromJust();
        i->total_samples = Nan::To<uint32_t>(Nan::Get(obj, Nan::New("total_samples").ToLocalChecked()).ToLocalChecked()).FromJust();
        Local<Array> md5sum = Nan::Get(obj, Nan::New("md5sum").ToLocalChecked()).ToLocalChecked().As<Array>();
        for(uint32_t o = 0; o < 16; o++) {
            i->md5sum[o] = Nan::To<uint32_t>(Nan::Get(md5sum, o).ToLocalChecked()).FromJust();
        }
    }

    template<>
    Local<Object> structToJs(const FLAC__StreamMetadata_Padding* i) {
        Nan::EscapableHandleScope scope;
        return scope.Escape(Nan::New<Object>());
    }

    template<>
    void jsToStruct(const Local<Object> &obj, FLAC__StreamMetadata_Padding* i) {
        i->dummy = 0;
    }

    template<>
    Local<Object> structToJs(const FLAC__StreamMetadata_Unknown* i) {
        Nan::EscapableHandleScope scope;
        return scope.Escape(WrapPointer(i->data).ToLocalChecked());
    }

    template<>
    void jsToStruct(const Local<Object> &obj, FLAC__StreamMetadata_Unknown* i) {
        i->data = UnwrapPointer<FLAC__byte>(obj);
    }

    template<>
    Local<Object> structToJs(const FLAC__StreamMetadata_SeekTable* i) {
        Nan::EscapableHandleScope scope;
        Local<Array> arr = Nan::New<Array>();
        for(uint32_t o = 0; o < i->num_points; i++) {
            Local<Object> obj = Nan::New<Object>();
            Nan::Set(obj, Nan::New("sample_number").ToLocalChecked(), Nan::New<Number>(i->points[o].sample_number));
            Nan::Set(obj, Nan::New("stream_offset").ToLocalChecked(), Nan::New<Number>(i->points[o].stream_offset));
            Nan::Set(obj, Nan::New("frame_samples").ToLocalChecked(), Nan::New<Number>(i->points[o].frame_samples));
            Nan::Set(arr, o, obj);
        }
        return scope.Escape(arr);
    }

    template<>
    void jsToStruct(const Local<Object> &obj, FLAC__StreamMetadata_SeekTable* i) {
        Local<Array> arr = Local<Array>::Cast(obj);
        uint32_t length = arr->Length();
        i->points = new FLAC__StreamMetadata_SeekPoint[length];
        for(uint32_t o = 0; o < length; o++) {
            Local<Object> obj = Nan::Get(arr, o).ToLocalChecked().As<Object>();
            MaybeLocal<Number> sample_number = Nan::To<Number>(Nan::Get(obj, Nan::New("sample_number").ToLocalChecked()).ToLocalChecked());
            MaybeLocal<Number> stream_offset = Nan::To<Number>(Nan::Get(obj, Nan::New("stream_offset").ToLocalChecked()).ToLocalChecked());
            MaybeLocal<Number> frame_samples = Nan::To<Number>(Nan::Get(obj, Nan::New("frame_samples").ToLocalChecked()).ToLocalChecked());
            i->points[o].sample_number = Nan::To<uint32_t>(sample_number.ToLocalChecked()).FromJust();
            i->points[o].stream_offset = Nan::To<uint32_t>(stream_offset.ToLocalChecked()).FromJust();
            i->points[o].frame_samples = Nan::To<uint32_t>(frame_samples.ToLocalChecked()).FromJust();
        }
    }

    template<>
    Local<Object> structToJs(const FLAC__StreamMetadata_CueSheet* i) {
        Nan::EscapableHandleScope scope;
        Local<Object> obj = Nan::New<Object>();
        Local<Array> tracks = Nan::New<Array>();

        Nan::Set(obj, Nan::New("media_catalog_number").ToLocalChecked(), Nan::New(i->media_catalog_number).ToLocalChecked());
        Nan::Set(obj, Nan::New("lead_in").ToLocalChecked(), Nan::New<Number>(i->lead_in));
        Nan::Set(obj, Nan::New("is_cd").ToLocalChecked(), Nan::New<Boolean>(i->is_cd));

        for(uint32_t o = 0; o < i->num_tracks; o++) {
            Local<Object> track = Nan::New<Object>();
            Local<Array> indices = Nan::New<Array>();
            char isrc[14];
            memcpy(isrc, i->tracks[o].isrc, 13);
            isrc[13] = '\0';

            Nan::Set(track, Nan::New("offset").ToLocalChecked(), Nan::New<Number>(i->tracks[o].offset));
            Nan::Set(obj, Nan::New("number").ToLocalChecked(), Nan::New(i->tracks[o].number));
            Nan::Set(obj, Nan::New("type").ToLocalChecked(), Nan::New(i->tracks[o].type));
            Nan::Set(obj, Nan::New("pre_emphasis").ToLocalChecked(), Nan::New<Boolean>(i->tracks[o].pre_emphasis));
            Nan::Set(obj, Nan::New("isrc").ToLocalChecked(), Nan::New(isrc).ToLocalChecked());

            for(uint32_t u = 0; u < i->tracks[o].num_indices; u++) {
                Local<Object> indice = Nan::New<Object>();
                Nan::Set(indice, Nan::New("offset").ToLocalChecked(), Nan::New<Number>(i->tracks[o].indices[u].offset));
                Nan::Set(indice, Nan::New("number").ToLocalChecked(), Nan::New(i->tracks[o].indices[u].number));
                Nan::Set(indices, u, indice);
            }

            Nan::Set(track, Nan::New("indices").ToLocalChecked(), indices);
            Nan::Set(tracks, o, track);
        }

        Nan::Set(obj, Nan::New("tracks").ToLocalChecked(), tracks);
        return scope.Escape(obj);
    }

    template<>
    void jsToStruct(const Local<Object> &obj, FLAC__StreamMetadata_CueSheet* i) {
        memset(i->media_catalog_number, 0, 129);
        Local<String> mediaCatalogNumber = Nan::Get(obj, Nan::New("media_catalog_number").ToLocalChecked()).ToLocalChecked().As<String>();
        Local<Number> leadIn = Nan::Get(obj, Nan::New("lead_in").ToLocalChecked()).ToLocalChecked().As<Number>();
        Local<Boolean> isCd = Nan::Get(obj, Nan::New("is_cd").ToLocalChecked()).ToLocalChecked().As<Boolean>();
        Local<Array> tracks = Nan::Get(obj, Nan::New("tracks").ToLocalChecked()).ToLocalChecked().As<Array>();

        mediaCatalogNumber->WriteUtf8(i->media_catalog_number);
        i->lead_in = Nan::To<uint32_t>(leadIn).FromJust();
        i->is_cd = Nan::To<FLAC__bool>(isCd).FromJust();
        i->num_tracks = tracks->Length();

        i->tracks = new FLAC__StreamMetadata_CueSheet_Track[i->num_tracks];
        for(unsigned o = 0; o < i->num_tracks; o++) {
            Local<Object> item = Nan::Get(tracks, o).ToLocalChecked().As<Object>();
            Local<Number> offset = Nan::Get(item, Nan::New("offset").ToLocalChecked()).ToLocalChecked().As<Number>();
            Local<Number> number = Nan::Get(item, Nan::New("number").ToLocalChecked()).ToLocalChecked().As<Number>();
            Local<String> isrc = Nan::Get(item, Nan::New("isrc").ToLocalChecked()).ToLocalChecked().As<String>();
            Local<Number> type = Nan::Get(item, Nan::New("type").ToLocalChecked()).ToLocalChecked().As<Number>();
            Local<Boolean> pre_emphasis = Nan::Get(item, Nan::New("pre_emphasis").ToLocalChecked()).ToLocalChecked().As<Boolean>();
            Local<Array> indices = Nan::Get(item, Nan::New("indices").ToLocalChecked()).ToLocalChecked().As<Array>();

            isrc->WriteUtf8(i->tracks[o].isrc);
            i->tracks[o].offset = Nan::To<uint32_t>(offset).FromJust();
            i->tracks[o].number = Nan::To<uint32_t>(number).FromJust();
            i->tracks[o].type = Nan::To<uint32_t>(type).FromJust();
            i->tracks[o].pre_emphasis = Nan::To<uint32_t>(pre_emphasis).FromJust();
            i->tracks[o].num_indices = indices->Length();

            i->tracks[o].indices = new FLAC__StreamMetadata_CueSheet_Index[i->tracks[o].num_indices];
            for(unsigned u = 0; u < i->tracks[o].num_indices; u++) {
                Local<Object> index = Nan::Get(indices, u).ToLocalChecked().As<Object>();
                Local<Number> offset = Nan::Get(index, Nan::New("offset").ToLocalChecked()).ToLocalChecked().As<Number>();
                Local<Number> number = Nan::Get(index, Nan::New("number").ToLocalChecked()).ToLocalChecked().As<Number>();

                i->tracks[o].indices[u].offset = Nan::To<uint32_t>(offset).FromJust();
                i->tracks[o].indices[u].number = Nan::To<uint32_t>(number).FromJust();
            }
        }
    }

    template<>
    Local<Object> structToJs(const FLAC__StreamMetadata_Picture* i) {
        Nan::EscapableHandleScope scope;
        Local<Object> obj = Nan::New<Object>();

        Nan::Set(obj, Nan::New("type").ToLocalChecked(), Nan::New<Number>(i->type));
        Nan::Set(obj, Nan::New("mime_type").ToLocalChecked(), Nan::New(i->mime_type).ToLocalChecked());
        Nan::Set(obj, Nan::New("description").ToLocalChecked(), Nan::New((char*) i->description).ToLocalChecked());
        Nan::Set(obj, Nan::New("width").ToLocalChecked(), Nan::New<Number>(i->width));
        Nan::Set(obj, Nan::New("height").ToLocalChecked(), Nan::New<Number>(i->height));
        Nan::Set(obj, Nan::New("depth").ToLocalChecked(), Nan::New<Number>(i->depth));
        Nan::Set(obj, Nan::New("colors").ToLocalChecked(), Nan::New<Number>(i->colors));
        MaybeLocal<Object> data = Nan::NewBuffer((char*) i->data, i->data_length, no_free, nullptr);
        Nan::Set(obj, Nan::New("data").ToLocalChecked(), data.ToLocalChecked());

        return scope.Escape(obj);
    }

    template<>
    void jsToStruct(const Local<Object> &obj, FLAC__StreamMetadata_Picture* i) {
        Local<Number> type = Nan::Get(obj, Nan::New("type").ToLocalChecked()).ToLocalChecked().As<Number>();
        Local<String> mime_type = Nan::Get(obj, Nan::New("mime_type").ToLocalChecked()).ToLocalChecked().As<String>();
        Local<String> description = Nan::Get(obj, Nan::New("description").ToLocalChecked()).ToLocalChecked().As<String>();
        Local<Number> width = Nan::Get(obj, Nan::New("width").ToLocalChecked()).ToLocalChecked().As<Number>();
        Local<Number> height = Nan::Get(obj, Nan::New("height").ToLocalChecked()).ToLocalChecked().As<Number>();
        Local<Number> depth = Nan::Get(obj, Nan::New("depth").ToLocalChecked()).ToLocalChecked().As<Number>();
        Local<Number> colors = Nan::Get(obj, Nan::New("colors").ToLocalChecked()).ToLocalChecked().As<Number>();
        Local<Object> data = Nan::Get(obj, Nan::New("data").ToLocalChecked()).ToLocalChecked().As<Object>();

        i->type = (FLAC__StreamMetadata_Picture_Type) Nan::To<int>(type).FromJust();
        i->width = Nan::To<uint32_t>(width).FromJust();
        i->height = Nan::To<uint32_t>(height).FromJust();
        i->colors = Nan::To<uint32_t>(colors).FromJust();
        i->depth = Nan::To<uint32_t>(depth).FromJust();
        i->data = UnwrapPointer<FLAC__byte>(data);
        i->data_length = Buffer::Length(data);

        i->mime_type = new char[mime_type->Utf8Length() + 1];
        i->description = new FLAC__byte[description->Utf8Length() + 1];
        mime_type->WriteUtf8(i->mime_type);
        description->WriteUtf8((char*) i->description);
        i->mime_type[mime_type->Utf8Length()] = i->description[description->Utf8Length()] = '\0';
    }

    template<>
    Local<Object> structToJs(const FLAC__StreamMetadata* i) {
        Nan::EscapableHandleScope scope;
        Local<Object> obj;
        switch(i->type) {
            case FLAC__METADATA_TYPE_STREAMINFO: obj = structToJs(&i->data.stream_info); break;
            case FLAC__METADATA_TYPE_PADDING: obj = structToJs(&i->data.padding); break;
            case FLAC__METADATA_TYPE_APPLICATION: obj = structToJs(&i->data.application); break;
            case FLAC__METADATA_TYPE_SEEKTABLE: obj = structToJs(&i->data.seek_table); break;
            case FLAC__METADATA_TYPE_VORBIS_COMMENT: obj = structToJs(&i->data.vorbis_comment); break;
            case FLAC__METADATA_TYPE_CUESHEET: obj = structToJs(&i->data.cue_sheet); break;
            case FLAC__METADATA_TYPE_PICTURE: obj = structToJs(&i->data.picture); break;
            case FLAC__METADATA_TYPE_UNDEFINED: obj = structToJs(&i->data.unknown); break;
            default: obj = Nan::New<Object>();
        }

        Nan::Set(obj, Nan::New("type").ToLocalChecked(), Nan::New(i->type));
        Nan::Set(obj, Nan::New("isLast").ToLocalChecked(), Nan::New<Boolean>(i->is_last));
        Nan::Set(obj, Nan::New("length").ToLocalChecked(), Nan::New(i->length));
        return scope.Escape(obj);
    }

    template<>
    void jsToStruct(const Local<Object> &obj, FLAC__StreamMetadata* i) {
        FLAC__MetadataType type = (FLAC__MetadataType) Nan::To<uint32_t>(Nan::Get(obj, Nan::New("type").ToLocalChecked()).ToLocalChecked()).FromJust();
        Nan::MaybeLocal<Value> lengthJs = Nan::Get(obj, Nan::New("length").ToLocalChecked());
        Nan::MaybeLocal<Value> isLastJs = Nan::Get(obj, Nan::New("isLast").ToLocalChecked());
        FLAC__bool isLast = false;
        unsigned length = 0;
        if(!isLastJs.IsEmpty()) isLast = Nan::To<FLAC__bool>(isLastJs.ToLocalChecked()).FromJust();
        if(!lengthJs.IsEmpty()) length = Nan::To<unsigned>(lengthJs.ToLocalChecked()).FromJust();

        switch(i->type) {
            case FLAC__METADATA_TYPE_STREAMINFO: jsToStruct(obj, &i->data.stream_info); break;
            case FLAC__METADATA_TYPE_PADDING: jsToStruct(obj, &i->data.padding); break;
            case FLAC__METADATA_TYPE_APPLICATION: jsToStruct(obj, &i->data.application); break;
            case FLAC__METADATA_TYPE_SEEKTABLE: jsToStruct(obj, &i->data.seek_table); break;
            case FLAC__METADATA_TYPE_VORBIS_COMMENT: jsToStruct(obj, &i->data.vorbis_comment); break;
            case FLAC__METADATA_TYPE_CUESHEET: jsToStruct(obj, &i->data.cue_sheet); break;
            case FLAC__METADATA_TYPE_PICTURE: jsToStruct(obj, &i->data.picture); break;
            case FLAC__METADATA_TYPE_UNDEFINED: jsToStruct(obj, &i->data.unknown); break;
            default: break;
        }

        i->type = type;
        i->is_last = isLast;
        i->length = length;
    }

    template<>
    Local<Object> structToJs(const FLAC__Frame* i) {
        Nan::EscapableHandleScope scope;
        Local<Object> obj = Nan::New<Object>();
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
                    MaybeLocal<Object> data = Nan::NewBuffer((char*) i->subframes[o].data.verbatim.data,
                        i->header.blocksize * i->header.bits_per_sample * sizeof(int32_t), no_free, nullptr);
                    Nan::Set(subframe, Nan::New("data").ToLocalChecked(), data.ToLocalChecked());
                    break;
                }
                case FLAC__SUBFRAME_TYPE_FIXED: {
                    MaybeLocal<Object> residual = Nan::NewBuffer((char*) i->subframes[o].data.fixed.residual,
                        (i->header.blocksize - i->subframes[o].data.fixed.order) * sizeof(int32_t), no_free, nullptr);
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
                    MaybeLocal<Object> residual = Nan::NewBuffer((char*) i->subframes[o].data.lpc.residual,
                        (i->header.blocksize - i->subframes[o].data.lpc.order) * sizeof(int32_t), no_free, nullptr);
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
        return scope.Escape(obj);
    }

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
        unsigned length = Nan::To<unsigned>(info[1]).FromJust();//TODO
        FLAC__bool ret = FLAC__format_vorbiscomment_entry_value_is_legal(NULL, length);
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
    }

    NAN_METHOD(node_FLAC__format_vorbiscomment_entry_is_legal) {
        unsigned length = Nan::To<unsigned>(info[1]).FromJust();//TODO
        FLAC__bool ret = FLAC__format_vorbiscomment_entry_is_legal(NULL, length);
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
    }

    NAN_METHOD(node_FLAC__format_seektable_is_legal) {
        FLAC__StreamMetadata_SeekTable table;
        jsToStruct(info[0].As<Array>(), &table);
        FLAC__bool ret = FLAC__format_seektable_is_legal(&table);
        info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        delete[] table.points;
    }

    NAN_METHOD(node_FLAC__format_seektable_sort) {
        FLAC__StreamMetadata_SeekTable table;
        jsToStruct(info[0].As<Array>(), &table);
        FLAC__format_seektable_sort(&table);
        info.GetReturnValue().Set(structToJs(&table));
        delete[] table.points;
    }

    NAN_METHOD(node_FLAC__format_cuesheet_is_legal) {
        FLAC__StreamMetadata_CueSheet cue;
        jsToStruct(info[0].As<Object>(), &cue);
        bool check = Nan::To<int>(info[1].As<Boolean>()).FromJust();
        const char* violation = NULL;
        FLAC__bool ret = FLAC__format_cuesheet_is_legal(&cue, check, &violation);
        if(ret) {
            info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        } else {
            info.GetReturnValue().Set(Nan::New(violation).ToLocalChecked());
        }
    }

    NAN_METHOD(node_FLAC__format_picture_is_legal) {
        FLAC__StreamMetadata_Picture picture;
        jsToStruct(info[0].As<Object>(), &picture);
        const char* violation = NULL;
        FLAC__bool ret = FLAC__format_picture_is_legal(&picture, &violation);
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

    NAN_INDEX_GETTER(EntropyCodingMethodTypeString) {
        if(index < 2) {
            info.GetReturnValue().Set(Nan::New(FLAC__EntropyCodingMethodTypeString[index]).ToLocalChecked());
        } else {
            info.GetReturnValue().SetNull();
        }
    }

    NAN_INDEX_GETTER(SubframeTypeString) {
        if(index < 4) {
            info.GetReturnValue().Set(Nan::New(FLAC__SubframeTypeString[index]).ToLocalChecked());
        } else {
            info.GetReturnValue().SetNull();
        }
    }

    NAN_INDEX_GETTER(ChannelAssignmentString) {
        if(index < 4) {
            info.GetReturnValue().Set(Nan::New(FLAC__ChannelAssignmentString[index]).ToLocalChecked());
        } else {
            info.GetReturnValue().SetNull();
        }
    }

    NAN_INDEX_GETTER(FrameNumberTypeString) {
        if(index < 2) {
            info.GetReturnValue().Set(Nan::New(FLAC__FrameNumberTypeString[index]).ToLocalChecked());
        } else {
            info.GetReturnValue().SetNull();
        }
    }

    NAN_INDEX_GETTER(StreamMetadata_Picture_TypeString) {
        if(index < 22) {
            info.GetReturnValue().Set(Nan::New(FLAC__StreamMetadata_Picture_TypeString[index]).ToLocalChecked());
        } else {
            info.GetReturnValue().SetNull();
        }
    }

    NAN_MODULE_INIT(initFormat) {
        Local<Object> obj = Nan::New<Object>();

        FLAC__VERSION_STRING = *(const char**) dlsym(libFlacHandle, "FLAC__VERSION_STRING");
        if(FLAC__VERSION_STRING == nullptr) printf("%s\n", dlerror());
        FLAC__VENDOR_STRING = *(const char**) dlsym(libFlacHandle, "FLAC__VENDOR_STRING");
        if(FLAC__VENDOR_STRING == nullptr) printf("%s\n", dlerror());

        Nan::Set(obj, Nan::New("FLAC__VERSION_STRING").ToLocalChecked(), Nan::New<String>(FLAC__VERSION_STRING).ToLocalChecked());
        Nan::Set(obj, Nan::New("FLAC__VENDOR_STRING").ToLocalChecked(), Nan::New<String>(FLAC__VENDOR_STRING).ToLocalChecked());
        Nan::SetMethod(obj, "FLAC__format_sample_rate_is_valid", node_FLAC__format_sample_rate_is_valid);
        Nan::SetMethod(obj, "FLAC__format_blocksize_is_subset", node_FLAC__format_blocksize_is_subset);
        Nan::SetMethod(obj, "FLAC__format_sample_rate_is_subset", node_FLAC__format_sample_rate_is_subset);
        Nan::SetMethod(obj, "FLAC__format_vorbiscomment_entry_name_is_legal", node_FLAC__format_vorbiscomment_entry_name_is_legal);
        Nan::SetMethod(obj, "FLAC__format_vorbiscomment_entry_value_is_legal", node_FLAC__format_vorbiscomment_entry_value_is_legal);
        Nan::SetMethod(obj, "FLAC__format_vorbiscomment_entry_is_legal", node_FLAC__format_vorbiscomment_entry_is_legal);
        Nan::SetMethod(obj, "FLAC__format_seektable_is_legal", node_FLAC__format_seektable_is_legal);
        Nan::SetMethod(obj, "FLAC__format_seektable_sort", node_FLAC__format_seektable_sort);
        Nan::SetMethod(obj, "FLAC__format_cuesheet_is_legal", node_FLAC__format_cuesheet_is_legal);
        Nan::SetMethod(obj, "FLAC__format_picture_is_legal", node_FLAC__format_picture_is_legal);

        #define propertyGetter(func) \
        Local<ObjectTemplate> _JOIN(func, Var) = Nan::New<ObjectTemplate>(); \
        Nan::SetNamedPropertyHandler(_JOIN(func, Var), func); \
        Nan::Set(obj, Nan::New(#func).ToLocalChecked(), _JOIN(func, Var)->NewInstance());

        propertyGetter(MetadataType);
        propertyGetter(EntropyCodingMethodType);
        propertyGetter(SubframeType);
        propertyGetter(ChannelAssignment);
        propertyGetter(FrameNumberType);
        propertyGetter(StreamMetadata_Picture_Type);

        #define indexGetter(func) \
        _JOIN(FLAC__, func) = (const char* const*) dlsym(libFlacHandle, "FLAC__" #func); \
        Local<ObjectTemplate> _JOIN(func, _template) = Nan::New<ObjectTemplate>(); \
        Nan::SetIndexedPropertyHandler(_JOIN(func, _template), func); \
        Nan::Set(obj, Nan::New(#func).ToLocalChecked(), _JOIN(func, _template)->NewInstance());

        indexGetter(MetadataTypeString);
        indexGetter(EntropyCodingMethodTypeString);
        indexGetter(SubframeTypeString);
        indexGetter(ChannelAssignmentString);
        indexGetter(FrameNumberTypeString);
        indexGetter(StreamMetadata_Picture_TypeString);

        Nan::Set(target, Nan::New("format").ToLocalChecked(), obj);
    }
}
