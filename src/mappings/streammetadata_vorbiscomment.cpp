#include "defs.hpp"
#include "mappings.hpp"
#include "../metadata.hpp"

namespace flac_bindings {

    V8_GETTER(VorbisCommentMetadata::vendorString) {
        unwrap(VorbisCommentMetadata);
        info.GetReturnValue().Set(
            Nan::New(
                (char*) self->metadata->data.vorbis_comment.vendor_string.entry,
                self->metadata->data.vorbis_comment.vendor_string.length
            ).ToLocalChecked()
        );
    }

    V8_GETTER(VorbisCommentMetadata::comments) {
        unwrap(VorbisCommentMetadata);
        Local<Array> array = Nan::New<Array>();
        for(uint32_t i = 0; i < self->metadata->data.vorbis_comment.num_comments; i++) {
            Nan::Set(array, i,
                Nan::New(
                    (char*) self->metadata->data.vorbis_comment.comments[i].entry,
                    self->metadata->data.vorbis_comment.comments[i].length
                ).ToLocalChecked()
            );
        }
        info.GetReturnValue().Set(array);
    }

    NAN_METHOD(VorbisCommentMetadata::commentsIterator) {
        Local<Object> obj = Nan::New<Object>();
        Nan::Set(obj, Nan::New("it").ToLocalChecked(), info.This());
        Nan::Set(obj, Nan::New("pos").ToLocalChecked(), Nan::New<Number>(0));
        Nan::SetMethod(obj, "next", [] (Nan::NAN_METHOD_ARGS_TYPE info) -> void {
            MaybeLocal<Value> parent = Nan::Get(info.This(), Nan::New("it").ToLocalChecked());
            if(parent.IsEmpty() || !parent.ToLocalChecked()->IsObject()) {
                Nan::ThrowTypeError("Unexpected this type for iterator");
                return;
            }

            Local<String> posKey = Nan::New("pos").ToLocalChecked();
            VorbisCommentMetadata* self = Nan::ObjectWrap::Unwrap<VorbisCommentMetadata>(parent.ToLocalChecked().As<Object>());
            Local<Number> jsPos = Nan::Get(info.This(), posKey).ToLocalChecked().As<Number>();
            uint32_t pos = Nan::To<uint32_t>(jsPos).FromJust();
            Local<Object> ret = Nan::New<Object>();
            if(pos >= self->metadata->data.vorbis_comment.num_comments) {
                Nan::Set(ret, Nan::New("done").ToLocalChecked(), Nan::True());
            } else {
                FLAC__StreamMetadata_VorbisComment_Entry entry = self->metadata->data.vorbis_comment.comments[pos];
                Local<String> entryStr = Nan::New((char*) entry.entry, entry.length).ToLocalChecked();
                Nan::Set(ret, Nan::New("value").ToLocalChecked(), entryStr);
                Nan::Set(ret, Nan::New("done").ToLocalChecked(), Nan::False());
            }
            Nan::Set(info.This(), posKey, Nan::New<Number>(pos + 1));
            info.GetReturnValue().Set(ret);
        });

        info.GetReturnValue().Set(obj);
    }

    NAN_METHOD(VorbisCommentMetadata::create) {
        VorbisCommentMetadata* self = new VorbisCommentMetadata;
        self->Wrap(info.This());

        if(info.Length() > 0 && Buffer::HasInstance(info[0])) {
            Local<Value> args[] = { info[0], info.Length() > 1 ? info[1] : static_cast<Local<Value>>(Nan::False()) };
            if(Nan::Call(Metadata::getFunction(), info.This(), 2, args).IsEmpty()) return;
        } else {
            Local<Value> args[] = { Nan::New<Number>(FLAC__MetadataType::FLAC__METADATA_TYPE_VORBIS_COMMENT) };
            if(Nan::Call(Metadata::getFunction(), info.This(), 1, args).IsEmpty()) return;
        }

        nativeProperty(info.This(), "vendorString", vendorString);
        info.This()->Set(Symbol::GetIterator(info.GetIsolate()), Nan::GetFunction(Nan::New<FunctionTemplate>(commentsIterator)).ToLocalChecked());

        info.GetReturnValue().Set(info.This());
    }

