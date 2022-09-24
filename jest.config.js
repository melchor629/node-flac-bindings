/**
 * @type {import('jest').Config}
 */
const config = {
  coveragePathIgnorePatterns: [
    'test/',
    'node_modules',
  ],
  coverageProvider: 'v8',
  coverageReporters: [
    [
      'lcovonly',
      {
        file: 'js.info',
      },
    ],
    'text',
  ],
  roots: [
    'test',
  ],
  setupFilesAfterEnv: [
    'jest-extended/all',
  ],
  testEnvironment: 'node',
  transform: {},
}

export default config
