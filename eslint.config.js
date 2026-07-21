import melchor629 from '@melchor629/eslint-config'
import vitest from '@vitest/eslint-plugin'

export default [
  ...melchor629({
    env: ['node', 'nodeBuiltin'],
    noJsx: true,
  }),
  {
    name: 'flac-bindings:vitest',
    files: ['packages/flac-bindings/test/**'],
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
      'import-x/no-unresolved': 'off',
      'import-x/no-extraneous-dependencies': 'off',
      'no-console': 'off',
    },
  },
]
