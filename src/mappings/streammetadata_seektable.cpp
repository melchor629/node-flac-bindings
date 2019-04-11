#include "mappings.hpp"
#include "../metadata.hpp"

namespace flac_bindings {

    V8_GETTER(SeekTableMetadata::points) {
        unwrap(SeekTableMetadata);
        Local<Array> arr = Nan::New<Array>();
        for(uint32_t i = 0; i < self->metadata->data.seek_table.num_points; i++) {
            Nan::Set(arr, i, structToJs(&self->metadata->data.seek_table.points[i]));
        }
        info.GetReturnValue().Set(arr);
    }

    NAN_METHOD(SeekTableMetadata::pointsIterator) {
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
            SeekTableMetadata* self = Nan::ObjectWrap::Unwrap<SeekTableMetadata>(parent.ToLocalChecked().As<Object>());
            Local<Value> jsPos = Nan::Get(info.This(), posKey).ToLocalChecked();
            uint32_t pos = numberFromJs<uint32_t>(jsPos).FromJust();
            Local<Object> ret = Nan::New<Object>();
            if(pos >= self->metadata->data.seek_table.num_points) {
                Nan::Set(ret, Nan::New("done").ToLocalChecked(), Nan::True());
            } else {
                FLAC__StreamMetadata_SeekPoint point = self->metadata->data.seek_table.points[pos];
                Nan::Set(ret, Nan::New("value").ToLocalChecked(), structToJs(&point));
                Nan::Set(ret, Nan::New("done").ToLocalChecked(), Nan::False());
            }
            Nan::Set(info.This(), posKey, Nan::New<Number>(pos + 1));
            info.GetReturnValue().Set(ret);
        });

        info.GetReturnValue().Set(obj);
    }

    NAN_METHOD(SeekTableMetadata::create) {
        SeekTableMetadata* self = new SeekTableMetadata;
        self->Wrap(info.This());

        if(info.Length() > 0 && Buffer::HasInstance(info[0])) {
            Local<Value> args[] = { info[0], info.Length() > 1 ? info[1] : static_cast<Local<Value>>(Nan::False()) };
            if(Nan::Call(Metadata::getFunction(), info.This(), 2, args).IsEmpty()) return;
        } else {
            Local<Value> args[] = { numberToJs<int>(FLAC__MetadataType::FLAC__METADATA_TYPE_SEEKTABLE) };
            if(Nan::Call(Metadata::getFunction(), info.This(), 1, args).IsEmpty()) return;
        }

        nativeReadOnlyProperty(info.This(), "points", points);
        info.This()->Set(Symbol::GetIterator(info.GetIsolate()), Nan::GetFunction(Nan::New<FunctionTemplate>(pointsIterator)).ToLocalChecked());

        info.GetReturnValue().Set(info.This());
    }

    NAN_METHOD(SeekTableMetadata::resizePoints) {
        unwrap(SeekTableMetadata);
        auto maybeNum = numberFromJs<unsigned>(info[0]);
        if(maybeNum.IsNothing()) {
            Nan::ThrowTypeError("Expected first argument to be number");
        } else {
            unsigned num = maybeNum.FromJust();
            bool ret = FLAC__metadata_object_seektable_resize_points(self->metadata, num);
            info.GetReturnValue().Set(Nan::New<Boolean>(ret));
        }
    }

    NAN_METHOD(SeekTableMetadata::setPoint) {
        unwrap(SeekTableMetadata);
        auto maybeNum = numberFromJs<unsigned>(info[0]);
        if(maybeNum.IsNothing()) {
            Nan::ThrowTypeError("Expected first argument to be number");
        } else if(info[1].IsEmpty() || !info[1]->IsObject()) {
            Nan::ThrowTypeError("Expected second argument to be a SeekTableMetadata");
        } else {
            unsigned pointNum = maybeNum.FromJust();
            assertThrowing(self->metadata->data.seek_table.num_points > pointNum, "Point position is invalid");
            SeekPoint* seekPoint = Nan::ObjectWrap::Unwrap<SeekPoint>(Nan::To<Object>(info[1]).ToLocalChecked());
            FLAC__metadata_object_seektable_set_point(self->metadata, pointNum, seekPoint->point);
        }
    }

    NAN_METHOD(SeekTableMetadata::insertPoint) {
        unwrap(SeekTableMetadata);
        auto maybeNum = numberFromJs<unsigned>(info[0]);
        if(maybeNum.IsNothing()) {
            Nan::ThrowTypeError("Expected first argument to be number");
        } else if(info[1].IsEmpty() || !info[1]->IsObject()) {
            Nan::ThrowTypeError("Expected second argument to be a SeekTableMetadata");
        } else {
            unsigned pointNum = maybeNum.FromJust();
            assertThrowing(self->metadata->data.seek_table.num_points >= pointNum, "Point position is invalid");
            SeekPoint* seekPoint = Nan::ObjectWrap::Unwrap<SeekPoint>(Nan::To<Object>(info[1]).ToLocalChecked());
            bool r = FLAC__metadata_object_seektable_insert_point(self->metadata, pointNum, seekPoint->point);
            info.GetReturnValue().Set(Nan::New<Boolean>(r));
        }
    }

    NAN_METHOD(SeekTableMetadata::deletePoint) {
        unwrap(SeekTableMetadata);
        auto maybeNum = numberFromJs<unsigned>(info[0]);
        if(maybeNum.IsNothing()) {
            Nan::ThrowTypeError("Expected first argument to be number");
        } else {
            unsigned pointNum = maybeNum.FromJust();
            assertThrowing(self->metadata->data.seek_table.num_points > pointNum, "Point position is invalid");
            bool res = FLAC__metadata_object_seektable_delete_point(self->metadata, pointNum);
            info.GetReturnValue().Set(Nan::New<Boolean>(res));
        }
    }

    NAN_METHOD(SeekTableMetadata::isLegal) {
        unwrap(SeekTableMetadata);
        bool res = FLAC__metadata_object_seektable_is_legal(self->metadata);
        info.GetReturnValue().Set(Nan::New<Boolean>(res));
    }

    NAN_METHOD(SeekTableMetadata::templateAppendPlaceholders) {
        unwrap(SeekTableMetadata);
        auto maybeNum = numberFromJs<unsigned>(info[0]);
        if(maybeNum.IsNothing()) {
            Nan::ThrowTypeError("Expected first argument to be number");
        } else {
            unsigned num = maybeNum.FromJust();
            bool res = FLAC__metadata_object_seektable_template_append_placeholders(self->metadata, num);
            info.GetReturnValue().Set(Nan::New<Boolean>(res));
        }
    }

    NAN_METHOD(SeekTableMetadata::templateAppendPoint) {
        unwrap(SeekTableMetadata);
        Nan::Maybe<uint64_t> maybeNum = numberFromJs<uint64_t>(info[0]);
        if(info[0].IsEmpty() || maybeNum.IsNothing()) {
            Nan::ThrowTypeError("Expected first argument to be number or BigInt");
        } else {
            uint64_t num = maybeNum.FromJust();
            bool res = FLAC__metadata_object_seektable_template_append_point(self->metadata, num);
            info.GetReturnValue().Set(Nan::New<Boolean>(res));
        }
    }

    NAN_METHOD(SeekTableMetadata::templateAppendPoints) {
        unwrap(SeekTableMetadata);
        if(info[0].IsEmpty() || !info[0]->IsArray()) {
            Nan::ThrowTypeError("Expected first argument to be an Array");
        } else {
            Local<Array> points = info[0].As<Array>();
            uint64_t* nums = new uint64_t[points->Length()];
            for(size_t i = 0; i < points->Length(); i++) {
                Nan::Maybe<uint64_t> maybeNum = numberFromJs<uint64_t>(Nan::Get(points, i));
                if(maybeNum.IsNothing()) {
                    std::string errorStr = "Element at position " + std::to_string(i) + " is not a number or BigInt";
                    Nan::ThrowTypeError(errorStr.c_str());
                    return;
                }
                nums[i] = maybeNum.FromJust();
            }
            bool res = FLAC__metadata_object_seektable_template_append_points(self->metadata, nums, points->Length());
            info.GetReturnValue().Set(Nan::New<Boolean>(res));
            delete[] nums;
        }
    }

    NAN_METHOD(SeekTableMetadata::templateAppendSpacedPoints) {
        unwrap(SeekTableMetadata);
        Nan::Maybe<unsigned> maybeNum = numberFromJs<unsigned>(info[0]);
        Nan::Maybe<uint64_t> maybeSamples = numberFromJs<uint64_t>(info[1]);
        if(info[0].IsEmpty() || maybeNum.IsNothing()) {
            Nan::ThrowTypeError("Expected first argument to be number");
        } else if(info[1].IsEmpty() || maybeSamples.IsNothing()) {
            Nan::ThrowTypeError("Expected second argument to be number or BigInt");
        } else {
            unsigned num = maybeNum.FromJust();
            uint64_t samples = maybeSamples.FromJust();
            assertThrowing(samples > 0, "Total samples is 0");
            bool res = FLAC__metadata_object_seektable_template_append_spaced_points(self->metadata, num, samples);
            info.GetReturnValue().Set(Nan::New<Boolean>(res));
        }
    }

    NAN_METHOD(SeekTableMetadata::templateAppendSpacedPointsBySamples) {
        unwrap(SeekTableMetadata);
        Nan::Maybe<unsigned> maybeNum = numberFromJs<unsigned>(info[0]);
        Nan::Maybe<uint64_t> maybeSamples = numberFromJs<uint64_t>(info[1]);
        if(info[0].IsEmpty() || maybeNum.IsNothing()) {
            Nan::ThrowTypeError("Expected first argument to be number");
        } else if(info[1].IsEmpty() || maybeSamples.IsNothing()) {
            Nan::ThrowTypeError("Expected second argument to be number or BigInt");
        } else {
            unsigned samples = maybeNum.FromJust();
            uint64_t totalSamples = maybeSamples.FromJust();
            assertThrowing(samples > 0, "samples is 0");
            assertThrowing(totalSamples > 0, "totalSamples is 0");
            bool res = FLAC__metadata_object_seektable_template_append_spaced_points_by_samples(self->metadata, samples, totalSamples);
            info.GetReturnValue().Set(Nan::New<Boolean>(res));
        }
    }

    NAN_METHOD(SeekTableMetadata::templateSort) {
        unwrap(SeekTableMetadata);
        bool compact = Nan::To<bool>(info[0]).FromMaybe(false);
        bool res = FLAC__metadata_object_seektable_template_sort(self->metadata, compact);
        info.GetReturnValue().Set(Nan::New<Boolean>(res));
    }

    Nan::Persistent<Function> SeekTableMetadata::jsFunction;
    NAN_MODULE_INIT(SeekTableMetadata::init) {
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(create);
        tpl->SetClassName(Nan::New("SeekTableMetadata").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);
        tpl->Inherit(Metadata::getProto());

        Nan::SetPrototypeMethod(tpl, "resizePoints", resizePoints);
        Nan::SetPrototypeMethod(tpl, "setPoint", setPoint);
        Nan::SetPrototypeMethod(tpl, "insertPoint", insertPoint);
        Nan::SetPrototypeMethod(tpl, "deletePoint", deletePoint);
        Nan::SetPrototypeMethod(tpl, "isLegal", isLegal);
        Nan::SetPrototypeMethod(tpl, "templateAppendPlaceholders", templateAppendPlaceholders);
        Nan::SetPrototypeMethod(tpl, "templateAppendPoint", templateAppendPoint);
        Nan::SetPrototypeMethod(tpl, "templateAppendPoints", templateAppendPoints);
        Nan::SetPrototypeMethod(tpl, "templateAppendSpacedPoints", templateAppendSpacedPoints);
        Nan::SetPrototypeMethod(tpl, "templateAppendSpacedPointsBySamples", templateAppendSpacedPointsBySamples);
        Nan::SetPrototypeMethod(tpl, "templateSort", templateSort);

        Local<Function> metadata = Nan::GetFunction(tpl).ToLocalChecked();
        jsFunction.Reset(metadata);
        Nan::Set(target, Nan::New("SeekTableMetadata").ToLocalChecked(), metadata);
    }

}
