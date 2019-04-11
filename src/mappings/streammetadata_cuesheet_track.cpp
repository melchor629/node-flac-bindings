#include "mappings.hpp"
#include "../metadata.hpp"

namespace flac_bindings {

    V8_GETTER(CueSheetTrack::offset) {
        unwrap(CueSheetTrack);
        info.GetReturnValue().Set(numberToJs(self->track->offset));
    }

    V8_SETTER(CueSheetTrack::offset) {
        unwrap(CueSheetTrack);
        checkValueIsNumber(uint64_t) {
            self->track->offset = newValue;
        }
    }

    V8_GETTER(CueSheetTrack::number) {
        unwrap(CueSheetTrack);
        info.GetReturnValue().Set(numberToJs(self->track->number));
    }

    V8_SETTER(CueSheetTrack::number) {
        unwrap(CueSheetTrack);
        checkValueIsNumber(uint32_t) {
            self->track->number = newValue & 0xFF;
        }
    }

    V8_GETTER(CueSheetTrack::isrc) {
        unwrap(CueSheetTrack);
        info.GetReturnValue().Set(Nan::New(self->track->isrc).ToLocalChecked());
    }

    V8_SETTER(CueSheetTrack::isrc) {
        unwrap(CueSheetTrack);
        checkValue(String) {
            Nan::Utf8String isrc(_newValue.ToLocalChecked());
            if(isrc.length() == 12) {
                strcpy(self->track->isrc, *isrc);
            } else {
                Nan::ThrowError(Nan::Error("ISRC string must be 12 bytes length"));
            }
        }
    }

    V8_GETTER(CueSheetTrack::type) {
        unwrap(CueSheetTrack);
        info.GetReturnValue().Set(numberToJs(self->track->type));
    }

    V8_SETTER(CueSheetTrack::type) {
        unwrap(CueSheetTrack);
        checkValueIsNumber(uint32_t) {
            self->track->type = newValue & 1;
        }
    }

    V8_GETTER(CueSheetTrack::preEmphasis) {
        unwrap(CueSheetTrack);
        info.GetReturnValue().Set(Nan::New<Boolean>(self->track->pre_emphasis));
    }

    V8_SETTER(CueSheetTrack::preEmphasis) {
        unwrap(CueSheetTrack);
        checkValue(Boolean) {
            self->track->pre_emphasis = getValue(bool);
        }
    }

    V8_GETTER(CueSheetTrack::indices) {
        unwrap(CueSheetTrack);
        Local<Array> array = Nan::New<Array>();
        for(int i = 0; i < self->track->num_indices; i++) {
            Nan::Set(array, i, structToJs(&self->track->indices[i]));
        }
        info.GetReturnValue().Set(array);
    }

    NAN_METHOD(CueSheetTrack::indicesIterator) {
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
            CueSheetTrack* self = Nan::ObjectWrap::Unwrap<CueSheetTrack>(parent.ToLocalChecked().As<Object>());
            Local<Value> jsPos = Nan::Get(info.This(), posKey).ToLocalChecked();
            uint32_t pos = numberFromJs<uint32_t>(jsPos).FromJust();
            Local<Object> ret = Nan::New<Object>();
            if(pos >= self->track->num_indices) {
                Nan::Set(ret, Nan::New("done").ToLocalChecked(), Nan::True());
            } else {
                FLAC__StreamMetadata_CueSheet_Index* index = &self->track->indices[pos];
                Nan::Set(ret, Nan::New("value").ToLocalChecked(), structToJs(index));
                Nan::Set(ret, Nan::New("done").ToLocalChecked(), Nan::False());
            }
            Nan::Set(info.This(), posKey, Nan::New<Number>(pos + 1));
            info.GetReturnValue().Set(ret);
        });

        info.GetReturnValue().Set(obj);
    }

    NAN_METHOD(CueSheetTrack::clone) {
        unwrap(CueSheetTrack);
        FLAC__StreamMetadata_CueSheet_Track* newTrack = FLAC__metadata_object_cuesheet_track_clone(self->track);
        Local<Value> args[] = { WrapPointer(newTrack).ToLocalChecked(), Nan::True() };
        auto metadata = Nan::NewInstance(CueSheetTrack::getFunction(), 2, args);
        info.GetReturnValue().Set(metadata.ToLocalChecked());
    }

    NAN_METHOD(CueSheetTrack::create) {
        if(throwIfNotConstructorCall(info)) return;
        CueSheetTrack* cueSheetTrack = new CueSheetTrack;
        cueSheetTrack->Wrap(info.This());

        if(!info[0].IsEmpty() && info[0]->IsObject() && Buffer::HasInstance(info[0].As<Object>())) {
            cueSheetTrack->track = UnwrapPointer<FLAC__StreamMetadata_CueSheet_Track>(info[0]);
            cueSheetTrack->mustBeDeleted = Nan::To<bool>(info[1]).FromMaybe(false);
        } else {
            cueSheetTrack->track = FLAC__metadata_object_cuesheet_track_new();
            cueSheetTrack->mustBeDeleted = true;
        }

        nativeProperty(info.This(), "offset", offset);
        nativeProperty(info.This(), "number", number);
        nativeProperty(info.This(), "isrc", isrc);
        nativeProperty(info.This(), "type", type);
        nativeProperty(info.This(), "preEmphasis", preEmphasis);
        nativeReadOnlyProperty(info.This(), "indices", indices);
        info.This()->Set(Symbol::GetIterator(info.GetIsolate()), Nan::GetFunction(Nan::New<FunctionTemplate>(indicesIterator)).ToLocalChecked());

        info.GetReturnValue().Set(info.This());
    }

    Nan::Persistent<Function> CueSheetTrack::jsFunction;
    NAN_MODULE_INIT(CueSheetTrack::init) {
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(create);
        tpl->SetClassName(Nan::New("CueSheetTrack").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        Nan::SetPrototypeMethod(tpl, "clone", clone);

        Local<Function> metadata = Nan::GetFunction(tpl).ToLocalChecked();
        jsFunction.Reset(metadata);
        Nan::Set(target, Nan::New("CueSheetTrack").ToLocalChecked(), metadata);
    }

    CueSheetTrack::~CueSheetTrack() {
        if(mustBeDeleted && track != nullptr) {
            FLAC__metadata_object_cuesheet_track_delete(track);
        }
    }

    template<>
    Local<Object> structToJs(const FLAC__StreamMetadata_CueSheet_Track* track) {
        Local<Value> args[] = { WrapPointer(track).ToLocalChecked(), Nan::False() };
        auto metadata = Nan::NewInstance(CueSheetTrack::getFunction(), 2, args);
        return metadata.ToLocalChecked();
    }

}
