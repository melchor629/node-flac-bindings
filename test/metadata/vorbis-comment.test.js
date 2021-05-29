const { VorbisCommentMetadata } = require('../../lib/index').api.metadata
const { getTags } = require('../../lib/index').api.metadata0
const { MetadataType } = require('../../lib/index').api.format
const { pathForFile: { tags: pathForFile }, gc } = require('../helper')

describe('VorbisCommentMetadata', () => {
  it('create new object should work', () => {
    expect(new VorbisCommentMetadata()).not.toBeNull()
  })

  it('object has the right type', () => {
    const vc = new VorbisCommentMetadata()

    expect(vc.type).toEqual(MetadataType.VORBIS_COMMENT)
  })

  it('vendor string can be modified', () => {
    const vc = new VorbisCommentMetadata()

    vc.vendorString = 'mee'

    expect(vc.vendorString).toEqual('mee')
  })

  it('.comments should contain all the expected comments', () => {
    const comments = Array.from(getTags(pathForFile('vc-p.flac')))

    expect(comments.length).toEqual(6)
    expect(comments[0]).toEqual('TITLE=Metadata Test')
    expect(comments[1]).toEqual('ARTIST=melchor629')
    expect(comments[2]).toEqual('ALBUM=flac-bindings')
    expect(comments[3]).toEqual('COMMENT=Nice comment tho')
    expect(comments[4]).toEqual('DATE=2019')
    expect(comments[5]).toEqual('TRACKNUMBER=1')
  })

  it('iterator should iterate over all the expected comments', () => {
    const it = getTags(pathForFile('vc-p.flac'))[Symbol.iterator]()
    let m

    m = it.next()
    expect(m.done).toBe(false)
    expect(m.value).toEqual('TITLE=Metadata Test')

    m = it.next()
    expect(m.done).toBe(false)
    expect(m.value).toEqual('ARTIST=melchor629')

    m = it.next()
    expect(m.done).toBe(false)
    expect(m.value).toEqual('ALBUM=flac-bindings')

    m = it.next()
    expect(m.done).toBe(false)
    expect(m.value).toEqual('COMMENT=Nice comment tho')

    m = it.next()
    expect(m.done).toBe(false)
    expect(m.value).toEqual('DATE=2019')

    m = it.next()
    expect(m.done).toBe(false)
    expect(m.value).toEqual('TRACKNUMBER=1')

    m = it.next()
    expect(m.done).toBe(true)
  })

  it('get() should get the value of an existing entry', () => {
    const vc = getTags(pathForFile('vc-p.flac'))

    const entryValue = vc.get('COMMENT')

    expect(entryValue).not.toBeNull()
    expect(entryValue).toEqual('Nice comment tho')
  })

  it('get() should return null for a non existing entry', () => {
    const vc = getTags(pathForFile('vc-p.flac'))

    const entryValue = vc.get('TIME')

    expect(entryValue).toBeNull()
  })

  it('get() should throw if the first argument is not a string', () => {
    expect(() => new VorbisCommentMetadata().get(function () {})).toThrow()
  })

  it('resizeComments() should add and remove comment entries', () => {
    const vc = new VorbisCommentMetadata()

    expect(vc.resizeComments(10)).toBe(true)
    expect(vc.count).toEqual(10)
    expect(vc.resizeComments(1)).toBe(true)
    expect(vc.count).toEqual(1)
  })

  it('resizeComments() should throw if size is not a number', () => {
    const vc = new VorbisCommentMetadata()

    expect(() => vc.resizeComments(null)).toThrow()
  })

  it('setComment() should replace the comment entry if the position is valid', () => {
    const vc = new VorbisCommentMetadata()
    vc.resizeComments(1)

    expect(vc.setComment(0, 'ARTIST=Mario')).toBe(true)

    expect(vc.count).toEqual(1)
    expect(Array.from(vc)[0]).toEqual('ARTIST=Mario')
  })

  it('setComment() should throw if the position is invalid', () => {
    const vc = new VorbisCommentMetadata()

    expect(() => vc.setComment(0, '')).toThrow()
    vc.resizeComments(1)
    expect(() => vc.setComment(38, '')).toThrow()
    expect(() => vc.setComment(-9, '')).toThrow()
  })

  it('setComment() should throw if the first argument is not a number', () => {
    expect(() => new VorbisCommentMetadata().setComment(null)).toThrow()
  })

  it('setComment() should throw if the second argument is not a string', () => {
    expect(() => new VorbisCommentMetadata().setComment(0, NaN)).toThrow()
  })

  it('insertComment() should add a new comment entry if the position is valid', () => {
    const vc = new VorbisCommentMetadata()

    expect(vc.insertComment(0, 'TITLE=tests')).toBe(true)

    expect(vc.count).toEqual(1)
    expect(Array.from(vc)[0]).toEqual('TITLE=tests')
  })

  it('insertComment() should throw if the position is invalid', () => {
    const vc = new VorbisCommentMetadata()

    expect(() => vc.insertComment(38, '')).toThrow()
    expect(() => vc.insertComment(-9, '')).toThrow()
  })

  it('insertComment() should throw if the first argument is not a number', () => {
    expect(() => new VorbisCommentMetadata().insertComment(null)).toThrow()
  })

  it('insertComment() should throw if the second argument is not a string', () => {
    expect(() => new VorbisCommentMetadata().insertComment(0, NaN)).toThrow()
  })

  it('appendComment() should add a new comment entry', () => {
    const vc = new VorbisCommentMetadata()

    expect(vc.appendComment('TITLE=tests')).toBe(true)

    expect(vc.count).toEqual(1)
    expect(Array.from(vc)[0]).toEqual('TITLE=tests')
  })

  it('appendComment() should throw if the first argument is not a string', () => {
    expect(() => new VorbisCommentMetadata().appendComment({ 1: '2' })).toThrow()
  })

  it('replaceComment() should replace comments', () => {
    const vc = new VorbisCommentMetadata()
    vc.appendComment('TITLE=E')

    expect(vc.replaceComment('TITLE=tests', true)).toBe(true)

    expect(vc.count).toEqual(1)
    expect(Array.from(vc)[0]).toEqual('TITLE=tests')
  })

  it('deleteComment() should remove the comment entry if the position is valid', () => {
    const vc = new VorbisCommentMetadata()
    vc.resizeComments(1)

    expect(vc.deleteComment(0, 'TITLE=tests')).toBe(true)

    expect(vc.count).toEqual(0)
  })

  it('deleteComment() should throw if the position is invalid', () => {
    const vc = new VorbisCommentMetadata()

    expect(() => vc.deleteComment(38, '')).toThrow()
    expect(() => vc.deleteComment(-9, '')).toThrow()
  })

  it('deleteComment() should throw if the first argument is not a number', () => {
    expect(() => new VorbisCommentMetadata().deleteComment(null)).toThrow()
  })

  it('findEntryFrom() should return the position of the entry', () => {
    const vc = new VorbisCommentMetadata()
    vc.insertComment(0, 'TITLE=me')

    const pos = vc.findEntryFrom(0, 'title')

    expect(pos).toEqual(0)
  })

  it('findEntryFrom() should return the -1 if the entry cannot be found', () => {
    const vc = new VorbisCommentMetadata()
    vc.insertComment(0, 'TITLE=me')

    const pos = vc.findEntryFrom(0, 'ARTIST')

    expect(pos).toEqual(-1)
  })

  it('findEntryFrom() should throw if the first argument is not a number', () => {
    expect(() => new VorbisCommentMetadata().findEntryFrom(null)).toThrow()
  })

  it('findEntryFrom() should throw if the second argument is not a string', () => {
    expect(() => new VorbisCommentMetadata().findEntryFrom(0, undefined)).toThrow()
  })

  it('removeEntryMatching() should return 0 if no entries has been removed', () => {
    const vc = new VorbisCommentMetadata()
    vc.insertComment(0, 'TITLE=me')

    const ret = vc.removeEntryMatching('ARTIST')

    expect(ret).toEqual(0)
    expect(vc.count).toEqual(1)
  })

  it('removeEntryMatching() should return 1 if at least one entry has been removed', () => {
    const vc = new VorbisCommentMetadata()
    vc.insertComment(0, 'TITLE=me')
    vc.appendComment('TITLE=mo')

    const ret = vc.removeEntryMatching('TITLE')

    expect(ret).toEqual(1)
    expect(vc.count).toEqual(1)
  })

  it('removeEntryMatching() should throw if the first argument is not a string', () => {
    expect(() => new VorbisCommentMetadata().removeEntryMatching(NaN)).toThrow()
  })

  it('removeEntriesMatching() should return 0 if no entries has been removed', () => {
    const vc = new VorbisCommentMetadata()
    vc.insertComment(0, 'TITLE=me')

    const ret = vc.removeEntriesMatching('ARTIST')

    expect(ret).toEqual(0)
    expect(vc.count).toEqual(1)
  })

  it('removeEntriesMatching() should return 2 if entries has been removed', () => {
    const vc = new VorbisCommentMetadata()
    vc.insertComment(0, 'TITLE=me')
    vc.appendComment('TITLE=mo')

    const ret = vc.removeEntriesMatching('TITLE')

    expect(ret).toEqual(2)
    expect(vc.count).toEqual(0)
  })

  it('removeEntriesMatching() should throw if the first argument is not a string', () => {
    expect(() => new VorbisCommentMetadata().removeEntriesMatching(NaN)).toThrow()
  })

  describe('gc', () => {
    it('gc should work', () => {
      gc()
    })
  })
})
