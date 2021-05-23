#include "../flac_addon.hpp"
#include "mappings.hpp"
#include "native_iterator.hpp"
#include <FLAC/metadata.h>

namespace flac_bindings {

  using namespace Napi;

  Function VorbisCommentMetadata::init(Napi::Env env, FlacAddon& addon) {
    EscapableHandleScope scope(env);

    Function constructor = DefineClass(
      env,
      "VorbisCommentMetadata",
      {
        InstanceAccessor(
          "vendorString",
          &VorbisCommentMetadata::getVendorString,
          &VorbisCommentMetadata::setVendorString,
          napi_property_attributes::napi_enumerable),
        InstanceAccessor(
          "count",
          &VorbisCommentMetadata::getCount,
          nullptr,
          napi_property_attributes::napi_enumerable),
        InstanceMethod(Napi::Symbol::WellKnown(env, "iterator"), &VorbisCommentMetadata::iterator),
        InstanceMethod("resizeComments", &VorbisCommentMetadata::resizeComments),
        InstanceMethod("setComment", &VorbisCommentMetadata::setComment),
        InstanceMethod("insertComment", &VorbisCommentMetadata::insertComment),
        InstanceMethod("appendComment", &VorbisCommentMetadata::appendComment),
        InstanceMethod("replaceComment", &VorbisCommentMetadata::replaceComment),
        InstanceMethod("deleteComment", &VorbisCommentMetadata::deleteComment),
        InstanceMethod("findEntryFrom", &VorbisCommentMetadata::findEntryFrom),
        InstanceMethod("removeEntryMatching", &VorbisCommentMetadata::removeEntryMatching),
        InstanceMethod("removeEntriesMatching", &VorbisCommentMetadata::removeEntriesMatching),
        InstanceMethod("get", &VorbisCommentMetadata::get),
      });

    addon.vorbisCommentMetadataConstructor = Persistent(constructor);

    return scope.Escape(constructor).As<Function>();
  }

  VorbisCommentMetadata::VorbisCommentMetadata(const CallbackInfo& info):
      ObjectWrap<VorbisCommentMetadata>(info), Metadata(info, FLAC__METADATA_TYPE_VORBIS_COMMENT) {}

  static String entryToString(const Env& env, const FLAC__StreamMetadata_VorbisComment_Entry* e) {
    return String::New(env, (const char*) e->entry, e->length);
  }

  static FLAC__StreamMetadata_VorbisComment_Entry entryFromString(const Value& value) {
    std::string strong = stringFromJs(value);
    FLAC__StreamMetadata_VorbisComment_Entry entry {
      (uint32_t) strong.length(),
      (FLAC__byte*) strdup(strong.c_str()),
    };

    return entry;
  }

  Napi::Value VorbisCommentMetadata::getVendorString(const CallbackInfo& info) {
    return entryToString(info.Env(), &data->data.vorbis_comment.vendor_string);
  }

  void VorbisCommentMetadata::setVendorString(const CallbackInfo& info, const Napi::Value& value) {
    auto entry = entryFromString(value);

    // Tell FLAC API that the string we created is now under its ownership (false)
    FLAC__bool r = FLAC__metadata_object_vorbiscomment_set_vendor_string(data, entry, false);
    if (!r) {
      free(entry.entry);
      Error::New(info.Env(), "Invalid vendor string");
    }
  }

  Napi::Value VorbisCommentMetadata::getCount(const CallbackInfo& info) {
    return numberToJs(info.Env(), data->data.vorbis_comment.num_comments);
  }

  Napi::Value VorbisCommentMetadata::iterator(const CallbackInfo& info) {
    return NativeIterator::newIterator(
      info.Env(),
      [this](auto env, auto pos) -> NativeIterator::IterationReturnValue {
        EscapableHandleScope scope(env);

        if (pos >= data->data.vorbis_comment.num_comments) {
          return {};
        } else {
          auto jsString = entryToString(env, data->data.vorbis_comment.comments + pos);
          return {scope.Escape(jsString)};
        }
      });
  }

