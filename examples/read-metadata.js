const { api: { SimpleIterator } } = require('flac-bindings')
const printMetadata = require('./_print-metadata')

const asyncVersion = async (file) => {
  const iterator = new SimpleIterator()
  if(!(await iterator.initAsync(file))) {
    throw new Error(SimpleIterator.StatusString[iterator.status()])
  }

  for await(const metadata of iterator) {
    printMetadata(metadata)
  }
}

const syncVersion = (file) => {
  const iterator = new SimpleIterator()
  if(!iterator.init(file)) {
    throw new Error(SimpleIterator.StatusString[iterator.status()])
  }

  // you can also do `Array.from(iterator)` and have an array of the metadata blocks
  for(const metadata of iterator) {
    printMetadata(metadata)
  }
}

const altAsyncVersion = async (file) => {
  const iterator = new SimpleIterator()
  if(!(await iterator.initAsync(file))) {
    throw new Error(SimpleIterator.StatusString[iterator.status()])
  }

  while(await iterator.nextAsync()) {
    printMetadata(await iterator.getBlockAsync())
  }
}

const altSyncVersion = (file) => {
  const iterator = new SimpleIterator()
  if(!iterator.init(file)) {
    throw new Error(SimpleIterator.StatusString[iterator.status()])
  }

  while(iterator.next()) {
    printMetadata(iterator.getBlock())
  }
}

const file = 'some.flac'
console.log(`Metadata of ${file}`)
// asyncVersion(file).catch(error => console.error(error))
// altAsyncVersion(file).catch(error => console.error(error))
// syncVersion(file)
// altSyncVersion(file)
// ** NOTE: Choose one of above if you want to try
