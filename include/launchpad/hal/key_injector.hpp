#pragma once

#include "launchpad/core/result.hpp"
#include "launchpad/core/units.hpp"
#include "launchpad/domain/ascii_translator.hpp"

namespace launchpad::hal {

class IKeyInjector {
public:
    virtual ~IKeyInjector() = default;

    virtual core::Result<void> inject_key(core::RawKeyCode code, domain::SendMode mode) = 0;
    virtual core::Result<void> inject_ascii_char(char c, const domain::AsciiTranslator& translator) = 0;
};

} // namespace launchpad::hal
