/* eslint-disable prefer-arrow-callback */
/// <reference path="../../lib/index.d.ts" />
const { UnknownMetadata } = require('../../lib/index').api.metadata;
const { MetadataType } = require('../../lib/index').api.format;
const { assert } = require('chai');

describe('UnknownMetadata', function() {
    it('create new object should work', function() {
        new UnknownMetadata();
    });

    it('object has the right type', function() {
        const st = new UnknownMetadata();

        assert.equal(st.type, MetadataType.UNDEFINED);
    });

    it('object with custom type has that type', function() {
        const st = new UnknownMetadata(MetadataType.UNDEFINED + 5);

        assert.equal(st.type, MetadataType.UNDEFINED);
    });

    it('object with valid metadata type should have UNDEFINED type', function() {
        const st = new UnknownMetadata(MetadataType.CUESHEET);

        assert.equal(st.type, MetadataType.UNDEFINED);
    });

    it('object with a really high type should have maximum valid number as type', function() {
        const st = new UnknownMetadata(MetadataType.MAX_METADATA_TYPE * 38);

        assert.equal(st.type, MetadataType.UNDEFINED);
    });

    it('data is a buffer', function() {
        const st = new UnknownMetadata();

        assert.isTrue(Buffer.isBuffer(st.data) || st.data === null);
    });

    describe('gc', function() {
        it('gc should work', function() {
            require('../helper/gc')();
        });
    });
});
