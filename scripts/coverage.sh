#!/bin/bash

set -e

export NODE_OPTIONS=--experimental-vm-modules

if [[ -d coverage ]]; then
  rm -rf coverage
fi
mkdir coverage

if [[ ! -z "$1" ]] && [[ "$1" = "rebuild" ]]; then
  npx cmake-js clean
  npx cmake-js configure --CDCOVERAGE=ON --CDFLAC_BINDINGS_USE_EXTERNAL_LIBRARY=ON --debug
  npx cmake-js build --debug
fi

npx jest --coverage

echo Extracting coverage report from C++
lcov -c --quiet --directory build/CMakeFiles/flac-bindings.dir/src --base-directory src -o coverage/cpp.info --no-external

echo Generating html
cat coverage/js.info coverage/cpp.info > coverage/combined.info
genhtml --quiet -o coverage/report coverage/combined.info
