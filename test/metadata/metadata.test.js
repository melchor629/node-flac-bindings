/* eslint-disable prefer-arrow-callback */
/// <reference path="../../lib/index.d.ts" />
const {
    ApplicationMetadata,
    Metadata,
} = require('../../lib/index').api.metadata;
const { assert } = require('chai');

describe('Metadata', function() {
    it('create metadata object should throw', function() {
        assert.throws(() => new Metadata(1), /Metadata is not a constructor/);
    });

    it('get isLast should work', function() {
        const m = new ApplicationMetadata();

        assert.isFalse(m.isLast);
    });

    it('isEqual() returns true if the objects are similar', function() {
        const am1 = new ApplicationMetadata();
        const am2 = new ApplicationMetadata();
        am1.data = am2.data = Buffer.from('no');
        am1.id = am2.id = Buffer.from('node');

        assert.isTrue(am1.isEqual(am2));
        assert.isTrue(am2.isEqual(am1));
    });

    it('isEqual() returns false if the objects are different', function() {
        const am1 = new ApplicationMetadata();
        const am2 = new ApplicationMetadata();
        am1.data = am2.id = Buffer.from('nodo');
        am1.id = am2.data = Buffer.from('node');

        assert.isFalse(am1.isEqual(am2));
        assert.isFalse(am2.isEqual(am1));
    });

    it('isEqual() throws if first argument is not provided', function() {
        assert.throws(() => new ApplicationMetadata().isEqual(), /Expected undefined to be object/);
    });

    it('isEqual() throws if first argument is not Metadata object', function() {
        assert.throws(() => new ApplicationMetadata().isEqual({}), /Expected \[object Object\] to be an instance of Metadata/);
    });

    it('clone() should create a different object but equal', function() {
        const am1 = new ApplicationMetadata();
        am1.data = Buffer.from('nodo');
        am1.id = Buffer.from('node');

        const am2 = am1.clone();

        assert.isTrue(am1.isEqual(am2));
        assert.isTrue(am2.isEqual(am1));
        assert.isFalse(Object.is(am1, am2));
    });

    describe('gc', function() {
        it('gc should work', function() {
            require('../helper/gc')();
        });
    });
});
