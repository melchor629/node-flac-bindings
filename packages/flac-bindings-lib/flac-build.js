import cp from 'node:child_process'
import os from 'node:os'
import debugFactory from 'debug'

const debug = debugFactory('flac:build')

const envOpts = {
  useFlacSources: typeof process.env.FLAC_BINDINGS_USE_FLAC_SOURCES === 'string',
  pkgConfigPath: process.env.PKG_CONFIG_PATH,
  ci: typeof process.env.CI === 'string',
}

const run = (command, pipe = true) => {
  debug(`Running command ${command}`)
  const proc = cp.spawnSync(command, {
    encoding: 'utf-8',
    shell: true,
    stdio: pipe ? 'inherit' : undefined,
  })
  if (proc.error) {
    throw proc.error
  }

  return proc
}

const checkBuildDeps = async () => {
  let hasSomethingWrong = false
  // first check deps
  const [cmakeJs, nodeAddonApi] = await Promise.all([
    import('cmake-js').catch(() => null),
    import('node-addon-api').catch(() => null),
  ])
  if (!cmakeJs) {
    process.stderr.write('[!!] cmake-js peer dependency is not installed. It is required to build the sources.\n')
    hasSomethingWrong = true
  }
  if (!nodeAddonApi) {
    process.stderr.write('[!!] node-addon-api peer dependency is not installed. It is required to build the sources.\n')
    hasSomethingWrong = true
  }

  // then check if cmake is installed
  const cmakeResult = run('cmake --version', false)
  if (cmakeResult.status) {
    process.stderr.write('[!!] cmake cli is required to build the sources.\n')
    hasSomethingWrong = true
  }

  return hasSomethingWrong
}

if (envOpts.ci) {
  debug('CI environment, stopping build')
  process.exit(0)
}

debug('Checking build dependencies before building source...')
if (await checkBuildDeps()) {
  process.exit(1)
}

if (envOpts.useFlacSources) {
  debug('Trying to compile bindings using external libflac...')
  if (run('cmake-js configure --CDUSE_FLAC_SOURCES').status) {
    process.exit(1)
  }
}

debug('Trying to compile with built-in libflac...')
if (run(`cmake-js build -p ${os.cpus().length}`).status) {
  process.exit(1)
}
