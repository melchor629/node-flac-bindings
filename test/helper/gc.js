import v8 from 'v8'
import vm from 'vm'

const gc = () => {
  v8.setFlagsFromString('--expose_gc')
  return vm.runInNewContext('gc')()
}

export default gc
