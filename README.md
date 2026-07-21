# flac-bindings

Nodejs bindings to [libFLAC](https://github.com/xiph/flac)

![Node-API v9](https://raw.githubusercontent.com/nodejs/abi-stable-node/doc/assets/Node-API%20v9%20Badge.svg)
[![Node CI](https://github.com/melchor629/node-flac-bindings/actions/workflows/nodejs.yml/badge.svg)](https://github.com/melchor629/node-flac-bindings/actions/workflows/nodejs.yml)
[![Build Artifacts](https://github.com/melchor629/node-flac-bindings/actions/workflows/artifacts.yml/badge.svg)](https://github.com/melchor629/node-flac-bindings/actions/workflows/artifacts.yml)
[![Coverage Status](https://coveralls.io/repos/github/melchor629/node-flac-bindings/badge.svg?branch=dev)](https://coveralls.io/github/melchor629/node-flac-bindings?branch=dev)

## Project structure

There are these packages:

- [flac-bindings](./packages/flac-bindings/) that contains the JS code and native bindings loader.
- [flac-bindings-lib](./packages/flac-bindings-lib/) that contains the binding code and compilation scripts.

## How to compile for dev

To compile the bindings you need [Cmake](https://www.cmake.org) installed in your system and accessible from the terminal, and the C and C++ compilers as well. On Windows, the compilers can be installed easily with `npm install --global --production windows-build-tools`. Don't forget `git`. It is mandatory!

There are some options to use when compiling. The build tries to use an already installed compatible dev package of `libflac` (`apt install libflac-dev`, `pacman -S flac`, `apk add flac-dev`, `brew install flac`...), but with `FLAC_BINDINGS_USE_FLAC_SOURCES` will force to download the sources and compile everything from there.

> Supported `libFLAC` versions are 1.3.x and 1.4.x (binary versions 10 and 12).

Then, you just need to recompile the package with: `npm rebuild @melchor629/flac-bindings-lib`. If you are inside this repo tree, then run `npm run install`.

For more advanced commands for compilation inside the repo tree, see below:

```sh
cd packages/flac-bindings-lib

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
cd packages/flac-bindings-lib
npx cmake-js configure --debug
npx cmake-js build --debug -p 4
cd ../flac-bindings
npm test

# To run tests with coverage (requires lcov to be installed)
scripts/coverage.sh rebuild # first time
script/coverage.sh          # next times
```

Happy `npm test` runs :)

## The API

See the [wiki](https://github.com/melchor629/node-flac-bindings/wiki) for the documentation.
