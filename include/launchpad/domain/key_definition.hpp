/**
 * @file key_definition.hpp
 * @brief Symbolic key definitions and bidirectional keycode lookup table.
 * @details Maps between Linux evdev scancodes, Kindle Keyboard 3 hardware overrides,
 *          and human-readable symbolic string names (e.g. "Shift", "Enter", "Space").
 */

#pragma once

#include "launchpad/core/units.hpp"
#include <string>
#include <string_view>
#include <unordered_map>
#include <optional>

namespace launchpad::domain {

/**
 * @brief Constants for Linux evdev and Kindle-specific hardware keycodes.
 */
namespace key_codes {
    inline constexpr core::RawKeyCode KPKEY_1 = 2;       ///< Number 1
    inline constexpr core::RawKeyCode KPKEY_2 = 3;       ///< Number 2
    inline constexpr core::RawKeyCode KPKEY_3 = 4;       ///< Number 3
    inline constexpr core::RawKeyCode KPKEY_4 = 5;       ///< Number 4
    inline constexpr core::RawKeyCode KPKEY_5 = 6;       ///< Number 5
    inline constexpr core::RawKeyCode KPKEY_6 = 7;       ///< Number 6
    inline constexpr core::RawKeyCode KPKEY_7 = 8;       ///< Number 7
    inline constexpr core::RawKeyCode KPKEY_8 = 9;       ///< Number 8
    inline constexpr core::RawKeyCode KPKEY_9 = 10;      ///< Number 9
    inline constexpr core::RawKeyCode KPKEY_0 = 11;      ///< Number 0
    inline constexpr core::RawKeyCode KPKEY_Q = 16;      ///< Letter Q
    inline constexpr core::RawKeyCode KPKEY_W = 17;      ///< Letter W
    inline constexpr core::RawKeyCode KPKEY_E = 18;      ///< Letter E
    inline constexpr core::RawKeyCode KPKEY_R = 19;      ///< Letter R
    inline constexpr core::RawKeyCode KPKEY_T = 20;      ///< Letter T
    inline constexpr core::RawKeyCode KPKEY_Y = 21;      ///< Letter Y
    inline constexpr core::RawKeyCode KPKEY_U = 22;      ///< Letter U
    inline constexpr core::RawKeyCode KPKEY_I = 23;      ///< Letter I
    inline constexpr core::RawKeyCode KPKEY_O = 24;      ///< Letter O
    inline constexpr core::RawKeyCode KPKEY_P = 25;      ///< Letter P
    inline constexpr core::RawKeyCode KPKEY_A = 30;      ///< Letter A
    inline constexpr core::RawKeyCode KPKEY_S = 31;      ///< Letter S
    inline constexpr core::RawKeyCode KPKEY_D = 32;      ///< Letter D
    inline constexpr core::RawKeyCode KPKEY_F = 33;      ///< Letter F
    inline constexpr core::RawKeyCode KPKEY_G = 34;      ///< Letter G
    inline constexpr core::RawKeyCode KPKEY_H = 35;      ///< Letter H
    inline constexpr core::RawKeyCode KPKEY_J = 36;      ///< Letter J
    inline constexpr core::RawKeyCode KPKEY_K = 37;      ///< Letter K
    inline constexpr core::RawKeyCode KPKEY_L = 38;      ///< Letter L
    inline constexpr core::RawKeyCode KPKEY_DEL = 14;    ///< Delete / Backspace
    inline constexpr core::RawKeyCode KPKEY_Z = 44;      ///< Letter Z
    inline constexpr core::RawKeyCode KPKEY_X = 45;      ///< Letter X
    inline constexpr core::RawKeyCode KPKEY_C = 46;      ///< Letter C
    inline constexpr core::RawKeyCode KPKEY_V = 47;      ///< Letter V
    inline constexpr core::RawKeyCode KPKEY_B = 48;      ///< Letter B
    inline constexpr core::RawKeyCode KPKEY_N = 49;      ///< Letter N
    inline constexpr core::RawKeyCode KPKEY_M = 50;      ///< Letter M
    inline constexpr core::RawKeyCode KPKEY_DOT = 52;    ///< Period (.)
    inline constexpr core::RawKeyCode KPKEY_SLASH = 53;  ///< Slash (/)
    inline constexpr core::RawKeyCode KPKEY_ENTER = 28;  ///< Enter / Return
    inline constexpr core::RawKeyCode KPKEY_SHIFT = 42;  ///< Shift key
    inline constexpr core::RawKeyCode KPKEY_ALT = 56;    ///< Alt key
    inline constexpr core::RawKeyCode KPKEY_SPACE = 57;  ///< Spacebar
    inline constexpr core::RawKeyCode KPKEY_AA = 90;     ///< Font size (Aa) key (K2/DX)
    inline constexpr core::RawKeyCode KPKEY_SYM = 94;    ///< Symbol key (K2/DX)
    inline constexpr core::RawKeyCode KPKEY_VPLUS = 115; ///< Volume up
    inline constexpr core::RawKeyCode KPKEY_VMINUS = 114;///< Volume down
    inline constexpr core::RawKeyCode KPKEY_HOME = 98;   ///< Home key (K2/DX)
    inline constexpr core::RawKeyCode KPKEY_PGBCK = 109; ///< Page backward (K2/DX)
    inline constexpr core::RawKeyCode KPKEY_PGFWD = 124; ///< Page forward (K2/DX)
    inline constexpr core::RawKeyCode KPKEY_MENU = 139;  ///< Menu key
    inline constexpr core::RawKeyCode KPKEY_BACK = 91;   ///< Back key (K2/DX)
    inline constexpr core::RawKeyCode KPKEY_FW_LEFT = 105;  ///< 5-way left
    inline constexpr core::RawKeyCode KPKEY_FW_RIGHT = 106; ///< 5-way right
    inline constexpr core::RawKeyCode KPKEY_FW_UP = 122;    ///< 5-way up (K2/DX)
    inline constexpr core::RawKeyCode KPKEY_FW_DOWN = 123;  ///< 5-way down (K2/DX)
    inline constexpr core::RawKeyCode KPKEY_FW_PRESS = 92;  ///< 5-way press (K2/DX)

