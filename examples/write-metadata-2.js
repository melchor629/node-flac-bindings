const { api: { format, Chain, metadata } } = require('flac-bindings')

const asyncVersion = async (file) => {
  const chain = new Chain()
  if(!(await chain.readAsync(file))) {
    throw new Error(Chain.StatusString[chain.status()])
  }

  const iterator = chain.createIterator()
  // look for the tags metadata block
  while(iterator.next() && iterator.getBlockType() !== format.MetadataType.VORBIS_COMMENT) {}

  // if there is not vorbis comment already, create one
  if(iterator.getBlockType() !== format.MetadataType.VORBIS_COMMENT) {
    if(!iterator.insertBlockAfter(new metadata.VorbisCommentMetadata())) {
      throw new Error('Could not insert block')
    }
  }

  // alter the block and save it
  const vorbisComment = iterator.getBlock()
  vorbisComment.appendComment('TITLE=Example')
  vorbisComment.appendComment(`ARTIST=${require('path').basename(require('os').homedir())}`)
  vorbisComment.appendComment(`DATE=${new Date().toISOString()}`)
  iterator.setBlock(vorbisComment) // <- this does not save

  await chain.writeAsync(file) // <- this saves
}

const syncVersion = (file) => {
  const chain = new Chain()
  if(!chain.read(file)) {
    throw new Error(Chain.StatusString[chain.status()])
  }

  const iterator = chain.createIterator()
  // look for the tags metadata block
  while(iterator.next() && iterator.getBlockType() !== format.MetadataType.VORBIS_COMMENT) {}

  // if there is not vorbis comment already, create one
  if(iterator.getBlockType() !== format.MetadataType.VORBIS_COMMENT) {
    if(!iterator.insertBlockAfter(new metadata.VorbisCommentMetadata())) {
      throw new Error('Could not insert block')
    }
  }

  // alter the block and save it
  const vorbisComment = iterator.getBlock()
  vorbisComment.appendComment('TITLE=Example')
  vorbisComment.appendComment(`ARTIST=${require('path').basename(require('os').homedir())}`)
  vorbisComment.appendComment(`DATE=${new Date().toISOString()}`)
  iterator.setBlock(vorbisComment) // <- this does not save

  chain.write(file) // <- this saves
}

const file = 'out.flac' // you can use the output from wav2flac.js or mic2flac.js
console.log(`Updating ${file}`)
// asyncVersion(file).catch(error => console.error(error))
// syncVersion(file)
// ** NOTE: Choose one of above if you want to try
