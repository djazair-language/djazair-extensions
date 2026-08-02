# dpack - Djazair Standalone Bundler

`dpack` is the official bundler for the **Djazair Programming Language**. It allows you to package your Djazair scripts (`.dz`) into self-contained, standalone executables that can run on any machine—even if Djazair is not installed on the target system.

## Features

- **Zero-Dependency Executables**: Bundles the Djazair interpreter, standard library (`std`), and `prelude` natively.
- **Smart Extension Detection**: Automatically scans your source code for `use` statements and bundles any required extensions (e.g., `raylib`, `qt`, `zip`, `kasbah`).
- **Media & Asset Bundling**: Easily bundle external assets like images, sounds, fonts, and directories alongside your script.
- **Automatic Path Resolution**: `dpack` intelligently resolves relative paths based on the directory of the script that invoked it, saving you from path-management headaches.
- **Cross-Platform Support**: Generates `.exe` files on Windows and native binaries on Linux.

---

## Installation

`dpack` comes pre-installed as a core extension with Djazair. If you are developing it, make sure you have built the C-stub by running:

```bash
cd extensions/dpack
build.bat  # On Windows
```

---

## Basic Usage

To package a script, simply create a build script (e.g., `pack.dz`) next to your main script:

```dz
use dpack

# Packs 'app.dz' into an executable automatically named 'app.exe'
dpack.pack("app.dz")
```

Run your build script using Djazair:
```bash
djazair pack.dz
```

---

## Advanced Usage & API

The `pack` function signature provides complete control over the bundling process:

```dz
dpack.pack(scriptPath, outputPath = Null, extraExtensions = [], assets = [], quiet = False)
```

### Parameters:
- **`scriptPath`** *(String)*: The relative or absolute path to the `.dz` entry script you want to package.
- **`outputPath`** *(String | Null)*: The path/name of the generated executable. If `Null`, `dpack` will automatically generate it in the same directory as the source script (e.g., `app.exe`).
- **`extraExtensions`** *(Array)*: An array of strings representing any hidden or dynamically loaded extensions that the scanner might miss. Example: `["sqlite", "kasbah"]`.
- **`assets`** *(Array)*: (Deprecated/Optional) An array of strings representing additional files or folders to include. Since version 1.0, `dpack` **automatically** bundles all files and directories located in the same directory as the entry script, so you rarely need to use this.
- **`quiet`** *(Boolean)*: If `True`, disables the build logs in the console.

### Example: Bundling a Game with Assets

If you have a game that relies on external files (e.g., sound effects, `images/` folder, etc.) located in the same folder as the script, you don't need to specify them. `dpack` automatically detects and bundles them:

```dz
use dpack

# Packs 'snake.dz' into an executable, and automatically bundles 
# any 'death.wav', 'eat.wav', or 'assets/' folder found next to 'snake.dz'
dpack.pack("snake.dz")
```

---

## How It Works

Under the hood, `dpack` works by:
1. Creating a temporary staging directory (`.dpack_stage_tmp`).
2. Copying your script (renaming it to `__main__.dz`), the interpreter, standard libraries, extensions, and user-defined assets into the staging directory.
3. Compressing the staging directory into a Zip archive.
4. Appending the Zip archive to a pre-compiled C-binary stub (`stub/stub_win.exe`).
5. When the user launches the final executable, the stub transparently extracts the Zip archive into the system's temporary folder, runs the interpreter silently, and cleans up the files upon exit.
