import { existsSync } from 'node:fs'
import { createRequire } from 'node:module'
import { dirname, join } from 'node:path'
import { fileURLToPath } from 'node:url'
import debug from 'debug'

const log = debug('flac:native')
const moduleRoot = dirname(dirname(fileURLToPath(import.meta.url)))
const libraryPath = ['build/Debug', 'build/Release']
  .map((p) => join(moduleRoot, p, 'flac-bindings.node'))
  .find((p) => existsSync(p))

log('Module root path:', moduleRoot)
log('Library path:', libraryPath ?? '<not found>')
if (!libraryPath) {
  throw new Error(
    'Could not find the the native addon. Please ensure the install script has run or run it manually.',
  )
}

const require = createRequire(import.meta.url)
export const {
  _coverageFlush,
  _testAsync,
  Chain,
  Decoder,
  DecoderBuilder,
  Encoder,
  EncoderBuilder,
  Iterator,
  SimpleIterator,
  fns,
  format,
  metadata,
  metadata0,
  napiVersion,
} = require(libraryPath)
