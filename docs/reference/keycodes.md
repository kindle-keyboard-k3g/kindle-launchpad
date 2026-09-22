# Keycodes & Symbolic Key Reference

This document catalogs all recognized symbolic key identifiers, Linux evdev scancodes, Kindle 3 hardware overrides, and ASCII mappings used in Kindle Launchpad.

---

## 1. Alphanumeric Keys

### 1.1 Numeric Keys
| Symbolic Name | Evdev Code (`KPKEY_*`) | Kindle 3 Code | Character |
| :--- | :--- | :--- | :--- |
| `1` | 2 | 2 | `1` |
| `2` | 3 | 3 | `2` |
| `3` | 4 | 4 | `3` |
| `4` | 5 | 5 | `4` |
| `5` | 6 | 6 | `5` |
| `6` | 7 | 7 | `6` |
| `7` | 8 | 8 | `7` |
| `8` | 9 | 9 | `8` |
| `9` | 10 | 10 | `9` |
| `0` | 11 | 11 | `0` |

### 1.2 Alphabet Keys
| Symbolic Name | Evdev Code (`KPKEY_*`) | Character | Symbolic Name | Evdev Code (`KPKEY_*`) | Character |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `Q` | 16 | `q`, `Q` | `A` | 30 | `a`, `A` |
| `W` | 17 | `w`, `W` | `S` | 31 | `s`, `S` |
| `E` | 18 | `e`, `E` | `D` | 32 | `d`, `D` |
| `R` | 19 | `r`, `R` | `F` | 33 | `f`, `F` |
| `T` | 20 | `t`, `T` | `G` | 34 | `g`, `G` |
| `Y` | 21 | `y`, `Y` | `H` | 35 | `h`, `H` |
| `U` | 22 | `u`, `U` | `J` | 36 | `j`, `J` |
| `I` | 23 | `i`, `I` | `K` | 37 | `k`, `K` |
| `O` | 24 | `o`, `O` | `L` | 38 | `l`, `L` |
| `P` | 25 | `p`, `P` | `Z` | 44 | `z`, `Z` |
| `X` | 45 | `x`, `X` | `C` | 46 | `c`, `C` |
| `V` | 47 | `v`, `V` | `B` | 48 | `b`, `B` |
| `N` | 49 | `n`, `N` | `M` | 50 | `m`, `M` |

---

## 2. Punctuation & Modifiers

| Symbolic Name | Evdev Code | Aliases | Description |
| :--- | :--- | :--- | :--- |
| `Shift` | 42 | `LShift`, `RShift` | Shift modifier / Default Introducer key |
| `Alt` | 56 | `LAlt`, `RAlt` | Alt modifier key |
| `Enter` | 28 | `Return` | Enter / Return / Default Trailer key |
| `Space` | 57 | `Spacebar` | Spacebar key |
| `Del` | 14 | `Delete`, `Backspace` | Backspace / Delete |
| `Dot` | 52 | `Period`, `.` | Period / full stop |
| `Slash` | 53 | `/` | Forward slash |

---

## 3. Kindle Navigation & Special Function Keys

| Symbolic Name | K2 / DX Evdev | K3 Scancode | Description |
| :--- | :--- | :--- | :--- |
| `Home` | 98 | 102 | Home button (returns to book list) |
| `Back` | 91 | 158 | Back button |
| `Menu` | 139 | 139 | Context menu button |
| `Sym` | 94 | 126 | Symbol selector menu key |
| `Aa` | 90 | 190 | Font size and typography adjustment key |
| `PgFwd` | 124 | 191 | Right page turn forward |
| `PgBck` | 109 | 109 | Right page turn backward |
| `LPgFwd` | 124 | 104 | Left page turn forward (K3) |
| `LPgBck` | 109 | 193 | Left page turn backward (K3) |
| `VPlus` | 115 | 115 | Volume up button |
| `VMinus` | 114 | 114 | Volume down button |

---

## 4. Five-Way Navigation Controller

| Symbolic Name | K2 / DX Scancode | K3 Scancode | Description |
| :--- | :--- | :--- | :--- |
| `FW_Left` | 105 | 105 | 5-way joystick left |
| `FW_Right` | 106 | 106 | 5-way joystick right |
| `FW_Up` | 122 | 103 | 5-way joystick up |
| `FW_Down` | 123 | 108 | 5-way joystick down |
| `FW_Press` | 92 | 194 | 5-way joystick center select press |
