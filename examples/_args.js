import { fileURLToPath } from 'url'

export default (script) => (
  process.argv
    .filter((a) => a !== process.argv0)
    .filter((a) => a !== fileURLToPath(script))
    .filter((a) => a !== process.execPath)
)
