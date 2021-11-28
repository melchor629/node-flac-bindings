import { format, SimpleIterator, metadata } from 'flac-bindings'
import { basename } from 'path'
import { homedir } from 'os'
import createArgs from './_args.js'

const args = createArgs(import.meta.url)

// first argument is the file to modify
// second is one of async or sync

const asyncVersion = async (file) => {
  const iterator = new SimpleIterator()
  // throws exception if it fails
  await iterator.initAsync(file)

  // look for the tags metadata block
  // eslint-disable-next-line no-await-in-loop
  while (await iterator.nextAsync()
    && iterator.getBlockType() !== format.MetadataType.VORBIS_COMMENT);

  // if there is not vorbis comment already, create one
  if (iterator.getBlockType() !== format.MetadataType.VORBIS_COMMENT) {
    if (!(await iterator.insertBlockAfterAsync(new metadata.VorbisCommentMetadata()))) {
      throw new Error(SimpleIterator.StatusString[iterator.status()])
    }
  }

  // alter the block and save it
  const vorbisComment = await iterator.getBlockAsync()
  vorbisComment.appendComment('TITLE=Example')
  vorbisComment.appendComment(`ARTIST=${basename(homedir())}`)
  vorbisComment.appendComment(`DATE=${new Date().toISOString()}`)
  await iterator.setBlockAsync(vorbisComment) // <- this saves
}

const syncVersion = (file) => {
  const iterator = new SimpleIterator()
  // throws exception if it fails
  iterator.init(file)

  // look for the tags metadata block
  while (iterator.next() && iterator.getBlockType() !== format.MetadataType.VORBIS_COMMENT);

  // if there is not vorbis comment already, create one
  if (iterator.getBlockType() !== format.MetadataType.VORBIS_COMMENT) {
    if (!iterator.insertBlockAfter(new metadata.VorbisCommentMetadata())) {
      throw new Error(SimpleIterator.StatusString[iterator.status()])
    }
  }

  // alter the block and save it
  const vorbisComment = iterator.getBlock()
  vorbisComment.appendComment('TITLE=Example')
  vorbisComment.appendComment(`ARTIST=${basename(homedir())}`)
  vorbisComment.appendComment(`DATE=${new Date().toISOString()}`)
  iterator.setBlock(vorbisComment) // <- this saves
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
