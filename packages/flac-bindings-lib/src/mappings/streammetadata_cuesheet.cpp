#include "../flac_addon.hpp"
#include "mappings.hpp"
#include "native_iterator.hpp"
#include <FLAC/metadata.h>

namespace flac_bindings {

  using namespace Napi;

  Function CueSheetMetadata::init(Napi::Env env, FlacAddon& addon) {
    EscapableHandleScope scope(env);

    auto attributes = napi_property_attributes::napi_enumerable;
    Function constructor = DefineClass(
      env,
      "CueSheetMetadata",
      {
        InstanceAccessor(
          "mediaCatalogNumber",
          &CueSheetMetadata::getMediaCatalogNumber,
          &CueSheetMetadata::setMediaCatalogNumber,
          attributes),
        InstanceAccessor(
          "leadIn",
          &CueSheetMetadata::getLeadIn,
          &CueSheetMetadata::setLeadIn,
          attributes),
        InstanceAccessor(
          "isCd",
          &CueSheetMetadata::getIsCd,
          &CueSheetMetadata::setIsCd,
          attributes),
        InstanceAccessor("count", &CueSheetMetadata::getCount, nullptr, attributes),
        InstanceMethod(Napi::Symbol::WellKnown(env, "iterator"), &CueSheetMetadata::iterator),
        InstanceMethod("trackResizeIndices", &CueSheetMetadata::trackResizeIndices),
        InstanceMethod("trackInsertIndex", &CueSheetMetadata::trackInsertIndex),
        InstanceMethod("trackInsertBlankIndex", &CueSheetMetadata::trackInsertBlankIndex),
        InstanceMethod("trackDeleteIndex", &CueSheetMetadata::trackDeleteIndex),
        InstanceMethod("resizeTracks", &CueSheetMetadata::resizeTracks),
        InstanceMethod("setTrack", &CueSheetMetadata::setTrack),
        InstanceMethod("insertTrack", &CueSheetMetadata::insertTrack),
        InstanceMethod("insertBlankTrack", &CueSheetMetadata::insertBlankTrack),
        InstanceMethod("deleteTrack", &CueSheetMetadata::deleteTrack),
        InstanceMethod("isLegal", &CueSheetMetadata::isLegal),
        InstanceMethod("calculateCddbId", &CueSheetMetadata::calculateCddbId),
      });

    addon.cueSheetMetadataConstructor = Persistent(constructor);

    return scope.Escape(constructor).As<Function>();
  }

  CueSheetMetadata::CueSheetMetadata(const CallbackInfo& info):
      ObjectWrap<CueSheetMetadata>(info), Metadata(info, FLAC__METADATA_TYPE_CUESHEET) {}

  Napi::Value CueSheetMetadata::getMediaCatalogNumber(const CallbackInfo& info) {
    return String::New(info.Env(), data->data.cue_sheet.media_catalog_number);
  }

  void CueSheetMetadata::setMediaCatalogNumber(const CallbackInfo& info, const Napi::Value& value) {
    auto string = stringFromJs(value);
    if (string.length() > 128) {
      throw RangeError::New(info.Env(), "String as UTF-8 is more than 128 bytes");
    }

    strcpy(data->data.cue_sheet.media_catalog_number, string.c_str());
    memset(data->data.cue_sheet.media_catalog_number + string.length(), 0, 129 - string.length());
  }

  Napi::Value CueSheetMetadata::getLeadIn(const CallbackInfo& info) {
    return numberToJs(info.Env(), data->data.cue_sheet.lead_in);
  }

  void CueSheetMetadata::setLeadIn(const CallbackInfo&, const Napi::Value& value) {
    auto leadIn = numberFromJs<uint64_t>(value);
    data->data.cue_sheet.lead_in = leadIn;
  }

  Napi::Value CueSheetMetadata::getIsCd(const CallbackInfo& info) {
    return booleanToJs(info.Env(), data->data.cue_sheet.is_cd);
  }

  void CueSheetMetadata::setIsCd(const CallbackInfo&, const Napi::Value& value) {
    auto isCd = booleanFromJs<FLAC__bool>(value);
    data->data.cue_sheet.is_cd = isCd;
  }

  Napi::Value CueSheetMetadata::getCount(const CallbackInfo& info) {
    return numberToJs(info.Env(), data->data.cue_sheet.num_tracks);
  }

  Napi::Value CueSheetMetadata::iterator(const CallbackInfo& info) {
    return NativeIterator::newIterator(
      info.Env(),
      [this](auto env, auto pos) -> NativeIterator::IterationReturnValue {
        EscapableHandleScope scope(env);

        if (pos >= data->data.cue_sheet.num_tracks) {
          return {};
        } else {
          auto track = CueSheetTrack::toJs(env, data->data.cue_sheet.tracks + pos);
          return {scope.Escape(track)};
        }
      });
  }

  Napi::Value CueSheetMetadata::trackResizeIndices(const CallbackInfo& info) {
    auto trackNumber = numberFromJs<uint32_t>(info[0]);
    auto indicesNumber = numberFromJs<uint32_t>(info[1]);

    if (data->data.cue_sheet.num_tracks <= trackNumber) {
      throw RangeError::New(info.Env(), "Invalid track position");
    }

    FLAC__bool ret =
      FLAC__metadata_object_cuesheet_track_resize_indices(data, trackNumber, indicesNumber);
    return booleanToJs(info.Env(), ret);
  }

