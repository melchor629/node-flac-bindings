#include "../format.h"
#include "../../utils/symbol.hpp"

namespace flac_bindings {

    FunctionSymbol<FLAC__StreamMetadata*(FLAC__MetadataType type)> FLAC__metadata_object_new = "FLAC__metadata_object_new";
    FunctionSymbol<FLAC__StreamMetadata*(const FLAC__StreamMetadata *object)> FLAC__metadata_object_clone = "FLAC__metadata_object_clone";
    FunctionSymbol<void(FLAC__StreamMetadata *object)> FLAC__metadata_object_delete = "FLAC__metadata_object_delete";
    FunctionSymbol<FLAC__bool(const FLAC__StreamMetadata *block1, const FLAC__StreamMetadata *block2)> FLAC__metadata_object_is_equal = "FLAC__metadata_object_is_equal";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, FLAC__byte *data, unsigned length, FLAC__bool copy)> FLAC__metadata_object_application_set_data = "FLAC__metadata_object_application_set_data";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned new_num_points)> FLAC__metadata_object_seektable_resize_points = "FLAC__metadata_object_seektable_resize_points";
    FunctionSymbol<void(FLAC__StreamMetadata *object, unsigned point_num, FLAC__StreamMetadata_SeekPoint point)> FLAC__metadata_object_seektable_set_point = "FLAC__metadata_object_seektable_set_point";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned point_num, FLAC__StreamMetadata_SeekPoint point)> FLAC__metadata_object_seektable_insert_point = "FLAC__metadata_object_seektable_insert_point";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned point_num)> FLAC__metadata_object_seektable_delete_point = "FLAC__metadata_object_seektable_delete_point";
    FunctionSymbol<FLAC__bool(const FLAC__StreamMetadata *object)> FLAC__metadata_object_seektable_is_legal = "FLAC__metadata_object_seektable_is_legal";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned num)> FLAC__metadata_object_seektable_template_append_placeholders = "FLAC__metadata_object_seektable_template_append_placeholders";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, uint64_t sample_number)> FLAC__metadata_object_seektable_template_append_point = "FLAC__metadata_object_seektable_template_append_point";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, uint64_t sample_numbers[], unsigned num)> FLAC__metadata_object_seektable_template_append_points = "FLAC__metadata_object_seektable_template_append_points";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned num, uint64_t total_samples)> FLAC__metadata_object_seektable_template_append_spaced_points = "FLAC__metadata_object_seektable_template_append_spaced_points";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned samples, uint64_t total_samples)> FLAC__metadata_object_seektable_template_append_spaced_points_by_samples = "FLAC__metadata_object_seektable_template_append_spaced_points_by_samples";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, FLAC__bool compact)> FLAC__metadata_object_seektable_template_sort = "FLAC__metadata_object_seektable_template_sort";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, FLAC__StreamMetadata_VorbisComment_Entry entry, FLAC__bool copy)> FLAC__metadata_object_vorbiscomment_set_vendor_string = "FLAC__metadata_object_vorbiscomment_set_vendor_string";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned new_num_comments)> FLAC__metadata_object_vorbiscomment_resize_comments = "FLAC__metadata_object_vorbiscomment_resize_comments";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned comment_num, FLAC__StreamMetadata_VorbisComment_Entry entry, FLAC__bool copy)> FLAC__metadata_object_vorbiscomment_set_comment = "FLAC__metadata_object_vorbiscomment_set_comment";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned comment_num, FLAC__StreamMetadata_VorbisComment_Entry entry, FLAC__bool copy)> FLAC__metadata_object_vorbiscomment_insert_comment = "FLAC__metadata_object_vorbiscomment_insert_comment";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, FLAC__StreamMetadata_VorbisComment_Entry entry, FLAC__bool copy)> FLAC__metadata_object_vorbiscomment_append_comment = "FLAC__metadata_object_vorbiscomment_append_comment";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, FLAC__StreamMetadata_VorbisComment_Entry entry, FLAC__bool all, FLAC__bool copy)> FLAC__metadata_object_vorbiscomment_replace_comment = "FLAC__metadata_object_vorbiscomment_replace_comment";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned comment_num)> FLAC__metadata_object_vorbiscomment_delete_comment = "FLAC__metadata_object_vorbiscomment_delete_comment";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata_VorbisComment_Entry *entry, const char *field_name, const char *field_value)> FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair = "FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair";
    FunctionSymbol<FLAC__bool(const FLAC__StreamMetadata_VorbisComment_Entry entry, char **field_name, char **field_value)> FLAC__metadata_object_vorbiscomment_entry_to_name_value_pair = "FLAC__metadata_object_vorbiscomment_entry_to_name_value_pair";
    FunctionSymbol<FLAC__bool(const FLAC__StreamMetadata_VorbisComment_Entry entry, const char *field_name, unsigned field_name_length)> FLAC__metadata_object_vorbiscomment_entry_matches = "FLAC__metadata_object_vorbiscomment_entry_matches";
    FunctionSymbol<int(const FLAC__StreamMetadata *object, unsigned offset, const char *field_name)> FLAC__metadata_object_vorbiscomment_find_entry_from = "FLAC__metadata_object_vorbiscomment_find_entry_from";
    FunctionSymbol<int(FLAC__StreamMetadata *object, const char *field_name)> FLAC__metadata_object_vorbiscomment_remove_entry_matching = "FLAC__metadata_object_vorbiscomment_remove_entry_matching";
    FunctionSymbol<int(FLAC__StreamMetadata *object, const char *field_name)> FLAC__metadata_object_vorbiscomment_remove_entries_matching = "FLAC__metadata_object_vorbiscomment_remove_entries_matching";
    FunctionSymbol<FLAC__StreamMetadata_CueSheet_Track*(void)> FLAC__metadata_object_cuesheet_track_new = "FLAC__metadata_object_cuesheet_track_new";
    FunctionSymbol<FLAC__StreamMetadata_CueSheet_Track*(const FLAC__StreamMetadata_CueSheet_Track *object)> FLAC__metadata_object_cuesheet_track_clone = "FLAC__metadata_object_cuesheet_track_clone";
    FunctionSymbol<void(FLAC__StreamMetadata_CueSheet_Track *object)> FLAC__metadata_object_cuesheet_track_delete = "FLAC__metadata_object_cuesheet_track_delete";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned track_num, unsigned new_num_indices)> FLAC__metadata_object_cuesheet_track_resize_indices = "FLAC__metadata_object_cuesheet_track_resize_indices";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned track_num, unsigned index_num, FLAC__StreamMetadata_CueSheet_Index index)> FLAC__metadata_object_cuesheet_track_insert_index = "FLAC__metadata_object_cuesheet_track_insert_index";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned track_num, unsigned index_num)> FLAC__metadata_object_cuesheet_track_insert_blank_index = "FLAC__metadata_object_cuesheet_track_insert_blank_index";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned track_num, unsigned index_num)> FLAC__metadata_object_cuesheet_track_delete_index = "FLAC__metadata_object_cuesheet_track_delete_index";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned new_num_tracks)> FLAC__metadata_object_cuesheet_resize_tracks = "FLAC__metadata_object_cuesheet_resize_tracks";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned track_num, FLAC__StreamMetadata_CueSheet_Track *track, FLAC__bool copy)> FLAC__metadata_object_cuesheet_set_track = "FLAC__metadata_object_cuesheet_set_track";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned track_num, FLAC__StreamMetadata_CueSheet_Track *track, FLAC__bool copy)> FLAC__metadata_object_cuesheet_insert_track = "FLAC__metadata_object_cuesheet_insert_track";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned track_num)> FLAC__metadata_object_cuesheet_insert_blank_track = "FLAC__metadata_object_cuesheet_insert_blank_track";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned track_num)> FLAC__metadata_object_cuesheet_delete_track = "FLAC__metadata_object_cuesheet_delete_track";
    FunctionSymbol<FLAC__bool(const FLAC__StreamMetadata *object, FLAC__bool check_cd_da_subset, const char **violation)> FLAC__metadata_object_cuesheet_is_legal = "FLAC__metadata_object_cuesheet_is_legal";
    FunctionSymbol<uint32_t(const FLAC__StreamMetadata *object)> FLAC__metadata_object_cuesheet_calculate_cddb_id = "FLAC__metadata_object_cuesheet_calculate_cddb_id";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, char *mime_type, FLAC__bool copy)> FLAC__metadata_object_picture_set_mime_type = "FLAC__metadata_object_picture_set_mime_type";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, FLAC__byte *description, FLAC__bool copy)> FLAC__metadata_object_picture_set_description = "FLAC__metadata_object_picture_set_description";
    FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, FLAC__byte *data, uint32_t length, FLAC__bool copy)> FLAC__metadata_object_picture_set_data = "FLAC__metadata_object_picture_set_data";
    FunctionSymbol<FLAC__bool(const FLAC__StreamMetadata *object, const char **violation)> FLAC__metadata_object_picture_is_legal = "FLAC__metadata_object_picture_is_legal";

}
