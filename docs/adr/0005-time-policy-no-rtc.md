# ADR-0005: No RTC in the prototype; time policy

Status: accepted

## Decision
Time comes from SNTP or is set from HA. The last-known epoch is saved to NVS periodically. After a boot without sync,
the controller continues with the saved time plus uptime and sets `time_valid = false`. Timer schedules run on the estimated time by default
(configurable: `wait` until valid). Level control and safety never depend on the clock.

## Consequences
- After a long power cut without WiFi the clock can be wrong. A DS3231 I²C module can be added later (M4).
