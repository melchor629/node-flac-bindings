/* eslint-disable prefer-arrow-callback */
/// <reference path="../../lib/index.d.ts" />
const { SeekTableMetadata, SeekPoint } = require('../../lib/index').api.metadata;
const { MetadataType } = require('../../lib/index').api.format;
const { SimpleIterator } = require('../../lib/index').api;
const { assert } = require('chai');
const { pathForFile: { tags: pathForFile } } = require('../helper');

describe('SeekTableMetadata', function() {

    it('create new object should work', function() {
        new SeekTableMetadata();
    });

    it('object has the right type', function() {
        const st = new SeekTableMetadata();

        assert.equal(st.type, MetadataType.SEEKTABLE);
    });

    it('.points gets all points in the object', function() {
        const it = new SimpleIterator();
        it.init(pathForFile('vc-cs.flac'), true);
        it.next();
        const points = Array.from(it.getBlock());

        assert.equal(points.length, 2);
        assert.equal(points[0].sampleNumber, 0n);
        assert.equal(points[0].streamOffset, 0n);
        assert.equal(points[0].frameSamples, 4096n);
        assert.equal(points[1].sampleNumber, 16384n);
        assert.equal(points[1].streamOffset, 8780n);
        assert.equal(points[1].frameSamples, 4096n);
    });

    it('iterator should iterate over all points in the object', function() {
        const it = new SimpleIterator();
        it.init(pathForFile('vc-cs.flac'), true);
        it.next();
        const st = it.getBlock();
        const i = st[Symbol.iterator]();

        let v = i.next();
        assert.isFalse(v.done);
        assert.equal(v.value.sampleNumber, 0n);
        assert.equal(v.value.streamOffset, 0n);
        assert.equal(v.value.frameSamples, 4096n);

        v = i.next();
        assert.isFalse(v.done);
        assert.equal(v.value.sampleNumber, 16384n);
        assert.equal(v.value.streamOffset, 8780n);
        assert.equal(v.value.frameSamples, 4096n);

        v = i.next();
        assert.isTrue(v.done);
    });

    it('resizePoints() should insert and remove points', function() {
        const st = new SeekTableMetadata();

        assert.equal(st.count, 0);
        assert.isTrue(st.resizePoints(10));
        assert.equal(st.count, 10);
        assert.isTrue(st.resizePoints(1));
        assert.equal(st.count, 1);
    });

    it('resizePoints() should throw if size is not a number', function() {
        const st = new SeekTableMetadata();

        assert.throws(() => st.resizePoints(null), /Expected null to be number or bigint/);
    });

    it('insertPoint() should insert a point if the position is valid', function() {
        const st = new SeekTableMetadata();

        assert.isTrue(st.insertPoint(0, new SeekPoint()));

        assert.equal(st.count, 1);
    });

    it('insertPoint() should throw if the position is invalid', function() {
        const st = new SeekTableMetadata();

        assert.throws(() => st.insertPoint(11, new SeekPoint()), /Point position is invalid/);
        assert.throws(() => st.insertPoint(-1, new SeekPoint()), /Number -1 is negative - expected unsigned value/);

        assert.equal(st.count, 0);
    });

    it('insertPoint() should throw if the position is not a number', function() {
        const st = new SeekTableMetadata();

        assert.throws(() => st.insertPoint(null, new SeekPoint()), /Expected null to be number or bigint/);
    });

    it('insertPoint() should throw if the point is not a SeekPoint', function() {
        const st = new SeekTableMetadata();

        assert.throws(() => st.insertPoint(1, null), /Expected null to be object/);
    });

    it('setPoint() should replace a point if the position is valid', function() {
        const st = new SeekTableMetadata();
        st.insertPoint(0, new SeekPoint());

        st.setPoint(0, new SeekPoint(3n, 2n, 1n));

        const points = Array.from(st);
        assert.equal(points.length, 1);
        assert.equal(points[0].sampleNumber, 3n);
        assert.equal(points[0].streamOffset, 2n);
        assert.equal(points[0].frameSamples, 1n);
    });

    it('setPoint() should throw if the position is invalid', function() {
        const st = new SeekTableMetadata();

        assert.throws(() => st.setPoint(0, new SeekPoint()), /Point position is invalid/);
        st.insertPoint(0, new SeekPoint());
        assert.throws(() => st.setPoint(91, new SeekPoint()), /Point position is invalid/);
        assert.throws(() => st.setPoint(-1, new SeekPoint()), /Number -1 is negative/);
    });

    it('setPoint() should throw if the position is not a number', function() {
        const st = new SeekTableMetadata();

        assert.throws(() => st.setPoint(null, new SeekPoint()), /Expected null to be number or bigint/);
    });

    it('setPoint() should throw if the point is not a SeekPoint', function() {
        const st = new SeekTableMetadata();

        assert.throws(() => st.setPoint(1, null), /Expected null to be object/);
    });

    it('deletePoint() should delete a point if the position is valid', function() {
        const st = new SeekTableMetadata();
        st.insertPoint(0, new SeekPoint());

        assert.isTrue(st.deletePoint(0));

        assert.equal(st.count, 0);
    });

    it('deletePoint() should throw if the position is invalid', function() {
        const st = new SeekTableMetadata();

        assert.throws(() => st.deletePoint(0), /Point position is invalid/);
        st.insertPoint(0, new SeekPoint());
        assert.throws(() => st.deletePoint(91), /Point position is invalid/);
        assert.throws(() => st.deletePoint(-1), /Number -1 is negative/);
    });

    it('deletePoint() should throw if the position is not a number', function() {
        const st = new SeekTableMetadata();

        assert.throws(() => st.deletePoint(null), /Expected null to be number/);
    });

    it('templateAppendPlaceholders() should insert points at the end', function() {
        const st = new SeekTableMetadata();
        st.insertPoint(0, new SeekPoint(998877665544332211n));

        assert.isTrue(st.templateAppendPlaceholders(10));

        const points = Array.from(st);
        assert.equal(points.length, 11);
        assert.equal(points[0].sampleNumber, 998877665544332211n);
    });

    it('templateAppendPlaceholders() should throw if count is not a number', function() {
        const st = new SeekTableMetadata();

        assert.throws(() => st.templateAppendPlaceholders(null), /Expected null to be number/);
    });

    it('templateAppendPoint() should append a new point to the end', function() {
        const st = new SeekTableMetadata();
        st.insertPoint(0, new SeekPoint(998877665544332211n));

        assert.isTrue(st.templateAppendPoint(675n));

        const points = Array.from(st);
        assert.equal(points.length, 2);
        assert.equal(points[0].sampleNumber, 998877665544332211n);
        assert.equal(points[1].sampleNumber, 675n);
    });

    it('templateAppendPoint() should throw if sampleNumber is not a number', function() {
        const st = new SeekTableMetadata();

        assert.throws(() => st.templateAppendPoint(null), /Expected null to be number/);
    });

    it('templateAppendPoints() should append some new points to the end', function() {
        const st = new SeekTableMetadata();
        st.insertPoint(0, new SeekPoint(998877665544332211n));

        assert.isTrue(st.templateAppendPoints([ 675n, 879n, 213n ]));

        const points = Array.from(st);
        assert.equal(points.length, 4);
        assert.equal(points[0].sampleNumber, 998877665544332211n);
        assert.equal(points[1].sampleNumber, 675n);
        assert.equal(points[2].sampleNumber, 879n);
        assert.equal(points[3].sampleNumber, 213n);
    });

    it('templateAppendPoints() should throw if sampleNumbers is not an array', function() {
        const st = new SeekTableMetadata();

        assert.throws(() => st.templateAppendPoints(null), /Expected null to be Array/);
    });

    it('templateAppendPoints() should throw if sampleNumbers array contains a non-number value', function() {
        const st = new SeekTableMetadata();

        assert.throws(() => st.templateAppendPoints([ 1, false, 3 ]), /Expected false to be number/);
    });

    it('templateAppendSpacedPoints() should append some new points to the end', function() {
        const st = new SeekTableMetadata();
        st.insertPoint(0, new SeekPoint(123n));

        assert.isTrue(st.templateAppendSpacedPoints(5, 25n));

        const points = Array.from(st);
        assert.equal(points.length, 6);
        assert.equal(points[0].sampleNumber, 123n);
        assert.equal(points[1].sampleNumber, 5n * 0n);
        assert.equal(points[2].sampleNumber, 5n * 1n);
        assert.equal(points[3].sampleNumber, 5n * 2n);
        assert.equal(points[4].sampleNumber, 5n * 3n);
        assert.equal(points[5].sampleNumber, 5n * 4n);
    });

    it('templateAppendSpacedPoints() should throw if totalSamples is 0', function() {
        assert.throws(() => new SeekTableMetadata().templateAppendSpacedPoints(1, 0n), /Total samples is 0/);
    });

    it('templateAppendSpacedPoints() should throw if num is not a number', function() {
        assert.throws(() => new SeekTableMetadata().templateAppendSpacedPoints(null, 0n), /Expected null to be number/);
    });

    it('templateAppendSpacedPoints() should throw if totalSamples is not a number', function() {
        assert.throws(() => new SeekTableMetadata().templateAppendSpacedPoints(0, null), /Expected null to be number/);
    });

    it('templateAppendSpacedPointsBySamples() should append some new points to the end', function() {
        const st = new SeekTableMetadata();
        st.insertPoint(0, new SeekPoint(123n));

        assert.isTrue(st.templateAppendSpacedPointsBySamples(5, 25));

        const points = Array.from(st);
        assert.equal(points.length, 6);
        assert.equal(points[0].sampleNumber, 123n);
        assert.equal(points[1].sampleNumber, 5n * 0n);
        assert.equal(points[2].sampleNumber, 5n * 1n);
        assert.equal(points[3].sampleNumber, 5n * 2n);
        assert.equal(points[4].sampleNumber, 5n * 3n);
        assert.equal(points[5].sampleNumber, 5n * 4n);
    });

    it('templateAppendSpacedPointsBySamples() should throw if samples is 0', function() {
        assert.throws(() => new SeekTableMetadata().templateAppendSpacedPointsBySamples(0, 25n), /Samples is 0/);
    });

    it('templateAppendSpacedPointsBySamples() should throw if totalSamples is 0', function() {
        assert.throws(() => new SeekTableMetadata().templateAppendSpacedPointsBySamples(5, 0n), /Total samples is 0/);
    });

    it('templateAppendSpacedPointsBySamples() should throw if num is not a number', function() {
        assert.throws(() => new SeekTableMetadata().templateAppendSpacedPointsBySamples(null, 0n), /Expected null to be number/);
    });

    it('templateAppendSpacedPointsBySamples() should throw if totalSamples is not a number', function() {
        assert.throws(() => new SeekTableMetadata().templateAppendSpacedPointsBySamples(0, null), /Expected null to be number/);
    });

    it('templateSort() sorts the points', function() {
        const st = new SeekTableMetadata();
        st.insertPoint(0, new SeekPoint(123n));
        st.templateAppendSpacedPointsBySamples(5, 25);
        st.insertPoint(4, new SeekPoint(500n));
        st.insertPoint(7, new SeekPoint(5n, 5n));

        assert.isTrue(st.templateSort());

        const points = Array.from(st);
        assert.equal(points.length, 8);
        assert.equal(points[0].sampleNumber, 5n * 0n);
        assert.equal(points[1].sampleNumber, 5n * 1n);
        assert.equal(points[2].sampleNumber, 5n * 2n);
        assert.equal(points[3].sampleNumber, 5n * 3n);
        assert.equal(points[4].sampleNumber, 5n * 4n);
        assert.equal(points[5].sampleNumber, 123n);
        assert.equal(points[6].sampleNumber, 500n);
        assert.equal(points[7].sampleNumber, 18446744073709551615n);
    });

    it('templateSort(true) sorts the points and removes duplicates', function() {
        const st = new SeekTableMetadata();
        st.insertPoint(0, new SeekPoint(123n));
        st.templateAppendSpacedPointsBySamples(5, 25);
        st.insertPoint(4, new SeekPoint(500n));
        st.insertPoint(7, new SeekPoint(5n, 5n));

        assert.isTrue(st.templateSort(true));

        const points = Array.from(st);
        assert.equal(points.length, 7);
        assert.equal(points[0].sampleNumber, 5n * 0n);
        assert.equal(points[1].sampleNumber, 5n * 1n);
        assert.equal(points[2].sampleNumber, 5n * 2n);
        assert.equal(points[3].sampleNumber, 5n * 3n);
        assert.equal(points[4].sampleNumber, 5n * 4n);
        assert.equal(points[5].sampleNumber, 123n);
        assert.equal(points[6].sampleNumber, 500n);
    });

    it('isLegal() should work', function() {
        const st = new SeekTableMetadata();
        assert.isTrue(st.isLegal());
    });

    describe('gc', function() {
        it('gc should work', function() {
            require('../helper/gc')();
        });
    });
});
