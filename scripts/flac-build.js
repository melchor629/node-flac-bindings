const cp = require('child_process');

const envOpts = {
    useExternalLibrary: typeof process.env.FLAC_BINDINGS_USE_EXTERNAL_LIBRARY === 'string',
    pkgConfigPath: process.env.PKG_CONFIG_PATH,
};

const run = (command, pipe = true) => {
    console.log(`> ${command}`);
    const proc = cp.spawnSync(command, {
        encoding: 'utf-8',
        shell: true,
        stdio: pipe ? 'inherit' : undefined,
    });
    if(proc.error) {
        throw proc.error;
    }

    return proc;
};

// from https://github.com/lovell/sharp/blob/master/lib/libvips.js
const pkgConfigPath = function () {
    if(process.platform !== 'win32') {
        const brewPkgConfigPath = run(
            'which brew >/dev/null 2>&1 && eval $(brew --env) && echo $PKG_CONFIG_LIBDIR',
            false,
        ).stdout || '';
        return [ brewPkgConfigPath.trim(), envOpts.pkgConfigPath, '/usr/local/lib/pkgconfig', '/usr/lib/pkgconfig' ]
            .filter((p) => !!p)
            .join(':');
    } else {
        return '';
    }
};

const hasGlobalInstalledFlac = () => {
    if(process.platform !== 'win32') {
        const flacVersion = (
            run(
                `PKG_CONFIG_PATH="${pkgConfigPath()}" pkg-config --modversion flac`,
                false,
            ).stdout || ''
        ).trim();
        if(!flacVersion) {
            return false;
        }

        const [ major, minor ] = flacVersion.split('.').map((n) => parseInt(n, 10));
        return major === 1 && minor >= 3;
    }

    return false;
};

if(!envOpts.useExternalLibrary) {
    if(!run('prebuild-install').status) {
        process.exit(0);
    }
}

if(envOpts.useExternalLibrary || hasGlobalInstalledFlac()) {
    if(run('cmake-js configure --CDFLAC_BINDINGS_USE_EXTERNAL_LIBRARY=ON').status) {
        process.exit(1);
    }
}

if(run('cmake-js build').status) {
    process.exit(1);
}
