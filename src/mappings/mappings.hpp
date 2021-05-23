#pragma once

#include "../utils/converters.hpp"
#include "mapping.hpp"
#include <FLAC/format.h>

namespace flac_bindings {

  using namespace Napi;

  Object frameToJs(const Env&, const FLAC__Frame*);

  class FlacAddon;

  class Metadata: public Mapping<FLAC__StreamMetadata> {
  public:
    Metadata(const CallbackInfo&, FLAC__MetadataType type);

    virtual ~Metadata();

    static Value getType(const CallbackInfo&);
    static Value getIsLast(const CallbackInfo&);
    static Value getLength(const CallbackInfo&);
    static Value clone(const CallbackInfo&);
    static Value isEqual(const CallbackInfo&);
  };

  class StreamInfoMetadata: public ObjectWrap<StreamInfoMetadata>, public Metadata {
    pointer::BufferReference<FLAC__byte> md5SumBuffer;

  public:
    explicit StreamInfoMetadata(const CallbackInfo&);

    Napi::Value getMinBlocksize(const CallbackInfo&);
    void setMinBlocksize(const CallbackInfo&, const Napi::Value&);
    Napi::Value getMaxBlocksize(const CallbackInfo&);
    void setMaxBlocksize(const CallbackInfo&, const Napi::Value&);
    Napi::Value getMinFramesize(const CallbackInfo&);
    void setMinFramesize(const CallbackInfo&, const Napi::Value&);
    Napi::Value getMaxFramesize(const CallbackInfo&);
    void setMaxFramesize(const CallbackInfo&, const Napi::Value&);
    Napi::Value getChannels(const CallbackInfo&);
    void setChannels(const CallbackInfo&, const Napi::Value&);
    Napi::Value getBitsPerSample(const CallbackInfo&);
    void setBitsPerSample(const CallbackInfo&, const Napi::Value&);
    Napi::Value getSampleRate(const CallbackInfo&);
    void setSampleRate(const CallbackInfo&, const Napi::Value&);
    Napi::Value getTotalSamples(const CallbackInfo&);
    void setTotalSamples(const CallbackInfo&, const Napi::Value&);
    Napi::Value getMd5sum(const CallbackInfo&);
    void setMd5sum(const CallbackInfo&, const Napi::Value&);

    static Function init(Napi::Env, FlacAddon&);
  };

  class PaddingMetadata: public ObjectWrap<PaddingMetadata>, public Metadata {
  public:
    explicit PaddingMetadata(const CallbackInfo&);

    static Function init(Napi::Env, FlacAddon&);
  };

  class ApplicationMetadata: public ObjectWrap<ApplicationMetadata>, public Metadata {
    pointer::BufferReference<FLAC__byte> idBuffer;
    pointer::BufferReference<FLAC__byte> dataBuffer;

  public:
    explicit ApplicationMetadata(const CallbackInfo&);

    Napi::Value getId(const CallbackInfo&);
    void setId(const CallbackInfo&, const Napi::Value&);
    Napi::Value getData(const CallbackInfo&);
    void setData(const CallbackInfo&, const Napi::Value&);

    static Function init(Napi::Env, FlacAddon&);
  };

  class SeekTableMetadata: public ObjectWrap<SeekTableMetadata>, public Metadata {
  public:
    explicit SeekTableMetadata(const CallbackInfo&);

    Napi::Value getCount(const CallbackInfo&);
    Napi::Value iterator(const CallbackInfo&);
    Napi::Value resizePoints(const CallbackInfo&);
    void setPoint(const CallbackInfo&);
    Napi::Value insertPoint(const CallbackInfo&);
    Napi::Value deletePoint(const CallbackInfo&);
    Napi::Value isLegal(const CallbackInfo&);
    Napi::Value templateAppendPlaceholders(const CallbackInfo&);
    Napi::Value templateAppendPoint(const CallbackInfo&);
    Napi::Value templateAppendPoints(const CallbackInfo&);
    Napi::Value templateAppendSpacedPoints(const CallbackInfo&);
    Napi::Value templateAppendSpacedPointsBySamples(const CallbackInfo&);
    Napi::Value templateSort(const CallbackInfo&);

    static Function init(Napi::Env, FlacAddon&);
  };

  class SeekPoint: public ObjectWrap<SeekPoint>, public Mapping<FLAC__StreamMetadata_SeekPoint> {
  public:
    explicit SeekPoint(const CallbackInfo&);
    virtual ~SeekPoint();

    Napi::Value getSampleNumber(const CallbackInfo&);
    void setSampleNumber(const CallbackInfo&, const Napi::Value&);
    Napi::Value getStreamOffset(const CallbackInfo&);
    void setStreamOffset(const CallbackInfo&, const Napi::Value&);
    Napi::Value getFrameSamples(const CallbackInfo&);
    void setFrameSamples(const CallbackInfo&, const Napi::Value&);

    static Function init(Napi::Env, FlacAddon&);
  };

  class VorbisCommentMetadata: public ObjectWrap<VorbisCommentMetadata>, public Metadata {
  public:
    explicit VorbisCommentMetadata(const CallbackInfo&);

