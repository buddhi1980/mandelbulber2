# Specification Quality Checklist: Baseline 1 and the Parity Gate

**Purpose**: Validate specification completeness and quality before proceeding to planning
**Created**: 2026-11-08
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
- **Clarifications**: The maintainer chose to force one math path (FR-001 to FR-003) and to start capture with a push of its branch (FR-007). The Clarifications section records both answers, and Decisions gives the options not chosen.
- **Validation**: The first pass failed one item besides the two open questions. FR-005, FR-006's read-only access, FR-007, FR-013, FR-016, FR-018, and FR-022 had no success criterion. FR-013 also asked for a record of the red push run while letting nothing land, and FR-028 forbade changing "the render settings", which a math path record might live beside. SC-002, SC-004, SC-005, SC-006, and SC-008 now cover those requirements, FR-013 records the run after Baseline 1 merges, and FR-028 names the resolutions and overrides. The second pass found no other failures.
- **After the answers**: The third pass rewrote User Story 1, FR-001 to FR-005, FR-007, FR-022, FR-024, FR-030, SC-004, and SC-006 for the two answers, and added FR-004's Cross-Build Check rule, because forcing the path changes what collect renders on Linux. A reread then found that the new edge case for a later glibc promised a check FR-002 can't make. It now relies on the toolchain record, which turns such a change into a reference target mismatch. User Story 7 names version 1.1.1.
- **Named files and options**: The spec names workflows, harness modes, branches, and repository settings, because the request and the constitution's closing conditions are written in those terms. They are what this feature changes, not choices of how to build it. The same reasoning covers the two items above about implementation details and technology-agnostic criteria.
- **Audience**: This is an internal engineering change, so its stakeholders are engine developers and maintainers. Terms such as math path, status check, and artifact are part of their everyday language.
- **Deviations from the playbook**:
  - The gate steps land by direct push after capture, right before Baseline 1's pull request opens, instead of in a pull request merged just before it (FR-013).
  - Capture can't start from "Run workflow" while `master` is the default branch, so a push of its `baseline/<number>` branch starts it (FR-007).
  - The math path step becomes harness work: every Linux render takes glibc's plain x86-64 path, the reference set records it, and the evidence measures what that changes (FR-001 to FR-005).
  - The required gate lets the maintainer's direct pushes through (FR-020), because the maintainer works on `deterministic-regression` directly.
  - The amendment's pull request edits only the constitution, and the playbook, `CLAUDE.md`, and `README.md` corrections land directly (FR-024 to FR-027).
- **Added beyond the request**:
  - The runner's CPU and math features, and which items differ between glibc's FMA path and the forced path (FR-005), so the cost of forcing is measured.
  - Collect and cross-check keep working, proven locally before the harness change lands (FR-004), because a push to `deterministic-regression` doesn't run Cross-Build Check.
  - What happens when capture fails Self-Parity (FR-010), and when a direct push fails the gate (FR-021).
  - A short, empty red window between the gate steps and Baseline 1 (FR-013).
  - The gate time, which SC-004 of `specs/001-regression-harness` bounds and which only a real gate run can show (FR-014).
  - References byte-identical to the artifact, because a line-ending conversion would break the set.
  - Corrections to `CLAUDE.md` and the playbook, and the README section the playbook's appendix asks for after Phase 4 (FR-025 to FR-027).
  - Deleting every branch this feature pushes (FR-030, SC-009).
