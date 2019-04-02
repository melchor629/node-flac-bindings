#include "defs.hpp"
#include "mappings.hpp"
#include "../metadata.hpp"

namespace flac_bindings {

    V8_GETTER(CueSheetMetadata::mediaCatalogNumber) {
        unwrap(CueSheetMetadata);
        info.GetReturnValue().Set(Nan::New(self->metadata->data.cue_sheet.media_catalog_number).ToLocalChecked());
    }

    V8_SETTER(CueSheetMetadata::mediaCatalogNumber) {
        unwrap(CueSheetMetadata);
        checkValue(String) {
            Nan::Utf8String str(_newValue.ToLocalChecked());
            if(str.length() >= 129) {
                Nan::ThrowError(Nan::Error("String is too large, max 128 simple characters (128 bytes)"));
            } else {
                strcpy(self->metadata->data.cue_sheet.media_catalog_number, *str);
                memset(self->metadata->data.cue_sheet.media_catalog_number + str.length(), 0, 129 - str.length());
            }
        }
    }

    V8_GETTER(CueSheetMetadata::leadIn) {
        unwrap(CueSheetMetadata);
        info.GetReturnValue().Set(numberToJs(self->metadata->data.cue_sheet.lead_in));
    }

    V8_SETTER(CueSheetMetadata::leadIn) {
        unwrap(CueSheetMetadata);
        auto maybeValue = numberFromJs<uint64_t>(value);
        if(maybeValue.IsJust()) {
            self->metadata->data.cue_sheet.lead_in = maybeValue.FromJust();
        } else {
            Nan::ThrowTypeError("Expected value to be number or bigint");
        }
    }

    V8_GETTER(CueSheetMetadata::isCd) {
        unwrap(CueSheetMetadata);
        info.GetReturnValue().Set(Nan::New<Boolean>(self->metadata->data.cue_sheet.is_cd));
    }

    V8_SETTER(CueSheetMetadata::isCd) {
        unwrap(CueSheetMetadata);
        checkValue(Boolean) {
            self->metadata->data.cue_sheet.is_cd = getValue(bool);
        }
    }

    V8_GETTER(CueSheetMetadata::tracks) {
        unwrap(CueSheetMetadata);
        Local<Array> array = Nan::New<Array>();
        for(uint32_t i = 0; i < self->metadata->data.cue_sheet.num_tracks; i++) {
            Nan::Set(array, i, structToJs(&self->metadata->data.cue_sheet.tracks[i]));
        }
        info.GetReturnValue().Set(array);
    }

