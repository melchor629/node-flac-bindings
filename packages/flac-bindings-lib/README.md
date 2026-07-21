# Native code of `flac-bindings`

Optional dependency of [`flac-bindings`](https://www.npmjs.com/package/flac-bindings). This package contains the native code for compiling the bindings in machines that do not have prebuilt packages or it is prefered to have more control of the generated native code.

Required software to have installed:

- `cmake`
- A C compiler (MSVC, clang or gcc)
- A C++ compiler (MSVC, clang or gcc)
- `git`

Additionally, allow this package to run `postinstall` scripts in order to compile the bindings during install. Otherwise, run `npm rebuild @melchor629/flac-bindings-lib`.

In Windows, may be easy to start with `npm install --global --production windows-build-tools`, and use chocolatey or scoop to install `cmake` and `git`.

## Compile using OS-provided libraries

It is possible to use already installed libraries from the package manager of your system to link the bindings to. For linux and macOS, it is needed:

- `pkg-config`
- `libflac` with headers (`apt install libflac-dev`, `pacman -S flac`, `apk add flac-dev`, `brew install flac`...)
- `libogg` with headers (probably installed with flac)
- `git` is not needed for this

Once all dependencies are installed, the build will look for them and use them directly.

> Supported `libFLAC` versions are 1.3.x and 1.4.x (binary versions 10 and 12).
