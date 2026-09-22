# Kindle Launchpad Documentation Portal

Welcome to the central documentation portal for **Kindle Launchpad**, a lightweight daemon and hotkey launcher for Amazon Kindle e-ink devices featuring physical keyboards.

---

## 🧭 Documentation Map & Reading Paths

```
docs/
├── README.md                           # Master Documentation Portal (this file)
├── getting-started.md                  # Quickstart guide: build, run, and configure
│
├── architecture/                       # Subsystem Architecture & Design
│   ├── overview.md                     # High-level architecture, composition root, data flow
│   ├── core.md                         # Monadic Result<T, E>, RAII types, units, logger
│   ├── configuration.md                # Multi-file INI scanning, precedence, SIGHUP reload
│   ├── domain.md                       # Hotkey FSM, sequence matching, action dispatch
│   ├── hal.md                          # Hardware Abstraction Layer, evdev, EVIOCGRAB, display
│   ├── ui.md                           # 12x22 font rasterizer, status bar overlay, BMP exporter
│   └── system.md                       # Daemon lifecycle, signals, PID lock, RCE server
│
├── hardware/                           # Device Hardware Profiles & Kernel Drivers
│   ├── kindle-k3.md                    # Kindle Keyboard 3 (K3G, K3W, K3GB) specifications
│   ├── kindle-dx.md                    # Kindle DX & DX Graphite specifications
│   ├── kindle-k2.md                    # Kindle 2 & K2 International specifications
│   ├── eink-framebuffer.md             # 4bpp packed pixel format, mmap, and refresh mechanics
│   └── kindle-ioctls.md                # EVIOCGRAB and FBIO_EINK_UPDATE_DISPLAY_AREA ioctls
│
├── reference/                          # Technical Reference Manuals
│   ├── configuration-spec.md           # Authoritative [Settings] and [Actions] INI guide
│   ├── keycodes.md                     # Symbolic key table (KPKEY_*) to evdev and ASCII
│   └── cli.md                          # Command-line options reference (-d, -kb, -fw)
│
├── development/                        # Developer & Contributor Guides
│   ├── build-and-test.md               # CMake presets, compiler warnings, CTest execution
│   ├── contributing.md                 # TDD workflow (/tdd), SOLID rules, Object Calisthenics
│   └── deployment-and-packaging.md     # Kindle OTA packages, kindletool, install scripts
│
├── adr/                                # Architecture Decision Records (ADRs)
│   ├── README.md                       # ADR registry, status definitions, and index
│   ├── 0001-modern-cpp20-refactoring.md
│   ├── 0002-zero-exception-error-handling.md
│   ├── 0003-hardware-abstraction-layer-for-host-testing.md
│   ├── 0004-object-calisthenics-composition.md
│   ├── 0005-host-mocks-and-native-tests.md
│   └── 0006-backward-compatible-ini-actions.md
│
└── sot/                                # Source of Truth (SoT) Specifications
    ├── README.md                       # SoT directory index and normative precedence
    ├── coding-standards.md             # Normative engineering standards (TDD, SOLID, C++20)
    ├── hardware-profile.md             # Normative device capabilities and device node matrix
    └── config-schema.md                # Normative INI configuration schema
```

---

## 🚀 Quick Navigation

### 1. New to the Project?
- Start with **[Getting Started](getting-started.md)** to configure, build, and test Kindle Launchpad on your local development machine in under 2 minutes.
- Learn how to build and execute automated tests in **[Building & Testing Guide](development/build-and-test.md)**.

### 2. Understanding System Architecture
- Read the **[Architectural Overview](architecture/overview.md)** for a high-level view of how components interact.
- Explore the **[Hardware Abstraction Layer (HAL)](architecture/hal.md)** to see how Launchpad isolates hardware ioctls from business logic.
- Learn about the hotkey state machine in **[Domain Subsystem](architecture/domain.md)**.
- Read how non-destructive e-ink overlays work in **[UI Subsystem](architecture/ui.md)**.

### 3. Hardware & Low-Level Interfaces
- Review device profiles: **[Kindle Keyboard (K3)](hardware/kindle-k3.md)**, **[Kindle DX](hardware/kindle-dx.md)**, or **[Kindle 2](hardware/kindle-k2.md)**.
- Understand the 4bpp packed grayscale display buffer in **[E-Ink Framebuffer Guide](hardware/eink-framebuffer.md)**.
- Review kernel ioctls and safety rules in **[Kindle Kernel IOCTL Reference](hardware/kindle-ioctls.md)**.

### 4. Contributing & Standards
- Read the **[Contributing Guidelines](development/contributing.md)** for TDD rules, SOLID principles, and Object Calisthenics constraints.
- Consult the authoritative **[Source of Truth: Coding Standards](sot/coding-standards.md)**.
- Review past design decisions in the **[Architecture Decision Records](adr/README.md)**.