  Napi::Value VorbisCommentMetadata::resizeComments(const CallbackInfo& info) {
    auto size = numberFromJs<uint32_t>(info[0]);
    FLAC__bool res = FLAC__metadata_object_vorbiscomment_resize_comments(data, size);
    return booleanToJs(info.Env(), res);
  }

  Napi::Value VorbisCommentMetadata::setComment(const CallbackInfo& info) {
    auto pos = numberFromJs<uint32_t>(info[0]);
    if (data->data.vorbis_comment.num_comments <= pos) {
      throw RangeError::New(info.Env(), "Invalid comment number");
    }

    auto entry = entryFromString(info[1]);
    FLAC__bool r = FLAC__metadata_object_vorbiscomment_set_comment(data, pos, entry, false);
    if (!r) {
      free(entry.entry);
    }
    return booleanToJs(info.Env(), r);
  }

  Napi::Value VorbisCommentMetadata::insertComment(const CallbackInfo& info) {
    auto pos = numberFromJs<uint32_t>(info[0]);
    if (data->data.vorbis_comment.num_comments < pos) {
      throw RangeError::New(info.Env(), "Invalid comment number");
    }

    auto entry = entryFromString(info[1]);
    FLAC__bool r = FLAC__metadata_object_vorbiscomment_insert_comment(data, pos, entry, false);
    if (!r) {
      free(entry.entry);
    }
    return booleanToJs(info.Env(), r);
  }

  Napi::Value VorbisCommentMetadata::appendComment(const CallbackInfo& info) {
    auto entry = entryFromString(info[0]);

    FLAC__bool r = FLAC__metadata_object_vorbiscomment_append_comment(data, entry, false);
    if (!r) {
      free(entry.entry);
    }
    return booleanToJs(info.Env(), r);
  }

  Napi::Value VorbisCommentMetadata::replaceComment(const CallbackInfo& info) {
    auto entry = entryFromString(info[0]);
    auto all = maybeBooleanFromJs<FLAC__bool>(info[1]).value_or(false);

    FLAC__bool r = FLAC__metadata_object_vorbiscomment_replace_comment(data, entry, all, false);
    if (!r) {
      free(entry.entry);
    }
    return booleanToJs(info.Env(), r);
  }

  Napi::Value VorbisCommentMetadata::deleteComment(const CallbackInfo& info) {
    auto pos = numberFromJs<uint32_t>(info[0]);
    if (data->data.vorbis_comment.num_comments <= pos) {
      throw RangeError::New(info.Env(), "Invalid comment number");
    }

    FLAC__bool r = FLAC__metadata_object_vorbiscomment_delete_comment(data, pos);
    return booleanToJs(info.Env(), r);
  }

  Napi::Value VorbisCommentMetadata::findEntryFrom(const CallbackInfo& info) {
    auto offset = numberFromJs<uint32_t>(info[0]);
    auto key = stringFromJs(info[1]);

    int pos = FLAC__metadata_object_vorbiscomment_find_entry_from(data, offset, key.c_str());
    return numberToJs(info.Env(), pos);
  }

  Napi::Value VorbisCommentMetadata::removeEntryMatching(const CallbackInfo& info) {
    auto key = stringFromJs(info[0]);

    int pos = FLAC__metadata_object_vorbiscomment_remove_entry_matching(data, key.c_str());
    return numberToJs(info.Env(), pos);
  }

  Napi::Value VorbisCommentMetadata::removeEntriesMatching(const CallbackInfo& info) {
    auto key = stringFromJs(info[0]);

    int pos = FLAC__metadata_object_vorbiscomment_remove_entries_matching(data, key.c_str());
    return numberToJs(info.Env(), pos);
  }

  Napi::Value VorbisCommentMetadata::get(const CallbackInfo& info) {
    auto key = stringFromJs(info[0]);

    int pos = FLAC__metadata_object_vorbiscomment_find_entry_from(data, 0, key.c_str());
    if (pos == -1) {
      return info.Env().Null();
    } else {
      const char* entry = (const char*) data->data.vorbis_comment.comments[pos].entry;
      const char* equalPos = strchr(entry, '=');
      auto jsString = String::New(info.Env(), equalPos ? equalPos + 1 : entry);
      return jsString;
    }
  }

}
