# ADR 0004: Object Calisthenics Composition for Instance Variables

- **Status**: Accepted
- **Date**: 2026-09-22
- **Deciders**: Core Engineering Team

---

## Context

Kindle Launchpad enforces **Object Calisthenics** to maintain clean code, high cohesion, and low coupling. Rule 8 of Object Calisthenics specifies:

> **No more than 2 instance variables per entity.**

In a complex daemon application, orchestrator components such as `Application` must coordinate numerous subsystem dependencies:
- Hardware drivers (`IInputSource`, `IDisplay`, `ICommandRunner`, `IKeyInjector`).
- Domain models (`ActionRegistry`, `SequenceMatcher`).
- Configuration state (`LaunchpadSettings`).
- UI overlays (`StatusBar`).
- Process primitives (`SignalHandler`, `PidLock`).

If declared as individual member variables in a monolithic class, `Application` would contain 10+ instance variables, violating Rule 8 and creating a tightly-coupled "god class".

---

## Decision

We decided to strictly enforce Rule 8 by decomposing multi-variable entities into cohesive, structured **composition aggregates** and **value objects**:

1. **Service Aggregates**:
   - `ApplicationServices`: Groups the 4 core HAL interfaces (`IInputSource&`, `IDisplay&`, `ICommandRunner&`, `IKeyInjector&`).
2. **Context Aggregates**:
   - `ApplicationContext`: Encapsulates runtime domain state (`LaunchpadSettings`, `ActionRegistry`, `SequenceMatcher`, `StatusBar`).
3. **Structured Application Class**:
   - `Application` holds exactly **two** instance variables:
     ```cpp
     class Application {
     private:
         ApplicationServices services_;
         ApplicationContext context_;
     };
     ```
4. **Configuration Value Objects**:
   - `MatcherConfig`: Aggregates sequence timing thresholds and delimiter codes into a single parameter object.
   - `RceConfig`: Aggregates multicast network settings (`port`, `multicast_ip`, `enable_string`).

---

## Consequences

### Positive
- **Strict Object Calisthenics Compliance**: Every class across the codebase strictly maintains $\le 2$ instance variables.
- **High Cohesion**: Related services and state are grouped logically rather than scattered as disjoint fields.
- **Simplified Testing & Mocking**: Tests can construct and inject mock service bundles in a single line without configuring 8 individual constructor arguments.

### Negative / Trade-offs
- **Intermediate Access Indirection**: Accessing deeply nested collaborators requires traversing through the aggregate (e.g. `services_.display().width()`), although this can be mitigated with concise private helper accessors.
