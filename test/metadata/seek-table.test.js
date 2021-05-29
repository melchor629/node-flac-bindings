const { SeekTableMetadata, SeekPoint } = require('../../lib/index').api.metadata
const { MetadataType } = require('../../lib/index').api.format
const { SimpleIterator } = require('../../lib/index').api
const { pathForFile: { tags: pathForFile }, gc } = require('../helper')

describe('SeekTableMetadata', () => {
  it('create new object should work', () => {
    expect(new SeekTableMetadata()).not.toBeNull()
  })

  it('object has the right type', () => {
    const st = new SeekTableMetadata()

    expect(st.type).toEqual(MetadataType.SEEKTABLE)
  })

  it('.points gets all points in the object', () => {
    const it = new SimpleIterator()
    it.init(pathForFile('vc-cs.flac'), true)
    it.next()
    const points = Array.from(it.getBlock())

    expect(points.length).toEqual(2)
    expect(points[0].sampleNumber).toEqual(0)
    expect(points[0].streamOffset).toEqual(0)
    expect(points[0].frameSamples).toEqual(4096)
    expect(points[1].sampleNumber).toEqual(16384)
    expect(points[1].streamOffset).toEqual(8780)
    expect(points[1].frameSamples).toEqual(4096)
  })

  it('iterator should iterate over all points in the object', () => {
    const it = new SimpleIterator()
    it.init(pathForFile('vc-cs.flac'), true)
    it.next()
    const st = it.getBlock()
    const i = st[Symbol.iterator]()

    let v = i.next()
    expect(v.done).toBe(false)
    expect(v.value.sampleNumber).toEqual(0)
    expect(v.value.streamOffset).toEqual(0)
    expect(v.value.frameSamples).toEqual(4096)

    v = i.next()
    expect(v.done).toBe(false)
    expect(v.value.sampleNumber).toEqual(16384)
    expect(v.value.streamOffset).toEqual(8780)
    expect(v.value.frameSamples).toEqual(4096)

    v = i.next()
    expect(v.done).toBe(true)
  })

  it('resizePoints() should insert and remove points', () => {
    const st = new SeekTableMetadata()

    expect(st.count).toEqual(0)
    expect(st.resizePoints(10)).toBe(true)
    expect(st.count).toEqual(10)
    expect(st.resizePoints(1)).toBe(true)
    expect(st.count).toEqual(1)
  })

  it('resizePoints() should throw if size is not a number', () => {
    const st = new SeekTableMetadata()

    expect(() => st.resizePoints(null)).toThrow()
  })

  it('insertPoint() should insert a point if the position is valid', () => {
    const st = new SeekTableMetadata()

    expect(st.insertPoint(0, new SeekPoint())).toBe(true)

    expect(st.count).toEqual(1)
  })

  it('insertPoint() should throw if the position is invalid', () => {
    const st = new SeekTableMetadata()

    expect(() => st.insertPoint(11, new SeekPoint())).toThrow()
    expect(() => st.insertPoint(-1, new SeekPoint())).toThrow()

    expect(st.count).toEqual(0)
  })

  it('insertPoint() should throw if the position is not a number', () => {
    const st = new SeekTableMetadata()

    expect(() => st.insertPoint(null, new SeekPoint())).toThrow()
  })

  it('insertPoint() should throw if the point is not a SeekPoint', () => {
    const st = new SeekTableMetadata()

    expect(() => st.insertPoint(1, null)).toThrow()
  })

  it('setPoint() should replace a point if the position is valid', () => {
    const st = new SeekTableMetadata()
    st.insertPoint(0, new SeekPoint())

    st.setPoint(0, new SeekPoint(3n, 2n, 1n))

    const points = Array.from(st)
    expect(points.length).toEqual(1)
    expect(points[0].sampleNumber).toEqual(3)
    expect(points[0].streamOffset).toEqual(2)
    expect(points[0].frameSamples).toEqual(1)
  })

  it('setPoint() should throw if the position is invalid', () => {
    const st = new SeekTableMetadata()

    expect(() => st.setPoint(0, new SeekPoint())).toThrow()
    st.insertPoint(0, new SeekPoint())
    expect(() => st.setPoint(91, new SeekPoint())).toThrow()
    expect(() => st.setPoint(-1, new SeekPoint())).toThrow()
  })

  it('setPoint() should throw if the position is not a number', () => {
    const st = new SeekTableMetadata()

    expect(() => st.setPoint(null, new SeekPoint())).toThrow()
  })

  it('setPoint() should throw if the point is not a SeekPoint', () => {
    const st = new SeekTableMetadata()

    expect(() => st.setPoint(1, null)).toThrow()
  })

  it('deletePoint() should delete a point if the position is valid', () => {
    const st = new SeekTableMetadata()
    st.insertPoint(0, new SeekPoint())

    expect(st.deletePoint(0)).toBe(true)

    expect(st.count).toEqual(0)
  })

  it('deletePoint() should throw if the position is invalid', () => {
    const st = new SeekTableMetadata()

    expect(() => st.deletePoint(0)).toThrow()
    st.insertPoint(0, new SeekPoint())
    expect(() => st.deletePoint(91)).toThrow()
    expect(() => st.deletePoint(-1)).toThrow()
  })

  it('deletePoint() should throw if the position is not a number', () => {
    const st = new SeekTableMetadata()

    expect(() => st.deletePoint(null)).toThrow()
  })

  it('templateAppendPlaceholders() should insert points at the end', () => {
    const st = new SeekTableMetadata()
    st.insertPoint(0, new SeekPoint(998877665544332211n))

    expect(st.templateAppendPlaceholders(10)).toBe(true)

    const points = Array.from(st)
    expect(points.length).toEqual(11)
    expect(points[0].sampleNumber).toEqual(998877665544332211n)
  })

  it('templateAppendPlaceholders() should throw if count is not a number', () => {
    const st = new SeekTableMetadata()

    expect(() => st.templateAppendPlaceholders(null)).toThrow()
  })

  it('templateAppendPoint() should append a new point to the end', () => {
    const st = new SeekTableMetadata()
    st.insertPoint(0, new SeekPoint(998877665544332211n))

    expect(st.templateAppendPoint(675n)).toBe(true)

    const points = Array.from(st)
    expect(points.length).toEqual(2)
    expect(points[0].sampleNumber).toEqual(998877665544332211n)
    expect(points[1].sampleNumber).toEqual(675)
  })

  it('templateAppendPoint() should throw if sampleNumber is not a number', () => {
    const st = new SeekTableMetadata()

    expect(() => st.templateAppendPoint(null)).toThrow()
  })

  it('templateAppendPoints() should append some new points to the end', () => {
    const st = new SeekTableMetadata()
    st.insertPoint(0, new SeekPoint(998877665544332211n))

    expect(st.templateAppendPoints([675n, 879n, 213n])).toBe(true)

    const points = Array.from(st)
    expect(points.length).toEqual(4)
    expect(points[0].sampleNumber).toEqual(998877665544332211n)
    expect(points[1].sampleNumber).toEqual(675)
    expect(points[2].sampleNumber).toEqual(879)
    expect(points[3].sampleNumber).toEqual(213)
  })

  it('templateAppendPoints() should throw if sampleNumbers is not an array', () => {
    const st = new SeekTableMetadata()

    expect(() => st.templateAppendPoints(null)).toThrow()
  })

  it('templateAppendPoints() should throw if sampleNumbers contains a non-number value', () => {
    const st = new SeekTableMetadata()

    expect(() => st.templateAppendPoints([1, false, 3])).toThrow()
  })

  it('templateAppendSpacedPoints() should append some new points to the end', () => {
    const st = new SeekTableMetadata()
    st.insertPoint(0, new SeekPoint(123n))

    expect(st.templateAppendSpacedPoints(5, 25n)).toBe(true)

    const points = Array.from(st)
    expect(points.length).toEqual(6)
    expect(points[0].sampleNumber).toEqual(123)
    expect(points[1].sampleNumber).toEqual(5 * 0)
    expect(points[2].sampleNumber).toEqual(5 * 1)
    expect(points[3].sampleNumber).toEqual(5 * 2)
    expect(points[4].sampleNumber).toEqual(5 * 3)
    expect(points[5].sampleNumber).toEqual(5 * 4)
  })

  it('templateAppendSpacedPoints() should throw if totalSamples is 0', () => {
    expect(() => new SeekTableMetadata().templateAppendSpacedPoints(1, 0n)).toThrow()
  })

  it('templateAppendSpacedPoints() should throw if num is not a number', () => {
    expect(() => new SeekTableMetadata().templateAppendSpacedPoints(null, 0n)).toThrow()
  })

  it('templateAppendSpacedPoints() should throw if totalSamples is not a number', () => {
    expect(() => new SeekTableMetadata().templateAppendSpacedPoints(0, null)).toThrow()
  })

  it('templateAppendSpacedPointsBySamples() should append some new points to the end', () => {
    const st = new SeekTableMetadata()
    st.insertPoint(0, new SeekPoint(123n))

    expect(st.templateAppendSpacedPointsBySamples(5, 25)).toBe(true)

    const points = Array.from(st)
    expect(points.length).toEqual(6)
    expect(points[0].sampleNumber).toEqual(123)
    expect(points[1].sampleNumber).toEqual(5 * 0)
    expect(points[2].sampleNumber).toEqual(5 * 1)
    expect(points[3].sampleNumber).toEqual(5 * 2)
    expect(points[4].sampleNumber).toEqual(5 * 3)
    expect(points[5].sampleNumber).toEqual(5 * 4)
  })

  it('templateAppendSpacedPointsBySamples() should throw if samples is 0', () => {
    expect(() => new SeekTableMetadata().templateAppendSpacedPointsBySamples(0, 25n)).toThrow()
  })

  it('templateAppendSpacedPointsBySamples() should throw if totalSamples is 0', () => {
    expect(() => new SeekTableMetadata().templateAppendSpacedPointsBySamples(5, 0n)).toThrow()
  })

  it('templateAppendSpacedPointsBySamples() should throw if num is not a number', () => {
    expect(
      () => new SeekTableMetadata().templateAppendSpacedPointsBySamples(null, 0n),
    ).toThrow()
  })

  it('templateAppendSpacedPointsBySamples() should throw if totalSamples is not a number', () => {
    expect(() => new SeekTableMetadata().templateAppendSpacedPointsBySamples(0, null)).toThrow()
  })

  it('templateSort() sorts the points', () => {
    const st = new SeekTableMetadata()
    st.insertPoint(0, new SeekPoint(123n))
    st.templateAppendSpacedPointsBySamples(5, 25)
    st.insertPoint(4, new SeekPoint(500n))
    st.insertPoint(7, new SeekPoint(5n, 5n))

    expect(st.templateSort()).toBe(true)

    const points = Array.from(st)
    expect(points.length).toEqual(8)
    expect(points[0].sampleNumber).toEqual(5 * 0)
    expect(points[1].sampleNumber).toEqual(5 * 1)
    expect(points[2].sampleNumber).toEqual(5 * 2)
    expect(points[3].sampleNumber).toEqual(5 * 3)
    expect(points[4].sampleNumber).toEqual(5 * 4)
    expect(points[5].sampleNumber).toEqual(123)
    expect(points[6].sampleNumber).toEqual(500)
    expect(points[7].sampleNumber).toEqual(18446744073709551615n)
  })

  it('templateSort(true) sorts the points and removes duplicates', () => {
    const st = new SeekTableMetadata()
    st.insertPoint(0, new SeekPoint(123n))
    st.templateAppendSpacedPointsBySamples(5, 25)
    st.insertPoint(4, new SeekPoint(500n))
    st.insertPoint(7, new SeekPoint(5n, 5n))

    expect(st.templateSort(true)).toBe(true)

    const points = Array.from(st)
    expect(points.length).toEqual(7)
    expect(points[0].sampleNumber).toEqual(5 * 0)
    expect(points[1].sampleNumber).toEqual(5 * 1)
    expect(points[2].sampleNumber).toEqual(5 * 2)
    expect(points[3].sampleNumber).toEqual(5 * 3)
    expect(points[4].sampleNumber).toEqual(5 * 4)
    expect(points[5].sampleNumber).toEqual(123)
    expect(points[6].sampleNumber).toEqual(500)
  })

  it('isLegal() should work', () => {
    const st = new SeekTableMetadata()
    expect(st.isLegal()).toBe(true)
  })

  describe('gc', () => {
    it('gc should work', () => {
      gc()
    })
  })
})
