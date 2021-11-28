import fs from 'fs'

const generateFlacCallbacks = {
  sync: (Class, path, flags) => {
    const fd = fs.openSync(path, flags || 'w+')
    let position = 0
    const read = (buffer) => {
      const bytes = fs.readSync(fd, buffer, 0, buffer.length, position)
      if (position !== null) {
        position += bytes
      }

      if (bytes === 0) {
        return { bytes: 0, returnValue: Class.ReadStatus.END_OF_STREAM }
      }

      return { bytes, returnValue: Class.ReadStatus.CONTINUE }
    }

    const write = (buffer) => {
      const bytes = fs.writeSync(fd, buffer, 0, buffer.length, position)
      if (position !== null) {
        position += bytes
      }

      return Class.WriteStatus.CONTINUE || Class.WriteStatus.OK
    }

    const seek = (offset) => {
      position = offset
      return Class.SeekStatus.OK
    }

    const tell = () => {
      if (position !== null) {
        return { offset: position, returnValue: Class.TellStatus.OK }
      }

      return { offset: 0, returnValue: Class.TellStatus.UNSUPPORTED }
    }

    const length = () => ({ returnValue: 0, length: fs.fstatSync(fd).size })

    const eof = () => (position !== null ? length() <= position : false)

    const close = () => fs.closeSync(fd)

    return {
      read,
      write,
      seek,
      tell,
      length,
      eof,
      close,
    }
  },
  async: async (Class, path, flags) => {
    const fh = await fs.promises.open(path, flags || 'w+')
    let position = 0
    const read = async (buffer) => {
      const { bytesRead: bytes } = await fh.read(buffer, 0, buffer.length, position)
      if (position !== null) {
        position += bytes
      }

      if (bytes === 0) {
        return { bytes: 0, returnValue: Class.ReadStatus.END_OF_STREAM }
      }

      return { bytes, returnValue: Class.ReadStatus.CONTINUE }
    }

    const write = async (buffer) => {
      const { bytesWritten: bytes } = await fh.write(buffer, 0, buffer.length, position)
      if (position !== null) {
        position += bytes
      }

      return Class.WriteStatus.CONTINUE || Class.WriteStatus.OK
    }

    const seek = (offset) => {
      position = offset
      return Class.SeekStatus.OK
    }

    const tell = () => {
      if (position !== null) {
        return { offset: position, returnValue: Class.TellStatus.OK }
      }

      return { offset: 0, returnValue: Class.TellStatus.UNSUPPORTED }
    }

    const length = async () => ({ length: (await fh.stat()).size, returnValue: 0 })

    const eof = async () => (position !== null ? await length() <= position : false)

    const close = () => fh.close()

    return {
      read,
      write,
      seek,
      tell,
      length,
      eof,
      close,
    }
  },
  flacio: async (path, flags) => {
    const fh = await fs.promises.open(path, flags || 'w+')
    let position = 0
    const read = async (buffer, sizeOfItem, numberOfItems) => {
      const { bytesRead } = await fh.read(buffer, 0, sizeOfItem * numberOfItems, position)
      position += bytesRead
      return bytesRead / sizeOfItem
    }

    const write = async (buffer, sizeOfItem, numberOfItems) => {
      const { bytesWritten } = await fh.write(buffer, 0, sizeOfItem * numberOfItems, position)
      position += bytesWritten
      return bytesWritten / sizeOfItem
    }

    const length = async () => (await fh.stat()).size

    const seek = (offset, whence) => {
      switch (whence) {
        case 'set':
          position = offset
          break
        case 'cur':
          position += offset
          break
        case 'end':
          position = length() + offset
          break
        default:
          return 1
      }
      return 0
    }

    const tell = () => position

    const eof = async () => (position !== null ? await length() <= position : false)

    const close = () => fh.close()

    return {
      read,
      write,
      seek,
      tell,
      length,
      eof,
      close,
    }
  },
}

export default generateFlacCallbacks
