# Phase 1 bench checklist (low voltage)

Used by WP1.5. Run with LED or 12 V lamp loads instead of pumps. No mains. See [safety.md](../safety.md).
Values marked `TBD` must be set in the Phase 1 specification (WP1.1) before the test.

| # | Test | Pass when | Result |
|---|---|---|---|
| 1 | Power on | Both relay outputs stay OFF during boot (no flicker) | |
| 2 | Watchdog reset (force a reset) | Outputs OFF after reset | |
| 3 | Pressure sensor unplugged | Dependent pump output OFF within `TBD` s | |
| 4 | Pressure sensor value out of range | Treated as invalid, output OFF | |
| 5 | Level control | Pump 1 starts below `TBD` and stops above `TBD` (hysteresis) | |
| 6 | Dry-run interlock (reservoir "not empty" float open while running) | Output OFF, fault latched | |
| 7 | Fault clears by itself | After safe state and condition gone, fault clears after `TBD` s | |
| 8 | Max run time | Output OFF after `TBD` s, MAX_RUNTIME fault | |
| 9 | Min off time | No restart before `TBD` s | |
| 10 | IBC 1 overflow float closes | Pump 0 stops (hardware or controller, per drawing V02) | |
| 11 | WiFi off | Steps 3–9 behave the same | |

Date, firmware commit and tester go in the PR or the Notion WP.
