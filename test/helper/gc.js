import v8 from 'node:v8'
import vm from 'node:vm'

const gc = () => {
  v8.setFlagsFromString('--expose_gc')
  return vm.runInNewContext('gc')()
}

export default gc