    NAN_METHOD(CueSheetMetadata::tracksIterator) {
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
            CueSheetMetadata* self = Nan::ObjectWrap::Unwrap<CueSheetMetadata>(parent.ToLocalChecked().As<Object>());
            Local<Number> jsPos = Nan::Get(info.This(), posKey).ToLocalChecked().As<Number>();
            uint32_t pos = Nan::To<uint32_t>(jsPos).FromJust();
            Local<Object> ret = Nan::New<Object>();
            if(pos >= self->metadata->data.cue_sheet.num_tracks) {
                Nan::Set(ret, Nan::New("done").ToLocalChecked(), Nan::True());
            } else {
                FLAC__StreamMetadata_CueSheet_Track* track = &self->metadata->data.cue_sheet.tracks[pos];
                Nan::Set(ret, Nan::New("value").ToLocalChecked(), structToJs(track));
                Nan::Set(ret, Nan::New("done").ToLocalChecked(), Nan::False());
            }
            Nan::Set(info.This(), posKey, Nan::New<Number>(pos + 1));
            info.GetReturnValue().Set(ret);
        });

        info.GetReturnValue().Set(obj);
    }

    NAN_METHOD(CueSheetMetadata::create) {
        CueSheetMetadata* self = new CueSheetMetadata;
        self->Wrap(info.This());

        if(info.Length() > 0 && Buffer::HasInstance(info[0])) {
            Local<Value> args[] = { info[0], info.Length() > 1 ? info[1] : static_cast<Local<Value>>(Nan::False()) };
            if(Nan::Call(Metadata::getFunction(), info.This(), 2, args).IsEmpty()) return;
        } else {
            Local<Value> args[] = { Nan::New<Number>(FLAC__MetadataType::FLAC__METADATA_TYPE_CUESHEET) };
            if(Nan::Call(Metadata::getFunction(), info.This(), 1, args).IsEmpty()) return;
        }

        nativeProperty(info.This(), "mediaCatalogNumber", mediaCatalogNumber);
        nativeProperty(info.This(), "leadIn", leadIn);
        nativeProperty(info.This(), "isCd", isCd);
        nativeReadOnlyProperty(info.This(), "tracks", tracks);
        info.This()->Set(Symbol::GetIterator(info.GetIsolate()), Nan::GetFunction(Nan::New<FunctionTemplate>(tracksIterator)).ToLocalChecked());

        info.GetReturnValue().Set(info.This());
    }

    NAN_METHOD(CueSheetMetadata::trackResizeIndices) {
        unwrap(CueSheetMetadata);
        MaybeLocal<Number> maybeTrackNum = Nan::To<Number>(info[0]);
        MaybeLocal<Number> maybeNewNumberIndices = Nan::To<Number>(info[1]);
        if(maybeTrackNum.IsEmpty() || !info[0]->IsNumber()) {
            Nan::ThrowTypeError("Expected first argument to be number");
        } else if(maybeNewNumberIndices.IsEmpty() || !info[1]->IsNumber()) {
            Nan::ThrowTypeError("Expected second argument to be number");
        } else {
            uint32_t trackNum = Nan::To<uint32_t>(maybeTrackNum.ToLocalChecked()).FromJust();
            uint32_t newNumberIndices = Nan::To<uint32_t>(maybeNewNumberIndices.ToLocalChecked()).FromJust();
            bool ret = FLAC__metadata_object_cuesheet_track_resize_indices(self->metadata, trackNum, newNumberIndices);
            info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        }
    }

    NAN_METHOD(CueSheetMetadata::trackInsertIndex) {
        unwrap(CueSheetMetadata);
        MaybeLocal<Number> maybeTrackNum = Nan::To<Number>(info[0]);
        MaybeLocal<Number> maybeIndexNum = Nan::To<Number>(info[1]);
        MaybeLocal<Object> maybeTrack = Nan::To<Object>(info[2]);
        if(maybeTrackNum.IsEmpty() || !info[0]->IsNumber()) {
            Nan::ThrowTypeError("Expected first argument to be number");
        } else if(maybeIndexNum.IsEmpty() || !info[1]->IsNumber()) {
            Nan::ThrowTypeError("Expected second argument to be number");
        } else if(maybeTrack.IsEmpty() || !info[2]->IsObject()) {
            Nan::ThrowTypeError("Expected third argument to be CueSheetIndex");
        } else {
            uint32_t trackNum = Nan::To<uint32_t>(maybeTrackNum.ToLocalChecked()).FromJust();
            uint32_t indexNum = Nan::To<uint32_t>(maybeIndexNum.ToLocalChecked()).FromJust();
            CueSheetIndex* index = Nan::ObjectWrap::Unwrap<CueSheetIndex>(maybeTrack.ToLocalChecked());
            bool ret = FLAC__metadata_object_cuesheet_track_insert_index(self->metadata, trackNum, indexNum, index->index);
            info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        }
    }

    NAN_METHOD(CueSheetMetadata::trackInsertBlankIndex) {
        unwrap(CueSheetMetadata);
        MaybeLocal<Number> maybeTrackNum = Nan::To<Number>(info[0]);
        MaybeLocal<Number> maybeIndexNum = Nan::To<Number>(info[1]);
        if(maybeTrackNum.IsEmpty() || !info[0]->IsNumber()) {
            Nan::ThrowTypeError("Expected first argument to be number");
        } else if(maybeIndexNum.IsEmpty() || !info[1]->IsNumber()) {
            Nan::ThrowTypeError("Expected second argument to be number");
        } else {
            uint32_t trackNum = Nan::To<uint32_t>(maybeTrackNum.ToLocalChecked()).FromJust();
            uint32_t indexNum = Nan::To<uint32_t>(maybeIndexNum.ToLocalChecked()).FromJust();
            bool ret = FLAC__metadata_object_cuesheet_track_insert_blank_index(self->metadata, trackNum, indexNum);
            info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        }
    }

    NAN_METHOD(CueSheetMetadata::trackDeleteIndex) {
        unwrap(CueSheetMetadata);
        MaybeLocal<Number> maybeTrackNum = Nan::To<Number>(info[0]);
        MaybeLocal<Number> maybeIndexNum = Nan::To<Number>(info[1]);
        if(maybeTrackNum.IsEmpty() || !info[0]->IsNumber()) {
            Nan::ThrowTypeError("Expected first argument to be number");
        } else if(maybeIndexNum.IsEmpty() || !info[1]->IsNumber()) {
            Nan::ThrowTypeError("Expected second argument to be number");
        } else {
            uint32_t trackNum = Nan::To<uint32_t>(maybeTrackNum.ToLocalChecked()).FromJust();
            uint32_t indexNum = Nan::To<uint32_t>(maybeIndexNum.ToLocalChecked()).FromJust();
            bool ret = FLAC__metadata_object_cuesheet_track_delete_index(self->metadata, trackNum, indexNum);
            info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        }
    }

    NAN_METHOD(CueSheetMetadata::resizeTracks) {
        unwrap(CueSheetMetadata);
        MaybeLocal<Number> maybeNewTrackSize = Nan::To<Number>(info[0]);
        if(maybeNewTrackSize.IsEmpty() || !info[0]->IsNumber()) {
            Nan::ThrowTypeError("Expected first argument to be number");
        } else {
            uint32_t newTrackSize = Nan::To<uint32_t>(maybeNewTrackSize.ToLocalChecked()).FromJust();
            bool ret = FLAC__metadata_object_cuesheet_resize_tracks(self->metadata, newTrackSize);
            info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        }
    }

    NAN_METHOD(CueSheetMetadata::setTrack) {
        unwrap(CueSheetMetadata);
        MaybeLocal<Number> maybeTrackNum = Nan::To<Number>(info[0]);
        MaybeLocal<Object> maybeTrack = Nan::To<Object>(info[1]);
        if(maybeTrackNum.IsEmpty() || !info[0]->IsNumber()) {
            Nan::ThrowTypeError("Expected first argument to be number");
        } else if(maybeTrack.IsEmpty() || !info[1]->IsObject()) {
            Nan::ThrowTypeError("Expected second argument to be CueSheetTrack");
        } else {
            uint32_t trackNum = Nan::To<uint32_t>(maybeTrackNum.ToLocalChecked()).FromJust();
            CueSheetTrack* track = Nan::ObjectWrap::Unwrap<CueSheetTrack>(maybeTrack.ToLocalChecked());
            //In this case, prefer to copy the whole object, just in case
            bool ret = FLAC__metadata_object_cuesheet_set_track(self->metadata, trackNum, track->track, true);
            info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        }
    }

    NAN_METHOD(CueSheetMetadata::insertTrack) {
        unwrap(CueSheetMetadata);
        MaybeLocal<Number> maybeTrackNum = Nan::To<Number>(info[0]);
        MaybeLocal<Object> maybeTrack = Nan::To<Object>(info[1]);
        if(maybeTrackNum.IsEmpty() || !info[0]->IsNumber()) {
            Nan::ThrowTypeError("Expected first argument to be number");
        } else if(maybeTrack.IsEmpty() || !info[1]->IsObject()) {
            Nan::ThrowTypeError("Expected second argument to be CueSheetTrack");
        } else {
            uint32_t trackNum = Nan::To<uint32_t>(maybeTrackNum.ToLocalChecked()).FromJust();
            CueSheetTrack* track = Nan::ObjectWrap::Unwrap<CueSheetTrack>(maybeTrack.ToLocalChecked());
            bool ret = FLAC__metadata_object_cuesheet_insert_track(self->metadata, trackNum, track->track, true);
            info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        }
    }

    NAN_METHOD(CueSheetMetadata::insertBlankTrack) {
        unwrap(CueSheetMetadata);
        MaybeLocal<Number> maybeTrackNum = Nan::To<Number>(info[0]);
        if(maybeTrackNum.IsEmpty() || !info[0]->IsNumber()) {
            Nan::ThrowTypeError("Expected first argument to be number");
        } else {
            uint32_t trackNum = Nan::To<uint32_t>(maybeTrackNum.ToLocalChecked()).FromJust();
            bool ret = FLAC__metadata_object_cuesheet_insert_blank_track(self->metadata, trackNum);
            info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        }
    }

    NAN_METHOD(CueSheetMetadata::deleteTrack) {
        unwrap(CueSheetMetadata);
        MaybeLocal<Number> maybeTrackNum = Nan::To<Number>(info[0]);
        if(maybeTrackNum.IsEmpty() || !info[0]->IsNumber()) {
            Nan::ThrowTypeError("Expected first argument to be number");
        } else {
            uint32_t trackNum = Nan::To<uint32_t>(maybeTrackNum.ToLocalChecked()).FromJust();
            bool ret = FLAC__metadata_object_cuesheet_delete_track(self->metadata, trackNum);
            info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        }
    }

    NAN_METHOD(CueSheetMetadata::isLegal) {
        unwrap(CueSheetMetadata);
        bool check = Nan::To<bool>(info[0]).FromMaybe(false);
        const char* sadness = nullptr;
        bool ret = FLAC__metadata_object_cuesheet_is_legal(self->metadata, check, &sadness);
        if(ret) {
            info.GetReturnValue().Set(Nan::True());
        } else {
            info.GetReturnValue().Set(Nan::New(sadness).ToLocalChecked());
        }
    }

    NAN_METHOD(CueSheetMetadata::calculateCddbId) {
        unwrap(CueSheetMetadata);
        if(!self->metadata->data.cue_sheet.is_cd) {
            Nan::ThrowError("Cannot calculate CDDB ID if CueSheet is not a CD - can lead to undefined behaviour");
        } else {
            uint32_t id = FLAC__metadata_object_cuesheet_calculate_cddb_id(self->metadata);
            info.GetReturnValue().Set(Nan::New<Number>(id));
        }
    }


    Nan::Persistent<Function> CueSheetMetadata::jsFunction;
    NAN_MODULE_INIT(CueSheetMetadata::init) {
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(create);
        tpl->SetClassName(Nan::New("CueSheetMetadata").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);
        tpl->Inherit(Metadata::getProto());

        Nan::SetPrototypeMethod(tpl, "trackResizeIndices", trackResizeIndices);
        Nan::SetPrototypeMethod(tpl, "trackInsertIndex", trackInsertIndex);
        Nan::SetPrototypeMethod(tpl, "trackInsertBlankIndex", trackInsertBlankIndex);
        Nan::SetPrototypeMethod(tpl, "trackDeleteIndex", trackDeleteIndex);
        Nan::SetPrototypeMethod(tpl, "resizeTracks", resizeTracks);
        Nan::SetPrototypeMethod(tpl, "setTrack", setTrack);
        Nan::SetPrototypeMethod(tpl, "insertTrack", insertTrack);
        Nan::SetPrototypeMethod(tpl, "insertBlankTrack", insertBlankTrack);
        Nan::SetPrototypeMethod(tpl, "deleteTrack", deleteTrack);
        Nan::SetPrototypeMethod(tpl, "isLegal", isLegal);
        Nan::SetPrototypeMethod(tpl, "calculateCddbId", calculateCddbId);

        Local<Function> metadata = Nan::GetFunction(tpl).ToLocalChecked();
        jsFunction.Reset(metadata);
        Nan::Set(target, Nan::New("CueSheetMetadata").ToLocalChecked(), metadata);
    }

}
