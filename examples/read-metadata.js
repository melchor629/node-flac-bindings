/* eslint-disable no-await-in-loop */
const { api: { SimpleIterator } } = require('flac-bindings')
const args = require('./_args')(__filename)
const printMetadata = require('./_print-metadata')

// first argument is the flac file
// second argument must be one of async, alt-async, sync or alt-sync

const asyncVersion = async (file) => {
  const iterator = new SimpleIterator()
  // throws exception if it fails
  await iterator.initAsync(file)

  for await (const metadata of iterator) {
    printMetadata(metadata)
  }
}

const syncVersion = (file) => {
  const iterator = new SimpleIterator()
  // throws exception if it fails
  iterator.init(file)

  // you can also do `Array.from(iterator)` and have an array of the metadata blocks
  for (const metadata of iterator) {
    printMetadata(metadata)
  }
}

const altAsyncVersion = async (file) => {
  const iterator = new SimpleIterator()
  // throws exception if it fails
  await iterator.initAsync(file)

  while (await iterator.nextAsync()) {
    printMetadata(await iterator.getBlockAsync())
  }
}

const altSyncVersion = (file) => {
  const iterator = new SimpleIterator()
  // throws exception if it fails
  iterator.init(file)

  while (iterator.next()) {
    printMetadata(iterator.getBlock())
  }
}

const file = args[0] || 'some.flac'
console.log(`Metadata of ${file}`)
switch (args[1]) {
  case 'async':
    asyncVersion(file).catch((error) => console.error(error))
    break

  case 'alt-async':
    altAsyncVersion(file).catch((error) => console.error(error))
    break

  case 'sync':
    syncVersion(file)
    break

  case 'alt-sync':
    altSyncVersion(file)
    break

  default:
    console.log('Second argument must be async, alt-async, sync or alt-sync')
}

// ** NOTE: Choose one of above if you want to try
