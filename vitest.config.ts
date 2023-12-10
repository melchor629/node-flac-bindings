import { defineConfig } from 'vitest/config'

const isCI = ['true', '1'].includes(process.env.CI || '')

export default defineConfig({
  test: {
    environment: 'node',
    coverage: {
      enabled: isCI,
      provider: 'v8',
      include: ['lib'],
      exclude: ['lib/index.cjs', '**/*.d.ts'],
      reporter: [
        'text',
        ['lcovonly', { file: 'js.info' }],
      ],
    },
    include: ['test/**/*.test.[jt]s'],
    reporters: isCI ? ['default', 'junit'] : ['default'],
    setupFiles: './test/helper/setup.js',
    globalSetup: './test/helper/global-setup.js',
    outputFile: 'tests.junit.xml',
    poolOptions: {
      threads: {
        useAtomics: true,
      },
    },
  },
})
