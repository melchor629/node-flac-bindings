const { assert } = require('chai')
const { PaddingMetadata } = require('../../lib/index').api.metadata
const { MetadataType } = require('../../lib/index').api.format
const { gc } = require('../helper')

describe('PaddingMetadata', function () {
  it('create new object should work', function () {
    return new PaddingMetadata()
  })

  it('the object should have the right type', function () {
    const p = new PaddingMetadata()

    assert.equal(p.type, MetadataType.PADDING)
  })

  it('the object initially has 0 bytes of padding', function () {
    const p = new PaddingMetadata()

    assert.equal(p.length, 0)
  })

  it('the object created with a size should have that size in bytes of padding', function () {
    const p = new PaddingMetadata(123)

    assert.equal(p.length, 123)
  })

  describe('gc', function () {
    it('gc should work', function () {
      gc()
    })
  })
})
