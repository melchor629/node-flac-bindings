/* eslint-disable prefer-arrow-callback */
/// <reference path="../lib/index.d.ts" />
const { testAsync } = require('../lib/index').api;
const { assert, use } = require('chai');

use(require('./helper/async-chai-extensions.js'));
const progressValues = [ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' ];

describe('async', function() {

    describe('AsyncBackgroundTask', function() {
        it('should resolve', function(done) {
            testAsync('resolve', (c) => c, done);
        });

        it('should reject', function(done) {
            testAsync('reject', (c) => c, (err) => {
                if(err) {
                    done();
                } else {
                    done('Expected task to fail');
                }
            });
        });

        it('resolve should send the end result in the callback', function(done) {
            testAsync('resolve', (c) => c, (err, res) => {
                if(err) {
                    return done(err);
                }

                assert.isTrue(res);
                done();
            });
        });

        it('should reject if an internal exception is thrown', function(done) {
            testAsync('exception', (c) => c, (err) => {
                if(err) {
                    done();
                } else {
                    done('Expected task to fail');
                }
            });
        });

        [ 'resolve', 'reject', 'exception' ].forEach(function(endMode) {
            it(`should call the process callback with all the values using end mode ${endMode}`, function(done) {
                const values = [];
                testAsync(endMode, (c) => values.push(c), () => {
                    assert.deepEqual(values, progressValues);
                    done();
                });
            });

            it(`should call the process callback, waiting for it, with all the values using end mode ${endMode}`,
                function(done) {
                    const values = [];
                    testAsync(endMode, (c) => Promise.resolve(values.push(c)), () => {
                        assert.deepEqual(values, progressValues);
                        done();
                    });
                }
            );

            it(`should reject if the progress callback throws an exception end mode ${endMode}`, function(done) {
                testAsync(endMode, (c) => {
                    throw new Error(c);
                }, (err) => {
                    if(err) {
                        done();
                    } else {
                        done('Expected task to fail');
                    }
                });
            });

            it(`should reject if the progress callback rejects the promise end mode ${endMode}`, function(done) {
                testAsync(endMode, (c) => Promise.reject(new Error(c)), (err) => {
                    if(err) {
                        done();
                    } else {
                        done('Expected task to fail');
                    }
                });
            });
        });
    });

    describe('PromisifiedAsyncBackgroundTask', function() {
        it('should resolve', async function() {
            await testAsync('resolve', (c) => c);
        });

        it('should reject', async function() {
            await assert.throwsAsync(() => testAsync('reject', (c) => c));
        });

        it('resolve should send the end result in the callback', async function() {
            assert.isTrue(await testAsync('resolve', (c) => c));
        });

        it('should reject if an internal exception is thrown', async function() {
            await assert.throwsAsync(() => testAsync('exception', (c) => c));
        });

        [ 'resolve', 'reject', 'exception' ].forEach(function(endMode) {
            it(`should call the process callback with all the values using end mode ${endMode}`, async function() {
                const values = [];
                try {
                    await testAsync(endMode, (c) => values.push(c));
                // eslint-disable-next-line no-empty
                } catch(e) {}
                assert.deepEqual(values, progressValues);
            });

            it(`should call the process callback, waiting for it, with all the values using end mode ${endMode}`,
                async function() {
                    const values = [];
                    try {
                        await testAsync(endMode, (c) => Promise.resolve(values.push(c)));
                    // eslint-disable-next-line no-empty
                    } catch(e) {}
                    assert.deepEqual(values, progressValues);
                }
            );

            it(`should reject if the progress callback throws an exception end mode ${  endMode}`, async function() {
                await assert.throwsAsync(() => testAsync(endMode, (c) => {
                    throw new Error(c);
                }));
            });

            it(`should reject if the progress callback rejects the promise end mode ${  endMode}`, async function() {
                await assert.throwsAsync(() => testAsync(endMode, (c) => Promise.reject(new Error(c))));
            });
        });
    });

});