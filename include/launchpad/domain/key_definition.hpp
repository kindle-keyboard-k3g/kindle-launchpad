#pragma once

#include "launchpad/core/units.hpp"
#include <string>
#include <string_view>
#include <unordered_map>
#include <optional>

namespace launchpad::domain {

namespace key_codes {
    inline constexpr core::RawKeyCode KPKEY_1 = 2;
    inline constexpr core::RawKeyCode KPKEY_2 = 3;
    inline constexpr core::RawKeyCode KPKEY_3 = 4;
    inline constexpr core::RawKeyCode KPKEY_4 = 5;
    inline constexpr core::RawKeyCode KPKEY_5 = 6;
    inline constexpr core::RawKeyCode KPKEY_6 = 7;
    inline constexpr core::RawKeyCode KPKEY_7 = 8;
    inline constexpr core::RawKeyCode KPKEY_8 = 9;
    inline constexpr core::RawKeyCode KPKEY_9 = 10;
    inline constexpr core::RawKeyCode KPKEY_0 = 11;
    inline constexpr core::RawKeyCode KPKEY_Q = 16;
    inline constexpr core::RawKeyCode KPKEY_W = 17;
    inline constexpr core::RawKeyCode KPKEY_E = 18;
    inline constexpr core::RawKeyCode KPKEY_R = 19;
    inline constexpr core::RawKeyCode KPKEY_T = 20;
    inline constexpr core::RawKeyCode KPKEY_Y = 21;
    inline constexpr core::RawKeyCode KPKEY_U = 22;
    inline constexpr core::RawKeyCode KPKEY_I = 23;
    inline constexpr core::RawKeyCode KPKEY_O = 24;
    inline constexpr core::RawKeyCode KPKEY_P = 25;
    inline constexpr core::RawKeyCode KPKEY_A = 30;
    inline constexpr core::RawKeyCode KPKEY_S = 31;
    inline constexpr core::RawKeyCode KPKEY_D = 32;
    inline constexpr core::RawKeyCode KPKEY_F = 33;
    inline constexpr core::RawKeyCode KPKEY_G = 34;
    inline constexpr core::RawKeyCode KPKEY_H = 35;
    inline constexpr core::RawKeyCode KPKEY_J = 36;
    inline constexpr core::RawKeyCode KPKEY_K = 37;
    inline constexpr core::RawKeyCode KPKEY_L = 38;
    inline constexpr core::RawKeyCode KPKEY_DEL = 14;
    inline constexpr core::RawKeyCode KPKEY_Z = 44;
    inline constexpr core::RawKeyCode KPKEY_X = 45;
    inline constexpr core::RawKeyCode KPKEY_C = 46;
    inline constexpr core::RawKeyCode KPKEY_V = 47;
    inline constexpr core::RawKeyCode KPKEY_B = 48;
    inline constexpr core::RawKeyCode KPKEY_N = 49;
    inline constexpr core::RawKeyCode KPKEY_M = 50;
    inline constexpr core::RawKeyCode KPKEY_DOT = 52;
    inline constexpr core::RawKeyCode KPKEY_SLASH = 53;
    inline constexpr core::RawKeyCode KPKEY_ENTER = 28;
    inline constexpr core::RawKeyCode KPKEY_SHIFT = 42;
    inline constexpr core::RawKeyCode KPKEY_ALT = 56;
    inline constexpr core::RawKeyCode KPKEY_SPACE = 57;
    inline constexpr core::RawKeyCode KPKEY_AA = 90;
    inline constexpr core::RawKeyCode KPKEY_SYM = 94;
    inline constexpr core::RawKeyCode KPKEY_VPLUS = 115;
    inline constexpr core::RawKeyCode KPKEY_VMINUS = 114;
    inline constexpr core::RawKeyCode KPKEY_HOME = 98;
    inline constexpr core::RawKeyCode KPKEY_PGBCK = 109;
    inline constexpr core::RawKeyCode KPKEY_PGFWD = 124;
    inline constexpr core::RawKeyCode KPKEY_MENU = 139;
    inline constexpr core::RawKeyCode KPKEY_BACK = 91;
    inline constexpr core::RawKeyCode KPKEY_FW_LEFT = 105;
    inline constexpr core::RawKeyCode KPKEY_FW_RIGHT = 106;
    inline constexpr core::RawKeyCode KPKEY_FW_UP = 122;
    inline constexpr core::RawKeyCode KPKEY_FW_DOWN = 123;
    inline constexpr core::RawKeyCode KPKEY_FW_PRESS = 92;

    inline constexpr core::RawKeyCode K3KEY_AA = 190;
    inline constexpr core::RawKeyCode K3KEY_SYM = 126;
    inline constexpr core::RawKeyCode K3KEY_HOME = 102;
    inline constexpr core::RawKeyCode K3KEY_BACK = 158;
    inline constexpr core::RawKeyCode K3KEY_PGFWD = 191;
    inline constexpr core::RawKeyCode K3KEY_LPGBCK = 193;
    inline constexpr core::RawKeyCode K3KEY_LPGFWD = 104;
    inline constexpr core::RawKeyCode K3KEY_VPLUS = 115;
    inline constexpr core::RawKeyCode K3KEY_VMINUS = 114;
    inline constexpr core::RawKeyCode K3KEY_FW_UP = 103;
    inline constexpr core::RawKeyCode K3KEY_FW_DOWN = 108;
    inline constexpr core::RawKeyCode K3KEY_FW_PRESS = 194;
} // namespace key_codes

class KeyTable {
public:
    KeyTable();

    void register_key(core::RawKeyCode code, std::string name);
    [[nodiscard]] std::optional<core::RawKeyCode> find_code(std::string_view name) const;
    [[nodiscard]] std::optional<std::string> find_name(core::RawKeyCode code) const;
    [[nodiscard]] core::RawKeyCode get_substitute_code(core::RawKeyCode code) const;

private:
    void populate_defaults();
    void populate_aliases();
    static std::string normalize_name(std::string_view name);

    std::unordered_map<std::string, core::RawKeyCode> name_to_code_;
    std::unordered_map<core::RawKeyCode, std::string> code_to_name_;
};

} // namespace launchpad::domain
