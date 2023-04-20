import { describe, expect, it } from 'vitest'
import { _testAsync as testAsync } from '../lib/api.js'

const progressValues = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9']

describe('async', () => {
  describe('asyncBackgroundTask', () => {
    it('should resolve', async () => {
      await expect(testAsync('resolve', (c) => c)).resolves.toBeTrue()
    })

    it('should reject', async () => {
      await expect(() => testAsync('reject', (c) => c)).rejects.toThrow()
    })

    it('resolve should send the end result in the callback', async () => {
      await expect(testAsync('resolve', (c) => c)).resolves.toBeTrue()
    })

    it('should reject if an internal exception is thrown', async () => {
      await expect(() => testAsync('exception', (c) => c)).rejects.toThrow()
    })

    describe.each(['resolve', 'reject', 'exception'])('%s', (endMode) => {
      it('should call the process callback with all the values using end mode', async () => {
        const values = []
        try {
          await testAsync(endMode, (c) => values.push(c))
        } catch (e) {
          // nothing to do here
        }
        expect(values).toStrictEqual(progressValues)
      })

      it('should call the process callback, waiting for it, with all the values using end mode', async () => {
        const values = []
        try {
          await testAsync(endMode, (c) => Promise.resolve(values.push(c)))
        } catch (e) {
          // nothing to do here
        }
        expect(values).toStrictEqual(progressValues)
      })

      it('should reject if the progress callback throws an exception end mode', async () => {
        await expect(() => testAsync(endMode, (c) => {
          throw new Error(c)
        })).rejects.toThrow()
      })

      it('should reject if the progress callback rejects the promise end mode', async () => {
        await expect(() => testAsync(endMode, (c) => Promise.reject(new Error(c))))
          .rejects
          .toThrow()
      })
    })
  })
})
