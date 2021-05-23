#include "../mappings/mappings.hpp"
#include "../utils/converters.hpp"
#include "../utils/enum.hpp"
#include <FLAC/format.h>

namespace flac_bindings {

  using namespace Napi;

  static Value sampleRateIsValid(const CallbackInfo& info) {
    auto sampleRate = numberFromJs<uint32_t>(info[0]);
    FLAC__bool ret = FLAC__format_sample_rate_is_valid(sampleRate);
    return booleanToJs(info.Env(), ret);
  }

  static Value sampleRateIsSubset(const CallbackInfo& info) {
    auto sampleRate = numberFromJs<uint32_t>(info[0]);
    FLAC__bool ret = FLAC__format_sample_rate_is_subset(sampleRate);
    return booleanToJs(info.Env(), ret);
  }

  static Value blocksizeIsSubset(const CallbackInfo& info) {
    auto blocksize = numberFromJs<uint32_t>(info[0]);
    auto sampleRate = numberFromJs<uint32_t>(info[1]);
    FLAC__bool ret = FLAC__format_blocksize_is_subset(blocksize, sampleRate);
    return booleanToJs(info.Env(), ret);
  }

  static Value vorbiscommentEntryNameIsLegal(const CallbackInfo& info) {
    auto string = stringFromJs(info[0]);
    FLAC__bool ret = FLAC__format_vorbiscomment_entry_name_is_legal(string.c_str());
    return booleanToJs(info.Env(), ret);
  }

  static Value vorbiscommentEntryValueIsLegal(const CallbackInfo& info) {
    auto string = stringFromJs(info[0]);
    FLAC__bool ret = FLAC__format_vorbiscomment_entry_value_is_legal(
      (FLAC__byte*) string.c_str(),
      string.length());
    return booleanToJs(info.Env(), ret);
  }

  static Value vorbiscommentEntryIsLegal(const CallbackInfo& info) {
    auto string = stringFromJs(info[0]);
    FLAC__bool ret =
      FLAC__format_vorbiscomment_entry_is_legal((FLAC__byte*) string.c_str(), string.length());
    return booleanToJs(info.Env(), ret);
  }

  static Value seektableIsLegal(const CallbackInfo& info) {
    FLAC__StreamMetadata* metadata = Metadata::fromJs(info[0]);
    if (metadata->type != FLAC__METADATA_TYPE_SEEKTABLE) {
      throw TypeError::New(info.Env(), "Metadata is not of type SeekTable");
    }

    FLAC__bool ret = FLAC__format_seektable_is_legal(&metadata->data.seek_table);
    return booleanToJs(info.Env(), ret);
  }

  static Value seektableSort(const CallbackInfo& info) {
    FLAC__StreamMetadata* metadata = Metadata::fromJs(info[0]);
    if (metadata->type != FLAC__METADATA_TYPE_SEEKTABLE) {
      throw TypeError::New(info.Env(), "Metadata is not of type SeekTable");
    }

    uint32_t numberOfPlaceholdersConvertedToTemplates =
      FLAC__format_seektable_sort(&metadata->data.seek_table);
    return numberToJs(info.Env(), numberOfPlaceholdersConvertedToTemplates);
  }

  static Value cuesheetIsLegal(const CallbackInfo& info) {
    FLAC__StreamMetadata* metadata = Metadata::fromJs(info[0]);
    if (metadata->type != FLAC__METADATA_TYPE_CUESHEET) {
      throw TypeError::New(info.Env(), "Metadata is not of type CueSheet");
    }

    FLAC__bool check = maybeBooleanFromJs<FLAC__bool>(info[1]).value_or(false);
    const char* sadness = nullptr;
    FLAC__bool ret = FLAC__format_cuesheet_is_legal(&metadata->data.cue_sheet, check, &sadness);
    if (ret) {
      return info.Env().Null();
    } else {
      return String::New(info.Env(), sadness);
    }
  }

  static Value pictureIsLegal(const CallbackInfo& info) {
    FLAC__StreamMetadata* metadata = Metadata::fromJs(info[0]);
    if (metadata->type != FLAC__METADATA_TYPE_PICTURE) {
      throw TypeError::New(info.Env(), "Metadata is not of type Picture");
    }

    const char* sadness = nullptr;
    FLAC__bool ret = FLAC__format_picture_is_legal(&metadata->data.picture, &sadness);
    if (ret) {
      return info.Env().Null();
    } else {
      return String::New(info.Env(), sadness);
    }
  }

