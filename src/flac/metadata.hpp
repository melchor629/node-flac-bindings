#pragma once

#include "format.h"
#include "../utils/symbol.hpp"

namespace flac_bindings {

    extern FunctionSymbol<FLAC__StreamMetadata*(FLAC__MetadataType type)> FLAC__metadata_object_new;
    extern FunctionSymbol<FLAC__StreamMetadata*(const FLAC__StreamMetadata *object)> FLAC__metadata_object_clone;
    extern FunctionSymbol<void(FLAC__StreamMetadata *object)> FLAC__metadata_object_delete;
    extern FunctionSymbol<FLAC__bool(const FLAC__StreamMetadata *block1, const FLAC__StreamMetadata *block2)> FLAC__metadata_object_is_equal;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, FLAC__byte *data, unsigned length, FLAC__bool copy)> FLAC__metadata_object_application_set_data;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned new_num_points)> FLAC__metadata_object_seektable_resize_points;
    extern FunctionSymbol<void(FLAC__StreamMetadata *object, unsigned point_num, FLAC__StreamMetadata_SeekPoint point)> FLAC__metadata_object_seektable_set_point;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned point_num, FLAC__StreamMetadata_SeekPoint point)> FLAC__metadata_object_seektable_insert_point;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned point_num)> FLAC__metadata_object_seektable_delete_point;
    extern FunctionSymbol<FLAC__bool(const FLAC__StreamMetadata *object)> FLAC__metadata_object_seektable_is_legal;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned num)> FLAC__metadata_object_seektable_template_append_placeholders;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, uint64_t sample_number)> FLAC__metadata_object_seektable_template_append_point;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, uint64_t sample_numbers[], unsigned num)> FLAC__metadata_object_seektable_template_append_points;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned num, uint64_t total_samples)> FLAC__metadata_object_seektable_template_append_spaced_points;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned samples, uint64_t total_samples)> FLAC__metadata_object_seektable_template_append_spaced_points_by_samples;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, FLAC__bool compact)> FLAC__metadata_object_seektable_template_sort;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, FLAC__StreamMetadata_VorbisComment_Entry entry, FLAC__bool copy)> FLAC__metadata_object_vorbiscomment_set_vendor_string;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned new_num_comments)> FLAC__metadata_object_vorbiscomment_resize_comments;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned comment_num, FLAC__StreamMetadata_VorbisComment_Entry entry, FLAC__bool copy)> FLAC__metadata_object_vorbiscomment_set_comment;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned comment_num, FLAC__StreamMetadata_VorbisComment_Entry entry, FLAC__bool copy)> FLAC__metadata_object_vorbiscomment_insert_comment;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, FLAC__StreamMetadata_VorbisComment_Entry entry, FLAC__bool copy)> FLAC__metadata_object_vorbiscomment_append_comment;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, FLAC__StreamMetadata_VorbisComment_Entry entry, FLAC__bool all, FLAC__bool copy)> FLAC__metadata_object_vorbiscomment_replace_comment;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned comment_num)> FLAC__metadata_object_vorbiscomment_delete_comment;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata_VorbisComment_Entry *entry, const char *field_name, const char *field_value)> FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair;
    extern FunctionSymbol<FLAC__bool(const FLAC__StreamMetadata_VorbisComment_Entry entry, char **field_name, char **field_value)> FLAC__metadata_object_vorbiscomment_entry_to_name_value_pair;
    extern FunctionSymbol<FLAC__bool(const FLAC__StreamMetadata_VorbisComment_Entry entry, const char *field_name, unsigned field_name_length)> FLAC__metadata_object_vorbiscomment_entry_matches;
    extern FunctionSymbol<int(const FLAC__StreamMetadata *object, unsigned offset, const char *field_name)> FLAC__metadata_object_vorbiscomment_find_entry_from;
    extern FunctionSymbol<int(FLAC__StreamMetadata *object, const char *field_name)> FLAC__metadata_object_vorbiscomment_remove_entry_matching;
    extern FunctionSymbol<int(FLAC__StreamMetadata *object, const char *field_name)> FLAC__metadata_object_vorbiscomment_remove_entries_matching;
    extern FunctionSymbol<FLAC__StreamMetadata_CueSheet_Track*(void)> FLAC__metadata_object_cuesheet_track_new;
    extern FunctionSymbol<FLAC__StreamMetadata_CueSheet_Track*(const FLAC__StreamMetadata_CueSheet_Track *object)> FLAC__metadata_object_cuesheet_track_clone;
    extern FunctionSymbol<void(FLAC__StreamMetadata_CueSheet_Track *object)> FLAC__metadata_object_cuesheet_track_delete;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned track_num, unsigned new_num_indices)> FLAC__metadata_object_cuesheet_track_resize_indices;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned track_num, unsigned index_num, FLAC__StreamMetadata_CueSheet_Index index)> FLAC__metadata_object_cuesheet_track_insert_index;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned track_num, unsigned index_num)> FLAC__metadata_object_cuesheet_track_insert_blank_index;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned track_num, unsigned index_num)> FLAC__metadata_object_cuesheet_track_delete_index;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned new_num_tracks)> FLAC__metadata_object_cuesheet_resize_tracks;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned track_num, FLAC__StreamMetadata_CueSheet_Track *track, FLAC__bool copy)> FLAC__metadata_object_cuesheet_set_track;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned track_num, FLAC__StreamMetadata_CueSheet_Track *track, FLAC__bool copy)> FLAC__metadata_object_cuesheet_insert_track;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned track_num)> FLAC__metadata_object_cuesheet_insert_blank_track;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, unsigned track_num)> FLAC__metadata_object_cuesheet_delete_track;
    extern FunctionSymbol<FLAC__bool(const FLAC__StreamMetadata *object, FLAC__bool check_cd_da_subset, const char **violation)> FLAC__metadata_object_cuesheet_is_legal;
    extern FunctionSymbol<uint32_t(const FLAC__StreamMetadata *object)> FLAC__metadata_object_cuesheet_calculate_cddb_id;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, char *mime_type, FLAC__bool copy)> FLAC__metadata_object_picture_set_mime_type;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, FLAC__byte *description, FLAC__bool copy)> FLAC__metadata_object_picture_set_description;
    extern FunctionSymbol<FLAC__bool(FLAC__StreamMetadata *object, FLAC__byte *data, uint32_t length, FLAC__bool copy)> FLAC__metadata_object_picture_set_data;
    extern FunctionSymbol<FLAC__bool(const FLAC__StreamMetadata *object, const char **violation)> FLAC__metadata_object_picture_is_legal;

}
