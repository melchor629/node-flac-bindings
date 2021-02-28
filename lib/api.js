const api = require('bindings')('flac-bindings');

Object.freeze(api._helpers);

module.exports = api;
