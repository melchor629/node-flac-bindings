import api = require('./api');
import encoder = require('./encoder');
import decoder = require('./decoder');

declare interface FlacBindings {
  api: typeof api,
  StreamEncoder: typeof encoder.StreamEncoder,
  FileEncoder: typeof encoder.FileEncoder,
  StreamDecoder: typeof decoder.StreamDecoder,
  FileDecoder: typeof decoder.FileDecoder,
}

declare const FlacBindings: FlacBindings;
export = FlacBindings;

declare module 'flac-bindings' {
  const FlacBindings: FlacBindings;
  export = FlacBindings;
}
