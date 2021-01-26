const { api: { Chain } } = require('flac-bindings')
const printMetadata = require('./_print-metadata')

const asyncVersion = async (file) => {
  const chain = new Chain()
  if(!(await chain.readAsync(file))) {
    throw new Error(Chain.StatusString[chain.status()])
  }

  for(const metadata of chain.createIterator()) {
    printMetadata(metadata)
  }
}

const syncVersion = (file) => {
  const chain = new Chain()
  if(!chain.read(file)) {
    throw new Error(Chain.StatusString[chain.status()])
  }

  // you can also do `Array.from(iterator)` and have an array of the metadata blocks
  for(const metadata of chain.createIterator()) {
    printMetadata(metadata)
  }
}

const altSyncVersion = (file) => {
  const chain = new Chain()
  if(!chain.read(file)) {
    throw new Error(Chain.StatusString[chain.status()])
  }

  // this can also be used with async version. In fact, the read operation reads all metadata blocks
  // into memory, so all operations inside the iterator are in-memory.
  const iterator = chain.createIterator()
  while(iterator.next()) {
    printMetadata(iterator.getBlock())
  }
}

const file = 'some.flac'
console.log(`Metadata of ${file}`)
// asyncVersion(file).catch(error => console.error(error))
// syncVersion(file)
// altSyncVersion(file)
// ** NOTE: Choose one of above if you want to try