  static c_enum::DefineReturnType createMetadataTypeEnum(const Env& env) {
    Object obj1 = Object::New(env);
    Object obj2 = Object::New(env);
    c_enum::defineValue(obj1, obj2, "STREAMINFO", FLAC__METADATA_TYPE_STREAMINFO);
    c_enum::defineValue(obj1, obj2, "PADDING", FLAC__METADATA_TYPE_PADDING);
    c_enum::defineValue(obj1, obj2, "APPLICATION", FLAC__METADATA_TYPE_APPLICATION);
    c_enum::defineValue(obj1, obj2, "SEEKTABLE", FLAC__METADATA_TYPE_SEEKTABLE);
    c_enum::defineValue(obj1, obj2, "VORBIS_COMMENT", FLAC__METADATA_TYPE_VORBIS_COMMENT);
    c_enum::defineValue(obj1, obj2, "CUESHEET", FLAC__METADATA_TYPE_CUESHEET);
    c_enum::defineValue(obj1, obj2, "PICTURE", FLAC__METADATA_TYPE_PICTURE);
    c_enum::defineValue(obj1, obj2, "UNDEFINED", FLAC__METADATA_TYPE_UNDEFINED);
    c_enum::defineValue(obj1, obj2, "MAX_METADATA_TYPE", FLAC__MAX_METADATA_TYPE);
    return std::make_tuple(obj1, obj2);
  }

  static c_enum::DefineReturnType createEntropyCodingMethodTypeEnum(const Env& env) {
    Object obj1 = Object::New(env);
    Object obj2 = Object::New(env);
    c_enum::defineValue(
      obj1,
      obj2,
      "PARTITIONED_RICE",
      FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE);
    c_enum::defineValue(
      obj1,
      obj2,
      "PARTITIONED_RICE2",
      FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE2);
    return std::make_tuple(obj1, obj2);
  }

  static c_enum::DefineReturnType createSubframeTypeEnum(const Env& env) {
    Object obj1 = Object::New(env);
    Object obj2 = Object::New(env);
    c_enum::defineValue(obj1, obj2, "CONSTANT", FLAC__SUBFRAME_TYPE_CONSTANT);
    c_enum::defineValue(obj1, obj2, "VERBATIM", FLAC__SUBFRAME_TYPE_VERBATIM);
    c_enum::defineValue(obj1, obj2, "FIXED", FLAC__SUBFRAME_TYPE_FIXED);
    c_enum::defineValue(obj1, obj2, "LPC", FLAC__SUBFRAME_TYPE_LPC);
    return std::make_tuple(obj1, obj2);
  }

  static c_enum::DefineReturnType createChannelAssignmentEnum(const Env& env) {
    Object obj1 = Object::New(env);
    Object obj2 = Object::New(env);
    c_enum::defineValue(obj1, obj2, "INDEPENDENT", FLAC__CHANNEL_ASSIGNMENT_INDEPENDENT);
    c_enum::defineValue(obj1, obj2, "LEFT_SIDE", FLAC__CHANNEL_ASSIGNMENT_LEFT_SIDE);
    c_enum::defineValue(obj1, obj2, "RIGHT_SIDE", FLAC__CHANNEL_ASSIGNMENT_RIGHT_SIDE);
    c_enum::defineValue(obj1, obj2, "MID_SIDE", FLAC__CHANNEL_ASSIGNMENT_MID_SIDE);
    return std::make_tuple(obj1, obj2);
  }

  static c_enum::DefineReturnType createFrameNumberTypeEnum(const Env& env) {
    Object obj1 = Object::New(env);
    Object obj2 = Object::New(env);
    c_enum::defineValue(obj1, obj2, "FRAME_NUMBER", FLAC__FRAME_NUMBER_TYPE_FRAME_NUMBER);
    c_enum::defineValue(obj1, obj2, "SAMPLE_NUMBER", FLAC__FRAME_NUMBER_TYPE_SAMPLE_NUMBER);
    return std::make_tuple(obj1, obj2);
  }

