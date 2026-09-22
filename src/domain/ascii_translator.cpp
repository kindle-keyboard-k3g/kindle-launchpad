#include "launchpad/domain/ascii_translator.hpp"
#include "launchpad/domain/key_definition.hpp"
#include <array>

namespace launchpad::domain {

using namespace key_codes;

namespace {

constexpr core::RawKeyCode KEY_ESC_CODE = 1;
constexpr core::RawKeyCode KEY_BACKSPACE_CODE = 14;
constexpr core::RawKeyCode KEY_TAB_CODE = 15;
constexpr core::RawKeyCode KEY_APOSTROPHE_CODE = 40;
constexpr core::RawKeyCode KEY_EQUAL_CODE = 13;
constexpr core::RawKeyCode KEY_COMMA_CODE = 51;
constexpr core::RawKeyCode KEY_MINUS_CODE = 12;
constexpr core::RawKeyCode KEY_SEMICOLON_CODE = 39;
constexpr core::RawKeyCode KEY_LEFTBRACE_CODE = 26;
constexpr core::RawKeyCode KEY_BACKSLASH_CODE = 43;
constexpr core::RawKeyCode KEY_RIGHTBRACE_CODE = 27;
constexpr core::RawKeyCode KEY_GRAVE_CODE = 41;

constexpr std::array<SymbolNavigation, 32> KDXG_SYMBOLS{{
    {-1, -1}, {0, -1}, {1, -1}, {2, -1}, {3, -1}, {4, -1}, {5, -1}, {6, -1}, {7, -1}, {8, -1}, {9, -1},
    {-1, 0}, {0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0},
    {-1, 1}, {0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}, {6, 1}, {7, 1}, {8, 1}
}};

constexpr std::array<SymbolNavigation, 48> K3_SYMBOLS{{
    {-1, -1}, {0, -1}, {1, -1}, {2, -1}, {3, -1}, {4, -1}, {5, -1}, {6, -1}, {7, -1}, {8, -1}, {9, -1}, {10, -1},
    {-1, 0}, {0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0},
    {-1, 1}, {0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}, {6, 1}, {7, 1}, {8, 1}, {9, 1}, {10, 1},
    {-1, 2}, {0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2}, {6, 2}, {7, 2}, {8, 2}, {9, 2}, {10, 2}
}};

struct TableEntry {
    core::RawKeyCode code;
    SendMode send_mode;
    int8_t sym_index; // -1 if no symbol navigation
};

#define SYM(idx) static_cast<int8_t>(idx)
#define NOSYM static_cast<int8_t>(-1)

constexpr std::array<TableEntry, 128> KDXG_TABLE{{
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KPKEY_SPACE, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KPKEY_SPACE, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KPKEY_SPACE, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KPKEY_SPACE, SendMode::Normal, NOSYM},
    {KEY_BACKSPACE_CODE, SendMode::Normal, NOSYM}, {KEY_TAB_CODE, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KPKEY_SPACE, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KPKEY_ENTER, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KPKEY_SPACE, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KPKEY_SPACE, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KPKEY_SPACE, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KPKEY_SPACE, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KPKEY_SPACE, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KPKEY_SPACE, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KEY_ESC_CODE, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KPKEY_SPACE, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KPKEY_SPACE, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, // 32 ' '
    {KPKEY_1, SendMode::Shift, SYM(13)},    // 33 '!'
    {KEY_APOSTROPHE_CODE, SendMode::Shift, SYM(0)}, // 34 '"'
    {KPKEY_3, SendMode::Shift, SYM(16)},    // 35 '#'
    {KPKEY_4, SendMode::Shift, SYM(28)},    // 36 '$'
    {KPKEY_5, SendMode::Shift, SYM(27)},    // 37 '%'
    {KPKEY_7, SendMode::Shift, SYM(22)},    // 38 '&'
    {KEY_APOSTROPHE_CODE, SendMode::Normal, SYM(1)}, // 39 '\''
    {KPKEY_9, SendMode::Shift, SYM(7)},     // 40 '('
    {KPKEY_0, SendMode::Shift, SYM(8)},     // 41 ')'
    {KPKEY_8, SendMode::Shift, SYM(5)},     // 42 '*'
    {KEY_EQUAL_CODE, SendMode::Shift, SYM(4)}, // 43 '+'
    {KEY_COMMA_CODE, SendMode::Normal, SYM(12)}, // 44 ','
    {KEY_MINUS_CODE, SendMode::Normal, SYM(3)},  // 45 '-'
    {KPKEY_DOT, SendMode::Normal, SYM(24)}, // 46 '.'
    {KPKEY_SLASH, SendMode::Normal, SYM(14)}, // 47 '/'
    {KPKEY_0, SendMode::Normal, NOSYM},     // 48 '0'
    {KPKEY_1, SendMode::Normal, NOSYM},     // 49 '1'
    {KPKEY_2, SendMode::Normal, NOSYM},     // 50 '2'
    {KPKEY_3, SendMode::Normal, NOSYM},     // 51 '3'
    {KPKEY_4, SendMode::Normal, NOSYM},     // 52 '4'
    {KPKEY_5, SendMode::Normal, NOSYM},     // 53 '5'
    {KPKEY_6, SendMode::Normal, NOSYM},     // 54 '6'
    {KPKEY_7, SendMode::Normal, NOSYM},     // 55 '7'
    {KPKEY_8, SendMode::Normal, NOSYM},     // 56 '8'
    {KPKEY_9, SendMode::Normal, NOSYM},     // 57 '9'
    {KEY_SEMICOLON_CODE, SendMode::Shift, SYM(23)}, // 58 ':'
    {KEY_SEMICOLON_CODE, SendMode::Normal, SYM(25)}, // 59 ';'
    {KEY_COMMA_CODE, SendMode::Shift, SYM(9)},  // 60 '<'
    {KEY_EQUAL_CODE, SendMode::Normal, SYM(15)}, // 61 '='
    {KPKEY_DOT, SendMode::Shift, SYM(10)},  // 62 '>'
    {KPKEY_SLASH, SendMode::Shift, SYM(11)}, // 63 '?'
    {KPKEY_2, SendMode::Shift, SYM(2)},     // 64 '@'
    {KPKEY_A, SendMode::Shift, NOSYM}, {KPKEY_B, SendMode::Shift, NOSYM},
    {KPKEY_C, SendMode::Shift, NOSYM}, {KPKEY_D, SendMode::Shift, NOSYM},
    {KPKEY_E, SendMode::Shift, NOSYM}, {KPKEY_F, SendMode::Shift, NOSYM},
    {KPKEY_G, SendMode::Shift, NOSYM}, {KPKEY_H, SendMode::Shift, NOSYM},
    {KPKEY_I, SendMode::Shift, NOSYM}, {KPKEY_J, SendMode::Shift, NOSYM},
    {KPKEY_K, SendMode::Shift, NOSYM}, {KPKEY_L, SendMode::Shift, NOSYM},
    {KPKEY_M, SendMode::Shift, NOSYM}, {KPKEY_N, SendMode::Shift, NOSYM},
    {KPKEY_O, SendMode::Shift, NOSYM}, {KPKEY_P, SendMode::Shift, NOSYM},
    {KPKEY_Q, SendMode::Shift, NOSYM}, {KPKEY_R, SendMode::Shift, NOSYM},
    {KPKEY_S, SendMode::Shift, NOSYM}, {KPKEY_T, SendMode::Shift, NOSYM},
    {KPKEY_U, SendMode::Shift, NOSYM}, {KPKEY_V, SendMode::Shift, NOSYM},
    {KPKEY_W, SendMode::Shift, NOSYM}, {KPKEY_X, SendMode::Shift, NOSYM},
    {KPKEY_Y, SendMode::Shift, NOSYM}, {KPKEY_Z, SendMode::Shift, NOSYM},
    {KEY_LEFTBRACE_CODE, SendMode::Normal, SYM(29)}, // 91 '['
    {KEY_BACKSLASH_CODE, SendMode::Normal, NOSYM},   // 92 '\\'
    {KEY_RIGHTBRACE_CODE, SendMode::Normal, SYM(30)},// 93 ']'
    {KPKEY_6, SendMode::Shift, SYM(6)},     // 94 '^'
    {KEY_MINUS_CODE, SendMode::Shift, SYM(26)}, // 95 '_'
    {KEY_GRAVE_CODE, SendMode::Normal, SYM(20)}, // 96 '`'
    {KPKEY_A, SendMode::Normal, NOSYM}, {KPKEY_B, SendMode::Normal, NOSYM},
    {KPKEY_C, SendMode::Normal, NOSYM}, {KPKEY_D, SendMode::Normal, NOSYM},
    {KPKEY_E, SendMode::Normal, NOSYM}, {KPKEY_F, SendMode::Normal, NOSYM},
    {KPKEY_G, SendMode::Normal, NOSYM}, {KPKEY_H, SendMode::Normal, NOSYM},
    {KPKEY_I, SendMode::Normal, NOSYM}, {KPKEY_J, SendMode::Normal, NOSYM},
    {KPKEY_K, SendMode::Normal, NOSYM}, {KPKEY_L, SendMode::Normal, NOSYM},
    {KPKEY_M, SendMode::Normal, NOSYM}, {KPKEY_N, SendMode::Normal, NOSYM},
    {KPKEY_O, SendMode::Normal, NOSYM}, {KPKEY_P, SendMode::Normal, NOSYM},
    {KPKEY_Q, SendMode::Normal, NOSYM}, {KPKEY_R, SendMode::Normal, NOSYM},
    {KPKEY_S, SendMode::Normal, NOSYM}, {KPKEY_T, SendMode::Normal, NOSYM},
    {KPKEY_U, SendMode::Normal, NOSYM}, {KPKEY_V, SendMode::Normal, NOSYM},
    {KPKEY_W, SendMode::Normal, NOSYM}, {KPKEY_X, SendMode::Normal, NOSYM},
    {KPKEY_Y, SendMode::Normal, NOSYM}, {KPKEY_Z, SendMode::Normal, NOSYM},
    {KEY_LEFTBRACE_CODE, SendMode::Shift, SYM(18)}, // 123 '{'
    {KEY_BACKSLASH_CODE, SendMode::Shift, SYM(17)}, // 124 '|'
    {KEY_RIGHTBRACE_CODE, SendMode::Shift, SYM(19)},// 125 '}'
    {KEY_GRAVE_CODE, SendMode::Shift, SYM(21)},     // 126 '~'
    {KPKEY_DEL, SendMode::Normal, SYM(31)}          // 127 DEL
}};

constexpr std::array<TableEntry, 128> K3_TABLE{{
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KPKEY_SPACE, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KPKEY_SPACE, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KPKEY_SPACE, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KPKEY_SPACE, SendMode::Normal, NOSYM},
    {KEY_BACKSPACE_CODE, SendMode::Normal, NOSYM}, {KEY_TAB_CODE, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KPKEY_SPACE, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KPKEY_ENTER, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KPKEY_SPACE, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KPKEY_SPACE, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KPKEY_SPACE, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KPKEY_SPACE, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KPKEY_SPACE, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KPKEY_SPACE, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KEY_ESC_CODE, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KPKEY_SPACE, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, {KPKEY_SPACE, SendMode::Normal, NOSYM},
    {KPKEY_SPACE, SendMode::Normal, NOSYM}, // 32 ' '
    {KPKEY_1, SendMode::Shift, SYM(24)},    // 33 '!'
    {KEY_APOSTROPHE_CODE, SendMode::Shift, SYM(3)}, // 34 '"'
    {KPKEY_3, SendMode::Shift, SYM(26)},    // 35 '#'
    {KPKEY_4, SendMode::Shift, SYM(27)},    // 36 '$'
    {KPKEY_5, SendMode::Shift, SYM(28)},    // 37 '%'
    {KPKEY_7, SendMode::Shift, SYM(30)},    // 38 '&'
    {KEY_APOSTROPHE_CODE, SendMode::Normal, SYM(15)}, // 39 '\''
    {KPKEY_9, SendMode::Shift, SYM(32)},    // 40 '('
    {KPKEY_0, SendMode::Shift, SYM(33)},    // 41 ')'
    {KPKEY_8, SendMode::Shift, SYM(31)},    // 42 '*'
    {KEY_EQUAL_CODE, SendMode::Shift, SYM(35)}, // 43 '+'
    {KEY_COMMA_CODE, SendMode::Normal, SYM(13)}, // 44 ','
    {KEY_MINUS_CODE, SendMode::Normal, SYM(46)}, // 45 '-'
    {KPKEY_DOT, SendMode::Normal, SYM(14)}, // 46 '.'
    {KPKEY_SLASH, SendMode::Normal, SYM(16)}, // 47 '/'
    {KPKEY_0, SendMode::Normal, SYM(45)},   // 48 '0'
    {KPKEY_1, SendMode::Normal, SYM(36)},   // 49 '1'
    {KPKEY_2, SendMode::Normal, SYM(37)},   // 50 '2'
    {KPKEY_3, SendMode::Normal, SYM(38)},   // 51 '3'
    {KPKEY_4, SendMode::Normal, SYM(39)},   // 52 '4'
    {KPKEY_5, SendMode::Normal, SYM(40)},   // 53 '5'
    {KPKEY_6, SendMode::Normal, SYM(41)},   // 54 '6'
    {KPKEY_7, SendMode::Normal, SYM(42)},   // 55 '7'
    {KPKEY_8, SendMode::Normal, SYM(43)},   // 56 '8'
    {KPKEY_9, SendMode::Normal, SYM(44)},   // 57 '9'
    {KEY_SEMICOLON_CODE, SendMode::Shift, SYM(0)}, // 58 ':'
    {KEY_SEMICOLON_CODE, SendMode::Normal, SYM(12)}, // 59 ';'
    {KEY_COMMA_CODE, SendMode::Shift, SYM(1)},  // 60 '<'
    {KEY_EQUAL_CODE, SendMode::Normal, SYM(47)}, // 61 '='
    {KPKEY_DOT, SendMode::Shift, SYM(2)},   // 62 '>'
    {KPKEY_SLASH, SendMode::Shift, SYM(4)}, // 63 '?'
    {KPKEY_2, SendMode::Shift, SYM(25)},    // 64 '@'
    {KPKEY_A, SendMode::Shift, NOSYM}, {KPKEY_B, SendMode::Shift, NOSYM},
    {KPKEY_C, SendMode::Shift, NOSYM}, {KPKEY_D, SendMode::Shift, NOSYM},
    {KPKEY_E, SendMode::Shift, NOSYM}, {KPKEY_F, SendMode::Shift, NOSYM},
    {KPKEY_G, SendMode::Shift, NOSYM}, {KPKEY_H, SendMode::Shift, NOSYM},
    {KPKEY_I, SendMode::Shift, NOSYM}, {KPKEY_J, SendMode::Shift, NOSYM},
    {KPKEY_K, SendMode::Shift, NOSYM}, {KPKEY_L, SendMode::Shift, NOSYM},
    {KPKEY_M, SendMode::Shift, NOSYM}, {KPKEY_N, SendMode::Shift, NOSYM},
    {KPKEY_O, SendMode::Shift, NOSYM}, {KPKEY_P, SendMode::Shift, NOSYM},
    {KPKEY_Q, SendMode::Shift, NOSYM}, {KPKEY_R, SendMode::Shift, NOSYM},
    {KPKEY_S, SendMode::Shift, NOSYM}, {KPKEY_T, SendMode::Shift, NOSYM},
    {KPKEY_U, SendMode::Shift, NOSYM}, {KPKEY_V, SendMode::Shift, NOSYM},
    {KPKEY_W, SendMode::Shift, NOSYM}, {KPKEY_X, SendMode::Shift, NOSYM},
    {KPKEY_Y, SendMode::Shift, NOSYM}, {KPKEY_Z, SendMode::Shift, NOSYM},
    {KEY_LEFTBRACE_CODE, SendMode::Normal, SYM(17)}, // 91 '['
    {KEY_BACKSLASH_CODE, SendMode::Normal, SYM(19)}, // 92 '\\'
    {KEY_RIGHTBRACE_CODE, SendMode::Normal, SYM(18)},// 93 ']'
    {KPKEY_6, SendMode::Shift, SYM(29)},    // 94 '^'
    {KEY_MINUS_CODE, SendMode::Shift, SYM(34)}, // 95 '_'
    {KEY_GRAVE_CODE, SendMode::Normal, SYM(8)}, // 96 '`'
    {KPKEY_A, SendMode::Normal, NOSYM}, {KPKEY_B, SendMode::Normal, NOSYM},
    {KPKEY_C, SendMode::Normal, NOSYM}, {KPKEY_D, SendMode::Normal, NOSYM},
    {KPKEY_E, SendMode::Normal, NOSYM}, {KPKEY_F, SendMode::Normal, NOSYM},
    {KPKEY_G, SendMode::Normal, NOSYM}, {KPKEY_H, SendMode::Normal, NOSYM},
    {KPKEY_I, SendMode::Normal, NOSYM}, {KPKEY_J, SendMode::Normal, NOSYM},
    {KPKEY_K, SendMode::Normal, NOSYM}, {KPKEY_L, SendMode::Normal, NOSYM},
    {KPKEY_M, SendMode::Normal, NOSYM}, {KPKEY_N, SendMode::Normal, NOSYM},
    {KPKEY_O, SendMode::Normal, NOSYM}, {KPKEY_P, SendMode::Normal, NOSYM},
    {KPKEY_Q, SendMode::Normal, NOSYM}, {KPKEY_R, SendMode::Normal, NOSYM},
    {KPKEY_S, SendMode::Normal, NOSYM}, {KPKEY_T, SendMode::Normal, NOSYM},
    {KPKEY_U, SendMode::Normal, NOSYM}, {KPKEY_V, SendMode::Normal, NOSYM},
    {KPKEY_W, SendMode::Normal, NOSYM}, {KPKEY_X, SendMode::Normal, NOSYM},
    {KPKEY_Y, SendMode::Normal, NOSYM}, {KPKEY_Z, SendMode::Normal, NOSYM},
    {KEY_LEFTBRACE_CODE, SendMode::Shift, SYM(5)}, // 123 '{'
    {KEY_BACKSLASH_CODE, SendMode::Shift, SYM(7)}, // 124 '|'
    {KEY_RIGHTBRACE_CODE, SendMode::Shift, SYM(6)},// 125 '}'
    {KEY_GRAVE_CODE, SendMode::Shift, SYM(9)},     // 126 '~'
    {KPKEY_DEL, SendMode::Normal, NOSYM}           // 127 DEL
}};

#undef SYM
#undef NOSYM

} // namespace

AsciiTranslator::AsciiTranslator(KindleModel model) noexcept : model_(model) {}

KindleModel AsciiTranslator::model() const noexcept {
    return model_;
}

void AsciiTranslator::set_model(KindleModel model) noexcept {
    model_ = model;
}

AsciiKeyMapping AsciiTranslator::translate(char c) const noexcept {
    auto idx = static_cast<size_t>(static_cast<unsigned char>(c) & 0x7f);
    const auto& entry = (model_ == KindleModel::Kindle3) ? K3_TABLE[idx] : KDXG_TABLE[idx];

    AsciiKeyMapping mapping;
    mapping.code = entry.code;
    mapping.send_mode = entry.send_mode;
    if (entry.sym_index >= 0) {
        auto sym_idx = static_cast<size_t>(entry.sym_index);
        mapping.symbol_navigation = (model_ == KindleModel::Kindle3) ? K3_SYMBOLS[sym_idx] : KDXG_SYMBOLS[sym_idx];
    }
    return mapping;
}

} // namespace launchpad::domain
