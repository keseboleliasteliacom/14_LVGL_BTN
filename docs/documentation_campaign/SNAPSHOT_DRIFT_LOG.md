# Snapshot and Drift Log

| Date | Repository | Reference | Commit | Meaning/action |
| --- | --- | --- | --- | --- |
| 2026-07-26 | `Glennergy-ESP` | historical baseline | `f2974fb64af25495f79ba412c2626c9b77688ee8` | Original plan reference; remains an ancestor of active `dev` |
| 2026-07-26 | `Glennergy-ESP` | `dev` / `origin/dev` | `b5a502afd9ca2ae374b3131b0031b8390f93b348` | Authoritative Phase 0 campaign snapshot |
| 2026-07-26 | `Glennergy-ESP` | `origin/main` | `daf35c538d84586576f8286c2d543eb1c3c89e6a` | Stable-production snapshot; local `main` was stale |
| 2026-07-26 | `glennergy` | `dev` / `origin/dev` | `42798bee227fcd621cbcb0b37c2b5da771210086` | Authoritative Phase 0 campaign snapshot |
| 2026-07-26 | `glennergy` | `origin/main` | `61761b5eda30bee417a0b6e33e10fb061e18db26` | Stable-production snapshot; local `main` was stale |
| 2026-08-03 | `Glennergy-ESP` | `dev` / `origin/dev` | `693dc8819ac5b6d8fb29ce057d287814a3b9a14d` | Incremental documentation refresh after UI, Wi-Fi, Settings, diagnostics, and 128-entry integration changes |
| 2026-08-03 | `glennergy` | `dev` / `origin/dev` | `63b1bad306d172e3d8cd337b314843f656715887` | Coordinated server snapshot after forward-window and 128-entry result changes |

Before incorporating later source changes, record the new commit and assess its
documentation impact rather than silently moving the campaign baseline.