  static c_enum::DefineReturnType createPictureTypeEnum(const Env& env) {
    Object obj1 = Object::New(env);
    Object obj2 = Object::New(env);
    c_enum::defineValue(obj1, obj2, "OTHER", FLAC__STREAM_METADATA_PICTURE_TYPE_OTHER);
    c_enum::defineValue(
      obj1,
      obj2,
      "FILE_ICON_STANDARD",
      FLAC__STREAM_METADATA_PICTURE_TYPE_FILE_ICON_STANDARD);
    c_enum::defineValue(obj1, obj2, "FILE_ICON", FLAC__STREAM_METADATA_PICTURE_TYPE_FILE_ICON);
    c_enum::defineValue(obj1, obj2, "FRONT_COVER", FLAC__STREAM_METADATA_PICTURE_TYPE_FRONT_COVER);
    c_enum::defineValue(obj1, obj2, "BACK_COVER", FLAC__STREAM_METADATA_PICTURE_TYPE_BACK_COVER);
    c_enum::defineValue(
      obj1,
      obj2,
      "LEAFLET_PAGE",
      FLAC__STREAM_METADATA_PICTURE_TYPE_LEAFLET_PAGE);
    c_enum::defineValue(obj1, obj2, "MEDIA", FLAC__STREAM_METADATA_PICTURE_TYPE_MEDIA);
    c_enum::defineValue(obj1, obj2, "LEAD_ARTIST", FLAC__STREAM_METADATA_PICTURE_TYPE_LEAD_ARTIST);
    c_enum::defineValue(obj1, obj2, "ARTIST", FLAC__STREAM_METADATA_PICTURE_TYPE_ARTIST);
    c_enum::defineValue(obj1, obj2, "CONDUCTOR", FLAC__STREAM_METADATA_PICTURE_TYPE_CONDUCTOR);
    c_enum::defineValue(obj1, obj2, "BAND", FLAC__STREAM_METADATA_PICTURE_TYPE_BAND);
    c_enum::defineValue(obj1, obj2, "COMPOSER", FLAC__STREAM_METADATA_PICTURE_TYPE_COMPOSER);
    c_enum::defineValue(obj1, obj2, "LYRICIST", FLAC__STREAM_METADATA_PICTURE_TYPE_LYRICIST);
    c_enum::defineValue(
      obj1,
      obj2,
      "RECORDING_LOCATION",
      FLAC__STREAM_METADATA_PICTURE_TYPE_RECORDING_LOCATION);
    c_enum::defineValue(
      obj1,
      obj2,
      "DURING_RECORDING",
      FLAC__STREAM_METADATA_PICTURE_TYPE_DURING_RECORDING);
    c_enum::defineValue(
      obj1,
      obj2,
      "DURING_PERFORMANCE",
      FLAC__STREAM_METADATA_PICTURE_TYPE_DURING_PERFORMANCE);
    c_enum::defineValue(
      obj1,
      obj2,
      "VIDEO_SCREEN_CAPTURE",
      FLAC__STREAM_METADATA_PICTURE_TYPE_VIDEO_SCREEN_CAPTURE);
    c_enum::defineValue(obj1, obj2, "FISH", FLAC__STREAM_METADATA_PICTURE_TYPE_FISH);
    c_enum::defineValue(
      obj1,
      obj2,
      "ILLUSTRATION",
      FLAC__STREAM_METADATA_PICTURE_TYPE_ILLUSTRATION);
    c_enum::defineValue(
      obj1,
      obj2,
      "BAND_LOGOTYPE",
      FLAC__STREAM_METADATA_PICTURE_TYPE_BAND_LOGOTYPE);
    c_enum::defineValue(
      obj1,
      obj2,
      "PUBLISHER_LOGOTYPE",
      FLAC__STREAM_METADATA_PICTURE_TYPE_PUBLISHER_LOGOTYPE);
    c_enum::defineValue(obj1, obj2, "UNDEFINED", FLAC__STREAM_METADATA_PICTURE_TYPE_UNDEFINED);
    return std::make_tuple(obj1, obj2);
  }

