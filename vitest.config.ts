import { defineConfig } from 'vitest/config'

export default defineConfig({
  test: {
    environment: 'node',
    coverage: {
      provider: 'v8',
      reporter: [
        'text',
        ['lcovonly', { file: 'js.info' }],
      ],
    },
    include: ['test/**/*.test.[jt]s'],
    reporters: ['default', 'junit'],
    setupFiles: './test/helper/setup.js',
    outputFile: 'tests.junit.xml',
    useAtomics: true,
  },
})