    // Kindle Keyboard 3 (K3) specific hardware keycode overrides
    inline constexpr core::RawKeyCode K3KEY_AA = 190;      ///< K3 Font size (Aa) key
    inline constexpr core::RawKeyCode K3KEY_SYM = 126;     ///< K3 Symbol key
    inline constexpr core::RawKeyCode K3KEY_HOME = 102;    ///< K3 Home key
    inline constexpr core::RawKeyCode K3KEY_BACK = 158;    ///< K3 Back key
    inline constexpr core::RawKeyCode K3KEY_PGFWD = 191;   ///< K3 Page forward
    inline constexpr core::RawKeyCode K3KEY_LPGBCK = 193;  ///< K3 Left page backward
    inline constexpr core::RawKeyCode K3KEY_LPGFWD = 104;  ///< K3 Left page forward
    inline constexpr core::RawKeyCode K3KEY_VPLUS = 115;   ///< K3 Volume up
    inline constexpr core::RawKeyCode K3KEY_VMINUS = 114;  ///< K3 Volume down
    inline constexpr core::RawKeyCode K3KEY_FW_UP = 103;   ///< K3 5-way up
    inline constexpr core::RawKeyCode K3KEY_FW_DOWN = 108; ///< K3 5-way down
    inline constexpr core::RawKeyCode K3KEY_FW_PRESS = 194;///< K3 5-way center press
} // namespace key_codes

/**
 * @brief Bidirectional lookup table between raw Linux evdev keycodes and symbolic string names.
 */
class KeyTable {
public:
    /**
     * @brief Constructs a KeyTable populated with default Kindle keyboard mappings and aliases.
     */
    KeyTable();

    /**
     * @brief Registers a mapping between a raw keycode and a symbolic name.
     * @param[in] code Raw evdev key code.
     * @param[in] name Symbolic string identifier.
     */
    void register_key(core::RawKeyCode code, std::string name);

    /**
     * @brief Resolves a symbolic string name to its corresponding raw keycode.
     * @param[in] name Case-insensitive key identifier (e.g. "Shift", "Enter", "Space").
     * @return Optional containing the resolved RawKeyCode, or std::nullopt if unknown.
     */
    [[nodiscard]] std::optional<core::RawKeyCode> find_code(std::string_view name) const;

    /**
     * @brief Resolves a raw keycode to its canonical symbolic name.
     * @param[in] code Raw evdev key code.
     * @return Optional containing the canonical name string, or std::nullopt if unmapped.
     */
    [[nodiscard]] std::optional<std::string> find_name(core::RawKeyCode code) const;

    /**
     * @brief Translates model-specific keycodes (e.g. K3 overrides) into canonical unified keycodes.
     * @param[in] code Hardware key code reported by evdev.
     * @return Canonicalized key code suitable for sequence matching.
     */
    [[nodiscard]] core::RawKeyCode get_substitute_code(core::RawKeyCode code) const;

private:
    void populate_defaults();
    void populate_aliases();
    static std::string normalize_name(std::string_view name);

    std::unordered_map<std::string, core::RawKeyCode> name_to_code_;
    std::unordered_map<core::RawKeyCode, std::string> code_to_name_;
};

} // namespace launchpad::domain
