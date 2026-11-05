# Contract: Collection

`--collect DIR` writes this layout, and `--cross-check` reads it. A collection is plain files, so it can be copied between machines, and the cross-check needs neither build.

## Layout

```text
<dir>/
├── collection.txt
├── scenes/
│   └── <scene name>.zip
└── formulas/
    └── <internal name>.zip
```

The zips use the entry format in [reference-set.md](reference-set.md). An item that failed has no zip.

## `collection.txt`

Written last, so its final `end` line proves the collection is complete. The file is UTF-8 with LF line endings on every platform.

```text
mandelbulber-collection 1
environment wsl
isolated true
source_commit 8d93bf50e0a1c7d2b3f4e5d6c7b8a9f0e1d2c3b4
source_dirty false
application Mandelbulber 2.35-dev
compiler gcc 13.3.0
math_library glibc 2.39
export_format 1
scene_resolution 320x240
formula_resolution 32x24
overrides opencl_enabled=0#normal_enabled=1#DOF_samples=8#DOF_min_samples=4
item	scene	IFS_001	passed
item	formula	mandelbulb	passed
item	scene	amazing_surf 002	failed	Can't load texture
end
```

**Rules**

- Header lines are `name value`, in the order shown. Values shown here are examples.
- `item` lines separate their fields with tabs, because scene names can contain spaces. The fields are `item`, the kind, the name, `passed` or `failed`, and for a failed item, the cause.
- `environment` follows the [Environment](../data-model.md) rules.
- `isolated` is `false` where the application ignores a redirected home folder (FR-005).
- `source_commit` is the output of `git rev-parse HEAD`.
- `source_dirty` is `true` if `git status --porcelain -- mandelbulber2 tests` prints anything.
- `application` is the first output line of `--version` that starts with `Mandelbulber `. Earlier lines can be startup messages.
- `compiler` and `math_library` come from the first passed item's manifest, or are `unknown` if no item passed.
- Items keep the order of the work item list.

## Collecting

1. If `DIR` holds anything other than a collection, refuse it, exit 3, and change nothing.
2. Delete any earlier collection in `DIR`, removing `collection.txt` first.
3. Render every work item once, with the gate's settings, isolation, and failure detection. A failed item is recorded, and the run continues.
4. Write `collection.txt` last.
5. Exit 0 if every item passed, or 3 if any item failed.

A setup error before rendering stops the run and leaves no `collection.txt`, so a cross-check can never read a partial collection.

## Cross-Checking

The harness reads both collections and starts neither build.

**Setup failures, exit 3**

- A collection is missing, has no final `end` line, or has an unknown format version.
- An item that passed has a missing or corrupted zip.
- `source_commit` differs between the collections.
- `source_dirty` is `true` on either side.
- `export_format`, `scene_resolution`, `formula_resolution`, or `overrides` differs between the collections.

**Functional disagreements, exit 1**

- An item failed on either build. The report names the build and the cause.
- An item exists in only one collection. Formulas are matched by internal name.
- `application` differs between the collections.
- An item's width, height, buffer names, sample types, or statistic names differ between the builds.
- An item's `final_image` buffer holds one value throughout on one build, and more than one on the other.

**Recorded, never a failure**

- Each buffer that differs, with its count of differing samples.
- Each statistic that differs, with both values.
- The number of items that matched exactly in every buffer and statistic.

Values are compared bit for bit, as in every other mode. There is no tolerance, threshold, or perceptual metric (FR-045).
