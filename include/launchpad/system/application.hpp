#pragma once

#include "launchpad/hal/input_source.hpp"
#include "launchpad/hal/display.hpp"
#include "launchpad/hal/command_runner.hpp"
#include "launchpad/hal/key_injector.hpp"
#include "launchpad/core/result.hpp"
#include <string>
#include <memory>

namespace launchpad::system {

struct ApplicationServices {
    hal::IInputSource& input;
    hal::IDisplay& display;
    hal::ICommandRunner& runner;
    hal::IKeyInjector& injector;
};

struct ApplicationContext;

class Application {
public:
    Application(ApplicationServices services, std::string config_directory);
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    int run();
    void step();
    void reload_configuration();

    [[nodiscard]] const ApplicationContext& context() const noexcept {
        return *context_;
    }

private:
    ApplicationServices services_;
    std::unique_ptr<ApplicationContext> context_;

    void load_configuration();
    void process_key_event(const hal::InputKeyEvent& event);
    void check_timer();
    void handle_completed_match(const domain::MatcherResult& result);
};

} // namespace launchpad::system
