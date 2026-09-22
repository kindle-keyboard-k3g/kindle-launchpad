#include "launchpad/domain/key_definition.hpp"
#include <algorithm>
#include <cctype>

namespace launchpad::domain {

using namespace key_codes;

KeyTable::KeyTable() {
    populate_defaults();
}

std::string KeyTable::normalize_name(std::string_view name) {
    std::string result;
    result.reserve(name.size());
    for (char c : name) {
        result.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }
    return result;
}

void KeyTable::register_key(core::RawKeyCode code, std::string name) {
    name_to_code_[normalize_name(name)] = code;
    code_to_name_[code] = std::move(name);
}

std::optional<core::RawKeyCode> KeyTable::find_code(std::string_view name) const {
    auto it = name_to_code_.find(normalize_name(name));
    if (it == name_to_code_.end()) {
        return std::nullopt;
    }
    return it->second;
}

std::optional<std::string> KeyTable::find_name(core::RawKeyCode code) const {
    auto it = code_to_name_.find(code);
    if (it == code_to_name_.end()) {
        return std::nullopt;
    }
    return it->second;
}

core::RawKeyCode KeyTable::get_substitute_code(core::RawKeyCode code) const {
    switch (code) {
        case K3KEY_AA: return KPKEY_AA;
        case K3KEY_SYM: return KPKEY_SYM;
        case K3KEY_HOME: return KPKEY_HOME;
        case K3KEY_BACK: return KPKEY_BACK;
        case K3KEY_PGFWD: return KPKEY_PGFWD;
        case K3KEY_LPGBCK: return KPKEY_PGBCK;
        case K3KEY_LPGFWD: return KPKEY_PGFWD;
        case K3KEY_VPLUS: return KPKEY_VPLUS;
        case K3KEY_VMINUS: return KPKEY_VMINUS;
        case K3KEY_FW_UP: return KPKEY_FW_UP;
        case K3KEY_FW_DOWN: return KPKEY_FW_DOWN;
        case K3KEY_FW_PRESS: return KPKEY_FW_PRESS;
        default: return code;
    }
}

void KeyTable::populate_defaults() {
    register_key(KPKEY_1, "1");
    register_key(KPKEY_2, "2");
    register_key(KPKEY_3, "3");
    register_key(KPKEY_4, "4");
    register_key(KPKEY_5, "5");
    register_key(KPKEY_6, "6");
    register_key(KPKEY_7, "7");
    register_key(KPKEY_8, "8");
    register_key(KPKEY_9, "9");
    register_key(KPKEY_0, "0");
    register_key(KPKEY_Q, "Q");
    register_key(KPKEY_W, "W");
    register_key(KPKEY_E, "E");
    register_key(KPKEY_R, "R");
    register_key(KPKEY_T, "T");
    register_key(KPKEY_Y, "Y");
    register_key(KPKEY_U, "U");
    register_key(KPKEY_I, "I");
    register_key(KPKEY_O, "O");
    register_key(KPKEY_P, "P");
    register_key(KPKEY_A, "A");
    register_key(KPKEY_S, "S");
    register_key(KPKEY_D, "D");
    register_key(KPKEY_F, "F");
    register_key(KPKEY_G, "G");
    register_key(KPKEY_H, "H");
    register_key(KPKEY_J, "J");
    register_key(KPKEY_K, "K");
    register_key(KPKEY_L, "L");
    register_key(KPKEY_DEL, "Del");
    register_key(KPKEY_Z, "Z");
    register_key(KPKEY_X, "X");
    register_key(KPKEY_C, "C");
    register_key(KPKEY_V, "V");
    register_key(KPKEY_B, "B");
    register_key(KPKEY_N, "N");
    register_key(KPKEY_M, "M");
    register_key(KPKEY_DOT, "Dot");
    register_key(KPKEY_SLASH, "Slash");
    register_key(KPKEY_ENTER, "Enter");
    register_key(KPKEY_SHIFT, "Shift");
    register_key(KPKEY_ALT, "Alt");
    register_key(KPKEY_SPACE, "Space");
    register_key(KPKEY_AA, "aA");
    register_key(KPKEY_SYM, "Sym");
    register_key(KPKEY_VPLUS, "VPlus");
    register_key(KPKEY_VMINUS, "VMinus");
    register_key(KPKEY_HOME, "Home");
    register_key(KPKEY_PGBCK, "<");
    register_key(KPKEY_PGFWD, ">");
    register_key(KPKEY_MENU, "Menu");
    register_key(KPKEY_BACK, "Back");
    register_key(KPKEY_FW_LEFT, "FW_Left");
    register_key(KPKEY_FW_RIGHT, "FW_Right");
    register_key(KPKEY_FW_UP, "FW_Up");
    register_key(KPKEY_FW_DOWN, "FW_Down");
    register_key(KPKEY_FW_PRESS, "FW_Press");
    populate_aliases();
}

void KeyTable::populate_aliases() {
    register_key(K3KEY_AA, "k3_aA");
    register_key(K3KEY_SYM, "k3_Sym");
    register_key(K3KEY_HOME, "k3_Home");
    register_key(K3KEY_BACK, "k3_Back");
    register_key(K3KEY_PGFWD, "k3_>");
    register_key(K3KEY_LPGBCK, "lk3_<");
    register_key(K3KEY_LPGFWD, "lk3_>");
    register_key(K3KEY_FW_UP, "k3_FW_Up");
    register_key(K3KEY_FW_DOWN, "k3_FW_Down");
    register_key(K3KEY_FW_PRESS, "k3_FW_Press");
}

} // namespace launchpad::domain
