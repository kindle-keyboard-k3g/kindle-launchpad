# Kindle Launchpad 🚀⌨️

**Kindle Launchpad** is a lightweight, low-level hotkey daemon and application launcher for Amazon Kindle e-ink devices featuring physical keyboards, primarily the **Kindle Keyboard (Kindle 3 / K3G / K3W)**, **Kindle DX (DX, DXi, DXG)**, and **Kindle 2 (K2, K2i)**.

It intercepts hardware key combinations directly from the Linux input subsystem (`/dev/input/event*`) while preserving the native Amazon Kindle framework, allowing users to trigger custom shell scripts, launch native applications, capture screenshots, and control system services without interrupting the stock reading experience.

---

## 📌 Latest Version: `0.0.1d`

The latest packaged release of Kindle Launchpad is **version 0.0.1d** (September 11, 2015), repackaged by **eschwartz** using modern `kindletool` binaries.

> [!NOTE]
> Version 0.0.1d specifically addresses and resolves the common **"Update Error: 3"** or greyed-out **"Update Your Kindle"** issue on newer Kindle firmware releases (such as **FW 3.4+** on Kindle Keyboard and Kindle DX) by generating correct OTA target version metadata.

* **Primary Discussion Thread**: [MobileRead #97636](https://www.mobileread.com/forums/showthread.php?t=97636)
* **Changelog & History**: See [`CHANGELOG.md`](CHANGELOG.md)

---

## 📦 Package Matrix (Pre-built Binaries)

Ready-to-install `.bin` packages are located in the [`packages/`](packages/) directory:

| Kindle Device Model | Device Code | Serial Prefix | Install Package | Uninstall Package |
| :--- | :---: | :---: | :--- | :--- |
| **Kindle Keyboard 3G (US)** | `k3g` | `B006` | [`update_launchpad_0.0.1d_k3g_install.bin`](packages/update_launchpad_0.0.1d_k3g_install.bin) | [`update_launchpad_0.0.1d_k3g_uninstall.bin`](packages/update_launchpad_0.0.1d_k3g_uninstall.bin) |
| **Kindle Keyboard 3G (UK/EU)** | `k3gb` | `B00A` | [`update_launchpad_0.0.1d_k3gb_install.bin`](packages/update_launchpad_0.0.1d_k3gb_install.bin) | [`update_launchpad_0.0.1d_k3gb_uninstall.bin`](packages/update_launchpad_0.0.1d_k3gb_uninstall.bin) |
| **Kindle Keyboard WiFi** | `k3w` | `B008` | [`update_launchpad_0.0.1d_k3w_install.bin`](packages/update_launchpad_0.0.1d_k3w_install.bin) | [`update_launchpad_0.0.1d_k3w_uninstall.bin`](packages/update_launchpad_0.0.1d_k3w_uninstall.bin) |
| **Kindle DX Graphite** | `dxg` | `B009` | [`update_launchpad_0.0.1d_dxg_install.bin`](packages/update_launchpad_0.0.1d_dxg_install.bin) | [`update_launchpad_0.0.1d_dxg_uninstall.bin`](packages/update_launchpad_0.0.1d_dxg_uninstall.bin) |
| **Kindle DX International** | `dxi` | `B005` | [`update_launchpad_0.0.1d_dxi_install.bin`](packages/update_launchpad_0.0.1d_dxi_install.bin) | [`update_launchpad_0.0.1d_dxi_uninstall.bin`](packages/update_launchpad_0.0.1d_dxi_uninstall.bin) |
| **Kindle DX (US)** | `dx` | `B004` | [`update_launchpad_0.0.1d_dx_install.bin`](packages/update_launchpad_0.0.1d_dx_install.bin) | [`update_launchpad_0.0.1d_dx_uninstall.bin`](packages/update_launchpad_0.0.1d_dx_uninstall.bin) |
| **Kindle 2 International** | `k2i` | `B003` | [`update_launchpad_0.0.1d_k2i_install.bin`](packages/update_launchpad_0.0.1d_k2i_install.bin) | [`update_launchpad_0.0.1d_k2i_uninstall.bin`](packages/update_launchpad_0.0.1d_k2i_uninstall.bin) |
| **Kindle 2 (US)** | `k2` | `B002` | [`update_launchpad_0.0.1d_k2_install.bin`](packages/update_launchpad_0.0.1d_k2_install.bin) | [`update_launchpad_0.0.1d_k2_uninstall.bin`](packages/update_launchpad_0.0.1d_k2_uninstall.bin) |

---

## 🛠️ Installation

### Prerequisites
1. A **jailbroken** Kindle device (Jailbreak version 0.13.N or higher recommended).
2. USB cable connecting your Kindle to your computer.

### Step-by-Step
1. Connect your Kindle to your PC in USB Drive mode.
2. Choose the `.bin` installer from [`packages/`](packages/) matching your exact device model (e.g. `update_launchpad_0.0.1d_k3g_install.bin` for Kindle Keyboard 3G).
3. Copy the chosen `.bin` file to the root directory of your Kindle's USB user storage (`/mnt/us/`).
4. Safely eject and unplug the USB cable from the Kindle.
5. On the Kindle device, navigate to:
   - **Home** -> press **[Menu]**
   - Select **Settings**
   - Press **[Menu]** again
   - Select **Update Your Kindle**
6. Wait for the update process to complete. The device will restart and Launchpad will automatically start in the background during boot (`/etc/init.d/launchpad`).

---

## ⌨️ Default Hotkeys & Shortcuts

Launchpad actions begin with the **Introducer Key** (`Shift` by default), followed by key sequence letters:

| Sequence | Action Description | Source Config |
| :--- | :--- | :--- |
| `Shift` `Shift` `Space` | **Reload Hotkey Database**: Re-scans all `.ini` files in `/mnt/us/launchpad/` without rebooting. | `servicecmds.ini` |
| `Shift` `Shift` `Shift` | **Take Screenshot**: Dumps framebuffer to `/mnt/us/screenshot.bmp`. | Built-in |
| `Shift` `Shift` `S` | **Sleep / Screensaver**: Triggers device sleep mode. | `servicecmds.ini` |
| `Shift` `Shift` `A` | **Restart Framework**: Restarts native Kindle UI (`/etc/init.d/framework restart`). | `servicecmds.ini` |
| `Shift` `Shift` `R` | **Reboot**: Safely reboots the Kindle. | `servicecmds.ini` |
| `Shift` `Shift` `H` | **Shutdown**: Powers down the Kindle. | `servicecmds.ini` |
| `Shift` `Shift` `I` | **Custom Updates**: Unpacks and runs update scripts from `/mnt/us/customupdates/`. | `servicecmds.ini` |
| `Shift` `F` `B` | **FBReader**: Launches FBReader ebook reader. | `fbreader.ini` |
| `Shift` `F` `R` | **fbKindle**: Launches the fbKindle port of FBReader. | `fbreader.ini` |

---

## ⚙️ Configuration Format

Launchpad configuration files are located in `/mnt/us/launchpad/` on the device. Any `.ini` file in this directory is automatically parsed at startup or upon pressing `Shift Shift Space`.

### Section Syntax
```ini
[Settings]
; Hot Interval in milliseconds to wait for subsequent keys
HotInterval = 700
; Delay between simulated keystrokes
InterKeyDelay = 50

[Actions]
; Action format: <KEY SEQUENCE> = <COMMAND>
; Command Prefixes:
; '!' -> Shell command
; '@' -> Kindle Framework script
; '#' -> Kindle Framework keystroke
; raw string -> Simulated key event sequence
```

### Examples
```ini
[Actions]
; Launch a custom shell script
U N = !/mnt/us/usbnet/bin/usbnet toggle

; Start KiTerm terminal
K T = !/mnt/us/kiterm/bin/kiterm

; Emulate key sequence into framework search box
G H = "http://github.com"
```

---

## 🏗️ Building from Source

### Native Host Build
```bash
make clean
make
```

### Kindle ARMv6 Cross-Compilation
```bash
# Uses arm-linux-gnueabi-gcc toolchain
make clean
make kindle
```
This produces an optimized ARMv6 binary (`binarm/launchpad`) stripped and ready for deployment on Kindle 2, DX, and Keyboard 3 devices.

---

## 📁 Repository Structure

```
kindle-launchpad/
├── CHANGELOG.md              # Complete version history and changelog
├── LICENSE                   # GNU General Public License v2 (GPL-2.0)
├── README.md                 # Project documentation and guide
├── makefile                  # Build script supporting native and ARM cross-compiles
├── src/                      # C source code (main, launchpad, inifile, screen, etc.)
├── include/                  # Header files (fnt12x22, einkfb, keydefs, etc.)
├── scripts/                  # Default helper scripts (SHIFT-*.sh)
├── customupdate.sh           # Script executed by Shift Shift I
├── launchpad.ini              # Main runtime configuration
├── keydefs.ini.default       # Key mapping definition template
├── servicecmds.ini           # System maintenance hotkey definitions
├── fbreader.ini              # Reader hotkey definitions
├── rce.ini                   # Remote Command Entry configuration
├── packages/                 # Version 0.0.1d OTA installer .bin packages for K2/DX/K3
├── packaging/                # Original update packager tool and scripts
│   ├── packager/kindle_update_tool.py
│   └── src/install.sh, uninstall.sh, build-updates.sh
├── archives/                 # Pristine upstream releases (.zip and .tar.gz)
└── contrib/                  # Popular community plugins (rotate, prevent_ss, record, sendtoreader)
```

---

## 📜 Credits & License

* **Original Author**: Andy M. aka **h1uke** (`h1ukeguy@gmail.com`)
* **Packaging & Updates**: **NiLuJe**, **eschwartz**
* **Community**: [MobileRead Kindle Developer's Corner](https://www.mobileread.com/forums/forumdisplay.php?f=150)
* **License**: [GNU General Public License v2.0](LICENSE) (GPL-2.0)
