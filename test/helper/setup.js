/* eslint-disable import-x/no-extraneous-dependencies */
import * as matchers from 'jest-extended'
import { afterAll, expect } from 'vitest'

expect.extend(matchers)

afterAll(async () => {
  // force flush code coverage from C++ code
  const { _coverageFlush } = await import('../../lib/api.js')
  _coverageFlush?.()
})
