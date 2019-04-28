#include "mappings.hpp"

namespace flac_bindings {

    using namespace node;

    V8_GETTER(CueSheetIndex::offset) {
        unwrap(CueSheetIndex);
        info.GetReturnValue().Set(numberToJs(self->index.offset));
    }

    V8_SETTER(CueSheetIndex::offset) {
        unwrap(CueSheetIndex);
        checkValueIsNumber(uint64_t) {
            self->index.offset = newValue;
        }
    }

    V8_GETTER(CueSheetIndex::number) {
        unwrap(CueSheetIndex);
        info.GetReturnValue().Set(numberToJs(self->index.number));
    }

    V8_SETTER(CueSheetIndex::number) {
        unwrap(CueSheetIndex);
        checkValueIsNumber(uint32_t) {
            self->index.number = newValue & 0xFF;
        }
    }

    NAN_METHOD(CueSheetIndex::create) {
        if(throwIfNotConstructorCall(info)) return;
        CueSheetIndex* cueSheetIndex = new CueSheetIndex;
        cueSheetIndex->Wrap(info.This());

        if(!info[0].IsEmpty() && info[0]->IsObject() && Buffer::HasInstance(info[0].As<Object>())) {
            memcpy(
                &cueSheetIndex->index,
                UnwrapPointer<FLAC__StreamMetadata_CueSheet_Index>(info[0]),
                sizeof(FLAC__StreamMetadata_CueSheet_Index)
            );
        } else {
            memset(&cueSheetIndex->index, 0, sizeof(FLAC__StreamMetadata_CueSheet_Index));
            if(!info[0].IsEmpty() && !info[0]->IsUndefined()) {
                auto maybeOffset = numberFromJs<uint64_t>(info[0]);
                if(maybeOffset.IsJust()) {
                    cueSheetIndex->index.offset = maybeOffset.FromJust();
                } else {
                    Nan::ThrowTypeError("First argument must be undefined, number or bigint");
                    return;
                }
            }

            if(!info[1].IsEmpty() && !info[1]->IsUndefined()) {
                auto maybeNumber = numberFromJs<uint32_t>(info[1]);
                if(maybeNumber.IsJust()) {
                    cueSheetIndex->index.number = maybeNumber.FromJust() & 0xFF;
                } else if(!info[1]->IsUndefined()) {
                    Nan::ThrowTypeError("Second argument must be undefined, number or bigint");
                    return;
                }
            }
        }

        nativeProperty(info.This(), "offset", offset);
        nativeProperty(info.This(), "number", number);

        info.GetReturnValue().Set(info.This());
    }

    Nan::Persistent<Function> CueSheetIndex::jsFunction;
    NAN_MODULE_INIT(CueSheetIndex::init) {
        Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(create);
        tpl->SetClassName(Nan::New("CueSheetIndex").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        Local<Function> metadata = Nan::GetFunction(tpl).ToLocalChecked();
        jsFunction.Reset(metadata);
        Nan::Set(target, Nan::New("CueSheetIndex").ToLocalChecked(), metadata);
    }

    template<>
    Local<Object> structToJs(const FLAC__StreamMetadata_CueSheet_Index* index, bool deleteHint) {
        Local<Value> args[] = { WrapPointer(index).ToLocalChecked() };
        auto metadata = Nan::NewInstance(CueSheetIndex::getFunction(), 1, args);
        return metadata.ToLocalChecked();
    }

}
