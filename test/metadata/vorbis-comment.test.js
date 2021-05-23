const { assert } = require('chai')
const { VorbisCommentMetadata } = require('../../lib/index').api.metadata
const { getTags } = require('../../lib/index').api.metadata0
const { MetadataType } = require('../../lib/index').api.format
const { pathForFile: { tags: pathForFile }, gc } = require('../helper')

describe('VorbisCommentMetadata', function () {
  it('create new object should work', function () {
    return new VorbisCommentMetadata()
  })

  it('object has the right type', function () {
    const vc = new VorbisCommentMetadata()

    assert.equal(vc.type, MetadataType.VORBIS_COMMENT)
  })

  it('vendor string can be modified', function () {
    const vc = new VorbisCommentMetadata()

    vc.vendorString = 'mee'

    assert.equal(vc.vendorString, 'mee')
  })

  it('.comments should contain all the expected comments', function () {
    const comments = Array.from(getTags(pathForFile('vc-p.flac')))

    assert.equal(comments.length, 6)
    assert.equal(comments[0], 'TITLE=Metadata Test')
    assert.equal(comments[1], 'ARTIST=melchor629')
    assert.equal(comments[2], 'ALBUM=flac-bindings')
    assert.equal(comments[3], 'COMMENT=Nice comment tho')
    assert.equal(comments[4], 'DATE=2019')
    assert.equal(comments[5], 'TRACKNUMBER=1')
  })

  it('iterator should iterate over all the expected comments', function () {
    const it = getTags(pathForFile('vc-p.flac'))[Symbol.iterator]()
    let m

    m = it.next()
    assert.isFalse(m.done)
    assert.equal(m.value, 'TITLE=Metadata Test')

    m = it.next()
    assert.isFalse(m.done)
    assert.equal(m.value, 'ARTIST=melchor629')

    m = it.next()
    assert.isFalse(m.done)
    assert.equal(m.value, 'ALBUM=flac-bindings')

    m = it.next()
    assert.isFalse(m.done)
    assert.equal(m.value, 'COMMENT=Nice comment tho')

    m = it.next()
    assert.isFalse(m.done)
    assert.equal(m.value, 'DATE=2019')

    m = it.next()
    assert.isFalse(m.done)
    assert.equal(m.value, 'TRACKNUMBER=1')

    m = it.next()
    assert.isTrue(m.done)
  })

  it('get() should get the value of an existing entry', function () {
    const vc = getTags(pathForFile('vc-p.flac'))

    const entryValue = vc.get('COMMENT')

    assert.isNotNull(entryValue)
    assert.equal(entryValue, 'Nice comment tho')
  })

  it('get() should return null for a non existing entry', function () {
    const vc = getTags(pathForFile('vc-p.flac'))

    const entryValue = vc.get('TIME')

    assert.isNull(entryValue)
  })

  it('get() should throw if the first argument is not a string', function () {
    assert.throws(() => new VorbisCommentMetadata().get(function () {}))
  })

  it('resizeComments() should add and remove comment entries', function () {
    const vc = new VorbisCommentMetadata()

    assert.isTrue(vc.resizeComments(10))
    assert.equal(vc.count, 10)
    assert.isTrue(vc.resizeComments(1))
    assert.equal(vc.count, 1)
  })

  it('resizeComments() should throw if size is not a number', function () {
    const vc = new VorbisCommentMetadata()

    assert.throws(() => vc.resizeComments(null), /Expected null to be number/)
  })

  it('setComment() should replace the comment entry if the position is valid', function () {
    const vc = new VorbisCommentMetadata()
    vc.resizeComments(1)

    assert.isTrue(vc.setComment(0, 'ARTIST=Mario'))

    assert.equal(vc.count, 1)
    assert.equal(Array.from(vc)[0], 'ARTIST=Mario')
  })

  it('setComment() should throw if the position is invalid', function () {
    const vc = new VorbisCommentMetadata()

    assert.throws(() => vc.setComment(0, ''))
    vc.resizeComments(1)
    assert.throws(() => vc.setComment(38, ''))
    assert.throws(() => vc.setComment(-9, ''))
  })

  it('setComment() should throw if the first argument is not a number', function () {
    assert.throws(() => new VorbisCommentMetadata().setComment(null))
  })

  it('setComment() should throw if the second argument is not a string', function () {
    assert.throws(() => new VorbisCommentMetadata().setComment(0, NaN))
  })

  it('insertComment() should add a new comment entry if the position is valid', function () {
    const vc = new VorbisCommentMetadata()

    assert.isTrue(vc.insertComment(0, 'TITLE=tests'))

    assert.equal(vc.count, 1)
    assert.equal(Array.from(vc)[0], 'TITLE=tests')
  })

  it('insertComment() should throw if the position is invalid', function () {
    const vc = new VorbisCommentMetadata()

    assert.throws(() => vc.insertComment(38, ''))
    assert.throws(() => vc.insertComment(-9, ''))
  })

  it('insertComment() should throw if the first argument is not a number', function () {
    assert.throws(() => new VorbisCommentMetadata().insertComment(null))
  })

  it('insertComment() should throw if the second argument is not a string', function () {
    assert.throws(() => new VorbisCommentMetadata().insertComment(0, NaN))
  })

  it('appendComment() should add a new comment entry', function () {
    const vc = new VorbisCommentMetadata()

    assert.isTrue(vc.appendComment('TITLE=tests'))

    assert.equal(vc.count, 1)
    assert.equal(Array.from(vc)[0], 'TITLE=tests')
  })

  it('appendComment() should throw if the first argument is not a string', function () {
    assert.throws(() => new VorbisCommentMetadata().appendComment({ 1: '2' }))
  })

  it('replaceComment() should replace comments', function () {
    const vc = new VorbisCommentMetadata()
    vc.appendComment('TITLE=E')

    assert.isTrue(vc.replaceComment('TITLE=tests', true))

    assert.equal(vc.count, 1)
    assert.equal(Array.from(vc)[0], 'TITLE=tests')
  })

  it('deleteComment() should remove the comment entry if the position is valid', function () {
    const vc = new VorbisCommentMetadata()
    vc.resizeComments(1)

    assert.isTrue(vc.deleteComment(0, 'TITLE=tests'))

    assert.equal(vc.count, 0)
  })

  it('deleteComment() should throw if the position is invalid', function () {
    const vc = new VorbisCommentMetadata()

    assert.throws(() => vc.deleteComment(38, ''))
    assert.throws(() => vc.deleteComment(-9, ''))
  })

  it('deleteComment() should throw if the first argument is not a number', function () {
    assert.throws(() => new VorbisCommentMetadata().deleteComment(null))
  })

  it('findEntryFrom() should return the position of the entry', function () {
    const vc = new VorbisCommentMetadata()
    vc.insertComment(0, 'TITLE=me')

    const pos = vc.findEntryFrom(0, 'title')

    assert.equal(pos, 0)
  })

  it('findEntryFrom() should return the -1 if the entry cannot be found', function () {
    const vc = new VorbisCommentMetadata()
    vc.insertComment(0, 'TITLE=me')

    const pos = vc.findEntryFrom(0, 'ARTIST')

    assert.equal(pos, -1)
  })

  it('findEntryFrom() should throw if the first argument is not a number', function () {
    assert.throws(() => new VorbisCommentMetadata().findEntryFrom(null), /Expected null to be number or bigint/)
  })

  it('findEntryFrom() should throw if the second argument is not a string', function () {
    assert.throws(() => new VorbisCommentMetadata().findEntryFrom(0, undefined), /Expected undefined to be string/)
  })

  it('removeEntryMatching() should return 0 if no entries has been removed', function () {
    const vc = new VorbisCommentMetadata()
    vc.insertComment(0, 'TITLE=me')

    const ret = vc.removeEntryMatching('ARTIST')

    assert.equal(ret, 0)
    assert.equal(vc.count, 1)
  })

  it('removeEntryMatching() should return 1 if at least one entry has been removed', function () {
    const vc = new VorbisCommentMetadata()
    vc.insertComment(0, 'TITLE=me')
    vc.appendComment('TITLE=mo')

    const ret = vc.removeEntryMatching('TITLE')

    assert.equal(ret, 1)
    assert.equal(vc.count, 1)
  })

  it('removeEntryMatching() should throw if the first argument is not a string', function () {
    assert.throws(() => new VorbisCommentMetadata().removeEntryMatching(NaN))
  })

  it('removeEntriesMatching() should return 0 if no entries has been removed', function () {
    const vc = new VorbisCommentMetadata()
    vc.insertComment(0, 'TITLE=me')

    const ret = vc.removeEntriesMatching('ARTIST')

    assert.equal(ret, 0)
    assert.equal(vc.count, 1)
  })

  it('removeEntriesMatching() should return 2 if entries has been removed', function () {
    const vc = new VorbisCommentMetadata()
    vc.insertComment(0, 'TITLE=me')
    vc.appendComment('TITLE=mo')

    const ret = vc.removeEntriesMatching('TITLE')

    assert.equal(ret, 2)
    assert.equal(vc.count, 0)
  })

  it('removeEntriesMatching() should throw if the first argument is not a string', function () {
    assert.throws(() => new VorbisCommentMetadata().removeEntriesMatching(NaN))
  })

  describe('gc', function () {
    it('gc should work', function () {
      gc()
    })
  })
})
