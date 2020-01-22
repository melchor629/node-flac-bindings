/* eslint-disable prefer-arrow-callback */
/// <reference path="../../lib/index.d.ts" />
const { PictureMetadata } = require('../../lib/index').api.metadata;
const { MetadataType } = require('../../lib/index').api.format;
const { assert } = require('chai');

describe('PictureMetadata', function() {
    it('create new object should work', function() {
        new PictureMetadata();
    });

    it('object has the right type', function() {
        const p = new PictureMetadata();

        assert.equal(p.type, MetadataType.PICTURE);
    });

    it('pictureType should change', function() {
        const p = new PictureMetadata();

        p.pictureType = 2;

        assert.equal(p.pictureType, 2);
    });

    it('pictureType should throw if not a number', function() {
        const p = new PictureMetadata();

        assert.throws(() => p.pictureType = null);
    });

    it('width should change', function() {
        const p = new PictureMetadata();

        p.width = 100;

        assert.equal(p.width, 100);
    });

    it('width should throw if not a number', function() {
        const p = new PictureMetadata();

        assert.throws(() => p.width = null);
    });

    it('height should change', function() {
        const p = new PictureMetadata();

        p.height = 100;

        assert.equal(p.height, 100);
    });

    it('height should throw if not a number', function() {
        const p = new PictureMetadata();

        assert.throws(() => p.height = null);
    });

    it('depth should change', function() {
        const p = new PictureMetadata();

        p.depth = 32;

        assert.equal(p.depth, 32);
    });

    it('depth should throw if not a number', function() {
        const p = new PictureMetadata();

        assert.throws(() => p.depth = null);
    });

    it('colors should change', function() {
        const p = new PictureMetadata();

        p.colors = 1;

        assert.equal(p.colors, 1);
    });

    it('colors should throw if not a number', function() {
        const p = new PictureMetadata();

        assert.throws(() => p.colors = null);
    });

    it('change mimeType should work', function() {
        const p = new PictureMetadata();

        assert.equal(p.mimeType, '');
        p.mimeType = 'image/jpg';
        assert.equal(p.mimeType, 'image/jpg');
    });

    it('change description should work', function() {
        const p = new PictureMetadata();

        assert.equal(p.description, '');
        p.description = 'Big description';
        assert.equal(p.description, 'Big description');
    });

    it('change data should work', function() {
        const p = new PictureMetadata();

        assert.isNull(p.data);
        p.data = Buffer.from('\x89PNG');
        assert.deepEqual(p.data, Buffer.from('\x89PNG'));
    });

    it('change data to empty buffer should work', function() {
        const p = new PictureMetadata();

        assert.isNull(p.data);
        p.data = Buffer.alloc(0);
        assert.isNull(p.data);
    });

    it('change data to null should work', function() {
        const p = new PictureMetadata();

        assert.isNull(p.data);
        p.data = null;
        assert.isNull(p.data);
    });

    it('isLegal() should work', function() {
        const p = new PictureMetadata();

        assert.isNull(p.isLegal());
    });

    describe('gc', function() {
        it('gc should work', function() {
            require('../helper/gc')();
        });
    });
});
