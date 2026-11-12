# AGENTS.md – Monsun

Instructions for AI coding agents (Claude Code and others). Keep this file short. Details live in `docs/`.

## Start here
1. Read this file, then `docs/concept.md` and `docs/safety.md`.
2. Planning lives in **Notion** (hub "Monsun": Work Packages, Decisions, Open Questions, BOM):
   https://app.notion.com/p/41546b9ee37081318764d9d42c404c0c
   If the task names a WP (e.g. `WP1.2`), read that WP and its "Done when" first.
3. Read only the docs and code the task needs. Do not scan the whole repo.

## Sources of truth
| What | Where |
|---|---|
| Code, docs, ADRs, test plans | this repo |
| Work packages, status, open questions | Notion |
| Final decisions | Notion "Decisions", then an ADR in `docs/adr/` |
| My parts, sourcing, prices | Notion "BOM" (`docs/bom.md` = reference BOM for builders) |
| System drawing (draw.io) | OneDrive `Projekte/Monsun/04_Projektunterlagen/02_Konzept` |

If a doc and Notion disagree, stop and report the conflict. Do not pick one silently.

## Pump names (always use these)
- Pump 0 = Regenschacht (own float switch, fills IBC 1)
- Pump 1 = IBC 1 (to IBC 2 through the moving bed filter)
- Pump 2 = IBC 2 (not bought yet)

## Rules
- Safety logic runs on the device. It never depends on network, Home Assistant, MQTT or time.
- Invalid or stale input: outputs OFF. Faults clear by themselves after the system reached a safe state.
- Mains (230 V) is out of scope for agents: describe, never claim it is safe. See `docs/safety.md`.
- Work on a branch (`claude/<topic>`). Never push to `main`. Open a PR; the owner reviews before merge.
- Architecture changes need an ADR in `docs/adr/`.
- C: C11, 4 spaces, `monsun_` prefix for public symbols. `firmware/libs` stays free of hardware and OS code.

## Build and test
```
cmake -S firmware/tests/host -B build/host
cmake --build build/host
ctest --test-dir build/host --output-on-failure
```
New engine or link behavior needs a host test in `firmware/tests/host/`.

## Definition of done
- The WP's "Done when" is met and shown (test output, checklist or photo note).
- Host tests pass. CI is green on the PR.
- Docs touched by the change are updated. Notion WP status and "Branch or PR" are updated.
- Unknown values are written as `TBD`, never guessed.

## Working style
- State assumptions at the top of the PR or answer. Ask at most one question, then proceed with the stated assumption unless the decision is safety-relevant or hard to undo.
- Keep answers and PR descriptions short.
