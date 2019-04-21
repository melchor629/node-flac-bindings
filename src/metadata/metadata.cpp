#include <nan.h>
#include "../utils/dl.hpp"

using namespace v8;
using namespace node;
#include "../utils/pointer.hpp"
#include "../format/format.h"
#include "../mappings/mappings.hpp"

#define _JOIN(a, b) a##b
#define _JOIN2(a,b,c) a##b##c

#define metadataFunction(ret, name, ...) \
typedef ret (*_JOIN2(FLAC__metadata_, name, _t))(__VA_ARGS__); \
_JOIN2(FLAC__metadata_, name, _t) _JOIN(FLAC__metadata_, name);

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

    NAN_MODULE_INIT(initMetadataObjectMethods) {
        Local<Object> obj = Nan::New<Object>();
        #define loadFunction(fn) \
        _JOIN(FLAC__metadata_object_, fn) = libFlac->getSymbolAddress<_JOIN2(FLAC__metadata_object_, fn, _t)>("FLAC__metadata_object_" #fn); \
        if(_JOIN(FLAC__metadata_object_, fn) == nullptr) printf("%s\n", libFlac->getLastError().c_str());

        #define setMethod(fn) \
        Nan::SetMethod(obj, #fn, _JOIN(node_FLAC__metadata_object_, fn)); \
        loadFunction(fn)

        loadFunction(new);
        loadFunction(clone);
        loadFunction(delete);
        loadFunction(is_equal);
        loadFunction(application_set_data);
        loadFunction(seektable_resize_points);
        loadFunction(seektable_set_point);
        loadFunction(seektable_insert_point);
        loadFunction(seektable_delete_point);
        loadFunction(seektable_is_legal);
        loadFunction(seektable_template_append_placeholders);
        loadFunction(seektable_template_append_point);
        loadFunction(seektable_template_append_points);
        loadFunction(seektable_template_append_spaced_points);
        loadFunction(seektable_template_append_spaced_points_by_samples);
        loadFunction(seektable_template_sort);
        loadFunction(vorbiscomment_set_vendor_string);
        loadFunction(vorbiscomment_resize_comments);
        loadFunction(vorbiscomment_set_comment);
        loadFunction(vorbiscomment_insert_comment);
        loadFunction(vorbiscomment_append_comment);
        loadFunction(vorbiscomment_replace_comment);
        loadFunction(vorbiscomment_delete_comment);
        loadFunction(vorbiscomment_find_entry_from);
        loadFunction(vorbiscomment_remove_entry_matching);
        loadFunction(vorbiscomment_remove_entries_matching);
        loadFunction(cuesheet_track_new);
        loadFunction(cuesheet_track_clone);
        loadFunction(cuesheet_track_delete);
        loadFunction(cuesheet_track_resize_indices);
        loadFunction(cuesheet_track_insert_index);
        loadFunction(cuesheet_track_insert_blank_index);
        loadFunction(cuesheet_track_delete_index);
        loadFunction(cuesheet_resize_tracks);
        loadFunction(cuesheet_set_track);
        loadFunction(cuesheet_insert_track);
        loadFunction(cuesheet_insert_blank_track);
        loadFunction(cuesheet_delete_track);
        loadFunction(cuesheet_is_legal);
        loadFunction(cuesheet_calculate_cddb_id);
        loadFunction(picture_set_mime_type);
        loadFunction(picture_set_description);
        loadFunction(picture_set_data);
        loadFunction(picture_is_legal);

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