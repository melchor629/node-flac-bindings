const createDeferredScope = () => {
  /**
   * @type {Array<() => Promise<void>}
   */
  const deferredFunctions = []
  return {
    /**
     * Adds a function that will be called at the end of the deferred scope.
     * @param {() => Promise<void>} fn Function to call at the end of the scope
     */
    defer: (fn) => {
      deferredFunctions.push(fn)
    },
    finalize: () => Promise.all(deferredFunctions.map((fn) => fn())),
  }
}

export default createDeferredScope
