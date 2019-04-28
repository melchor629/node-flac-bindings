#ifndef MAPPINGS_STREAMMETADATA_HPP
#define MAPPINGS_STREAMMETADATA_HPP

#include "../utils/defs.hpp"

namespace flac_bindings {

    using namespace v8;

    template<typename T>
    static void markDeleteObjectTo(Local<Value> m, bool hintToDelete);

    template<typename T>
    class WrappedObject: public Nan::ObjectWrap {
        template<typename TT>
        friend void markDeleteObjectTo(Local<Value> m, bool hintToDelete);
    protected:
        bool hintToDelete = false;
    public:
        virtual T* get() = 0;
    };

    template<typename T>
    Local<Object> structToJs(const T* i, bool deleteHint = false);

    template<typename T>
    static T* jsToStruct(const Local<Value> &m) {
        Nan::HandleScope scope;
        MaybeLocal<Object> maybeObj = Nan::To<Object>(m);
        if(maybeObj.IsEmpty()) { Nan::ThrowError("Expected type to be object"); return nullptr; }
        Local<Object> obj = maybeObj.ToLocalChecked();
        if(obj->InternalFieldCount() == 0) { Nan::ThrowError("Object does not seem to be valid"); return nullptr; }
        return Nan::ObjectWrap::Unwrap<WrappedObject<T>>(obj)->get();
    }

    template<typename T>
    static void markDeleteObjectTo(Local<Value> m, bool hintToDelete) {
        Nan::HandleScope scope;
        MaybeLocal<Object> maybeObj = Nan::To<Object>(m);
        if(maybeObj.IsEmpty()) return;
        Local<Object> obj = maybeObj.ToLocalChecked();
        if(obj->InternalFieldCount() == 0) return;
        Nan::ObjectWrap::Unwrap<WrappedObject<T>>(obj)->hintToDelete = hintToDelete;
    }


    class Metadata: public WrappedObject<FLAC__StreamMetadata> {
        static V8_GETTER(type);
        static V8_GETTER(isLast);
        static V8_GETTER(length);
        static NAN_METHOD(create);
        static NAN_METHOD(clone);
        static NAN_METHOD(isEqual);
        static Nan::Persistent<Function> metadataJs;
        static Nan::Persistent<FunctionTemplate> metadataProtoJs;

    public:
        static NAN_MODULE_INIT(init);
        static inline Local<FunctionTemplate> getProto(Isolate* isolate = Isolate::GetCurrent()) { return metadataProtoJs.Get(isolate); }
        static inline Local<Function> getFunction(Isolate* isolate = Isolate::GetCurrent()) { return metadataJs.Get(isolate); }

        FLAC__StreamMetadata* metadata = nullptr;
        virtual inline FLAC__StreamMetadata* get() { return metadata; }

        ~Metadata();
    };


    class StreamInfoMetadata: public Metadata {
        static V8_GETTER(minBlocksize);
        static V8_SETTER(minBlocksize);
        static V8_GETTER(maxBlocksize);
        static V8_SETTER(maxBlocksize);
        static V8_GETTER(minFramesize);
        static V8_SETTER(minFramesize);
        static V8_GETTER(maxFramesize);
        static V8_SETTER(maxFramesize);
        static V8_GETTER(channels);
        static V8_SETTER(channels);
        static V8_GETTER(bitsPerSample);
        static V8_SETTER(bitsPerSample);
        static V8_GETTER(sampleRate);
        static V8_SETTER(sampleRate);
        static V8_GETTER(totalSamples);
        static V8_SETTER(totalSamples);
        static V8_GETTER(md5sum);
        static V8_SETTER(md5sum);
        static NAN_METHOD(create);
        static Nan::Persistent<Function> jsFunction;

    public:
        static inline Local<Function> getFunction(Isolate* isolate = Isolate::GetCurrent()) { return jsFunction.Get(isolate); }
        static NAN_MODULE_INIT(init);
    };


