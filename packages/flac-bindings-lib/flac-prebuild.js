import cp from 'node:child_process'
import { once } from 'node:events'
import { createReadStream, createWriteStream } from 'node:fs'
import fs from 'node:fs/promises'
import os from 'node:os'
import path from 'node:path'
import zlib from 'node:zlib'
// eslint-disable-next-line import-x/no-extraneous-dependencies
import detectLibc from 'detect-libc'
// eslint-disable-next-line import-x/no-extraneous-dependencies
import tar from 'tar-stream'
import packageJson from './package.json' with { type: 'json' }

const opts = {
  arch: process.env.NODE_ARCH || process.arch,
  libc: process.env.LIBC || await detectLibc.family() || '',
  name: packageJson.name.replace('@melchor629/', ''),
  napiVersion: packageJson.binary.napi_versions[0],
  platform: process.platform,
  version: packageJson.version,
  npx: process.env.NPX || (process.platform === 'win32' ? 'npx.cmd' : 'npx'),
}

const run = async (command, args = [], pipe = true) => {
  const proc = cp.spawn(command, args, {
    encoding: 'utf-8',
    stdio: pipe ? 'inherit' : undefined,
    shell: process.platform === 'win32',
  })

  await once(proc, 'exit')

  return proc
}

const readFiles = async (dirPath) => {
  const files = await fs.readdir(path.resolve(dirPath))
  return files.map((p) => path.join(dirPath, p))
}

await fs.mkdir(path.resolve('prebuilds'), { recursive: true })
const packageName = `${opts.name}-${opts.platform}${opts.libc ? `-${opts.libc}` : ''}-${opts.arch}`
const tarPath = path.join('prebuilds', `${packageName}.tar.zstd`)

// build
process.stdout.write(`> Compiling for napi v${opts.napiVersion}\n\n`)
const compileProcess = await run(opts.npx, [
  'cmake-js',
  'rebuild',
  `--arch=${opts.arch}`,
  `--CDnapi_build_version=${opts.napiVersion}`,
  '--CDUSE_FLAC_SOURCES',
  '-p',
  os.cpus().length.toString(),
])

if (compileProcess.exitCode) {
  process.stderr.write('> Compilation failed!')
  process.exit(1)
}

// strip
if (process.platform !== 'win32') {
  process.stdout.write('\n> Stripping binaries\n')
  for (const releaseFilePath of await readFiles(path.join('build', 'Release'))) {
    if (releaseFilePath.endsWith('.node')) {
      process.stdout.write(`  - ${releaseFilePath}\n`)
      const args = [releaseFilePath]
      if (process.platform === 'darwin') {
        args.push('-Sx')
      } else if (['linux', 'freebsd'].includes(process.platform)) {
        args.push('--strip-all')
      }
      await run(process.env.STRIP || 'strip', args)
    }
  }
}

// add additional files
process.stdout.write('\n> Adding additional files\n')
const README = path.join('build', 'Release', 'README.md')
process.stdout.write(`  - ${README}\n`)
await fs.writeFile(README, `# ${packageName}

Native library of [\`flac-bindings\`](https://www.npmjs.com/package/flac-bindings) for version ${opts.version} and napi v${opts.napiVersion}, OS ${opts.platform}${opts.libc} and CPU ${opts.arch}.
`)
const napiPackageJson = path.join('build', 'Release', 'package.json')
process.stdout.write(`  - ${napiPackageJson}\n`)
await fs.writeFile(napiPackageJson, JSON.stringify({
  name: `@melchor629/${packageName}`,
  version: opts.version,
  main: 'flac-bindings.node',
  files: ['flac-bindings.node', 'README.md'],
  cpu: [opts.arch],
  os: [opts.platform],
  libc: opts.libc ? [opts.libc] : undefined,
  author: packageJson.author,
  license: packageJson.license,
  repository: packageJson.repository,
  bugs: packageJson.bugs,
  homepage: packageJson.homepage,
  engines: packageJson.engines,
  libflac: packageJson.libflac,
  libogg: packageJson.libogg,
}, null, 2))

// package
process.stdout.write(`\n> Packaging for napi v${opts.napiVersion}\n`)
const tarStream = tar.pack()
const outputStream = createWriteStream(tarPath)
const zstdStream = zlib.createZstdCompress({ params: { [zlib.constants.ZSTD_c_compressionLevel]: 12 } })

tarStream.pipe(zstdStream).pipe(outputStream)

for (const releaseFilePath of await readFiles(path.join('build', 'Release'))) {
  const releaseFileStat = await fs.stat(releaseFilePath)
  const releaseFileStream = tarStream.entry({
    name: releaseFilePath
      .replace(/\\/g, '/')
      .replace(/:/g, '_')
      .replace('build/Release', packageName),
    size: releaseFileStat.size,
    mode: releaseFileStat.mode | 0o222 | 0o444,
    gid: releaseFileStat.gid,
    uid: releaseFileStat.uid,
  })

  await once(createReadStream(releaseFilePath).pipe(releaseFileStream), 'finish')
}

tarStream.finalize()
await once(outputStream, 'finish')
process.stdout.write(`  - Stored file in ${tarPath}\n`)
