import { fileURLToPath } from 'node:url'
import bindings from 'bindings'

const moduleRoot = bindings.getRoot(fileURLToPath(import.meta.url))
export const {
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
} = bindings({ bindings: 'flac-bindings.node', module_root: moduleRoot })