    class PaddingMetadata: public Metadata {
        static NAN_METHOD(create);
        static Nan::Persistent<Function> paddingMetadataJs;

    public:
        static inline Local<Function> getFunction(Isolate* isolate = Isolate::GetCurrent()) { return paddingMetadataJs.Get(isolate); }
        static NAN_MODULE_INIT(init);
    };


    class ApplicationMetadata: public Metadata {
        static V8_GETTER(id);
        static V8_SETTER(id);
        static V8_GETTER(data);
        static V8_SETTER(data);
        static NAN_METHOD(create);
        static Nan::Persistent<Function> applicationMetadataJs;

    public:
        static inline Local<Function> getFunction(Isolate* isolate = Isolate::GetCurrent()) { return applicationMetadataJs.Get(isolate); }
        static NAN_MODULE_INIT(init);
    };


    class SeekTableMetadata: public Metadata {
        static V8_GETTER(points);
        static NAN_METHOD(pointsIterator);
        static NAN_METHOD(create);
        static NAN_METHOD(resizePoints);
        static NAN_METHOD(setPoint);
        static NAN_METHOD(insertPoint);
        static NAN_METHOD(deletePoint);
        static NAN_METHOD(isLegal);
        static NAN_METHOD(templateAppendPlaceholders);
        static NAN_METHOD(templateAppendPoint);
        static NAN_METHOD(templateAppendPoints);
        static NAN_METHOD(templateAppendSpacedPoints);
        static NAN_METHOD(templateAppendSpacedPointsBySamples);
        static NAN_METHOD(templateSort);
        static Nan::Persistent<Function> jsFunction;

    public:
        static inline Local<Function> getFunction(Isolate* isolate = Isolate::GetCurrent()) { return jsFunction.Get(isolate); }
        static NAN_MODULE_INIT(init);
    };


    class SeekPoint: public WrappedObject<FLAC__StreamMetadata_SeekPoint> {
        static V8_GETTER(sampleNumber);
        static V8_SETTER(sampleNumber);
        static V8_GETTER(streamOffset);
        static V8_SETTER(streamOffset);
        static V8_GETTER(frameSamples);
        static V8_SETTER(frameSamples);
        static NAN_METHOD(create);
        static Nan::Persistent<Function> jsFunction;

    public:
        static inline Local<Function> getFunction(Isolate* isolate = Isolate::GetCurrent()) { return jsFunction.Get(isolate); }
        static NAN_MODULE_INIT(init);

        FLAC__StreamMetadata_SeekPoint point;
        virtual inline FLAC__StreamMetadata_SeekPoint* get() { return &point; }
    };


    class VorbisCommentMetadata: public Metadata {
        static V8_GETTER(vendorString);
        static V8_GETTER(comments);
        static NAN_METHOD(commentsIterator);
        static NAN_METHOD(create);
        static V8_SETTER(vendorString);
        static NAN_METHOD(resizeComments);
        static NAN_METHOD(setComment);
        static NAN_METHOD(insertComment);
        static NAN_METHOD(appendComment);
        static NAN_METHOD(replaceComment);
        static NAN_METHOD(deleteComment);
        static NAN_METHOD(findEntryFrom);
        static NAN_METHOD(removeEntryMatching);
        static NAN_METHOD(removeEntriesMatching);
        static NAN_METHOD(get);
        static Nan::Persistent<Function> jsFunction;

    public:
        static inline Local<Function> getFunction(Isolate* isolate = Isolate::GetCurrent()) { return jsFunction.Get(isolate); }
        static NAN_MODULE_INIT(init);
    };


