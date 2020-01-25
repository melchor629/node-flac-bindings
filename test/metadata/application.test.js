/* eslint-disable prefer-arrow-callback */
const { ApplicationMetadata } = require('../../lib/index').api.metadata;
const { MetadataType } = require('../../lib/index').api.format;
const { assert } = require('chai');

describe('ApplicationMetadata', function() {
    it('new object has the right type', function() {
        const am = new ApplicationMetadata();

        assert.equal(am.type, MetadataType.APPLICATION);
    });

    it('set id of object should work', function() {
        const am = new ApplicationMetadata();

        am.id = Buffer.from('1234');
        assert.deepEqual(am.id, Buffer.from('1234'));
    });

    it('set id of object having less than 4 bytes should throw', function() {
        const am = new ApplicationMetadata();

        assert.throws(() => am.id = Buffer.from('si'));
    });

    it('set id of object having more than 4 bytes should work but discard the rest', function() {
        const am = new ApplicationMetadata();

        am.id = Buffer.from('1234===-><-');
        assert.deepEqual(am.id, Buffer.from('1234'));
    });

    it('set data of object should work', function() {
        const am = new ApplicationMetadata();

        am.data = Buffer.from('this is a wonderful data that is useless per se, pero bueno');
        assert.deepEqual(am.data, Buffer.from('this is a wonderful data that is useless per se, pero bueno'));
    });

    describe('gc', function() {
        it('gc should work', function() {
            require('../helper/gc')();
        });
    });
});
