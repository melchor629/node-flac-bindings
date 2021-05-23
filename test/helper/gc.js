/* eslint-disable global-require */
module.exports = () => {
  require('v8').setFlagsFromString('--expose_gc')
  return require('vm').runInNewContext('gc')()
}
