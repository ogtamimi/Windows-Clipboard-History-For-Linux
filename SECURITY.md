# Security Policy

## Supported Versions

We currently provide security fixes for the latest stable release of **Clipboard History**.

Older releases are not actively maintained. Please update to the latest release from the [Releases page](https://github.com/ogtamimi/clipboard-history/releases) before reporting an issue.

| Version               | Supported         |
| --------------------- | ----------------- |
| Latest stable release | Yes             |
| Older releases        | No              |
| Development (`main`)  | Not guaranteed |

## Reporting a Vulnerability

If you discover a security vulnerability in Clipboard History, **please do not open a public GitHub issue**.

Instead, report it privately using one of the following methods:

1. **Preferred:** Use GitHub's [private vulnerability reporting](https://github.com/ogtamimi/clipboard-history/security/advisories/new) feature:
   **Security → Report a vulnerability**

2. Alternatively, contact the maintainer, **ogtamimi**, privately through GitHub.

Please include as much of the following information as possible:

* A clear description of the vulnerability and its potential impact
* Steps to reproduce the issue
* Proof-of-concept code or commands, if applicable
* The affected version, commit, or package
* Your Linux distribution and version
* Your desktop environment
* Your session type (X11, XWayland, etc.)
* Any relevant logs or screenshots

Please avoid including sensitive personal information in your report.

## What to Expect

### Acknowledgement

We aim to acknowledge new security reports within **5 business days**.

### Assessment

We will investigate the report, assess its severity and impact, and may request additional information if necessary.

### Fix and Disclosure

Once a vulnerability is confirmed, we will work on a fix and release a patched version when appropriate.

Security reporters may be credited in the security advisory or release notes unless they prefer to remain anonymous.

We ask reporters to allow reasonable time for investigation and remediation before publicly disclosing a vulnerability.

## Scope

Clipboard History is a local Linux application that stores clipboard history in a local SQLite database.

Security reports of particular interest include, but are not limited to:

* Local privilege escalation through the application, installation process, `.deb` package, or installation scripts
* Arbitrary code execution caused by malicious or malformed clipboard content
* Unsafe processing of text, rich text, images, or other clipboard data
* Vulnerabilities involving the local SQLite database or stored clipboard data
* Path traversal or arbitrary file access through clipboard content or application functionality
* Security issues involving image storage or file handling
* Vulnerabilities in X11 global hotkey handling
* Paste injection or unsafe XTest interactions that could be abused by another local process
* Vulnerabilities that allow unauthorized access to clipboard history

## Out of Scope

The following generally do not qualify as security vulnerabilities unless they demonstrate a meaningful security impact:

* General bugs without a security impact
* Feature requests
* UI/UX issues
* Performance issues
* Vulnerabilities that require unrestricted root access unless they demonstrate additional privilege escalation
* Issues affecting unsupported or obsolete releases

For general bugs and feature requests, please use the appropriate GitHub issue templates.

## Responsible Disclosure

Please give the maintainers reasonable time to investigate and address reported vulnerabilities before publicly disclosing them.

We appreciate responsible disclosure and will work with security researchers to understand, reproduce, and resolve legitimate security issues.

Thank you for helping keep **Clipboard History** and its users safe.
