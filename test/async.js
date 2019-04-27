/// <reference path="../lib/index.d.ts" />
const { testAsync } = require('../lib/index').api;
const { assert } = require('chai');

const progressValues = [ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' ];

describe('async', function() {

    describe('AsyncBackgroundTask', function() {
        it('should resolve', function(done) {
            testAsync('resolve', (c) => c, done);
        });

        it('should reject', function(done) {
            testAsync('reject', (c) => c, (err) => {
                if(err) done();
                else done('Expected task to fail');
            });
        });

        it('resolve should send the end result in the callback', function(done) {
            testAsync('resolve', c => c, (err, res) => {
                if(err) {
                    return done(err);
                }

                assert.isTrue(res);
                done();
            });
        });

        it('should reject if an internal exception is thrown', function(done) {
            testAsync('exception', c => c, (err) => {
                if(err) done();
                else done('Expected task to fail');
            });
        });

        ['resolve', 'reject', 'exception'].forEach(function(endMode) {
            it('should call the process callback with all the values using end mode ' + endMode, function(done) {
                const values = [];
                testAsync(endMode, (c) => values.push(c), () => {
                    assert.deepEqual(values, progressValues);
                    done();
                });
            });

            it('should reject if the progress callback throws an exception end mode ' + endMode, function(done) {
                testAsync(endMode, (c) => { throw new Error(c); }, (err) => {
                    if(err) done();
                    else done('Expected task to fail');
                });
            });
        });
    });

    describe('PromisifiedAsyncBackgroundTask', function() {
        it('should resolve', async function() {
            await testAsync('resolve', (c) => c);
        });

        it('should reject', async function() {
            try {
                await testAsync('reject', (c) => c);
            } catch (e) {
                return;
            }
            throw new Error('Expected task to fail');
        });

        it('resolve should send the end result in the callback', async function() {
            assert.isTrue(await testAsync('resolve', c => c));
        });

        it('should reject if an internal exception is thrown', async function() {
            try {
                await testAsync('exception', c => c);
            } catch(e) {
                return;
            }
            throw new Error('Expected task to fail');
        });

        ['resolve', 'reject', 'exception'].forEach(function(endMode) {
            it('should call the process callback with all the values using end mode ' + endMode, async function() {
                const values = [];
                try { await testAsync(endMode, (c) => values.push(c)); } catch(e) {}
                assert.deepEqual(values, progressValues);
            });

            it('should reject if the progress callback throws an exception end mode ' + endMode, async function() {
                try {
                    await testAsync(endMode, (c) => { throw new Error(c); });
                } catch(e) {
                    return;
                }
                throw new Error('Expected task to fail');
            });
        });
    });

});