    static bool convertStringToEntry(const Local<Value> &obj, FLAC__StreamMetadata_VorbisComment_Entry &entry) {
        if(obj.IsEmpty()) return false;
        if(!obj->IsString()) return false;
        Nan::Utf8String vs(obj);
        entry.entry = (FLAC__byte*) strdup(*vs);
        entry.length = vs.length();
        return true;
    }

    V8_SETTER(VorbisCommentMetadata::vendorString) {
        unwrap(VorbisCommentMetadata);
        FLAC__StreamMetadata_VorbisComment_Entry n;
        if(!convertStringToEntry(value, n)) {
            Nan::ThrowTypeError("Expected first argument to be string");
            return;
        }

        //Tell FLAC API that the string we created is now under its ownership (false)
        FLAC__bool r = FLAC__metadata_object_vorbiscomment_set_vendor_string(self->metadata, n, false);
        if(!r) {
            Nan::ThrowError("Could not allocate memory to modify the vendor string");
        }
    }

    NAN_METHOD(VorbisCommentMetadata::resizeComments) {
        unwrap(VorbisCommentMetadata);
        MaybeLocal<Number> maybeCount = Nan::To<Number>(info[0]);
        if(maybeCount.IsEmpty() || !info[0]->IsNumber()) {
            Nan::ThrowTypeError("Expected first argument to be number");
            return;
        }

        uint32_t count = Nan::To<uint32_t>(maybeCount.ToLocalChecked()).FromJust();
        FLAC__bool r = FLAC__metadata_object_vorbiscomment_resize_comments(self->metadata, count);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(VorbisCommentMetadata::setComment) {
        unwrap(VorbisCommentMetadata);
        MaybeLocal<Number> maybePos = Nan::To<Number>(info[0]);
        if(maybePos.IsEmpty() || !info[0]->IsNumber()) {
            Nan::ThrowTypeError("Expected first argument to be number");
            return;
        }

        FLAC__StreamMetadata_VorbisComment_Entry n;
        if(!convertStringToEntry(info[1], n)) {
            Nan::ThrowTypeError("Expected first argument to be string");
            return;
        }

        uint32_t pos = Nan::To<uint32_t>(maybePos.ToLocalChecked()).FromJust();
        FLAC__bool r = FLAC__metadata_object_vorbiscomment_set_comment(self->metadata, pos, n, false);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(VorbisCommentMetadata::insertComment) {
        unwrap(VorbisCommentMetadata);
        MaybeLocal<Number> maybePos = Nan::To<Number>(info[0]);
        if(maybePos.IsEmpty() || !info[0]->IsNumber()) {
            Nan::ThrowTypeError("Expected first argument to be number");
            return;
        }

        FLAC__StreamMetadata_VorbisComment_Entry n;
        if(!convertStringToEntry(info[1], n)) {
            Nan::ThrowTypeError("Expected first argument to be string");
            return;
        }

        uint32_t pos = Nan::To<uint32_t>(maybePos.ToLocalChecked()).FromJust();
        FLAC__bool r = FLAC__metadata_object_vorbiscomment_insert_comment(self->metadata, pos, n, false);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(VorbisCommentMetadata::appendComment) {
        unwrap(VorbisCommentMetadata);
        FLAC__StreamMetadata_VorbisComment_Entry n;
        if(!convertStringToEntry(info[0], n)) {
            Nan::ThrowTypeError("Expected first argument to be string");
            return;
        }

        FLAC__bool r = FLAC__metadata_object_vorbiscomment_append_comment(self->metadata, n, false);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(VorbisCommentMetadata::replaceComment) {
        unwrap(VorbisCommentMetadata);
        FLAC__StreamMetadata_VorbisComment_Entry n;
        if(!convertStringToEntry(info[0], n)) {
            Nan::ThrowTypeError("Expected first argument to be string");
            return;
        }

        bool all = Nan::To<bool>(info[1]).FromMaybe(false);
        FLAC__bool r = FLAC__metadata_object_vorbiscomment_replace_comment(self->metadata, n, all, false);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(VorbisCommentMetadata::deleteComment) {
        unwrap(VorbisCommentMetadata);
        MaybeLocal<Number> maybePos = Nan::To<Number>(info[0]);
        if(maybePos.IsEmpty() || !info[0]->IsNumber()) {
            Nan::ThrowTypeError("Expected first argument to be number");
            return;
        }

        uint32_t pos = Nan::To<uint32_t>(maybePos.ToLocalChecked()).FromJust();
        FLAC__bool r = FLAC__metadata_object_vorbiscomment_delete_comment(self->metadata, pos);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(VorbisCommentMetadata::findEntryFrom) {
        unwrap(VorbisCommentMetadata);
        MaybeLocal<Number> maybePos = Nan::To<Number>(info[0]);
        if(maybePos.IsEmpty() || !info[0]->IsNumber()) {
            Nan::ThrowTypeError("Expected first argument to be number");
            return;
        }

        MaybeLocal<String> maybeKey = Nan::To<String>(info[1]);
        if(maybeKey.IsEmpty()) {
            Nan::ThrowTypeError("Expected second argument to be string");
            return;
        }

        uint32_t pos = Nan::To<uint32_t>(maybePos.ToLocalChecked()).FromJust();
        Nan::Utf8String key(maybeKey.ToLocalChecked());
        FLAC__bool r = FLAC__metadata_object_vorbiscomment_find_entry_from(self->metadata, pos, *key);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(VorbisCommentMetadata::removeEntryMatching) {
        unwrap(VorbisCommentMetadata);
        MaybeLocal<String> maybeKey = Nan::To<String>(info[0]);
        if(maybeKey.IsEmpty()) {
            Nan::ThrowTypeError("Expected first argument to be string");
            return;
        }

        Nan::Utf8String key(maybeKey.ToLocalChecked());
        FLAC__bool r = FLAC__metadata_object_vorbiscomment_remove_entry_matching(self->metadata, *key);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }

    NAN_METHOD(VorbisCommentMetadata::removeEntriesMatching) {
        unwrap(VorbisCommentMetadata);
        MaybeLocal<String> maybeKey = Nan::To<String>(info[0]);
        if(maybeKey.IsEmpty()) {
            Nan::ThrowTypeError("Expected first argument to be string");
            return;
        }

        Nan::Utf8String key(maybeKey.ToLocalChecked());
        FLAC__bool r = FLAC__metadata_object_vorbiscomment_remove_entries_matching(self->metadata, *key);
        info.GetReturnValue().Set(Nan::New<Boolean>(r));
    }


    Nan::Persistent<Function> VorbisCommentMetadata::jsFunction;
    NAN_MODULE_INIT(VorbisCommentMetadata::init) {
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(create);
        tpl->SetClassName(Nan::New("VorbisCommentMetadata").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);
        tpl->Inherit(Metadata::getProto());

        Nan::SetPrototypeMethod(tpl, "resizeComments", resizeComments);
        Nan::SetPrototypeMethod(tpl, "setComment", setComment);
        Nan::SetPrototypeMethod(tpl, "insertComment", insertComment);
        Nan::SetPrototypeMethod(tpl, "appendComment", appendComment);
        Nan::SetPrototypeMethod(tpl, "replaceComment", replaceComment);
        Nan::SetPrototypeMethod(tpl, "deleteComment", deleteComment);
        Nan::SetPrototypeMethod(tpl, "findEntryFrom", findEntryFrom);
        Nan::SetPrototypeMethod(tpl, "removeEntryMatching", removeEntryMatching);
        Nan::SetPrototypeMethod(tpl, "removeEntriesMatching", removeEntriesMatching);

        Local<Function> metadata = Nan::GetFunction(tpl).ToLocalChecked();
        jsFunction.Reset(metadata);
        Nan::Set(target, Nan::New("VorbisCommentMetadata").ToLocalChecked(), metadata);
    }

}
