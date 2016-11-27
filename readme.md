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

Every one will contain functions of every FLAC module and its constants and enums. A FLAC encoder function have the form of `FLAC__stream_encoder_new()`, with this binding you will call `flac_bindings.encoder.new()`: _you don't need to write all the function native name_. This (I think) simplifies the way you write the code. Same in constants and enums.

## What is the bad part?
Yes, it has. Almost every function expects his parameters and in the write type. If it not, the binding will just crash. So, pay attention on the types of the functions.

Callbacks don't follow exactly the signature that shows in Encoder and Decoder sections: the **first** and **last** parameters are **not sent** to Javascript callbacks. The first, basically because causes random crashes if the object is passed (caused by the GC). The second, because I see it unnecessary and difficult to use this private data in Javascript. Remember not to pass a private data argument in `init` functions!

Too, if you expect an async API, I have no idea how to implement all in async mode: _you need to call sometimes JS functions when the C code would be running in a background thread but you need it now_.

## Let's dance!
Download now with

```
$ npm install flac-bindings
```

For use it, include with

```
const flac = require('flag-bindings');
```

`flac` will be an object with `{bindings: [Bindings API], StreamEncoder: ..., StreamDecoder: ..., FileEncoder: ..., FileDecoder: ...}`. If the `libFLAC` library is not in the loader's path, you will get an object with a load function (`{load: [Function load]}`). You must call `load()` with as first argument the **full** path to the `libFLAC` dynamic library, and then `flac` (the object) will have all objects.

## Binding API
### bindings
Here is where all exposed before is located.

`flac_bindings.bindings.load(String)`
Loads the FLAC library or throws. When the library is loaded, `flac_bindings.bindings` object is changed completely with the bindings.

