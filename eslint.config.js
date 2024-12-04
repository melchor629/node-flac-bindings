import melchor629 from '@melchor629/eslint-config'
import vitest from '@vitest/eslint-plugin'

export default [
  ...melchor629({
    env: ['node', 'nodeBuiltin'],
    noJsx: true,
  }),
  {
    name: 'flac-bindings:vitest',
    files: ['test/**'],
    plugins: {
      vitest,
    },
    rules: {
      ...vitest.configs.recommended.rules,
    },
  },
  {
    name: 'flac-bindings:examples',
    files: ['examples/**'],
    rules: {
      'import/no-unresolved': 'off',
      'no-console': 'off',
    },
  },
]
