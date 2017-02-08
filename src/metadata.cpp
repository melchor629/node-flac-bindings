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
    metadataFunction(FLAC__StreamMetadata*, object_new, FLAC__MetadataType type);
    metadataFunction(FLAC__StreamMetadata*, object_clone, const FLAC__StreamMetadata *object);
    metadataFunction(void, object_delete, FLAC__StreamMetadata *object);
    metadataFunction(FLAC__bool, object_is_equal, const FLAC__StreamMetadata *block1, const FLAC__StreamMetadata *block2);
    metadataFunction(FLAC__bool, object_application_set_data, FLAC__StreamMetadata *object, FLAC__byte *data, unsigned length, FLAC__bool copy);
    metadataFunction(FLAC__bool, object_seektable_resize_points, FLAC__StreamMetadata *object, unsigned new_num_points);
    metadataFunction(void, object_seektable_set_point, FLAC__StreamMetadata *object, unsigned point_num, FLAC__StreamMetadata_SeekPoint point);
    metadataFunction(FLAC__bool, object_seektable_insert_point, FLAC__StreamMetadata *object, unsigned point_num, FLAC__StreamMetadata_SeekPoint point);
    metadataFunction(FLAC__bool, object_seektable_delete_point, FLAC__StreamMetadata *object, unsigned point_num);
    metadataFunction(FLAC__bool, object_seektable_is_legal, const FLAC__StreamMetadata *object);
    metadataFunction(FLAC__bool, object_seektable_template_append_placeholders, FLAC__StreamMetadata *object, unsigned num);
    metadataFunction(FLAC__bool, object_seektable_template_append_point, FLAC__StreamMetadata *object, uint64_t sample_number);
    metadataFunction(FLAC__bool, object_seektable_template_append_points, FLAC__StreamMetadata *object, uint64_t sample_numbers[], unsigned num);
    metadataFunction(FLAC__bool, object_seektable_template_append_spaced_points, FLAC__StreamMetadata *object, unsigned num, uint64_t total_samples);
    metadataFunction(FLAC__bool, object_seektable_template_append_spaced_points_by_samples, FLAC__StreamMetadata *object, unsigned samples, uint64_t total_samples);
    metadataFunction(FLAC__bool, object_seektable_template_sort, FLAC__StreamMetadata *object, FLAC__bool compact);
    metadataFunction(FLAC__bool, object_vorbiscomment_set_vendor_string, FLAC__StreamMetadata *object, FLAC__StreamMetadata_VorbisComment_Entry entry, FLAC__bool copy);
    metadataFunction(FLAC__bool, object_vorbiscomment_resize_comments, FLAC__StreamMetadata *object, unsigned new_num_comments);
    metadataFunction(FLAC__bool, object_vorbiscomment_set_comment, FLAC__StreamMetadata *object, unsigned comment_num, FLAC__StreamMetadata_VorbisComment_Entry entry, FLAC__bool copy);
    metadataFunction(FLAC__bool, object_vorbiscomment_insert_comment, FLAC__StreamMetadata *object, unsigned comment_num, FLAC__StreamMetadata_VorbisComment_Entry entry, FLAC__bool copy);
    metadataFunction(FLAC__bool, object_vorbiscomment_append_comment, FLAC__StreamMetadata *object, FLAC__StreamMetadata_VorbisComment_Entry entry, FLAC__bool copy);
    metadataFunction(FLAC__bool, object_vorbiscomment_replace_comment, FLAC__StreamMetadata *object, FLAC__StreamMetadata_VorbisComment_Entry entry, FLAC__bool all, FLAC__bool copy);
    metadataFunction(FLAC__bool, object_vorbiscomment_delete_comment, FLAC__StreamMetadata *object, unsigned comment_num);
    //metadataFunction(FLAC__bool, object_vorbiscomment_entry_from_name_value_pair, FLAC__StreamMetadata_VorbisComment_Entry *entry, const char *field_name, const char *field_value);
    //metadataFunction(FLAC__bool, object_vorbiscomment_entry_to_name_value_pair, const FLAC__StreamMetadata_VorbisComment_Entry entry, char **field_name, char **field_value);
    //metadataFunction(FLAC__bool, object_vorbiscomment_entry_matches, const FLAC__StreamMetadata_VorbisComment_Entry entry, const char *field_name, unsigned field_name_length);
    metadataFunction(int, object_vorbiscomment_find_entry_from, const FLAC__StreamMetadata *object, unsigned offset, const char *field_name);
    metadataFunction(int, object_vorbiscomment_remove_entry_matching, FLAC__StreamMetadata *object, const char *field_name);
    metadataFunction(int, object_vorbiscomment_remove_entries_matching, FLAC__StreamMetadata *object, const char *field_name);
    metadataFunction(FLAC__StreamMetadata_CueSheet_Track*, object_cuesheet_track_new, void);
    metadataFunction(FLAC__StreamMetadata_CueSheet_Track*, object_cuesheet_track_clone, const FLAC__StreamMetadata_CueSheet_Track *object);
    metadataFunction(void, object_cuesheet_track_delete, FLAC__StreamMetadata_CueSheet_Track *object);
    metadataFunction(FLAC__bool, object_cuesheet_track_resize_indices, FLAC__StreamMetadata *object, unsigned track_num, unsigned new_num_indices);
    metadataFunction(FLAC__bool, object_cuesheet_track_insert_index, FLAC__StreamMetadata *object, unsigned track_num, unsigned index_num, FLAC__StreamMetadata_CueSheet_Index index);
    metadataFunction(FLAC__bool, object_cuesheet_track_insert_blank_index, FLAC__StreamMetadata *object, unsigned track_num, unsigned index_num);
    metadataFunction(FLAC__bool, object_cuesheet_track_delete_index, FLAC__StreamMetadata *object, unsigned track_num, unsigned index_num);
    metadataFunction(FLAC__bool, object_cuesheet_resize_tracks, FLAC__StreamMetadata *object, unsigned new_num_tracks);
    metadataFunction(FLAC__bool, object_cuesheet_set_track, FLAC__StreamMetadata *object, unsigned track_num, FLAC__StreamMetadata_CueSheet_Track *track, FLAC__bool copy);
    metadataFunction(FLAC__bool, object_cuesheet_insert_track, FLAC__StreamMetadata *object, unsigned track_num, FLAC__StreamMetadata_CueSheet_Track *track, FLAC__bool copy);
    metadataFunction(FLAC__bool, object_cuesheet_insert_blank_track, FLAC__StreamMetadata *object, unsigned track_num);
    metadataFunction(FLAC__bool, object_cuesheet_delete_track, FLAC__StreamMetadata *object, unsigned track_num);
    metadataFunction(FLAC__bool, object_cuesheet_is_legal, const FLAC__StreamMetadata *object, FLAC__bool check_cd_da_subset, const char **violation);
    metadataFunction(uint32_t, object_cuesheet_calculate_cddb_id, const FLAC__StreamMetadata *object);
    metadataFunction(FLAC__bool, object_picture_set_mime_type, FLAC__StreamMetadata *object, char *mime_type, FLAC__bool copy);
    metadataFunction(FLAC__bool, object_picture_set_description, FLAC__StreamMetadata *object, FLAC__byte *description, FLAC__bool copy);
    metadataFunction(FLAC__bool, object_picture_set_data, FLAC__StreamMetadata *object, FLAC__byte *data, uint32_t length, FLAC__bool copy);
    metadataFunction(FLAC__bool, object_picture_is_legal, const FLAC__StreamMetadata *object, const char **violation);
}

