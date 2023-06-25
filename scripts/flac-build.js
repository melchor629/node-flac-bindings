import cp from 'node:child_process'
import fs, { createReadStream, createWriteStream } from 'node:fs'
import https from 'node:https'
import path from 'node:path'
import { pipeline } from 'node:stream'
import { promisify } from 'node:util'
import zlib from 'node:zlib'
import debugFactory from 'debug'
import detectLibc from 'detect-libc'
import tar from 'tar-stream'

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

/**
 * @param {string | URL} url url
 * @returns {Promise<import('http').IncomingMessage>} promise
 */
const fetchGet = (url) =>
  new Promise((resolve, reject) => https.get(url, (res) => {
    if (res.statusCode === 302) {
      // follow redirection
      fetchGet(res.headers.location).then(resolve).catch(reject)
    } else {
      resolve(res)
    }
  }).on('error', reject))

/**
 * @param {import('stream').Readable} stream read stream
 * @returns {Promise<void>} promise
 */
const extractTarStream = async (stream) => {
  const tarStream = tar.extract()
  tarStream.on('entry', (header, stream, next) => {
    if (!path.resolve(header.name).startsWith(process.cwd())) {
      debug(`File ${header.name} will exit the current folder, ignoring`)
      return
    }

    debug(`Extracting file ${header.name}`)
    fs.mkdirSync(path.dirname(header.name), { recursive: true })
    stream.pipe(createWriteStream(header.name))
    stream.on('end', next)
    stream.resume()
  })

  await promisify(pipeline)(stream, zlib.createBrotliDecompress(), tarStream)
}

const getFromPrebuilt = async () => {
  debug('Looking for prebuild packages')
  const packageJson = JSON.parse(fs.readFileSync('./package.json', 'utf-8'))
  const [napiVersion] = packageJson.binary.napi_versions
    .filter(v => v <= parseInt(process.versions.napi, 10))
    .sort((a, b) => a - b)
  const fileName = [
    packageJson.name,
    '-v', packageJson.version,
    '-napi',
    '-v', napiVersion,
    '-', process.platform,
    (detectLibc.isNonGlibcLinuxSync() && detectLibc.familySync()) || '',
    '-', process.arch,
    '.tar.br',
  ].join('')
  const tarPath = path.join('prebuilds', fileName)
  const tarUrl = new URL(`releases/download/v${packageJson.version}/${fileName}`, packageJson.repository)

  debug(`Downloading from ${tarUrl}`)
  const res = await fetchGet(tarUrl).catch((e) => {
    debug(`Donload failed: ${e.message}`)
    return null
  })
  if (res?.statusCode === 200) {
    debug(`Found one prebuild package: ${tarUrl}`)
    await extractTarStream(res)
    return true
  } else if (res) {
    res.setEncoding('utf-8')
    const chunks = []
    for await (const chunk of res) {
      chunks.push(chunk)
    }

    debug(`Download failed ${res.statusCode} ${res.statusMessage}`)
    if (res.statusCode !== 404) {
      debug(chunks.join(''))
    }
  }

  debug(`Reading from ${tarPath}`)
  if (!fs.existsSync(tarPath)) {
    debug('No suitable prebuild packages found')
    return false
  }

  debug(`Found one prebuild package: ${tarPath}`)
  await extractTarStream(createReadStream(tarPath))

  return true
}

if (envOpts.ci) {
  debug('CI environment, stopping build')
  process.exit(0)
}

debug('Trying to install from prebuilt package...')
if (await getFromPrebuilt()) {
  debug('Installed succesfully')
  process.exit(0)
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
