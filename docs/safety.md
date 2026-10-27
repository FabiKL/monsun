# Safety

> Monsun is **not** a certified product. Mains (230 V) work must be done or reviewed by a qualified electrician.

## Electrical
- Separate 230 V and low-voltage compartments (barrier or separate boxes). Keep creepage/clearance distances.
- Protective earth connected to the metal parts and pumps. RCD (30 mA) upstream of all pumps.
- Fuse or MCB per pump circuit sized for pump inrush and cable cross-section.
- Strain relief and IP-rated cable glands. Drip loops. Never open the box while mains is connected.
- Use relays rated for the inductive pump load (and inrush). Opto-isolated relay modules are not mains-certified; mount them
  in the 230 V compartment and treat all their pins as live where the module is not isolated.
- 24 V rails fused; flyback diodes on inductive 24 V loads.
- Bench bring-up with low-voltage loads (LEDs, 12 V lamps) before connecting mains.

## Software
- All outputs OFF at boot, on watchdog reset, on loss of bus communication (nodes) and on invalid safety inputs.
- Maximum run time per actuator, minimum off time, dry-run lockout with latched fault.
- Safety logic never depends on HA, MQTT or network time.
- Safety inputs must be hardwired (local I/O or MonsunLink) with staleness detection.

## Water
- Overflow protection must not rely on a single sensor type for a tank that can flood; add a hardwired float switch.
- Mind frost: drain pumps and lines in winter.
