import melchor629 from '@melchor629/eslint-config'
import vitest from '@vitest/eslint-plugin'

export default [
  ...melchor629({
    env: ['node', 'nodeBuiltin'],
    noJsx: true,
  }),
  {
    files: ['test/**'],
    plugins: {
      vitest,
    },
    rules: {
      ...vitest.configs.recommended.rules,
    },
  },
  {
    files: ['examples/**'],
    rules: {
      'import/no-unresolved': 'off',
      'no-console': 'off',
    },
  },
]
