import cp from 'child_process'
import debugFactory from 'debug'

const debug = debugFactory('flac:build')

const envOpts = {
  useExternalLibrary: typeof process.env.FLAC_BINDINGS_USE_EXTERNAL_LIBRARY === 'string',
  pkgConfigPath: process.env.PKG_CONFIG_PATH,
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

// from https://github.com/lovell/sharp/blob/master/lib/libvips.js
const pkgConfigPath = () => {
  if (process.platform !== 'win32') {
    debug('Retrieving pkg-config path')
    const brewPkgConfigPath = run(
      'which brew >/dev/null 2>&1 && eval $(brew --env) && echo $PKG_CONFIG_LIBDIR',
      false,
    ).stdout || ''
    return [brewPkgConfigPath.trim(), envOpts.pkgConfigPath, '/usr/local/lib/pkgconfig', '/usr/lib/pkgconfig']
      .filter((p) => !!p)
      .join(':')
  }
  return ''
}

const hasGlobalInstalledFlac = () => {
  if (process.platform !== 'win32') {
    debug('Checking if libflac is available')
    const flacVersion = (
      run(
        `PKG_CONFIG_PATH="${pkgConfigPath()}" pkg-config --modversion flac`,
        false,
      ).stdout || ''
    ).trim()
    if (!flacVersion) {
      debug('No libflac found')
      return false
    }

    debug(`libflac found with version ${flacVersion}`)
    const [major, minor] = flacVersion.split('.').map((n) => parseInt(n, 10))
    return major === 1 && minor >= 3
  }

  return false
}

if (!envOpts.useExternalLibrary) {
  debug('Trying to install from prebuilt package...')
  if (!run('prebuild-install -r napi').status) {
    debug('Installed succesfully')
    process.exit(0)
  }
}

if (envOpts.useExternalLibrary || hasGlobalInstalledFlac()) {
  debug('Trying to compile bindings using external libflac...')
  if (run('cmake-js configure --CDFLAC_BINDINGS_USE_EXTERNAL_LIBRARY=ON').status) {
    process.exit(1)
  }
}

debug('Trying to compile with built-in libflac...')
if (run('cmake-js build').status) {
  process.exit(1)
}
