# Contributing to Clipboard History

Thank you for your interest in contributing to **Clipboard History**!

Whether you're fixing a bug, improving the user interface, adding a feature, improving documentation, or testing the application on another Linux distribution, your contribution is welcome.

This guide explains how to set up the project, make changes, run tests, and submit a pull request.

By participating in this project, you agree to follow our [Code of Conduct](CODE_OF_CONDUCT.md).

---

## Table of Contents

* [Ways to Contribute](#ways-to-contribute)
* [Before You Start](#before-you-start)
* [Development Setup](#development-setup)
* [Making Changes](#making-changes)
* [Commit Message Guidelines](#commit-message-guidelines)
* [Testing](#testing)
* [Submitting a Pull Request](#submitting-a-pull-request)
* [Reporting Bugs](#reporting-bugs)
* [Suggesting Features](#suggesting-features)
* [Reporting Security Issues](#reporting-security-issues)

---

## Ways to Contribute

There are many ways to contribute to Clipboard History:

* Report bugs and unexpected behavior
* Suggest new features or improvements
* Improve documentation, examples, and guides
* Fix bugs
* Implement new features
* Improve the user interface or user experience
* Add or improve automated tests
* Improve performance, reliability, or accessibility
* Test the application on different Linux distributions and desktop environments
* Review pull requests and provide constructive feedback

You don't need to contribute code to help improve the project.

---

## Before You Start

Before starting work, please:

1. Check the [existing issues](https://github.com/ogtamimi/clipboard-history/issues) to see whether the problem or feature has already been discussed.
2. Check the [existing pull requests](https://github.com/ogtamimi/clipboard-history/pulls) to avoid duplicating ongoing work.
3. For significant changes, such as new features, major UI changes, or architectural changes, open an issue first to discuss the proposed approach.
4. Small fixes such as documentation updates, typo corrections, and minor bug fixes can generally be submitted directly as a pull request.
5. Make sure your changes are relevant to the project and avoid unrelated modifications.

If you're unsure whether an idea is appropriate, opening an issue to discuss it is encouraged.

---

## Development Setup

### Requirements

Clipboard History is currently developed and tested on 64-bit Debian-based Linux systems using X11.

Supported environments include:

* Debian 12+
* Ubuntu 22.04+
* Linux Mint 21+
* X11 or a compatible X11 environment
* C++23-compatible compiler
* CMake
* Qt 6
* SQLite 3
* Git

Some tests may require additional system packages.

### 1. Fork and Clone the Repository

Fork the repository on GitHub and clone your fork:

```bash
git clone https://github.com/<your-username>/clipboard-history.git
cd clipboard-history
```

Add the upstream repository so you can keep your local copy synchronized:

```bash
git remote add upstream https://github.com/ogtamimi/clipboard-history.git
```

### 2. Install Dependencies

The project includes a dependency installation script:

```bash
chmod +x install-dependencies.sh
./install-dependencies.sh
```

If you encounter dependency issues, check the [README](README.md) for additional build information.

### 3. Build the Project

Configure the project:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

Build the application:

```bash
cmake --build build -j"$(nproc)"
```

### 4. Run the Application

After a successful build:

```bash
./build/clipboard-history
```

For additional build and packaging instructions, see the [README](README.md#-build--package-contributors).

---

## Making Changes

### 1. Create a Branch

Start from the latest `main` branch:

```bash
git checkout main
git pull upstream main
git checkout -b feature/short-description
```

Use a descriptive branch name.

Examples:

```text
feature/search-filter
feature/image-preview
fix/duplicate-entries
fix/keyboard-shortcut
docs/update-installation-guide
refactor/clipboard-storage
```

### 2. Keep Changes Focused

Keep each branch and pull request focused on a single purpose whenever possible.

Avoid combining unrelated changes into the same pull request.

Focused pull requests are easier to review, test, and maintain.

### 3. Follow Existing Code Conventions

Clipboard History uses:

* C++23
* Qt 6
* QML
* SQLite
* CMake

Follow the existing code structure, naming conventions, formatting, and architectural patterns.

Avoid introducing unnecessary dependencies or making unrelated refactors.

### 4. Update Documentation

Update the relevant documentation when your changes affect:

* Application behavior
* User-facing features
* Keyboard shortcuts
* Installation
* Configuration
* Build instructions
* Troubleshooting
* Packaging

### 5. Add or Update Tests

Add or update tests when appropriate.

Changes that affect application behavior should include relevant test coverage whenever practical.

---

## Commit Message Guidelines

Use clear and descriptive commit messages.

The preferred format is:

```text
<type>: short summary

Optional longer description explaining why the change was made.
```

Common commit types:

| Type       | Purpose            |
| ---------- | ------------------ |
| `feat`     | New feature        |
| `fix`      | Bug fix            |
| `docs`     | Documentation      |
| `refactor` | Code restructuring |
| `test`     | Tests              |
| `chore`    | Maintenance        |
| `ci`       | CI/CD changes      |

Example:

```text
fix: prevent duplicate entries when copying identical images
```

Keep the first line concise and describe the purpose of the change clearly.

---

## Testing

Before submitting a pull request, make sure the project builds successfully and relevant tests pass.

### Build

```bash
cmake --build build -j"$(nproc)"
```

### Integration Tests

Run the integration test harness:

```bash
CLIPBOARD_DEBUG=1 ./build/clipboard-history &
python3 tests/integration_harness.py
```

The integration test suite requires:

* Python 3
* `python3-tk`
* `python3-xlib`
* `xwininfo`
* An active X11 display

The test suite should exit with status `0`.

If your change introduces new behavior, add or update the appropriate tests when practical.

### Manual Testing

For changes involving the UI, clipboard handling, keyboard shortcuts, system tray behavior, or desktop integration, manually test the affected functionality.

At minimum, verify that existing clipboard functionality continues to work as expected.

---

## Submitting a Pull Request

### 1. Push Your Branch

Push your branch to your fork:

```bash
git push origin feature/short-description
```

### 2. Open a Pull Request

Open a pull request against the `main` branch of the [Clipboard History repository](https://github.com/ogtamimi/clipboard-history).

### 3. Describe Your Changes

Complete the pull request template and clearly explain:

* **What** changed
* **Why** the change was needed
* **How** the change was implemented
* **How** it was tested
* Any related issues or discussions

When applicable, link related issues.

For example:

```text
Closes #123
```

### 4. Verify Your Pull Request

Before requesting review, make sure:

* The project builds successfully
* Relevant tests pass
* Your changes are focused
* Documentation is updated when necessary
* No unnecessary files or build artifacts are committed
* Your branch is up to date with `main`

### 5. Respond to Review Feedback

Maintainers may request changes before merging.

Please respond to review comments constructively and push additional commits to your branch when changes are requested.

All contributors are expected to follow the project's [Code of Conduct](CODE_OF_CONDUCT.md).

---

## Reporting Bugs

Please use the **Bug Report** issue template when reporting a bug.

Include as much relevant information as possible:

* Linux distribution and version
* Desktop environment
* X11 or XWayland
* Clipboard type involved, if relevant
* Steps to reproduce the issue
* Expected behavior
* Actual behavior
* Relevant logs or error messages
* Any troubleshooting steps you have already tried

When applicable, enable debug logging:

```bash
CLIPBOARD_DEBUG=1 ./build/clipboard-history
```

Please remove passwords, tokens, personal information, or other sensitive data from logs before submitting them.

---

## Suggesting Features

Please use the **Feature Request** issue template for feature suggestions.

Describe:

* The problem you are trying to solve
* Why the feature would be useful
* Your proposed solution
* Alternative solutions you considered
* Examples or references, if relevant

Feature requests are welcome, but they should remain aligned with the project's goals and scope.

---

## Reporting Security Issues

**Please do not open a public GitHub issue for security vulnerabilities.**

Security vulnerabilities should be reported privately so they can be investigated and addressed responsibly.

Please see [SECURITY.md](SECURITY.md) for the project's security reporting process.

---

## Code of Conduct

Please read the project's [Code of Conduct](CODE_OF_CONDUCT.md) before contributing.

All contributors are expected to participate respectfully and constructively.

---

## Thank You

Thank you for helping make **Clipboard History** better!

Every bug report, feature suggestion, documentation improvement, test, and code contribution helps improve the project for everyone.
