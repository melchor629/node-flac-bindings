import { fileURLToPath } from 'url'
import bindings from 'bindings'

const moduleRoot = bindings.getRoot(fileURLToPath(import.meta.url))
export const {
  _testAsync,
  napiVersion,
  EncoderBuilder,
  Encoder,
  DecoderBuilder,
  Decoder,
  format,
  metadata,
  metadata0,
  SimpleIterator,
  Chain,
  Iterator,
  fns,
} = bindings({ bindings: 'flac-bindings.node', module_root: moduleRoot })
