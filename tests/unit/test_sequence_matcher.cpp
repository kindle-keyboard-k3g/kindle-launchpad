#include "launchpad/domain/sequence_matcher.hpp"
#include "launchpad/domain/key_definition.hpp"
#include "test_framework.hpp"

using namespace launchpad::core;
using namespace launchpad::domain;
using namespace launchpad::domain::key_codes;

static SequenceMatcher make_matcher(const KeyTable& table) {
    MatcherConfig config{KPKEY_SHIFT, KPKEY_ENTER, &table};
    return SequenceMatcher(config);
}

TEST_CASE("SequenceMatcher - Initial State") {
    KeyTable key_table;
    auto matcher = make_matcher(key_table);
    CHECK(matcher.state() == MatcherState::Idle);
}

TEST_CASE("SequenceMatcher - Introducer Alone Starts Capture") {
    KeyTable key_table;
    auto matcher = make_matcher(key_table);

    auto r1 = matcher.process_key_event(KPKEY_SHIFT, KeyEventType::Pressed);
    CHECK(r1.type == MatcherActionType::None);
    CHECK(matcher.state() == MatcherState::Idle);

    auto r2 = matcher.process_key_event(KPKEY_SHIFT, KeyEventType::Released);
    CHECK(r2.type == MatcherActionType::StartCapture);
    CHECK(matcher.state() == MatcherState::Capturing);
}

TEST_CASE("SequenceMatcher - Multi-Key Introducer Disarmed") {
    KeyTable key_table;
    auto matcher = make_matcher(key_table);

    static_cast<void>(matcher.process_key_event(KPKEY_SHIFT, KeyEventType::Pressed));
    static_cast<void>(matcher.process_key_event(KPKEY_A, KeyEventType::Pressed));
    static_cast<void>(matcher.process_key_event(KPKEY_A, KeyEventType::Released));
    auto r = matcher.process_key_event(KPKEY_SHIFT, KeyEventType::Released);
    CHECK(r.type == MatcherActionType::None);
    CHECK(matcher.state() == MatcherState::Idle);
}

TEST_CASE("SequenceMatcher - Capture and Complete on Trailer") {
    KeyTable key_table;
    auto matcher = make_matcher(key_table);

    // Start capture: Shift press & release
    static_cast<void>(matcher.process_key_event(KPKEY_SHIFT, KeyEventType::Pressed));
    static_cast<void>(matcher.process_key_event(KPKEY_SHIFT, KeyEventType::Released));
    REQUIRE(matcher.state() == MatcherState::Capturing);

    // Press 'Space'
    auto r_space = matcher.process_key_event(KPKEY_SPACE, KeyEventType::Pressed);
    CHECK(r_space.type == MatcherActionType::KeyAppended);
    static_cast<void>(matcher.process_key_event(KPKEY_SPACE, KeyEventType::Released));

    // Press Trailer 'Enter'
    auto r_enter = matcher.process_key_event(KPKEY_ENTER, KeyEventType::Pressed);
    CHECK(r_enter.type == MatcherActionType::SequenceComplete);
    REQUIRE(r_enter.sequence.size() == 1);
    CHECK(r_enter.sequence.codes[0] == KPKEY_SPACE);
    CHECK(matcher.state() == MatcherState::Idle);
}

TEST_CASE("SequenceMatcher - Capture and Complete on Timeout") {
    KeyTable key_table;
    auto matcher = make_matcher(key_table);

    // Start capture
    static_cast<void>(matcher.process_key_event(KPKEY_SHIFT, KeyEventType::Pressed));
    static_cast<void>(matcher.process_key_event(KPKEY_SHIFT, KeyEventType::Released));

    // Press Shift again (for double shift screenshot)
    static_cast<void>(matcher.process_key_event(KPKEY_SHIFT, KeyEventType::Pressed));
    static_cast<void>(matcher.process_key_event(KPKEY_SHIFT, KeyEventType::Released));

    // Timeout triggers completion
    auto r_timeout = matcher.on_timeout();
    CHECK(r_timeout.type == MatcherActionType::SequenceComplete);
    REQUIRE(r_timeout.sequence.size() == 1);
    CHECK(r_timeout.sequence.codes[0] == KPKEY_SHIFT);
    CHECK(matcher.state() == MatcherState::Idle);
}

TEST_CASE("SequenceMatcher - Keycode Substitution") {
    KeyTable key_table;
    auto matcher = make_matcher(key_table);

    // Start capture
    static_cast<void>(matcher.process_key_event(KPKEY_SHIFT, KeyEventType::Pressed));
    static_cast<void>(matcher.process_key_event(KPKEY_SHIFT, KeyEventType::Released));

    // Press K3KEY_HOME (102) -> should translate to KPKEY_HOME (98)
    static_cast<void>(matcher.process_key_event(K3KEY_HOME, KeyEventType::Pressed));
    static_cast<void>(matcher.process_key_event(K3KEY_HOME, KeyEventType::Released));

    auto r_timeout = matcher.on_timeout();
    CHECK(r_timeout.type == MatcherActionType::SequenceComplete);
    REQUIRE(r_timeout.sequence.size() == 1);
    CHECK(r_timeout.sequence.codes[0] == KPKEY_HOME);
}

TEST_CASE("SequenceMatcher - Cancellation") {
    KeyTable key_table;
    auto matcher = make_matcher(key_table);

    static_cast<void>(matcher.process_key_event(KPKEY_SHIFT, KeyEventType::Pressed));
    static_cast<void>(matcher.process_key_event(KPKEY_SHIFT, KeyEventType::Released));
    static_cast<void>(matcher.process_key_event(KPKEY_A, KeyEventType::Pressed));

    auto r = matcher.cancel();
    CHECK(r.type == MatcherActionType::Cancelled);
    CHECK(matcher.state() == MatcherState::Idle);
}
