# Specification Quality Checklist: Strict Floating-Point Builds

**Purpose**: Validate specification completeness and quality before proceeding to planning
**Created**: 2026-11-07
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
- **Validation**: The first pass failed two items. FR-020, FR-029, FR-030, and FR-031 had no success criterion, and SC-004 left out the MSVC setting and would have matched the flag check's own text. Principle III's fixed instruction set was also unaddressed. SC-004, SC-006, SC-011, and SC-012 and one assumption fixed these, and the second pass found no failures.
- **Named files and options**: The spec names build files, compiler options, harness modes, and CI jobs, because the request and the constitution's closing conditions are written in those terms. They are what this feature changes, not choices of how to build it. The same reasoning covers the two items above about implementation details and technology-agnostic criteria.
- **Audience**: This is an internal engineering change, so its stakeholders are engine developers, packagers, and maintainers. Terms such as NaN, subnormal number, and fused instruction are part of their everyday language.
- **Deviations from the playbook**:
  - The flag check differs from the listing in the playbook. It also rejects `-mcpu=native` and other `-ffp-contract` values, reads the library list, and names each failure, because a test of the listing found those gaps (FR-011 to FR-014).
  - `parity-gate` gets read-only access, as Cross-Build Check has (FR-010).
  - The playbook says guards in `math_utilities.h` may take effect. `CheckNAN` has no callers, so FR-029 corrects that statement.
- **Choices the request left open**: The spec covers these under Decisions: what the flag check covers, keeping qmake's environment import, `/fp:precise` for MSVC Debug, leaving the NaN guards alone, no slowdown limit beyond the gate's 30 minutes, and closing KD-2 and KD-3 here. `-msse2` stays in the setup scripts (FR-006).
- **Added beyond the request**:
  - Self-Parity on the final commit (FR-017, SC-005), which Phase 3 requires. Three full runs match the bar KD-1 set.
  - Fixing what removing the options exposes (FR-018).
  - A record of what changed in the output, which items reach the NaN guards, and a review of the images (FR-019 to FR-021), because the guards change logic as well as rounding, and Baseline 1 locks in the result.
  - The expected effect on output (FR-022), which the constitution requires.
  - Seeded failures (FR-015), because KD-3 closes when the check rejects host-tuned flags, and a check that only ever passes does not show that.
  - One-CPU times (FR-024) and the cost of losing `-march=native` (FR-027), because the harness builds would not show either.
  - The constitution amendment and playbook corrections (FR-028, FR-029), which Governance requires.
- **Risk outside this feature**: The math library picks its code path from the CPU at run time, so identical flags can still give different results on different CPUs. The spec records this under Scope for Phase 4 to settle before Baseline 1.
