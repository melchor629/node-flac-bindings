# flac-bindings

Nodejs bindings to [libFLAC](https://xiph.org/flac/download.html)

| master | dev |
|--------|-----|
|![Node CI](https://github.com/melchor629/node-flac-bindings/workflows/Node%20CI/badge.svg?branch=master)|![Node CI](https://github.com/melchor629/node-flac-bindings/workflows/Node%20CI/badge.svg?branch=dev)|
|[![Coverage Status](https://coveralls.io/repos/github/melchor629/node-flac-bindings/badge.svg?branch=master)](https://coveralls.io/github/melchor629/node-flac-bindings?branch=master)|[![Coverage Status](https://coveralls.io/repos/github/melchor629/node-flac-bindings/badge.svg?branch=dev)](https://coveralls.io/github/melchor629/node-flac-bindings?branch=dev)|

## What can this binding do for me?

You can use all the functions from encoder and decoder modules inside Javascript with struct-to-js (and viceversa) conversions. The FLAC library will load dynamically on runtime or with some help, you can tell where the library is. Also, any chunk of data that the FLAC API needs is solved by a simple node Buffer. And it has some beautiful js classes for encoder and decoder too.

See the [FLAC API](https://xiph.org/flac/api/group__flac.html)? You can use it with a very intuitive form: almost equal.

## How it works?

First, tries to open the library with the usual paths in the system. If it fails, the module will provide you a `load` function, pass to it a **full** path to the library and it will load. The module will be divided in various sections:

 - [format](https://xiph.org/flac/api/group__flac__format.html) - includes only the functions and some types
 - [Encoder](https://xiph.org/flac/api/group__flac__encoder.html)
 - [Decoder](https://xiph.org/flac/api/group__flac__decoder.html)
 - [metadata](https://xiph.org/flac/api/group__flac__metadata__object.html) - includes the `StreamMetadata` structs and their methods.
 - [metadata0](https://xiph.org/flac/api/group__flac__metadata__level0.html)
 - [SimpleIterator](https://xiph.org/flac/api/group__flac__metadata__level1.html)
 - [Chain](https://xiph.org/flac/api/group__flac__metadata__level2.html)
 - [Iterator](https://xiph.org/flac/api/group__flac__metadata__level2.html)

The package includes [typings](https://github.com/melchor629/node-flac-bindings/blob/master/lib/index.d.ts) that could help you :)

The Encoder, Decoder, the metadata level 1 `SimpleIterator`, the metadata level 2 `Chain` and `Iterator` and `StreamMetadata` structs are classes that can be instantiated like usual JS classes. The constructor will create the underlying pointer to the object un the C API, and when the GC cleans up the objects, the pointer will be free'd as well.

## Things to take into account

Almost every function/method expects his parameters in his right type. If it not, node will crash or an JS exception will be thrown. So, pay attention on the types of the functions (the typings are just to help you 😀).

Callbacks don't follow exactly the signature that shows in Encoder and Decoder sections, they don't need some of the parameters as in JS there are other ways to get the encoder/decoder instance and some context. The init functions don't receive any private data.

You need node 10 or higher. Recommended use of `BigInt` when possible to have numbers be represented without truncation (`Number` can only store 53 bit integers! 🤨).

## How to install

```
$ npm install flac-bindings

$ yarn add flac-bindings
```

During installation, if the platform has a prebuild version of the bindings, it will download and install it. In case not, then it will look for FLAC dev package if installed and build the bindings with that package. But, if you don't have FLAC dev package installed, then it will download the FLAC sources and compile them alongisde the bindings. Note that prebuild versions and FLAC+bindings compilation version does not have enabled OGG support. See [How to compile][#how-to-compile] section for more information.

For use it, include with

```javascript
const flac = require('flac-bindings');  // default node import
import flac from 'flac-bindings';       // ES6 import
import * as flac from 'flac-bindings';  // TypeScript import
```

`flac` will be an object with `{ api: [Bindings API], StreamEncoder: ..., StreamDecoder: ..., FileEncoder: ..., FileDecoder: ... }`.

## How to compile

To compile the bindings you need [Cmake][https://www.cmake.org] installed in your system and accessible from the terminal, and the C and C++ compilers as well. On Windows, the compilers can be installed easily with `npm install --global --production windows-build-tools`.

There are some options to use when compiling. As mentioned before, Ogg support is disabled by default. If you would like to enable it, define the environment variable `FLAC_BINDINGS_ENABLE_OGG`. The development package of Ogg must be installed (`apt install libogg-dev`, `pacman -S libogg`, `apk add libogg-dev`, `brew install libogg`...). On Windows, this can be tricker, so compile and install the package somewhere and ensure to add the path of the `lib/cmake` from the installation in the `CMAKE_MODULE_PATH`.

On the other hand, if you have a FLAC dev package already installed (`apt install libflac-dev`, `pacman -S flac`, `apk add flac-dev`, `brew install flac`...) and you want to compile the bindings to use this library instead, define the environment variable `FLAC_BINDINGS_USE_EXTERNAL_LIBRARY` (this will be the default option if no prebuild version is available, but a dev package is available).

> **Note**: _Both environment variables will discard the prebuild version._

## How to run the tests

With a dev environment, and being able to compile the project, ensure to have installed the `flac` CLI (`apt install flac`, `pacman -S flac`, `apk add flac`, `brew install flac`...) and present in the `$PATH`. It is recommended to have installed the FLAC dev package and to have configured the project with `--CDFLAC_BINDINGS_USE_EXTERNAL_LIBRARY=ON`.

The recommended steps are:

```sh
npm run cmake-js -- configure --CDFLAC_BINDINGS_USE_EXTERNAL_LIBRARY=ON --debug
npm run cmake-js -- build --debug
npm test

# To run tests with coverage (requires lcov to be installed)
scripts/coverage.sh rebuild # first time
script/coverage.sh          # next times
```

Happy `npm test` runs :)

## The API

See the [wiki](https://github.com/melchor629/node-flac-bindings/wiki) for the documentation.
