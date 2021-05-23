const { api: { format } } = require('flac-bindings')

const printMetadata = (metadata) => {
  console.log(`- type: ${format.MetadataTypeString[metadata.type]}`)
  console.log(`  isLast: ${metadata.isLast}`)
  console.log(`  length: ${metadata.length}`)
  switch (metadata.type) {
    case format.MetadataType.APPLICATION:
      console.log(`  id: ${metadata.id.toString('utf8')}`)
      console.log(`  data: <Buffer ${metadata.data.length} bytes>`)
      break

    case format.MetadataType.CUESHEET:
      console.log(`  mediaCatalogNumber: ${metadata.mediaCatalogNumber}`)
      console.log(`  leadIn: ${metadata.leadIn}`)
      console.log(`  isCd: ${metadata.isCd}`)
      console.log('  tracks:')
      for (const track of metadata) {
        console.log(`    - offset: ${track.offset}`)
        console.log(`      number: ${track.number}`)
        console.log(`      isrc: ${track.isrc}`)
        console.log(`      type: ${track.type}`)
        console.log(`      preEmphasis: ${track.preEmphasis}`)
        console.log('      indices:')
        for (const index of track) {
          console.log(`        - ${index.offset} / ${index.number}`)
        }
      }
      break

    case format.MetadataType.PICTURE:
      console.log(`  pictureType: ${metadata.pictureType}`)
      console.log(`  mimeType: ${metadata.mimeType}`)
      console.log(`  description: ${metadata.description}`)
      console.log(`  width: ${metadata.width}`)
      console.log(`  height: ${metadata.height}`)
      console.log(`  depth: ${metadata.depth}`)
      console.log(`  colors: ${metadata.colors}`)
      console.log(`  data: <Buffer ${metadata.data.length} bytes>`)
      break

    case format.MetadataType.SEEKTABLE:
      console.log('  points:')
      for (const point of metadata) {
        console.log(`    - sampleNumber: ${point.sampleNumber}`)
        console.log(`      streamOffset: ${point.streamOffset}`)
        console.log(`      frameSamples: ${point.frameSamples}`)
      }
      break

    case format.MetadataType.STREAMINFO:
      console.log(`  minBlocksize: ${metadata.minBlocksize}`)
      console.log(`  maxBlocksize: ${metadata.maxBlocksize}`)
      console.log(`  minFramesize: ${metadata.minFramesize}`)
      console.log(`  maxFramesize: ${metadata.maxFramesize}`)
      console.log(`  channels: ${metadata.channels}`)
      console.log(`  bitsPerSample: ${metadata.bitsPerSample}`)
      console.log(`  sampleRate: ${metadata.sampleRate}`)
      console.log(`  totalSamples: ${metadata.totalSamples}`)
      console.log(`  md5sum: ${metadata.md5sum.toString('hex')}`)
      break

    case format.MetadataType.VORBIS_COMMENT:
      console.log(`  vendorString: ${metadata.vendorString}`)
      Array.from(metadata)
        .map((e) => e.split(/=/))
        .forEach(([key, ...values]) => console.log(`  ${key}: ${values.join('=')}`))
      break

    default:
  }
}

module.exports = printMetadata
