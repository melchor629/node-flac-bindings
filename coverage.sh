#!/bin/bash

set -e

export PATH="$PATH:$PWD/node_modules/.bin"

if [[ -d coverage ]]; then
    rm -rf coverage
fi
mkdir coverage

cmake-js build --debug --CDCOVERAGE

nyc mocha --recursive
nyc report --reporter=text-lcov > coverage/js.info

echo Extracting coverage report from C++
lcov -c --quiet --directory build/CMakeFiles/flac-bindings.dir/src --base-directory src -o coverage/cpp.info --no-external

echo Generating html
cat coverage/js.info coverage/cpp.info > coverage/combined.info
genhtml --quiet -o coverage coverage/combined.info
