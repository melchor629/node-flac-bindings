# flac-bindings
Nodejs bindings to [libFLAC](https://xiph.org/flac/download.html)

[![Coverage Status](https://coveralls.io/repos/github/melchor629/node-flac-bindings/badge.svg?branch=master)](https://coveralls.io/github/melchor629/node-flac-bindings?branch=master)

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

The Encoder, Decoder, the metadata level 1 `SimpleIterator`, the metadata level 2 `Chain`  and `Iterator` and `StreamMetadata` structs are classes that can be instantiated like usual JS classes. The constructor will create the underlying pointer to the object un the C API, and when the GC cleans up the objects, the pointer will be free'd as well.

## Things to take into account
Almost every function/method expects his parameters in his right type. If it not, node will crash or an JS exception will be thrown. So, pay attention on the types of the functions (the typings are just to help you 😀).

Callbacks don't follow exactly the signature that shows in Encoder and Decoder sections, they don't need some of the parameters as in JS there are other ways to get the encoder/decoder instance and some context. The init functions don't receive any private data.

You need node 8 or higher. Recommended to have 10.x series with `BigInt` support to have the number represented the right way (without truncation - `Number` can only store 53 bit integers! 🤨).

## How to install
```
$ npm install flac-bindings

$ yarn add flac-bindings
```

For use it, include with

```javascript
const flac = require('flac-bindings');  // default node import
import flac from 'flac-bindings';       // ES6 import
import * as flac from 'flac-bindings';  // TypeScript import
```

`flac` will be an object with `{ api: [Bindings API], StreamEncoder: ..., StreamDecoder: ..., FileEncoder: ..., FileDecoder: ... }`. If the `libFLAC` library is not in the loader's path, you will get an object with a load function (`{ load: [Function load] }`). You must call `load()` with the first argument as the **full** path to the `libFLAC` dynamic library, and then `flac` (the object) will have all objects. You can also use the environment variable `FLAC_LIBRARY` to set a path to the library (it won't throw any exception if it fails).

## What I need to compile the bindings?
Well, if you are on Linux x64 or on macOS and have node 8 LTS, 10 LTS or 12, you don't need to compile anything, I provide the binding binaries for you. You only need to provide the FLAC library binary.

In other case, you will need to install the development version of FLAC (those which includes the headers and the library binary). Also, you need to install `cmake` (no `node-gyp` nor `python-2` required).

If you are using Windows, and there's no binaries compiled for you, then you will need to install the build tools first (with **admin privileges**):

```
npm install --global --production windows-build-tools
```

After that, you need to compile (or obtain from elsewhere) the `libFLAC.dll`. If you compile it, you need also libogg (see README from flac). Ensure that the library is compiled/downloaded for 64bit because node uses 64bit for Windows in 64 bit. If you try to use 32bit library on 64bit node, loading will fail.

## The API
See the [wiki](https://github.com/melchor629/node-flac-bindings/wiki) for the documentation.