namespace flac_bindings {

    extern Library* libFlac;

    NAN_METHOD(node_FLAC__metadata_object_new) {
        FLAC__MetadataType type = (FLAC__MetadataType) Nan::To<int>(info[0]).FromJust();
        FLAC__StreamMetadata* m = FLAC__metadata_object_new(type);
        if(m == nullptr) {
            info.GetReturnValue().SetNull();
        } else {
            info.GetReturnValue().Set(tojs(m));
        }
    }

    NAN_METHOD(node_FLAC__metadata_object_clone) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        if(m == nullptr) {
            info.GetReturnValue().SetNull();
        } else {
            FLAC__StreamMetadata* n = FLAC__metadata_object_clone(m);
            if(n != nullptr) {
                info.GetReturnValue().Set(tojs(m));
            } else {
                info.GetReturnValue().SetNull();
            }
        }
    }

    NAN_METHOD(node_FLAC__metadata_object_delete) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        FLAC__metadata_object_delete(m);
    }

    NAN_METHOD(node_FLAC__metadata_object_is_equal) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        FLAC__StreamMetadata* n = fromjs<FLAC__StreamMetadata>(info[1]);
        FLAC__bool r = FLAC__metadata_object_is_equal(m, n);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_application_set_data) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        FLAC__byte* data = UnwrapPointer<FLAC__byte>(info[1]);
        uint32_t length = uint32_t(Buffer::Length(info[1]));
        FLAC__bool r = FLAC__metadata_object_application_set_data(m, data, length, true);
        //To avoid strange problems with pointer owners between node::Buffer and
        //FLAC API, will FLAC ALWAYS copy the contents of the pointer
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_seektable_resize_points) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        unsigned n = Nan::To<unsigned>(info[1]).FromJust();
        FLAC__bool r = FLAC__metadata_object_seektable_resize_points(m, n);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_seektable_set_point) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        unsigned n = Nan::To<unsigned>(info[1]).FromJust();
        FLAC__StreamMetadata_SeekPoint point;
        jsToStruct(info[2].As<Object>(), &point);
        FLAC__metadata_object_seektable_set_point(m, n, point);
    }

    NAN_METHOD(node_FLAC__metadata_object_seektable_insert_point) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        unsigned n = Nan::To<unsigned>(info[1]).FromJust();
        FLAC__StreamMetadata_SeekPoint point;
        jsToStruct(info[2].As<Object>(), &point);
        FLAC__bool r = FLAC__metadata_object_seektable_insert_point(m, n, point);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_seektable_delete_point) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        unsigned n = Nan::To<unsigned>(info[1]).FromJust();
        FLAC__bool r = FLAC__metadata_object_seektable_delete_point(m, n);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_seektable_is_legal) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        FLAC__bool r = FLAC__metadata_object_seektable_is_legal(m);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_seektable_template_append_placeholders) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        unsigned n = Nan::To<unsigned>(info[1]).FromJust();
        FLAC__bool r = FLAC__metadata_object_seektable_template_append_placeholders(m, n);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_seektable_template_append_point) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        unsigned n = Nan::To<unsigned>(info[1]).FromJust();
        FLAC__bool r = FLAC__metadata_object_seektable_template_append_placeholders(m, n);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_seektable_template_append_points) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        Local<Array> points = info[1].As<Array>();
        uint64_t* n = new uint64_t[points->Length()];
        for(uint32_t i = 0; i < points->Length(); i++) {
            n[i] = Nan::To<int64_t>(Nan::Get(points, i).ToLocalChecked()).FromJust();
        }
        FLAC__bool r = FLAC__metadata_object_seektable_template_append_points(m, n, points->Length());
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
        delete[] n;
    }

    NAN_METHOD(node_FLAC__metadata_object_seektable_template_append_spaced_points) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        unsigned n = Nan::To<unsigned>(info[1]).FromJust();
        int64_t o = Nan::To<int64_t>(info[2]).FromJust();
        FLAC__bool r = FLAC__metadata_object_seektable_template_append_spaced_points(m, n, o);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_seektable_template_append_spaced_points_by_samples) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        unsigned n = Nan::To<unsigned>(info[1]).FromJust();
        int64_t o = Nan::To<int64_t>(info[2]).FromJust();
        FLAC__bool r = FLAC__metadata_object_seektable_template_append_spaced_points_by_samples(m, n, o);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_seektable_template_sort) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        FLAC__bool n = Nan::To<unsigned>(info[1]).FromJust();
        FLAC__bool r = FLAC__metadata_object_seektable_template_sort(m, n);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_vorbiscomment_set_vendor_string) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        FLAC__StreamMetadata_VorbisComment_Entry n;
        Nan::Utf8String vs(info[1]);
        n.entry = (FLAC__byte*) *vs;
        n.length = vs.length();
        FLAC__bool r = FLAC__metadata_object_vorbiscomment_set_vendor_string(m, n, true);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_vorbiscomment_resize_comments) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        unsigned n = Nan::To<uint32_t>(info[1]).FromJust();
        FLAC__bool r = FLAC__metadata_object_vorbiscomment_resize_comments(m, n);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_vorbiscomment_set_comment) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        unsigned n = Nan::To<unsigned>(info[1]).FromJust();
        FLAC__StreamMetadata_VorbisComment_Entry o;
        Nan::Utf8String vs(info[1]);
        o.entry = (FLAC__byte*) *vs;
        o.length = vs.length();
        FLAC__bool r = FLAC__metadata_object_vorbiscomment_set_comment(m, n, o, true);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_vorbiscomment_insert_comment) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        unsigned n = Nan::To<unsigned>(info[1]).FromJust();
        FLAC__StreamMetadata_VorbisComment_Entry o;
        Nan::Utf8String vs(info[1]);
        o.entry = (FLAC__byte*) *vs;
        o.length = vs.length();
        FLAC__bool r = FLAC__metadata_object_vorbiscomment_insert_comment(m, n, o, true);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_vorbiscomment_append_comment) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        FLAC__StreamMetadata_VorbisComment_Entry n;
        Nan::Utf8String vs(info[1]);
        n.entry = (FLAC__byte*) *vs;
        n.length = vs.length();
        FLAC__bool r = FLAC__metadata_object_vorbiscomment_append_comment(m, n, true);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_vorbiscomment_replace_comment) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        FLAC__StreamMetadata_VorbisComment_Entry n;
        Nan::Utf8String vs(info[1]);
        n.entry = (FLAC__byte*) *vs;
        n.length = vs.length();
        FLAC__bool o = Nan::To<FLAC__bool>(info[2]).FromMaybe(0);
        FLAC__bool r = FLAC__metadata_object_vorbiscomment_replace_comment(m, n, o, true);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_vorbiscomment_delete_comment) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        FLAC__bool n = Nan::To<FLAC__bool>(info[1]).FromJust();
        FLAC__bool r = FLAC__metadata_object_vorbiscomment_delete_comment(m, n);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_vorbiscomment_find_entry_from) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        unsigned n = Nan::To<unsigned>(info[1]).FromJust();
        Nan::Utf8String o(info[2]);
        FLAC__bool r = FLAC__metadata_object_vorbiscomment_find_entry_from(m, n, *o);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_vorbiscomment_remove_entry_matching) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        Nan::Utf8String n(info[1]);
        FLAC__bool r = FLAC__metadata_object_vorbiscomment_remove_entry_matching(m, *n);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_vorbiscomment_remove_entries_matching) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        Nan::Utf8String n(info[1]);
        FLAC__bool r = FLAC__metadata_object_vorbiscomment_remove_entries_matching(m, *n);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_cuesheet_track_new) {
        FLAC__StreamMetadata_CueSheet_Track* ptr = FLAC__metadata_object_cuesheet_track_new();
        Local<Object> buff = structToJs(ptr);
        Nan::Set(buff, Nan::New("_ptr").ToLocalChecked(), WrapPointer(ptr, sizeof(FLAC__StreamMetadata_CueSheet_Track)).ToLocalChecked());
        info.GetReturnValue().Set(buff);
    }

    NAN_METHOD(node_FLAC__metadata_object_cuesheet_track_clone) {
        FLAC__StreamMetadata_CueSheet_Track* m = fromjs<FLAC__StreamMetadata_CueSheet_Track>(info[0]);
        FLAC__StreamMetadata_CueSheet_Track* ptr = FLAC__metadata_object_cuesheet_track_clone(m);
        Local<Object> buff = structToJs(ptr);
        Nan::Set(buff, Nan::New("_ptr").ToLocalChecked(), WrapPointer(ptr, sizeof(FLAC__StreamMetadata_CueSheet_Track)).ToLocalChecked());
        info.GetReturnValue().Set(buff);
    }

    NAN_METHOD(node_FLAC__metadata_object_cuesheet_track_delete) {
        FLAC__StreamMetadata_CueSheet_Track* m = fromjs<FLAC__StreamMetadata_CueSheet_Track>(info[0]);
        FLAC__metadata_object_cuesheet_track_delete(m);
    }

    NAN_METHOD(node_FLAC__metadata_object_cuesheet_track_resize_indices) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        unsigned n = Nan::To<unsigned>(info[1]).FromJust();
        unsigned o = Nan::To<unsigned>(info[2]).FromJust();
        FLAC__bool r = FLAC__metadata_object_cuesheet_track_resize_indices(m, n, o);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_cuesheet_track_insert_index) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        unsigned n = Nan::To<unsigned>(info[1]).FromJust();
        unsigned o = Nan::To<unsigned>(info[2]).FromJust();
        uint64_t p1 = Nan::To<int64_t>(info[3]).FromJust();
        FLAC__byte p2 = Nan::To<int>(info[4]).FromJust();
        FLAC__StreamMetadata_CueSheet_Index p = { p1, p2 };
        FLAC__bool r = FLAC__metadata_object_cuesheet_track_insert_index(m, n, o, p);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_cuesheet_track_insert_blank_index) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        unsigned n = Nan::To<unsigned>(info[1]).FromJust();
        unsigned o = Nan::To<unsigned>(info[2]).FromJust();
        FLAC__bool r = FLAC__metadata_object_cuesheet_track_insert_blank_index(m, n, o);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_cuesheet_track_delete_index) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        unsigned n = Nan::To<unsigned>(info[1]).FromJust();
        unsigned o = Nan::To<unsigned>(info[2]).FromJust();
        FLAC__bool r = FLAC__metadata_object_cuesheet_track_delete_index(m, n, o);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_cuesheet_resize_tracks) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        unsigned n = Nan::To<unsigned>(info[1]).FromJust();
        FLAC__bool r = FLAC__metadata_object_cuesheet_resize_tracks(m, n);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_cuesheet_set_track) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        unsigned n = Nan::To<unsigned>(info[1]).FromJust();
        FLAC__StreamMetadata_CueSheet_Track* o = fromjs<FLAC__StreamMetadata_CueSheet_Track>(info[2]);
        FLAC__bool r = FLAC__metadata_object_cuesheet_set_track(m, n, o, false);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_cuesheet_insert_track) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        unsigned n = Nan::To<unsigned>(info[1]).FromJust();
        FLAC__StreamMetadata_CueSheet_Track* o = fromjs<FLAC__StreamMetadata_CueSheet_Track>(info[2]);
        FLAC__bool r = FLAC__metadata_object_cuesheet_insert_track(m, n, o, false);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_cuesheet_insert_blank_track) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        unsigned n = Nan::To<unsigned>(info[1]).FromJust();
        FLAC__bool r = FLAC__metadata_object_cuesheet_insert_blank_track(m, n);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_cuesheet_delete_track) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        unsigned n = Nan::To<unsigned>(info[1]).FromJust();
        FLAC__bool r = FLAC__metadata_object_cuesheet_delete_track(m, n);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_cuesheet_is_legal) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        FLAC__bool n = Nan::To<unsigned>(info[1]).FromJust();
        const char* o;
        FLAC__bool r = FLAC__metadata_object_cuesheet_is_legal(m, n, &o);
        if(r) {
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
        } else {
            info.GetReturnValue().Set(Nan::New(o).ToLocalChecked());
        }
    }

    NAN_METHOD(node_FLAC__metadata_object_cuesheet_calculate_cddb_id) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        uint32_t r = FLAC__metadata_object_cuesheet_calculate_cddb_id(m);
        info.GetReturnValue().Set(Nan::New<Number>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_picture_set_mime_type) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        Nan::Utf8String n(info[1]);
        FLAC__bool r = FLAC__metadata_object_picture_set_mime_type(m, *n, true);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_picture_set_description) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        Nan::Utf8String n(info[1]);
        FLAC__bool r = FLAC__metadata_object_picture_set_description(m, (FLAC__byte*) *n, true);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_picture_set_data) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        FLAC__byte* n = UnwrapPointer<FLAC__byte>(info[1]);
        uint32_t o = uint32_t(Buffer::Length(info[1]));
        FLAC__bool r = FLAC__metadata_object_picture_set_data(m, n, o, true);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(node_FLAC__metadata_object_picture_is_legal) {
        FLAC__StreamMetadata* m = fromjs<FLAC__StreamMetadata>(info[0]);
        const char* n;
        FLAC__bool r = FLAC__metadata_object_picture_is_legal(m, &n);
        if(!r) {
            info.GetReturnValue().Set(Nan::New(n).ToLocalChecked());
        } else {
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
        }
    }

    NAN_METHOD(convertToJsObject) {
        FLAC__StreamMetadata* m = UnwrapPointer<FLAC__StreamMetadata>(info[0]);
        info.GetReturnValue().Set(structToJs(m));
    }

    NAN_MODULE_INIT(initMetadataObjectMethods) {
        Local<Object> obj = Nan::New<Object>();
        #define setMethod(fn) \
        Nan::SetMethod(obj, #fn, _JOIN(node_FLAC__metadata_object_, fn)); \
        _JOIN(FLAC__metadata_object_, fn) = libFlac->getSymbolAddress<_JOIN2(FLAC__metadata_object_, fn, _t)>("FLAC__metadata_object_" #fn); \
        if(_JOIN(FLAC__metadata_object_, fn) == nullptr) printf("%s\n", libFlac->getLastError().c_str());

        setMethod(new);
        setMethod(clone);
        setMethod(delete);
        setMethod(is_equal);
        setMethod(application_set_data);
        setMethod(seektable_resize_points);
        setMethod(seektable_set_point);
        setMethod(seektable_insert_point);
        setMethod(seektable_delete_point);
        setMethod(seektable_is_legal);
        setMethod(seektable_template_append_placeholders);
        setMethod(seektable_template_append_point);
        setMethod(seektable_template_append_points);
        setMethod(seektable_template_append_spaced_points);
        setMethod(seektable_template_append_spaced_points_by_samples);
        setMethod(seektable_template_sort);
        setMethod(vorbiscomment_set_vendor_string);
        setMethod(vorbiscomment_resize_comments);
        setMethod(vorbiscomment_set_comment);
        setMethod(vorbiscomment_insert_comment);
        setMethod(vorbiscomment_append_comment);
        setMethod(vorbiscomment_replace_comment);
        setMethod(vorbiscomment_delete_comment);
        setMethod(vorbiscomment_find_entry_from);
        setMethod(vorbiscomment_remove_entry_matching);
        setMethod(vorbiscomment_remove_entries_matching);
        setMethod(cuesheet_track_new);
        setMethod(cuesheet_track_clone);
        setMethod(cuesheet_track_delete);
        setMethod(cuesheet_track_resize_indices);
        setMethod(cuesheet_track_insert_index);
        setMethod(cuesheet_track_insert_blank_index);
        setMethod(cuesheet_track_delete_index);
        setMethod(cuesheet_resize_tracks);
        setMethod(cuesheet_set_track);
        setMethod(cuesheet_insert_track);
        setMethod(cuesheet_insert_blank_track);
        setMethod(cuesheet_delete_track);
        setMethod(cuesheet_is_legal);
        setMethod(cuesheet_calculate_cddb_id);
        setMethod(picture_set_mime_type);
        setMethod(picture_set_description);
        setMethod(picture_set_data);
        setMethod(picture_is_legal);
        Nan::SetMethod(obj, "convertToJsObject", convertToJsObject);

        Nan::Set(target, Nan::New("metadata").ToLocalChecked(), obj);
    }
}
