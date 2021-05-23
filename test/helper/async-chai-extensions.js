module.exports = function (chai) {
  // eslint-disable-next-line no-param-reassign
  chai.assert.throwsAsync = async function (f, pattern) {
    try {
      await f()
    } catch (e) {
      if (pattern) {
        if (typeof pattern === 'string' && pattern !== e.message) {
          throw new Error(`Expected message '${e.message}' to match string ${pattern}`)
        } else if (typeof pattern !== 'string' && !pattern.test(e.message)) {
          throw new Error(`Expected message '${e.message}' to match pattern ${pattern}`)
        }
      }
      return
    }
    throw new Error('Expected function to throw')
  }
}
