# flac-bindings
Nodejs bindings to [libFLAC](https://xiph.org/flac/download.html)

## What can this binding do for me?
You can use all the functions from encoder and decoder modules inside Javascript with struct-to-js (and viceversa) conversions. You don't need to have installed (or `LD_PATH`-visible) libFLAC to compile the binding, it will load dynamically on runtime or you can tell where is the library. Also, any chunk of data that the FLAC API needs is solved by a simple node Buffer.

See the [FLAC API](https://xiph.org/flac/api/group__flac.html)? You can use it with a very intuitive form: almost equal.

## How it works?
First, tries to open the library with the usual paths in the system. If it fails, the module will provide you a `load` function, pass to it a **full** path to the library and it will load. If it success, your module will be divided in various sections:

 - [format](https://xiph.org/flac/api/group__flac__format.html)
 - [encoder](https://xiph.org/flac/api/group__flac__encoder.html)
 - [decoder](https://xiph.org/flac/api/group__flac__decoder.html)
 - [metadata](https://xiph.org/flac/api/group__flac__metadata.html)

Every one will contain functions of every FLAC module and its constants and enums. A FLAC encoder function have the form of `FLAC__stream_encoder_new()`, with this binding you will call `flac.bindings.encoder.new()`: _you don't need to write all the function native name_. This (I think) simplifies the way you write the code. Same in constants and enums.

## What is the bad part?
Yes, it has. Almost every function expects his parameters in his right type. If it not, node will just crash. So, pay attention on the types of the functions.

Callbacks don't follow exactly the signature that shows in Encoder and Decoder sections: the **first** and **last** parameters are **not sent** to Javascript callbacks. The first, basically because causes random crashes if the object is passed (caused by the GC). The second, because I see it unnecessary and difficult to use this private data in Javascript. Remember not to pass a private data argument in `init` functions!

Also, if you expect an async API, I have no idea how to implement all in async mode: _you need to call sometimes JS functions when the C code would be running in a background thread but you need it now_.

## Let's dance!
Download now with

```
$ npm install flac-bindings
```

For use it, include with

```
const flac = require('flag-bindings');
```

`flac` will be an object with `{bindings: [Bindings API], StreamEncoder: ..., StreamDecoder: ..., FileEncoder: ..., FileDecoder: ...}`. If the `libFLAC` library is not in the loader's path, you will get an object with a load function (`{load: [Function load]}`). You must call `load()` with the first argument as the **full** path to the `libFLAC` dynamic library, and then `flac` (the object) will have all objects.

## What I need to compile the bindings?
Well, if you are on Linux x64 or on macOS and have 4.6.x LTS, 6.9.x LTS or latest node version, you don't need to compile anything, I provide the binding binaries for you. You only need to provide the FLAC library binary.

In other cases, you will need to install the development version of FLAC (those which includes the headers and the library binary).

## Binding API
### bindings
Here is where all exposed before is located.

`flac.bindings.load(String)`
Loads the FLAC library or throws. When the library is loaded, `flac_bindings.bindings` object is changed completely with the bindings.

### Want more?
See the [wiki](https://github.com/melchor629/node-flac-bindings/wiki) for a more accurated documentation.
