module.exports = (script) => (
  process.argv
    .filter((a) => a !== process.argv0)
    .filter((a) => a !== script)
    .filter((a) => a !== process.execPath)
)
