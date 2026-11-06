# Specification Quality Checklist: Deterministic CPU Rendering

**Purpose**: Validate specification completeness and quality before proceeding to planning
**Created**: 2026-11-05
**Feature**: [spec.md](../spec.md)

## Content Quality

- [x] No implementation details (languages, frameworks, APIs)
- [x] Focused on user value and business needs
- [x] Written for non-technical stakeholders
- [x] All mandatory sections completed

## Requirement Completeness

- [x] No [NEEDS CLARIFICATION] markers remain
- [x] Requirements are testable and unambiguous
- [x] Success criteria are measurable
- [x] Success criteria are technology-agnostic (no implementation details)
- [x] All acceptance scenarios are defined
- [x] Edge cases are identified
- [x] Scope is clearly bounded
- [x] Dependencies and assumptions identified

## Feature Readiness

- [x] All functional requirements have clear acceptance criteria
- [x] User scenarios cover primary flows
- [x] Feature meets measurable outcomes defined in Success Criteria
- [x] No implementation details leak into specification

## Notes

- Items marked incomplete require spec updates before `/speckit-clarify` or `/speckit-plan`
- **Named files and options**: The spec names `gRandomSeed`, the callers of `Random()`, the OpenMP regions, `--nogui`, `--self-parity`, and `limit_CPU_cores`, because the request sets them as the scope. It names `--threads` because the harness's stand-in renderer and tests already use it. These are interfaces and boundaries. The spec does not choose a random number algorithm, a counter design, or a way to fix the blend order.
- **Audience**: This is an internal engineering change, so its stakeholders are engine developers and maintainers. Terms such as raymarch, Monte Carlo, and post effect are part of their everyday language.
- **Deviation from the request**: The request says identical renders reported total iterations about 30 percent apart. That cannot be repeated on the current source, where total iterations read 0, so the spec records what was measured and has the evidence state the real spread (FR-009).
- **Choices the request left open**: The request allows a seed fixed in the scene or the harness. The spec chooses a scene setting with one fixed default, and it explains why under Decisions. It keeps the frame number out of the seed, because the request says values derive only from the seed and the position, so Monte Carlo noise stops changing between animation frames. Both are good questions for `/speckit-clarify`.
- **Added beyond the request**:
  - Keeping diagnostic changes out of the repository (FR-011).
  - Counting each pixel once, because threads can render one line twice (FR-025).
  - The real total iterations count, which the playbook places in this change (FR-026).
  - Live statistics that keep updating (FR-029) and an unchanged export format (FR-030).
  - Random values that match on every build (FR-018), no visible patterns (FR-019, SC-010), and unchanged ranges (FR-017).
  - Refusing `--threads` where it cannot take effect (FR-005).
  - The constitution amendment that closes KD-1 (FR-034), which Governance requires.
  - Render time before and after (FR-039), because the change sits on the hottest path and depth of field may slow down.
  - Three full Self-Parity runs in a row (SC-001), other thread counts (SC-002), and a Self-Parity run on MSVC (SC-013), because a rare race can pass one run, and the cross-check means nothing until both builds reproduce.