  Napi::Value CueSheetMetadata::trackInsertIndex(const CallbackInfo& info) {
    auto trackNumber = numberFromJs<uint32_t>(info[0]);
    auto indexNumber = numberFromJs<uint32_t>(info[1]);

    if (data->data.cue_sheet.num_tracks <= trackNumber) {
      throw RangeError::New(info.Env(), "Invalid track position");
    }
    if (data->data.cue_sheet.tracks[trackNumber].num_indices < indexNumber) {
      throw RangeError::New(info.Env(), "Invalid index position");
    }

    auto index = CueSheetIndex::fromJs(info[2]);

    FLAC__bool ret =
      FLAC__metadata_object_cuesheet_track_insert_index(data, trackNumber, indexNumber, *index);
    return booleanToJs(info.Env(), ret);
  }

  Napi::Value CueSheetMetadata::trackInsertBlankIndex(const CallbackInfo& info) {
    auto trackNumber = numberFromJs<uint32_t>(info[0]);
    auto indexNumber = numberFromJs<uint32_t>(info[1]);

    if (data->data.cue_sheet.num_tracks <= trackNumber) {
      throw RangeError::New(info.Env(), "Invalid track position");
    }
    if (data->data.cue_sheet.tracks[trackNumber].num_indices < indexNumber) {
      throw RangeError::New(info.Env(), "Invalid index position");
    }

    FLAC__bool ret =
      FLAC__metadata_object_cuesheet_track_insert_blank_index(data, trackNumber, indexNumber);
    return booleanToJs(info.Env(), ret);
  }

  Napi::Value CueSheetMetadata::trackDeleteIndex(const CallbackInfo& info) {
    auto trackNumber = numberFromJs<uint32_t>(info[0]);
    auto indexNumber = numberFromJs<uint32_t>(info[1]);

    if (data->data.cue_sheet.num_tracks <= trackNumber) {
      throw RangeError::New(info.Env(), "Invalid track position");
    }
    if (data->data.cue_sheet.tracks[trackNumber].num_indices < indexNumber) {
      throw RangeError::New(info.Env(), "Invalid index position");
    }

    FLAC__bool ret =
      FLAC__metadata_object_cuesheet_track_delete_index(data, trackNumber, indexNumber);
    return booleanToJs(info.Env(), ret);
  }

  Napi::Value CueSheetMetadata::resizeTracks(const CallbackInfo& info) {
    auto trackSize = numberFromJs<uint32_t>(info[0]);

    FLAC__bool ret = FLAC__metadata_object_cuesheet_resize_tracks(data, trackSize);
    return booleanToJs(info.Env(), ret);
  }

  Napi::Value CueSheetMetadata::setTrack(const CallbackInfo& info) {
    auto trackNumber = numberFromJs<uint32_t>(info[0]);

    if (data->data.cue_sheet.num_tracks <= trackNumber) {
      throw RangeError::New(info.Env(), "Invalid track position");
    }

    auto track = CueSheetTrack::fromJs(info[1]);

    FLAC__bool ret = FLAC__metadata_object_cuesheet_set_track(data, trackNumber, track, true);
    return booleanToJs(info.Env(), ret);
  }

  Napi::Value CueSheetMetadata::insertTrack(const CallbackInfo& info) {
    auto trackNumber = numberFromJs<uint32_t>(info[0]);

    if (data->data.cue_sheet.num_tracks < trackNumber) {
      throw RangeError::New(info.Env(), "Invalid track position");
    }

    auto track = CueSheetTrack::fromJs(info[1]);

    FLAC__bool ret = FLAC__metadata_object_cuesheet_insert_track(data, trackNumber, track, true);
    return booleanToJs(info.Env(), ret);
  }

  Napi::Value CueSheetMetadata::insertBlankTrack(const CallbackInfo& info) {
    auto trackNumber = numberFromJs<uint32_t>(info[0]);

    if (data->data.cue_sheet.num_tracks < trackNumber) {
      throw RangeError::New(info.Env(), "Invalid track position");
    }

    FLAC__bool ret = FLAC__metadata_object_cuesheet_insert_blank_track(data, trackNumber);
    return booleanToJs(info.Env(), ret);
  }

  Napi::Value CueSheetMetadata::deleteTrack(const CallbackInfo& info) {
    auto trackNumber = numberFromJs<uint32_t>(info[0]);

    if (data->data.cue_sheet.num_tracks <= trackNumber) {
      throw RangeError::New(info.Env(), "Invalid track position");
    }

    FLAC__bool ret = FLAC__metadata_object_cuesheet_delete_track(data, trackNumber);
    return booleanToJs(info.Env(), ret);
  }

  Napi::Value CueSheetMetadata::isLegal(const CallbackInfo& info) {
    FLAC__bool check = maybeBooleanFromJs<FLAC__bool>(info[0]).value_or(false);
    const char* sadness = nullptr;
    FLAC__bool ret = FLAC__metadata_object_cuesheet_is_legal(data, check, &sadness);
    if (ret) {
      return info.Env().Null();
    } else {
      return String::New(info.Env(), sadness);
    }
  }

  Napi::Value CueSheetMetadata::calculateCddbId(const CallbackInfo& info) {
    if (!data->data.cue_sheet.is_cd) {
      throw Error::New(info.Env(), "Cannot calculate CDDB ID if CueSheet is not a CD");
    }

    auto id = FLAC__metadata_object_cuesheet_calculate_cddb_id(data);
    return numberToJs(info.Env(), id);
  }

}
