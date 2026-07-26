# Campaign Checkpoint Manifest

Recorded: 2026-07-26

| Repository | Authoritative source | Source commit | Stable production | Campaign branch |
| --- | --- | --- | --- | --- |
| `Glennergy-ESP` | `dev` / `origin/dev` | `b5a502afd9ca2ae374b3131b0031b8390f93b348` | `origin/main` at `daf35c538d84586576f8286c2d543eb1c3c89e6a` | `docs/documentation-campaign-2026-07-26` |
| `glennergy` | `dev` / `origin/dev` | `42798bee227fcd621cbcb0b37c2b5da771210086` | `origin/main` at `61761b5eda30bee417a0b6e33e10fb061e18db26` | `docs/documentation-campaign-2026-07-26` |

## Pre-existing ESP working-tree changes

These files existed as modified or untracked before campaign branch creation.
They are not campaign-owned merely because they are present on the campaign
branch. Never stage them broadly.

```text
M  docs/Doxygen_TODO.md
?? CURRENT_PROJECT_STATE_AND_PLAN.md
?? GLENNERGY_ESP32_PLAN_SUMMARY.md
?? PROJECT_PRESENTATION_DOCS.md
?? SESSION_PLANNING_SUMMARY_2026-04-24.md
?? SIGNALFLOW.md
?? UART_ARCHITECTURE_ROADMAP.md
?? UART_NEXT_STEPS.md
?? docs/LEOP_CONNECTIVITY_MONITORING_DECISION.md
```

`docs/DOCUMENTATION_CAMPAIGN_BASELINE.md` was also untracked before branch
creation, but it is campaign-owned and is intentionally excluded from the list
above.

The Glennergy working tree was clean before branch creation.

## Recovery rule

Restore or revert campaign-owned paths and commits individually. Do not reset an
entire repository or discard the pre-existing ESP files. Recheck `git status` in
both repositories after any recovery action.