### bindings.decoder
All [`FLAC__stream_decoder_...`](https://xiph.org/flac/api/group__flac__stream__decoder.html) API is here.

_In this sections will only show the functions that have some changes from the original API and is not denoted before._

 - The `read_callback` from the Stream initialization **must** return an object like this `{bytes: SomeNumber, returnValue: AnotherNumber}`, corresponding to the `bytes` value that expect to be changed in the C callback, and `returnValue` is the value that will be returned with a `return;` statement.

 - The `tell_callback` from the Stream initialization **must** return an object like this `{length: SomeNumber, returnValue: AnotherNumber}`, corresponding to the `length` value that expect to be changed in the C callback, and `returnValue` is the value that will be returned with a `return;` statement.

 - The `length_callback` from the Stream initialization **must** return an object like this `{length: SomeNumber, returnValue: AnotherNumber}`, corresponding to the `length` value that expect to be changed in the C callback, and `returnValue` is the value that will be returned with a `return;` statement.

### bindings.encoder
All [`FLAC__stream_encoder_...`](https://xiph.org/flac/api/group__flac__stream__encoder.html) API is here.

 - `flac_bindings.encoder.process(enc, buffers, samples)`
A thing to remark is that `buffers` is an Array of node Buffers.

 - The `read_callback` from the OGG Stream initialization **must** return an object like this `{bytes: SomeNumber, returnValue: AnotherNumber}`, corresponding to the `bytes` value that expect to be changed in the C callback, and `returnValue` is the value that will be returned with a `return;` statement.

 - The `tell_callback` from the Stream initialization **must** return an object like this `{length: SomeNumber, returnValue: AnotherNumber}`, corresponding to the `length` value that expect to be changed in the C callback, and `returnValue` is the value that will be returned with a `return;` statement.

### bindings.format
A big part of [FLAC format](https://xiph.org/flac/api/group__flac__format.html) API is here. _Has more internal code that external._ The macros are not exported. Variables are only exported the 6 ones that are an array of C strings and `FLAC_VENDOR_STRING` and `FLAC_VERSION_STRING`.

 - In the `cpp` file, here is where the struct-to-js (and viceversa) conversions are done.

 - `FLAC__Frame` lacks the field `entropy_coding_method` in type Fixed and LPC. Every subframe element from the array has only the fields of its type, and are not hide behind the original form (an union), they are direct in the object.

 - `FLAC__StreamMetadata_Vorbiscomment_Entry` is not an object in Javascript. Instead uses a String because it's a String (`FLAC__byte*`) with a length. Its format is "KEY=Value". [See more](http://www.xiph.org/vorbis/doc/v-comment.html)

 - `FLAC__StreamMetadata` has the type struct fields directly in the same object instead of (ex.) `data->data.stream_info`.

### bindings.metadata0
All the enormous code from [metadata level 0 interface](https://xiph.org/flac/api/group__flac__metadata__level0.html).

 - All the functions return JS Objects. But you should call `flac_bindings.bindings.metadata.delete(obj)` with any of the objects returned in get_tags, get_cuesheet and get_picture.

### bindings.metadata1
All from [metadata level 1 interface](https://xiph.org/flac/api/group__flac__metadata__level1.html).

 - `bindings.metadata1.new` returns a node Buffer. Don't try to do anything on it.

 - `bindings.metadata1.init` arguments 3 and 4 are not mandatory. `read_only` defaults to false and `preserve_file_stats` same.

 - `bindings.metadata1.get_application_id` will return an array of 4 Numbers if the call works well, otherwise will return false and you should check why failed the call.

 - `bindings.metadata1.get_block` returns a node Buffer that is fully compatible with `bindings.metadata` (see below). But remember that you must call `bindings.metadata.delete` to free the native resources.

 - `bindings.metadata1.set_block` and `bindings.metadata1.insert_block_after` expects as second parameter a node Buffer that contains a `FLAC__StreamMetadata` object created using `bindings.metadata` API calls. The FLAC API won't modify nor delete the object, you must delete it after.

### bindings.metadata2
All from [metadata level 2 interface](https://xiph.org/flac/api/group__flac__metadata__level2.html).

 - The functions [`FLAC__metadata_chain_read_with_callbacks`](https://xiph.org/flac/api/group__flac__metadata__level2.html#ga595f55b611ed588d4d55a9b2eb9d2add), [`FLAC__metadata_chain_read_ogg_with_callbacks`](https://xiph.org/flac/api/group__flac__metadata__level2.html#gaccc2f991722682d3c31d36f51985066c), [`FLAC__metadata_chain_write_with_callbacks`](https://xiph.org/flac/api/group__flac__metadata__level2.html#ga70532b3705294dc891d8db649a4d4843), [`FLAC__metadata_chain_write_with_callbacks_and_tempfile`](https://xiph.org/flac/api/group__flac__metadata__level2.html#ga72facaa621e8d798036a4a7da3643e41) and [`FLAC__metadata_chain_check_if_tempfile_needed`](https://xiph.org/flac/api/group__flac__metadata__level2.html#ga46602f64d423cfe5d5f8a4155f8a97e2) are not binded to nodejs.

 - As always `bindings.metadata2.get_block` returns a node Buffer where you can read or convert to an JS Object. **Remember** that this object cannot be deleted with `bindings.metadata.delete`. Read **carefully** the documentation.

### bindings.metadata
All the functions from [metadata object methods](https://xiph.org/flac/api/group__flac__metadata__object.html). Here there are a lot of changes :)

 - Every FLAC function that ends with the parameter `FLAC__bool copy`, you can ignore it in JS, is always set to true because the property of the data you send. Set to false will cause a conflict due to the data is owned by node and the FLAC API. So always will make a copy of your data. Are two exceptions, see some lines below.

 - Remember `FLAC__StreamMetadata_Vorbiscomment_Entry` are Strings in JS.

 - `flac_bindings.bindings.metadata.new()` returns a Buffer instead of an object. Conversion between objects has a cost in the performance. To simplify, this API will use Buffers always.

 - `FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair`, `FLAC__metadata_object_vorbiscomment_entry_to_name_value_pair` and `FLAC__metadata_object_vorbiscomment_entry_matches` are not in the API. Are not needed because of the complex of the bindings that will cause. Instead **remember** that the format of a `Vorbiscomment` is a String with the form of "KEY=Value" in ASCII (the key)/UTF8 (the value). [See more](http://www.xiph.org/vorbis/doc/v-comment.html).

 - `FLAC__metadata_object_cuesheet_track_insert_index` instead of receiving the last parameter `FLAC__StreamMetadata_CueSheet_Index index`, receives two, the contents of [FLAC__StreamMetadata_CueSheet_Index](https://xiph.org/flac/api/structFLAC____StreamMetadata__CueSheet__Index.html). To simplify binding code.

 - `bindings.metadata.cuesheet_set_track` and `bindings.metadata.cuesheet_insert_track`  set the copy argument to false. The Cue Sheet Track object has to be created with `bindings.metadata.cuesheet_track_new()` and you must not call `bindings.metadata.cuesheet_track_delete(track)` with this object because now it's property of FLAC API.

 - `bindings.metadata.cuesheet_is_legal` and `bindings.metadata.picture_is_legal` returns `true` or a String in case of error, and is the value of `violation` parameter.

 - `bindings.metadata.convertToJsObject(buffer)`
This function gets a buffer created by `bindings.metadata.new()` (and maybe edited by other functions) and creates a Javascript Object that represents it. Is a **copy**, modifications in the buffer or the object will not reflect to the other.

## StreamEncoder
A [Transform](https://nodejs.org/dist/latest-v6.x/docs/api/stream.html#stream_class_stream_transform) type class that transform (_integer typed_) interleaved PCM data to FLAC.

`flac_bindings.StreamEncoder(options)`
You can pass all the options that can expect the [Transform](https://nodejs.org/dist/latest-v6.x/docs/api/stream.html#stream_class_stream_transform) object, but also you can pass:

 - **isOggStream**: Boolean
	 - Indicates if the output would be an ogg stream or a raw FLAC stream
	 - Defaults to **false**
 - **channels**: Number
	 - Indicates to the encoder the number of channels that will have the PCM stream
	 - Defaults to **2**
 - **bitsPerSample**: Number
	 - Indicates to the encoder the bits per sample of the PCM stream
	 - Defaults to **16**
 - **samplerate**: Number
	 - Indicates to the encoder the samplerate of the PCM stream
	 - Defaults to **44100**
 - **oggSerialNumber**: Number
	 - Indicates the serial number of the ogg stream, if its an ogg stream
	 - No default value, you **must** provide this value if its an **ogg stream**
 - **totalSamplesEstimate**: Number
	 - An estimation of the total samples of the PCM stream
	 - Optional

## FileEncoder
A [Writable](https://nodejs.org/dist/latest-v6.x/docs/api/stream.html#stream_writable_streams) type class that writes a PCM stream to a file.

`flac_bindings.FileEncoder(options)`
You can pass all the options that can expect the [Transform](https://nodejs.org/dist/latest-v6.x/docs/api/stream.html#stream_class_stream_transform) and StreamEncoder objects, but also you must pass:

 - **file**: String
	 - File to the output file
	 - You **must** provide this value

## StreamDecoder
A [Transform](https://nodejs.org/dist/latest-v6.x/docs/api/stream.html#stream_class_stream_transform) type class that transform FLAC to (_integer typed_) interleaved PCM.

`flac_bindings.StreamDecoder(options)`
You can pass all the options that can expect the [Transform](https://nodejs.org/dist/latest-v6.x/docs/api/stream.html#stream_class_stream_transform) object, but also you can pass:

 - **oggStream**: Boolean
	 - Determines if the Input data is an OGG Stream
	 - Defaults to false
 - **metadata**: Array or Boolean
    - If is an array, tells the decoder to emit `metadata` when metadata blocks of the types passed are found
    - If is `true`, tells the decoder to emit `metadata` on all metadata blocks found
    - Defaults to `STREAMINFO` only

Extra operations available are `getTotalSamples()`, `getChannels()`, `getChannelAssignment()` and `getBitsPerSample()`.

#### Event `metadata`

When some metadata has been read, is emitted in a event and it passes an JS representation of it.

### FileDecoder
A [Readable](https://nodejs.org/dist/latest-v6.x/docs/api/stream.html#stream_class_stream_readable) type class that gives you (_integer typed_) interleaved PCM from a file.

`flac_bindings.FileDecoder`
You can pass all the options that can expect the [Readable](https://nodejs.org/dist/latest-v6.x/docs/api/stream.html#stream_class_stream_readable) object, but also you can pass:

 - **oggStream**: Boolean
	 - Determines if the file is an OGG Stream or not
	 - Defaults to false
 - **file**: String
	 - The file with the FLAC stream and where it will be read
	 - **You must** provide this value
 - **metadata**: Array or Boolean
    - If is an array, tells the decoder to emit `metadata` when metadata blocks of the types passed are found
    - If is `true`, tells the decoder to emit `metadata` on all metadata blocks found
    - Defaults to `STREAMINFO` only

Extra operations available are `getTotalSamples()`, `getChannels()`, `getChannelAssignment()` and `getBitsPerSample()`.

#### Event `metadata`

When some metadata has been read, is emitted in a event and it passes an JS representation of it.

### Want more?
The text above is only a kind of short description. See the [wiki](https://github.com/melchor629/node-flac-bindings/wiki) for a more accurated documentation.
