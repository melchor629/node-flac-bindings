#include "../mappings/mappings.hpp"
#include "../utils/async.hpp"
#include "../utils/js_utils.hpp"
#include <FLAC/metadata.h>

namespace flac_bindings {

  using namespace Napi;

  static Value
    asyncImpl(const Env& env, const char* name, std::function<FLAC__StreamMetadata*()> impl) {
    EscapableHandleScope scope(env);
    auto worker = new AsyncBackgroundTask<FLAC__StreamMetadata*>(
      env,
      [impl](auto c) {
        auto metadata = impl();
        c.resolve(metadata);
      },
      nullptr,
      name,
      [](auto env, auto metadata) { return Metadata::toJs(env, metadata, true); });

    worker->Queue();
    return scope.Escape(worker->getPromise());
  }

  static Value getStreaminfo(const CallbackInfo& info) {
    FLAC__StreamMetadata metadata;
    std::string path = stringFromJs(info[0]);

    FLAC__bool ret = FLAC__metadata_get_streaminfo(path.c_str(), &metadata);
    if (ret) {
      return Metadata::toJs(info.Env(), FLAC__metadata_object_clone(&metadata), true);
    }

    return info.Env().Null();
  }

  static Value getStreaminfoAsync(const CallbackInfo& info) {
    std::string path = stringFromJs(info[0]);
    return asyncImpl(info.Env(), "flac_bindings::getStreaminfoAsync", [path]() {
      FLAC__StreamMetadata metadata;
      FLAC__bool ret = FLAC__metadata_get_streaminfo(path.c_str(), &metadata);
      return ret ? FLAC__metadata_object_clone(&metadata) : nullptr;
    });
  }

  static Value getTags(const CallbackInfo& info) {
    FLAC__StreamMetadata* metadata = nullptr;
    std::string path = stringFromJs(info[0]);

    FLAC__metadata_get_tags(path.c_str(), &metadata);
    return Metadata::toJs(info.Env(), metadata, true);
  }

  static Value getTagsAsync(const CallbackInfo& info) {
    std::string path = stringFromJs(info[0]);
    return asyncImpl(info.Env(), "flac_bindings::getTagsAsync", [path]() {
      FLAC__StreamMetadata* metadata = nullptr;
      FLAC__metadata_get_tags(path.c_str(), &metadata);
      return metadata;
    });
  }

  static Value getCuesheet(const CallbackInfo& info) {
    FLAC__StreamMetadata* metadata = nullptr;
    std::string path = stringFromJs(info[0]);

    FLAC__metadata_get_cuesheet(path.c_str(), &metadata);
    return Metadata::toJs(info.Env(), metadata, true);
  }

  static Value getCuesheetAsync(const CallbackInfo& info) {
    std::string path = stringFromJs(info[0]);
    return asyncImpl(info.Env(), "flac_bindings::getCuesheetAsync", [path]() {
      FLAC__StreamMetadata* metadata = nullptr;
      FLAC__metadata_get_cuesheet(path.c_str(), &metadata);
      return metadata;
    });
  }

  static Value getPicture(const CallbackInfo& info) {
    std::string path = stringFromJs(info[0]);
    auto type = maybeNumberFromJs<FLAC__StreamMetadata_Picture_Type>(info[1]).value_or(
      (FLAC__StreamMetadata_Picture_Type) -1);
    auto mimeType = maybeStringFromJs(info[2]);
    auto description = maybeStringFromJs(info[3]);
    auto maxWidth = maybeNumberFromJs<unsigned>(info[4]).value_or((unsigned) -1);
    auto maxHeight = maybeNumberFromJs<unsigned>(info[5]).value_or((unsigned) -1);
    auto maxDepth = maybeNumberFromJs<unsigned>(info[6]).value_or((unsigned) -1);
    auto maxColors = maybeNumberFromJs<unsigned>(info[7]).value_or((unsigned) -1);

    FLAC__StreamMetadata* picture = nullptr;
    FLAC__metadata_get_picture(
      path.c_str(),
      &picture,
      type,
      mimeType ? mimeType->c_str() : nullptr,
      description ? (FLAC__byte*) description->c_str() : nullptr,
      maxWidth,
      maxHeight,
      maxDepth,
      maxColors);

    return Metadata::toJs(info.Env(), picture, true);
  }

  static Value getPictureAsync(const CallbackInfo& info) {
    std::string path = stringFromJs(info[0]);
    auto type = maybeNumberFromJs<FLAC__StreamMetadata_Picture_Type>(info[1]).value_or(
      (FLAC__StreamMetadata_Picture_Type) -1);
    auto mimeType = maybeStringFromJs(info[2]);
    auto description = maybeStringFromJs(info[3]);
    auto maxWidth = maybeNumberFromJs<unsigned>(info[4]).value_or((unsigned) -1);
    auto maxHeight = maybeNumberFromJs<unsigned>(info[5]).value_or((unsigned) -1);
    auto maxDepth = maybeNumberFromJs<unsigned>(info[6]).value_or((unsigned) -1);
    auto maxColors = maybeNumberFromJs<unsigned>(info[7]).value_or((unsigned) -1);

    return asyncImpl(info.Env(), "flac_bindings::getPictureAsync", [=]() {
      FLAC__StreamMetadata* picture = nullptr;
      FLAC__metadata_get_picture(
        path.c_str(),
        &picture,
        type,
        mimeType ? mimeType->c_str() : nullptr,
        description ? (FLAC__byte*) description->c_str() : nullptr,
        maxWidth,
        maxHeight,
        maxDepth,
        maxColors);
      return picture;
    });
  }

  Object initMetadata0(const Env& env) {
    EscapableHandleScope scope(env);
    Object metadata0 = Object::New(env);

    auto attrs = napi_property_attributes::napi_enumerable;
    metadata0.DefineProperties({
      PropertyDescriptor::Function(env, metadata0, "getStreaminfo", &getStreaminfo, attrs),
      PropertyDescriptor::Function(
        env,
        metadata0,
        "getStreaminfoAsync",
        &getStreaminfoAsync,
        attrs),
      PropertyDescriptor::Function(env, metadata0, "getTags", &getTags, attrs),
      PropertyDescriptor::Function(env, metadata0, "getTagsAsync", &getTagsAsync, attrs),
      PropertyDescriptor::Function(env, metadata0, "getCuesheet", &getCuesheet, attrs),
      PropertyDescriptor::Function(env, metadata0, "getCuesheetAsync", &getCuesheetAsync, attrs),
      PropertyDescriptor::Function(env, metadata0, "getPicture", &getPicture, attrs),
      PropertyDescriptor::Function(env, metadata0, "getPictureAsync", &getPictureAsync, attrs),
    });

    return scope.Escape(objectFreeze(metadata0)).As<Object>();
  }

}
