# flac-bindings

Nodejs bindings to [libFLAC](https://github.com/xiph/flac)

![Node-API v8](https://raw.githubusercontent.com/nodejs/abi-stable-node/doc/assets/Node-API%20v8%20Badge.svg)
![Node CI](https://github.com/melchor629/node-flac-bindings/workflows/Node%20CI/badge.svg?branch=dev)
[![Coverage Status](https://coveralls.io/repos/github/melchor629/node-flac-bindings/badge.svg?branch=dev)](https://coveralls.io/github/melchor629/node-flac-bindings?branch=dev)

## How to install

```
$ npm install flac-bindings

$ yarn add flac-bindings
```

This is a pure ESM package. If you get an error like `require() of ES Module`, then [read this gist](https://gist.github.com/sindresorhus/a39789f98801d908bbc7ff3ecc99d99c). If you still want to use CJS, then either import using `require('flac-bindings')`, use dynamic import `import('flac-bindings')`, or use any of the `2.7.x` versions.

The library has some native code that binds the JS code to the flac library. Depending on your platform, an already-compiled library is available for you. The following logic applies:

- If your CPU is `amd64`/`x86_64` and the OS is `Linux/glibc`, `Linux/musl`, `macOS` or `Windows`, and node version is in the [N-API compatibility table](https://nodejs.org/dist/latest-v16.x/docs/api/n-api.html#n_api_node_api_version_matrix) for v8, then the prebuild version will download.
- If your CPU is `arm64`/`aarch64` and the OS is `Linux/glibc`, `Linux/musl` or `macOS`, and node version is in the aformentioned compatibility table for v8, then the prebuild version will download.
- If you have `pkg-config` and `libFLAC` development package installed (`apt install libflac-dev`, `pacman -S flac`, `apk add flac-dev`, `brew install flac`...), then it will use this library and only compile the binding code. Requires you to also have [Cmake](https://www.cmake.org) installed.
- In any other case, it will download `libogg` and `libFLAC` source code and compile both libraries plus the binding code. Requires you to also have [Cmake](https://www.cmake.org) and `git` installed.

See [How to compile](#how-to-compile) section for more information.

## How to use

For use it, include with

```javascript
// ESM import (Streams and native API alias)
import {
  api,
  FileDecoder,
  StreamDecoder,
  FileEncoder,
  StreamEncoder,
} from 'flac-bindings';

// ESM import for native API
import {
  Encoder,
  Decoder,
  format,
  metadata,
  metadata0,
  SimpleIterator,
  Chain,
  Iterator,
  fns,
} from 'flac-bindings/api';
```

> **Note**: _this library has its own TypeScript typings, so it's possible to use it in a TS project and have the right types_

> **Note**: _this library is written in ES Modules, and cannot be imported from CommonJS modules - see [this](https://gist.github.com/sindresorhus/a39789f98801d908bbc7ff3ecc99d99c)_

## Examples

Here's an example of using `flac-bindings` to encode some raw PCM data coming from `process.stdin` to a FLAC file that gets piped to `process.stdout`:

```js
import { StreamEncoder } from 'flac-bindings';

// create the Encoder instance
const encoder = new StreamEncoder({
  channels: 2,        // 2 channels (left and right)
  bitsPerSample: 16,  // 16-bit samples
  samplerate: 44100,  // 44,100 Hz sample rate

  compressionLevel: 7,
});

// raw PCM data from stdin gets piped into the encoder
process.stdin.pipe(encoder);

// the generated FLAC file gets piped to stdout
encoder.pipe(process.stdout);
```

See [examples](https://github.com/melchor629/node-flac-bindings/tree/dev/examples/) for more examples. See the [tests](https://github.com/melchor629/node-flac-bindings/tree/dev/test/) directory for even more examples using advanced API.

## How it works

The module is be divided in various sections:

- [StreamEncoder](https://github.com/melchor629/node-flac-bindings/blob/dev/lib/encoder/index.d.ts) - a `stream.Transform` class for encoding raw PCM streams
- [FileEncoder](https://github.com/melchor629/node-flac-bindings/blob/dev/lib/encoder/index.d.ts) - a `stream.Writable` class for encoding raw PCM streams into a file
- [StreamDecoder](https://github.com/melchor629/node-flac-bindings/blob/dev/lib/decoder/index.d.ts) - a `stream.Transform` class for decoding FLAC into a PCM stream
- [FileDecoder](https://github.com/melchor629/node-flac-bindings/blob/dev/lib/decoder/index.d.ts) - a `stream.Readable` class for decoding FLAC file into a PCM stream
- `api` - the native bindings, easy to use from JS side
    - [format](https://xiph.org/flac/api/group__flac__format.html) - includes only the functions and some types
    - [Encoder](https://xiph.org/flac/api/group__flac__encoder.html) - the `StreamEncoder` API
    - [Decoder](https://xiph.org/flac/api/group__flac__decoder.html) - the `StreamDecoder` API
    - [metadata](https://xiph.org/flac/api/group__flac__metadata__object.html) - includes the `StreamMetadata` classes and their methods
    - [metadata0](https://xiph.org/flac/api/group__flac__metadata__level0.html) - metadata level 0 APIs
    - [SimpleIterator](https://xiph.org/flac/api/group__flac__metadata__level1.html) - metadata level 1 iterator class
    - [Chain](https://xiph.org/flac/api/group__flac__metadata__level2.html) - metadata level 2 Chain class
    - [Iterator](https://xiph.org/flac/api/group__flac__metadata__level2.html) - metadata level 2 Iterator class

The package includes [typings](https://github.com/melchor629/node-flac-bindings/blob/dev/lib/index.d.ts) that could help you :)

All memory is managed by the library. But there are some methods that give you references to objects that can be destroyed before the JS object. These methods are documented with such special behaviour.

Almost every function/method expects his parameters in his right type (as in the FLAC documentation). If it not, an JS exception will be thrown, or in the worst scenario, a crash (_report an issue if this happens_). So, pay attention on the types of the functions (the typings are just to help you 😀).

Callbacks don't follow exactly the same signature that shows in Encoder and Decoder sections (from the FLAC documentation). They don't need some of the parameters as in JS there are other ways to get the encoder/decoder instance and some context. The init functions don't receive any private data.

There are asynchronous functions and methods for IO bound tasks. The syncrhonous API will be faster, but will block node. If you are writing an server or expect high concurrency, use the asynchronous API.

You need node version that supports v8 N-API ([see compatibility table](https://nodejs.org/docs/latest-v16.x/api/n-api.html#n_api_node_api_version_matrix)), which is supported in node v14.17.0/v16.0.0 or higher. Recommended use of `BigInt` when possible to have numbers be represented without truncation (`Number` can only store 53 bit integers! 🤨).

> **Note**: Buffers from `Encoder`, `Decoder` and `IO Callbacks` (metadata level 2) have a strict lifetime: buffers are ensured to be valid inside the callback itself, if the buffer must be used outside the callback, make a copy.

## Debug the library

When using `StreamEncoder`, `StreamDecoder`, `FileEncoder` or `FileDecoder`, and something does not work properly, you can enable verbose/debug logs by defining the environment variable `DEBUG=flac:*` (see [debug](https://www.npmjs.com/package/debug) package for more information). Each class has its own namespace, so you can enable debug logs only for some of them. See below the list of namespaces:

- `StreamEncoder`: `flac:encoder:stream`
- `FileEncoder`: `flac:encoder:file`
- `StreamDecoder`: `flac:decoder:stream`
- `FileDecoder`: `flac:decoder:file`
- The `post-install` script: `flac:build`

> !! These logs can be useful when creating a new issue.

You can also try to debug the native code by setting up a test JS file and launching the debugger with the `node` executable and arguments the script itself.

## How to compile

To compile the bindings you need [Cmake](https://www.cmake.org) installed in your system and accessible from the terminal, and the C and C++ compilers as well. On Windows, the compilers can be installed easily with `npm install --global --production windows-build-tools`. Don't forget `git`. It is mandatory!

There are some options to use when compiling. The build tries to use an already installed compatible dev package of `libflac` (`apt install libflac-dev`, `pacman -S flac`, `apk add flac-dev`, `brew install flac`...), but with `FLAC_BINDINGS_USE_FLAC_SOURCES` will force to download the sources and compile everything from there.

> Supported `libFLAC` versions are 1.3.x and 1.4.x (binary versions 10 and 12).

Then, you just need to recompile the package with: `npm rebuild flac-bindings`. If you are inside this repo tree, then run `npm run install`.

For more advanced commands for compilation inside the repo tree, see below:

```sh
# Compile (debug version)
# -p -> If desired, tell cmake to run with parallel jobs (faster)
npx cmake-js build --debug -p 4

# Compile with sanitizers (only available on Linux and macOS)
npx cmake-js configure --CDSANITIZE=ON --debug -p 4
npx cmake-js build --debug -p 4

# Force-compile with FLAC sources (can be combined with sanitizers)
npx cmake-js configure --CDUSE_FLAC_SOURCES --debug -p 4
npx cmake-js build --debug -p 4

# Clean compilation folder
npx cmake-js clean
```

## How to run the tests

With a dev environment, and being able to compile the project, ensure to have installed the `flac` CLI (`apt install flac`, `pacman -S flac`, `apk add flac`, `brew install flac`...) and present in the `$PATH`. It is recommended to have installed the FLAC dev package. Also ensure to have installed [Cmake](https://www.cmake.org) and available in the `$PATH`.

The recommended steps are:

```sh
# Do not run tests with sanitizers enabled, it's tricky to make it work
npx cmake-js configure --debug
npx cmake-js build --debug -p 4
npm test

# To run tests with coverage (requires lcov to be installed)
scripts/coverage.sh rebuild # first time
script/coverage.sh          # next times
```

Happy `npm test` runs :)

## The API

See the [wiki](https://github.com/melchor629/node-flac-bindings/wiki) for the documentation.
