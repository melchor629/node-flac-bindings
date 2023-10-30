import { resolve } from 'node:path'
import { defineConfig } from 'rollup'
import packageJson from './package.json' assert { type: 'json' }

export default defineConfig({
  input: resolve('lib/index.js'),
  output: {
    format: 'cjs',
    file: resolve('lib/index.cjs'),
  },
  external: [
    ...Object.keys(packageJson.dependencies),
    'stream',
    'url',
  ],
})