    class CueSheetMetadata: public Metadata {
        static V8_GETTER(mediaCatalogNumber);
        static V8_SETTER(mediaCatalogNumber);
        static V8_GETTER(leadIn);
        static V8_SETTER(leadIn);
        static V8_GETTER(isCd);
        static V8_SETTER(isCd);
        static V8_GETTER(tracks);
        static NAN_METHOD(tracksIterator);
        static NAN_METHOD(create);
        static NAN_METHOD(trackResizeIndices);
        static NAN_METHOD(trackInsertIndex);
        static NAN_METHOD(trackInsertBlankIndex);
        static NAN_METHOD(trackDeleteIndex);
        static NAN_METHOD(resizeTracks);
        static NAN_METHOD(setTrack);
        static NAN_METHOD(insertTrack);
        static NAN_METHOD(insertBlankTrack);
        static NAN_METHOD(deleteTrack);
        static NAN_METHOD(isLegal);
        static NAN_METHOD(calculateCddbId);
        static Nan::Persistent<Function> jsFunction;

    public:
        static inline Local<Function> getFunction(Isolate* isolate = Isolate::GetCurrent()) { return jsFunction.Get(isolate); }
        static NAN_MODULE_INIT(init);
    };


    class CueSheetIndex: public WrappedObject<FLAC__StreamMetadata_CueSheet_Index> {
        static V8_GETTER(offset);
        static V8_SETTER(offset);
        static V8_GETTER(number);
        static V8_SETTER(number);
        static NAN_METHOD(create);
        static Nan::Persistent<Function> jsFunction;

    public:
        static inline Local<Function> getFunction(Isolate* isolate = Isolate::GetCurrent()) { return jsFunction.Get(isolate); }
        static NAN_MODULE_INIT(init);

        FLAC__StreamMetadata_CueSheet_Index index;
        virtual inline FLAC__StreamMetadata_CueSheet_Index* get() { return &index; }
    };


    class CueSheetTrack: public WrappedObject<FLAC__StreamMetadata_CueSheet_Track> {
        static V8_GETTER(offset);
        static V8_SETTER(offset);
        static V8_GETTER(number);
        static V8_SETTER(number);
        static V8_GETTER(isrc);
        static V8_SETTER(isrc);
        static V8_GETTER(type);
        static V8_SETTER(type);
        static V8_GETTER(preEmphasis);
        static V8_SETTER(preEmphasis);
        static V8_GETTER(indices);
        static NAN_METHOD(indicesIterator);
        static NAN_METHOD(clone);
        static NAN_METHOD(create);
        static Nan::Persistent<Function> jsFunction;

    public:
        static inline Local<Function> getFunction(Isolate* isolate = Isolate::GetCurrent()) { return jsFunction.Get(isolate); }
        static NAN_MODULE_INIT(init);

        FLAC__StreamMetadata_CueSheet_Track* track = nullptr;
        virtual inline FLAC__StreamMetadata_CueSheet_Track* get() { return track; }
        ~CueSheetTrack();
    };


    class PictureMetadata: public Metadata {
        static V8_GETTER(pictureType);
        static V8_SETTER(pictureType);
        static V8_GETTER(mimeType);
        static V8_SETTER(mimeType);
        static V8_GETTER(description);
        static V8_SETTER(description);
        static V8_GETTER(width);
        static V8_SETTER(width);
        static V8_GETTER(height);
        static V8_SETTER(height);
        static V8_GETTER(depth);
        static V8_SETTER(depth);
        static V8_GETTER(colors);
        static V8_SETTER(colors);
        static V8_GETTER(data);
        static V8_SETTER(data);
        static NAN_METHOD(create);
        static NAN_METHOD(isLegal);
        static Nan::Persistent<Function> jsFunction;

    public:
        static inline Local<Function> getFunction(Isolate* isolate = Isolate::GetCurrent()) { return jsFunction.Get(isolate); }
        static NAN_MODULE_INIT(init);
    };


    class UnknownMetadata: public Metadata {
        static V8_GETTER(data);
        static NAN_METHOD(create);
        static Nan::Persistent<Function> jsFunction;

    public:
        static inline Local<Function> getFunction(Isolate* isolate = Isolate::GetCurrent()) { return jsFunction.Get(isolate); }
        static NAN_MODULE_INIT(init);
    };
}

#endif //MAPPINGS_STREAMMETADATA_HPP