    Napi::Value getVendorString(const CallbackInfo&);
    void setVendorString(const CallbackInfo&, const Napi::Value&);
    Napi::Value getCount(const CallbackInfo&);
    Napi::Value iterator(const CallbackInfo&);
    Napi::Value resizeComments(const CallbackInfo&);
    Napi::Value setComment(const CallbackInfo&);
    Napi::Value insertComment(const CallbackInfo&);
    Napi::Value appendComment(const CallbackInfo&);
    Napi::Value replaceComment(const CallbackInfo&);
    Napi::Value deleteComment(const CallbackInfo&);
    Napi::Value findEntryFrom(const CallbackInfo&);
    Napi::Value removeEntryMatching(const CallbackInfo&);
    Napi::Value removeEntriesMatching(const CallbackInfo&);
    Napi::Value get(const CallbackInfo&);

    static Function init(Napi::Env, FlacAddon&);
  };

  class CueSheetMetadata: public ObjectWrap<CueSheetMetadata>, public Metadata {
  public:
    explicit CueSheetMetadata(const CallbackInfo&);

    Napi::Value getMediaCatalogNumber(const CallbackInfo&);
    void setMediaCatalogNumber(const CallbackInfo&, const Napi::Value&);
    Napi::Value getLeadIn(const CallbackInfo&);
    void setLeadIn(const CallbackInfo&, const Napi::Value&);
    Napi::Value getIsCd(const CallbackInfo&);
    void setIsCd(const CallbackInfo&, const Napi::Value&);
    Napi::Value getCount(const CallbackInfo&);
    Napi::Value iterator(const CallbackInfo&);
    Napi::Value trackResizeIndices(const CallbackInfo&);
    Napi::Value trackInsertIndex(const CallbackInfo&);
    Napi::Value trackInsertBlankIndex(const CallbackInfo&);
    Napi::Value trackDeleteIndex(const CallbackInfo&);
    Napi::Value resizeTracks(const CallbackInfo&);
    Napi::Value setTrack(const CallbackInfo&);
    Napi::Value insertTrack(const CallbackInfo&);
    Napi::Value insertBlankTrack(const CallbackInfo&);
    Napi::Value deleteTrack(const CallbackInfo&);
    Napi::Value isLegal(const CallbackInfo&);
    Napi::Value calculateCddbId(const CallbackInfo&);

    static Function init(Napi::Env, FlacAddon&);
  };

  class CueSheetIndex:
      public ObjectWrap<CueSheetIndex>,
      public Mapping<FLAC__StreamMetadata_CueSheet_Index> {
  public:
    explicit CueSheetIndex(const CallbackInfo&);
    virtual ~CueSheetIndex();

    Napi::Value getOffset(const CallbackInfo&);
    void setOffset(const CallbackInfo&, const Napi::Value&);
    Napi::Value getNumber(const CallbackInfo&);
    void setNumber(const CallbackInfo&, const Napi::Value&);

    static Function init(Napi::Env, FlacAddon&);
  };

  class CueSheetTrack:
      public ObjectWrap<CueSheetTrack>,
      public Mapping<FLAC__StreamMetadata_CueSheet_Track> {
  public:
    explicit CueSheetTrack(const CallbackInfo&);
    virtual ~CueSheetTrack();

    Napi::Value getOffset(const CallbackInfo&);
    void setOffset(const CallbackInfo&, const Napi::Value&);
    Napi::Value getNumber(const CallbackInfo&);
    void setNumber(const CallbackInfo&, const Napi::Value&);
    Napi::Value getIsrc(const CallbackInfo&);
    void setIsrc(const CallbackInfo&, const Napi::Value&);
    Napi::Value getType(const CallbackInfo&);
    void setType(const CallbackInfo&, const Napi::Value&);
    Napi::Value getPreEmphasis(const CallbackInfo&);
    void setPreEmphasis(const CallbackInfo&, const Napi::Value&);
    Napi::Value getCount(const CallbackInfo&);
    Napi::Value iterator(const CallbackInfo&);
    Napi::Value clone(const CallbackInfo&);

    static Function init(Napi::Env, FlacAddon&);
  };

  class PictureMetadata: public ObjectWrap<PictureMetadata>, public Metadata {
    pointer::BufferReference<FLAC__byte> dataBuffer;

  public:
    explicit PictureMetadata(const CallbackInfo&);

    Napi::Value getPictureType(const CallbackInfo&);
    void setPictureType(const CallbackInfo&, const Napi::Value&);
    Napi::Value getMimeType(const CallbackInfo&);
    void setMimeType(const CallbackInfo&, const Napi::Value&);
    Napi::Value getDescription(const CallbackInfo&);
    void setDescription(const CallbackInfo&, const Napi::Value&);
    Napi::Value getWidth(const CallbackInfo&);
    void setWidth(const CallbackInfo&, const Napi::Value&);
    Napi::Value getHeight(const CallbackInfo&);
    void setHeight(const CallbackInfo&, const Napi::Value&);
    Napi::Value getDepth(const CallbackInfo&);
    void setDepth(const CallbackInfo&, const Napi::Value&);
    Napi::Value getColors(const CallbackInfo&);
    void setColors(const CallbackInfo&, const Napi::Value&);
    Napi::Value getData(const CallbackInfo&);
    void setData(const CallbackInfo&, const Napi::Value&);
    Napi::Value isLegal(const CallbackInfo&);

    static Function init(Napi::Env, FlacAddon&);
  };

  class UnknownMetadata: public ObjectWrap<UnknownMetadata>, public Metadata {
    pointer::BufferReference<FLAC__byte> dataBuffer;

  public:
    explicit UnknownMetadata(const CallbackInfo&);

    Napi::Value getData(const CallbackInfo&);

    static Function init(Napi::Env, FlacAddon&);
  };

}
