import { createRequire } from 'node:module'
import debug from 'debug'
import detectLibc from 'detect-libc'

const log = debug('flac:native')
const require = createRequire(import.meta.url)

const libc = detectLibc.familySync()
const precompiledPackageName = `@melchor629/flac-bindings-lib-${process.platform}${libc ? `-${libc}` : ''}-${process.arch}`
const libPackageName = '@melchor629/flac-bindings-lib'
/** @type {import('@melchor629/flac-bindings-lib') | null} */
let lib = null
try {
  log(`Trying to load library package ${libPackageName}`)
  lib = require(libPackageName)
} catch (e) {
  log(`Failed to load library package ${libPackageName}: ${e.message}`)
}
try {
  log(`Trying to load precompiled package ${precompiledPackageName}`)
  lib = require(precompiledPackageName)
} catch (e) {
  log(`Failed to load library package ${precompiledPackageName}: ${e.message}`)
}
if (lib == null) {
  throw new Error(`Could not find any pre-compiled package. Please install '${libPackageName}' with all required elements to compile the code.`)
}

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
} = lib
