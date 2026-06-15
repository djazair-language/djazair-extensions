# Djazair Extensions

<div align="center">

[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![Organization](https://img.shields.io/badge/organization-djazair--language-blue.svg)](https://github.com/djazair-language)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](https://github.com/djazair-language/djazair-extensions/pulls)

**Official extensions, modules, and packages for the Djazair Programming Language.**

[Installation](#-installation) · [Contributing](#-contributing)

</div>

---

This repository is a collection of official native extensions and libraries for the **Djazair** programming language. They extend the core capabilities of the language to support databases, web development, desktop GUIs, networking, and game development.

---

## ⚡ Installation

You can easily install any extension using **DPM (Djazair Package Manager)** which is bundled with the language.

### Basic Installation
To install an extension, open your terminal and run:
```bash
dpm install <package_name>
```

**Examples:**
```bash
dpm install sqlite
dpm install mysql
dpm install raylib
```

### Installing a Web Shell / MVC App (`dweb`)
`dweb` requires both `mysql` and `sqlite` to function. The package manager will automatically download and resolve these dependencies recursively:
```bash
dpm install dweb
```

---

## 🛠️ Building Native Extensions

Some extensions contain C/C++ source code that needs to be compiled to native DLLs. DPM manages this for you.

To compile the installed extension in your local project:
```bash
dpm build <package_name>
```

**Example:**
```bash
dpm build sqlite
```
*Note: Ensure you have a C compiler (`gcc` or `clang` / MinGW) installed on your system PATH.*

---

## 🤝 Contributing

Contributions are very welcome! If you want to improve an extension or add a new one:

1. **Fork** the repository.
2. Create your feature branch (`git checkout -b feature/amazing-extension`).
3. Commit your changes.
4. Push to the branch.
5. Open a **Pull Request**.

---

## 📄 License

This repository is licensed under the MIT License. See [LICENSE](LICENSE) for details.
