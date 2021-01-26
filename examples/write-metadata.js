const { api: { format, SimpleIterator, metadata } } = require('flac-bindings')

const asyncVersion = async (file) => {
  const iterator = new SimpleIterator()
  if(!(await iterator.initAsync(file))) {
    throw new Error(SimpleIterator.StatusString[iterator.status()])
  }

  // look for the tags metadata block
  while(await iterator.nextAsync() && iterator.getBlockType() !== format.MetadataType.VORBIS_COMMENT) {}

  // if there is not vorbis comment already, create one
  if(iterator.getBlockType() !== format.MetadataType.VORBIS_COMMENT) {
    if(!(await iterator.insertBlockAfterAsync(new metadata.VorbisCommentMetadata()))) {
      throw new Error(SimpleIterator.StatusString[iterator.status()])
    }
  }

  // alter the block and save it
  const vorbisComment = await iterator.getBlockAsync()
  vorbisComment.appendComment('TITLE=Example')
  vorbisComment.appendComment(`ARTIST=${require('path').basename(require('os').homedir())}`)
  vorbisComment.appendComment(`DATE=${new Date().toISOString()}`)
  await iterator.setBlockAsync(vorbisComment) // <- this saves
}

const syncVersion = (file) => {
  const iterator = new SimpleIterator()
  if(!iterator.init(file)) {
    throw new Error(SimpleIterator.StatusString[iterator.status()])
  }

  // look for the tags metadata block
  while(iterator.next() && iterator.getBlockType() !== format.MetadataType.VORBIS_COMMENT) {}

  // if there is not vorbis comment already, create one
  if(iterator.getBlockType() !== format.MetadataType.VORBIS_COMMENT) {
    if(!iterator.insertBlockAfter(new metadata.VorbisCommentMetadata())) {
      throw new Error(SimpleIterator.StatusString[iterator.status()])
    }
  }

  // alter the block and save it
  const vorbisComment = iterator.getBlock()
  vorbisComment.appendComment('TITLE=Example')
  vorbisComment.appendComment(`ARTIST=${require('path').basename(require('os').homedir())}`)
  vorbisComment.appendComment(`DATE=${new Date().toISOString()}`)
  iterator.setBlock(vorbisComment) // <- this saves
}

const file = 'out.flac' // you can use the output from wav2flac.js or mic2flac.js
console.log(`Updating ${file}`)
// asyncVersion(file).catch(error => console.error(error))
// syncVersion(file)
// ** NOTE: Choose one of above if you want to try
