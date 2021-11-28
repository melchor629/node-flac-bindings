import { gc } from '../helper/index.js'
import {
  metadata,
  format,
} from '../../lib/api.js'

const { ApplicationMetadata } = metadata
const { MetadataType } = format

describe('ApplicationMetadata', () => {
  it('new object has the right type', () => {
    const am = new ApplicationMetadata()

    expect(am.type).toEqual(MetadataType.APPLICATION)
  })

  it('set id of object should work', () => {
    const am = new ApplicationMetadata()

    am.id = Buffer.from('1234')
    expect(am.id).toEqual(Buffer.from('1234'))
  })

  it('set id of object having less than 4 bytes should throw', () => {
    const am = new ApplicationMetadata()

    expect(() => {
      am.id = Buffer.from('si')
    }).toThrow()
  })

  it('set id of object having more than 4 bytes should work but discard the rest', () => {
    const am = new ApplicationMetadata()

    am.id = Buffer.from('1234===-><-')
    expect(am.id).toEqual(Buffer.from('1234'))
  })

  it('set data of object should work', () => {
    const am = new ApplicationMetadata()

    am.data = Buffer.from('this is a wonderful data that is useless per se, pero bueno')
    expect(am.data).toEqual(Buffer.from('this is a wonderful data that is useless per se, pero bueno'))
  })

  describe('gc', () => {
    it('gc should work', () => {
      expect(gc).not.toThrow()
    })
  })
})
