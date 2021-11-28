import { Chain } from 'flac-bindings/api'
import createArgs from './_args.js'
import printMetadata from './_print-metadata.js'

const args = createArgs(import.meta.url)

// first argument is the flac
// second argument must be async, sync or alt-sync

const asyncVersion = async (file) => {
  const chain = new Chain()
  // throws exception if it fails
  await chain.readAsync(file)

  for (const metadata of chain.createIterator()) {
    printMetadata(metadata)
  }
}

const syncVersion = (file) => {
  const chain = new Chain()
  // throws exception if it fails
  chain.read(file)

  // you can also do `Array.from(iterator)` and have an array of the metadata blocks
  for (const metadata of chain.createIterator()) {
    printMetadata(metadata)
  }
}

const altSyncVersion = (file) => {
  const chain = new Chain()
  // throws exception if it fails
  chain.read(file)

  // this can also be used with async version. In fact, the read operation reads all metadata blocks
  // into memory, so all operations inside the iterator are in-memory.
  const iterator = chain.createIterator()
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

  case 'sync':
    syncVersion(file)
    break

  case 'alt-sync':
    altSyncVersion(file)
    break

  default:
    console.log('Second argument must be async, sync or alt-sync')
}

// ** NOTE: Choose one of above if you want to try
