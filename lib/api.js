const bindings = require('bindings')

const moduleRoot = bindings.getRoot(__filename)
const api = bindings({ bindings: 'flac-bindings.node', module_root: moduleRoot })

module.exports = api
