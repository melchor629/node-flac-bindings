#!/bin/bash

set -e

if [[ -d coverage ]]; then
  rm -rf coverage
fi
mkdir coverage

if [[ ! -z "$1" ]] && [[ "$1" = "rebuild" ]]; then
  echo Compiling native code
  cd packages/flac-bindings-lib
  npx cmake-js clean
  npx cmake-js configure --CDCOVERAGE=ON --debug -p 4
  npx cmake-js build --debug -j4
  cd ../..
fi

echo Running tests
npm test -w packages/flac-bindings -- --coverage || true

echo Extracting coverage report from C++
lcov -c --quiet --directory packages/flac-bindings-lib/build/CMakeFiles/flac-bindings.dir/src --base-directory packages/flac-bindings-lib/src -o coverage/cpp.info --no-external

echo Generating html
cat packages/flac-bindings/coverage/js.info coverage/cpp.info > coverage/combined.info
genhtml --quiet -o coverage/report coverage/combined.info
