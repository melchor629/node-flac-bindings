require('v8').setFlagsFromString('--expose_gc');
module.exports = require('vm').runInNewContext('gc');
