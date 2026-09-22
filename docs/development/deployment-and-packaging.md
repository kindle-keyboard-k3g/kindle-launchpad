# Deployment & Kindle OTA Packaging Guide

This guide details the packaging, installation, and deployment lifecycle of **Kindle Launchpad** on Amazon Kindle devices.

---

## 1. Overview of Kindle Packaging Architecture

Kindle devices utilize signed OTA (Over-The-Air) update packages (`.bin` files) processed by the native Kindle recovery updater (`/usr/sbin/otaup`).

Each Launchpad update package contains:
1. `install.sh` or `uninstall.sh`: POSIX shell scripts executed with root privileges during the update process.
2. `launchpad-init`: SysV init script deployed to `/etc/init.d/launchpad` to auto-start the daemon during device boot.
3. `launchpad.tar.gz`: Compressed archive containing the compiled executable (`launchpad`), default INI configuration files, keymaps, and action scripts.

---

## 2. Packaging Build Process

Kindle Launchpad automates staging and packaging via CMake targets:

### 2.1 Staging Filesystem (`package-stage`)

The staging target compiles the ARMv6 binary and populates the staging directory structure matching the runtime installation path (`/mnt/us/launchpad/`):

```bash
# Compile and stage ARMv6 binaries and configuration files
cmake --build --preset kindle-k3-release --target package-stage
```

The staged files are placed in:
```
build/kindle-k3-release/staging/src/launchpad/
├── launchpad                  # Stripped ARMv6 ELF executable
├── launchpad.ini              # Core daemon settings and hotkeys
├── servicecmds.ini            # Framework control shortcuts
├── sample.ini                 # Sample configuration recipes
├── padkeys.txt                # Keycode reference documentation
└── scripts/                   # Shell scripts executed by @actions
```

### 2.2 Generating OTA Update Packages (`package-ota`)

To build install and uninstall update binaries for all supported hardware models:

```bash
# Build OTA packages for all target Kindle models
cmake --build --preset kindle-k3-release --target package-ota
```

This target invokes `packaging/src/build-updates.sh`, which uses `kindle_update_tool.py` (or `kindletool`) to generate signed `.bin` packages in `packaging/`:

```
packaging/
├── update_launchpad_0.0.2_k2_install.bin
├── update_launchpad_0.0.2_k2_uninstall.bin
├── update_launchpad_0.0.2_k2i_install.bin
├── update_launchpad_0.0.2_k2i_uninstall.bin
├── update_launchpad_0.0.2_dx_install.bin
├── update_launchpad_0.0.2_dx_uninstall.bin
├── update_launchpad_0.0.2_dxg_install.bin
├── update_launchpad_0.0.2_dxg_uninstall.bin
├── update_launchpad_0.0.2_k3g_install.bin
├── update_launchpad_0.0.2_k3g_uninstall.bin
├── update_launchpad_0.0.2_k3w_install.bin
└── update_launchpad_0.0.2_k3gb_install.bin
```

---

## 3. Supported Hardware Targets

| Target Code | Device Model | Generation | Screen / Keyboard |
| :--- | :--- | :--- | :--- |
| `k2` | Kindle 2 (US) | Gen 2 | 6" E-Ink, 4-row keyboard, 5-way |
| `k2i` | Kindle 2 International | Gen 2 | 6" E-Ink, 4-row keyboard, 5-way |
| `dx` | Kindle DX (US) | Gen 2.5 | 9.7" E-Ink, 5-row keyboard (numeric) |
| `dxi` | Kindle DX International | Gen 2.5 | 9.7" E-Ink, 5-row keyboard (numeric) |
| `dxg` | Kindle DX Graphite | Gen 2.5 | 9.7" Pearl E-Ink, 5-row keyboard |
| `k3g` | Kindle Keyboard 3G (US) | Gen 3 | 6" Pearl E-Ink, 4-row keyboard |
| `k3w` | Kindle Keyboard WiFi | Gen 3 | 6" Pearl E-Ink, 4-row keyboard |
| `k3gb`| Kindle Keyboard 3G (UK) | Gen 3 | 6" Pearl E-Ink, 4-row keyboard |

---

## 4. Device Installation Lifecycle

### 4.1 Automated OTA Installation

1. Connect the Kindle device to your computer via USB mass storage mode.
2. Select the matching `update_launchpad_*_<model>_install.bin` package for your device.
3. Copy the `.bin` file to the root of the Kindle USB drive (`/mnt/us/`).
4. Safely eject and disconnect the USB cable.
5. On the Kindle, navigate to:
   ```
   Home ──> Menu ──> Settings ──> Menu ──> Update Your Kindle
   ```
6. The Kindle will display the standard software update progress bar.
7. `install.sh` performs the following automated steps:
   - Terminates any existing `launchpad` process (`killall launchpad`).
   - Extracts the custom directory contents into `/mnt/us/launchpad/`.
   - Preserves user custom `.ini` files without overwriting.
   - Deploys `launchpad-init` to `/etc/init.d/launchpad`.
   - Links the init script into the system runlevel:
     ```bash
     ln -sf /etc/init.d/launchpad /etc/rc5.d/S76launchpad
     ln -sf /etc/init.d/launchpad /etc/rc5.d/K09launchpad
     ```
   - Starts Launchpad daemon immediately.

### 4.2 Automated Uninstallation

To remove Launchpad cleanly:
1. Copy `update_launchpad_*_<model>_uninstall.bin` to `/mnt/us/`.
2. Execute **Update Your Kindle** from the Settings menu.
3. `uninstall.sh` terminates the daemon, removes init symlinks from `/etc/rc5.d/`, deletes `/etc/init.d/launchpad`, and removes the binary while leaving user custom configurations intact.

---

## 5. Manual Developer Deployment (SSH / USB)

During active development on jailbroken Kindles with USB networking (usbnet) enabled, you can deploy binaries directly without creating OTA packages:

```bash
# Build ARMv6 binary
cmake --preset kindle-k3-release
cmake --build --preset kindle-k3-release

# Copy stripped binary to device via scp
scp ./build/kindle-k3-release/launchpad root@192.168.15.244:/mnt/us/launchpad/launchpad

# SSH into Kindle, restart daemon
ssh root@192.168.15.244 "/etc/init.d/launchpad stop && /etc/init.d/launchpad start"
```
