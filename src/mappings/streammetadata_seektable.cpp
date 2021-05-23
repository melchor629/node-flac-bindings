#include "../flac_addon.hpp"
#include "mappings.hpp"
#include "native_iterator.hpp"
#include <FLAC/metadata.h>

namespace flac_bindings {

  using namespace Napi;

  Function SeekTableMetadata::init(Napi::Env env, FlacAddon& addon) {
    EscapableHandleScope scope(env);

    Function constructor = DefineClass(
      env,
      "SeekTableMetadata",
      {
        InstanceAccessor(
          "count",
          &SeekTableMetadata::getCount,
          nullptr,
          napi_property_attributes::napi_enumerable),
        InstanceMethod(Napi::Symbol::WellKnown(env, "iterator"), &SeekTableMetadata::iterator),
        InstanceMethod("resizePoints", &SeekTableMetadata::resizePoints),
        InstanceMethod("setPoint", &SeekTableMetadata::setPoint),
        InstanceMethod("insertPoint", &SeekTableMetadata::insertPoint),
        InstanceMethod("deletePoint", &SeekTableMetadata::deletePoint),
        InstanceMethod("isLegal", &SeekTableMetadata::isLegal),
        InstanceMethod(
          "templateAppendPlaceholders",
          &SeekTableMetadata::templateAppendPlaceholders),
        InstanceMethod("templateAppendPoint", &SeekTableMetadata::templateAppendPoint),
        InstanceMethod("templateAppendPoints", &SeekTableMetadata::templateAppendPoints),
        InstanceMethod(
          "templateAppendSpacedPoints",
          &SeekTableMetadata::templateAppendSpacedPoints),
        InstanceMethod(
          "templateAppendSpacedPointsBySamples",
          &SeekTableMetadata::templateAppendSpacedPointsBySamples),
        InstanceMethod("templateSort", &SeekTableMetadata::templateSort),
      });

    addon.seekTableMetadataConstructor = Persistent(constructor);

    return scope.Escape(constructor).As<Function>();
  }

  SeekTableMetadata::SeekTableMetadata(const CallbackInfo& info):
      ObjectWrap<SeekTableMetadata>(info), Metadata(info, FLAC__METADATA_TYPE_SEEKTABLE) {}

  Napi::Value SeekTableMetadata::getCount(const CallbackInfo& info) {
    return numberToJs(info.Env(), data->data.seek_table.num_points);
  }

  Napi::Value SeekTableMetadata::iterator(const CallbackInfo& info) {
    return NativeIterator::newIterator(
      info.Env(),
      [this](auto env, auto pos) -> NativeIterator::IterationReturnValue {
        EscapableHandleScope scope(env);

        if (pos >= data->data.seek_table.num_points) {
          return {};
        } else {
          auto value = SeekPoint::toJs(env, data->data.seek_table.points + pos);
          return {scope.Escape(value)};
        }
      });
  }

  Napi::Value SeekTableMetadata::resizePoints(const CallbackInfo& info) {
    auto size = numberFromJs<uint32_t>(info[0]);
    FLAC__bool ret = FLAC__metadata_object_seektable_resize_points(data, size);
    return booleanToJs(info.Env(), ret);
  }

  void SeekTableMetadata::setPoint(const CallbackInfo& info) {
    auto pos = numberFromJs<uint32_t>(info[0]);
    auto point = SeekPoint::fromJs(info[1]);
    if (data->data.seek_table.num_points <= pos) {
      throw RangeError::New(info.Env(), "Point position is invalid");
    }

    FLAC__metadata_object_seektable_set_point(data, pos, *point);
  }

  Napi::Value SeekTableMetadata::insertPoint(const CallbackInfo& info) {
    auto pos = numberFromJs<uint32_t>(info[0]);
    auto point = SeekPoint::fromJs(info[1]);
    if (data->data.seek_table.num_points < pos) {
      throw RangeError::New(info.Env(), "Point position is invalid");
    }

    FLAC__bool ret = FLAC__metadata_object_seektable_insert_point(data, pos, *point);
    return booleanToJs(info.Env(), ret);
  }

  Napi::Value SeekTableMetadata::deletePoint(const CallbackInfo& info) {
    auto pos = numberFromJs<uint32_t>(info[0]);
    if (data->data.seek_table.num_points <= pos) {
      throw RangeError::New(info.Env(), "Point position is invalid");
    }

    FLAC__bool ret = FLAC__metadata_object_seektable_delete_point(data, pos);
    return booleanToJs(info.Env(), ret);
  }

  Napi::Value SeekTableMetadata::isLegal(const CallbackInfo& info) {
    return booleanToJs(info.Env(), FLAC__metadata_object_seektable_is_legal(data));
  }

  Napi::Value SeekTableMetadata::templateAppendPlaceholders(const CallbackInfo& info) {
    auto num = numberFromJs<uint32_t>(info[0]);
    FLAC__bool res = FLAC__metadata_object_seektable_template_append_placeholders(data, num);
    return booleanToJs(info.Env(), res);
  }

  Napi::Value SeekTableMetadata::templateAppendPoint(const CallbackInfo& info) {
    auto sampleNumber = numberFromJs<uint64_t>(info[0]);
    FLAC__bool res = FLAC__metadata_object_seektable_template_append_point(data, sampleNumber);
    return booleanToJs(info.Env(), res);
  }

  Napi::Value SeekTableMetadata::templateAppendPoints(const CallbackInfo& info) {
    auto samples = arrayFromJs<uint64_t>(info[0], numberFromJs<uint64_t>);
    FLAC__bool res =
      FLAC__metadata_object_seektable_template_append_points(data, samples.data(), samples.size());
    return booleanToJs(info.Env(), res);
  }

  Napi::Value SeekTableMetadata::templateAppendSpacedPoints(const CallbackInfo& info) {
    auto num = numberFromJs<uint32_t>(info[0]);
    auto totalSamples = numberFromJs<uint64_t>(info[1]);
    if (totalSamples == 0) {
      throw RangeError::New(info.Env(), "Total samples is 0");
    }

    FLAC__bool res =
      FLAC__metadata_object_seektable_template_append_spaced_points(data, num, totalSamples);
    return booleanToJs(info.Env(), res);
  }

  Napi::Value SeekTableMetadata::templateAppendSpacedPointsBySamples(const CallbackInfo& info) {
    auto samples = numberFromJs<uint32_t>(info[0]);
    auto totalSamples = numberFromJs<uint64_t>(info[1]);
    if (samples == 0) {
      throw RangeError::New(info.Env(), "Samples is 0");
    }
    if (totalSamples == 0) {
      throw RangeError::New(info.Env(), "Total samples is 0");
    }

    FLAC__bool res = FLAC__metadata_object_seektable_template_append_spaced_points_by_samples(
      data,
      samples,
      totalSamples);
    return booleanToJs(info.Env(), res);
  }

  Napi::Value SeekTableMetadata::templateSort(const CallbackInfo& info) {
    auto compact = maybeBooleanFromJs<bool>(info[0]).value_or(false);
    FLAC__bool res = FLAC__metadata_object_seektable_template_sort(data, compact);
    return booleanToJs(info.Env(), res);
  }

}