  Object initFormat(const Env& env) {
    EscapableHandleScope scope(env);
    Object format = Object::New(env);

    napi_property_attributes attrs = napi_property_attributes::napi_enumerable;
    format.DefineProperties({
      PropertyDescriptor::Value("VERSION_STRING", String::New(env, FLAC__VERSION_STRING), attrs),
      PropertyDescriptor::Value("VENDOR_STRING", String::New(env, FLAC__VENDOR_STRING), attrs),
      PropertyDescriptor::Value("STREAM_SYNC", numberToJs(env, FLAC__STREAM_SYNC), attrs),
      PropertyDescriptor::Value(
        "STREAM_SYNC_STRING",
        String::New(env, (const char*) FLAC__STREAM_SYNC_STRING, FLAC__STREAM_SYNC_LENGTH),
        attrs),

      PropertyDescriptor::Value(
        "API_SUPPORTS_OGG_FLAC",
        booleanToJs(env, FLAC_API_SUPPORTS_OGG_FLAC),
        attrs),

      PropertyDescriptor::Value(
        "MAX_METADATA_TYPE_CODE",
        numberToJs(env, FLAC__MAX_METADATA_TYPE_CODE),
        attrs),
      PropertyDescriptor::Value("MIN_BLOCK_SIZE", numberToJs(env, FLAC__MIN_BLOCK_SIZE), attrs),
      PropertyDescriptor::Value("MAX_BLOCK_SIZE", numberToJs(env, FLAC__MAX_BLOCK_SIZE), attrs),
      PropertyDescriptor::Value(
        "SUBSET_MAX_BLOCK_SIZE_48000HZ",
        numberToJs(env, FLAC__SUBSET_MAX_BLOCK_SIZE_48000HZ),
        attrs),
      PropertyDescriptor::Value("MAX_CHANNELS", numberToJs(env, FLAC__MAX_CHANNELS), attrs),
      PropertyDescriptor::Value(
        "MIN_BITS_PER_SAMPLE",
        numberToJs(env, FLAC__MIN_BITS_PER_SAMPLE),
        attrs),
      PropertyDescriptor::Value(
        "MAX_BITS_PER_SAMPLE",
        numberToJs(env, FLAC__MAX_BITS_PER_SAMPLE),
        attrs),
      PropertyDescriptor::Value(
        "REFERENCE_CODEC_MAX_BITS_PER_SAMPLE",
        numberToJs(env, FLAC__REFERENCE_CODEC_MAX_BITS_PER_SAMPLE),
        attrs),
      PropertyDescriptor::Value("MAX_SAMPLE_RATE", numberToJs(env, FLAC__MAX_SAMPLE_RATE), attrs),
      PropertyDescriptor::Value("MAX_LPC_ORDER", numberToJs(env, FLAC__MAX_LPC_ORDER), attrs),
      PropertyDescriptor::Value(
        "SUBSET_MAX_LPC_ORDER_48000HZ",
        numberToJs(env, FLAC__SUBSET_MAX_LPC_ORDER_48000HZ),
        attrs),
      PropertyDescriptor::Value(
        "MIN_QLP_COEFF_PRECISION",
        numberToJs(env, FLAC__MIN_QLP_COEFF_PRECISION),
        attrs),
      PropertyDescriptor::Value(
        "MAX_QLP_COEFF_PRECISION",
        numberToJs(env, FLAC__MAX_QLP_COEFF_PRECISION),
        attrs),
      PropertyDescriptor::Value("MAX_FIXED_ORDER", numberToJs(env, FLAC__MAX_FIXED_ORDER), attrs),
      PropertyDescriptor::Value(
        "MAX_RICE_PARTITION_ORDER",
        numberToJs(env, FLAC__MAX_RICE_PARTITION_ORDER),
        attrs),
      PropertyDescriptor::Value(
        "SUBSET_MAX_RICE_PARTITION_ORDER",
        numberToJs(env, FLAC__SUBSET_MAX_RICE_PARTITION_ORDER),
        attrs),

      PropertyDescriptor::Function(env, format, "sampleRateIsValid", &sampleRateIsValid),
      PropertyDescriptor::Function(env, format, "blocksizeIsSubset", &blocksizeIsSubset),
      PropertyDescriptor::Function(env, format, "sampleRateIsSubset", &sampleRateIsSubset),
      PropertyDescriptor::Function(
        env,
        format,
        "vorbiscommentEntryNameIsLegal",
        &vorbiscommentEntryNameIsLegal),
      PropertyDescriptor::Function(
        env,
        format,
        "vorbiscommentEntryValueIsLegal",
        &vorbiscommentEntryValueIsLegal),
      PropertyDescriptor::Function(
        env,
        format,
        "vorbiscommentEntryIsLegal",
        &vorbiscommentEntryIsLegal),
      PropertyDescriptor::Function(env, format, "seektableIsLegal", &seektableIsLegal),
      PropertyDescriptor::Function(env, format, "seektableSort", &seektableSort),
      PropertyDescriptor::Function(env, format, "cuesheetIsLegal", &cuesheetIsLegal),
      PropertyDescriptor::Function(env, format, "pictureIsLegal", &pictureIsLegal),
    });

    c_enum::declareInObject(format, "MetadataType", createMetadataTypeEnum);
    c_enum::declareInObject(format, "EntropyCodingMethodType", createEntropyCodingMethodTypeEnum);
    c_enum::declareInObject(format, "SubframeType", createSubframeTypeEnum);
    c_enum::declareInObject(format, "ChannelAssignment", createChannelAssignmentEnum);
    c_enum::declareInObject(format, "FrameNumberType", createFrameNumberTypeEnum);
    c_enum::declareInObject(format, "PictureType", createPictureTypeEnum);

    return scope.Escape(objectFreeze(format)).As<Object>();
  }

}
