/* eslint-disable prefer-arrow-callback */
const { CueSheetIndex } = require('../../lib/index').api.metadata;
const { assert } = require('chai');

describe('CueSheetIndex', function() {
    it('create a new object with parameters should work', function() {
        const csi = new CueSheetIndex(10, 0);

        assert.equal(csi.offset, 10);
        assert.equal(csi.number, 0);
    });

    it('create a new object with offset not being a number should throw', function() {
        assert.throws(() => new CueSheetIndex(null, 0), /Expected null to be number/);
    });

    it('create a new object with number not being a number should throw', function() {
        assert.throws(() => new CueSheetIndex(1, null), /Expected null to be number/);
    });

    it('use of bigint in offset should work', function() {
        const csi = new CueSheetIndex();

        csi.offset = 9007199254740993n;

        assert.equal(csi.offset, 9007199254740993n);
    });

    it('set offset is not a number should throw', function() {
        const csi = new CueSheetIndex();

        assert.throws(() => csi.offset = null);
    });

    it('set number should work', function() {
        const csi = new CueSheetIndex();

        csi.number = 10;

        assert.equal(csi.number, 10);
    });

    it('set number is not a number should throw', function() {
        const csi = new CueSheetIndex();

        assert.throws(() => csi.number = null);
    });

    describe('gc', function() {
        it('gc should work', function() {
            require('../helper/gc')();
        });
    });
});
