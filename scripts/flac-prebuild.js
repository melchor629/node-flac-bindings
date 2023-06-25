import cp from 'node:child_process'
import { once } from 'node:events'
import { createReadStream, createWriteStream } from 'node:fs'
import os from 'node:os'
import fs from 'node:fs/promises'
import path from 'node:path'
import zlib from 'node:zlib'
import detectLibc from 'detect-libc'
import tar from 'tar-stream'
import packageJson from '../package.json' assert { type: 'json' }

const opts = {
  arch: process.env.NODE_ARCH || process.arch,
  libc: process.env.LIBC || (await detectLibc.isNonGlibcLinux() && await detectLibc.family()) || '',
  name: packageJson.name,
  napiVersions: packageJson.binary.napi_versions,
  platform: process.platform,
  version: packageJson.version,
  npx: process.env.NPX || (process.platform === 'win32' ? 'npx.cmd' : 'npx'),
}

const run = async (command, args = [], pipe = true) => {
  const proc = cp.spawn(command, args, {
    encoding: 'utf-8',
    stdio: pipe ? 'inherit' : undefined,
  })

  await once(proc, 'exit')

  return proc
}

const readFiles = async (dirPath) => {
  const files = await fs.readdir(path.resolve(dirPath))
  return files.map((p) => path.join(dirPath, p))
}

await fs.mkdir(path.resolve('prebuilds'), { recursive: true })
for (const napiVersion of opts.napiVersions) {
  const tarPath = path.join(
    'prebuilds',
    `${opts.name}-v${opts.version}-napi-v${napiVersion}-${opts.platform}${opts.libc}-${opts.arch}.tar.br`,
  )

  // build
  process.stdout.write(`> Compiling for napi v${napiVersion}\n\n`)
  const compileProcess = await run(opts.npx, [
    'cmake-js',
    'rebuild',
    `--arch=${opts.arch}`,
    `--CDnapi_build_version=${napiVersion}`,
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
    process.stdout.write(`\n> Stripping binaries\n`)
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

  // package
  process.stdout.write(`\n> Packaging for napi v${napiVersion}\n`)
  const tarStream = tar.pack()
  const outputStream = createWriteStream(tarPath)
  const brotliStream = zlib.createBrotliCompress()

  tarStream.pipe(brotliStream).pipe(outputStream)

  for (const releaseFilePath of await readFiles(path.join('build', 'Release'))) {
    const releaseFileStat = await fs.stat(releaseFilePath)
    const releaseFileStream = tarStream.entry({
      name: releaseFilePath.replace(/\\/g, '/').replace(/:/g, '_'),
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
}
