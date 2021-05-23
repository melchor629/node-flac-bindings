const { api: { format, Chain, metadata } } = require('flac-bindings')
const { basename } = require('path')
const { homedir } = require('os')
const args = require('./_args')(__filename)

// first argument is the file to modify
// second is one of async or sync

const asyncVersion = async (file) => {
  const chain = new Chain()
  // throws exception if it fails
  await chain.readAsync(file)

  const iterator = chain.createIterator()
  // look for the tags metadata block
  while (iterator.next() && iterator.getBlockType() !== format.MetadataType.VORBIS_COMMENT);

  // if there is not vorbis comment already, create one
  if (iterator.getBlockType() !== format.MetadataType.VORBIS_COMMENT) {
    if (!iterator.insertBlockAfter(new metadata.VorbisCommentMetadata())) {
      throw new Error('Could not insert block')
    }
  }

  // alter the block and save it
  const vorbisComment = iterator.getBlock()
  vorbisComment.appendComment('TITLE=Example')
  vorbisComment.appendComment(`ARTIST=${basename(homedir())}`)
  vorbisComment.appendComment(`DATE=${new Date().toISOString()}`)
  iterator.setBlock(vorbisComment) // <- this does not save

  // throws exception if it fails
  await chain.writeAsync(file) // <- this saves
}

const syncVersion = (file) => {
  const chain = new Chain()
  // throws exception if it fails
  chain.read(file)

  const iterator = chain.createIterator()
  // look for the tags metadata block
  while (iterator.next() && iterator.getBlockType() !== format.MetadataType.VORBIS_COMMENT);

  // if there is not vorbis comment already, create one
  if (iterator.getBlockType() !== format.MetadataType.VORBIS_COMMENT) {
    if (!iterator.insertBlockAfter(new metadata.VorbisCommentMetadata())) {
      throw new Error('Could not insert block')
    }
  }

  // alter the block and save it
  const vorbisComment = iterator.getBlock()
  vorbisComment.appendComment('TITLE=Example')
  vorbisComment.appendComment(`ARTIST=${basename(homedir())}`)
  vorbisComment.appendComment(`DATE=${new Date().toISOString()}`)
  iterator.setBlock(vorbisComment) // <- this does not save

  // throws exception if it fails
  chain.write(file) // <- this saves
}

const file = args[0] || 'out.flac' // you can use the output from wav2flac.js or mic2flac.js
console.log(`Updating ${file}`)
switch (args[1]) {
  case 'async':
    asyncVersion(file).catch((error) => console.error(error))
    break

  case 'sync':
    syncVersion(file)
    break

  default:
    console.log('Second argument must be one of async or sync')
}

// ** NOTE: Choose one of above if you want to try
