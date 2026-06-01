# RTC Power & Decoupling History

## Field Motivation

The Margay is a scientific data logger. Every record it produces is timestamped by the DS3231M RTC. When the RTC resets — losing the current time and returning to its epoch (January 1, 2000) — every subsequent log entry carries a wrong timestamp. The data is not merely imprecise; it is scientifically worthless. A sensor reading without a valid time cannot be placed in context, correlated with other measurements, or published.

Instruments are deployed in remote locations — stream gauges in wilderness watersheds, weather stations above treeline, soil sensors in arctic tundra — where servicing is expensive or seasonally impossible. A logger that silently resets its clock may accumulate weeks or months of invalid records before anyone notices. The fix is a site visit; the lost data is unrecoverable.

This is the engineering context behind the circuit changes described below. Keeping the clock running is not a convenience feature — it is the minimum condition for the instrument to do its job.

## Background

The DS3231M real-time clock has two power pins with distinct roles:

- **VCC** — primary power, supplied from the `3V3_AUX` switched rail. This rail is cut during MCU sleep to minimize quiescent current.
- **VBAT** — backup power, supplied from the CR1220 coin cell. The DS3231M switches to VBAT automatically when VCC falls below its internal threshold.

Because `3V3_AUX` is switched off every sleep cycle, the RTC transitions between VCC and VBAT every time the logger sleeps or wakes. The integrity of that handoff is the central concern of this page.

## Version History

### v0.0 and v1.0 — No VBAT decoupling

In the original designs, the VBAT pin connected directly to the coin cell with no decoupling capacitor. The net (`N$31` in the Eagle schematic) had only two members: `RTC1.VBAT` and `BT1.+`.

The `3V3_AUX` rail had minimal bulk decoupling — a single 1 µF capacitor at the LDO output.

**Effect:** When the MCU entered sleep and `3V3_AUX` collapsed, the DS3231M had to switch from VCC to VBAT with no local charge reservoir on VBAT to bridge the transition. The coin cell's ESR and the inductance of the PCB traces meant VBAT could dip transiently at exactly the moment the chip needed it. The DS3231M interpreted this as a power-on reset and cleared its time registers. Symptoms: logger woke up with the clock reset to its epoch (January 1, 2000), usually after a sleep cycle.

This was the primary cause of the RTC-reset failures observed in early field deployments.

### v2.1 — VBAT decoupling capacitor added

A 0.1 µF capacitor (C12, 0402) was added between the coin cell positive rail and the DS3231M VBAT pin. This matches the recommendation in the DS3231M datasheet.

The cap acts as a local charge reservoir: when VCC collapses, VBAT can supply instantaneous current from C12 while the coin cell's ESR settles, preventing the DS3231M from seeing a dip large enough to trigger a reset.

This change substantially reduced the frequency of RTC resets.

### v2.2 — Renumbering, more 3V3\_AUX bulk capacitance, switch topology change

- C12 was renumbered to **C19** (same value: 0.1 µF, 0402). The net was renamed from `N$31` to `VCOIN+`.
- `3V3_AUX` bulk decoupling grew significantly: from ~1 µF (v1.0) to ~21 µF total (C5 0.1 µF + C6 10 µF + C14 1 µF + C15 10 µF). A softer VCC collapse means a gentler handoff to VBAT.
- The `3V3_AUX` power switch was changed to **active-low control logic**, affecting the switching waveform on sleep entry.

### v3.0 — No change to RTC power circuit

v3.0 (USB-C, #4 mounting holes) made no changes to RTC power or decoupling. The circuit is identical to v2.2.

## Current Circuit (v2.2 / v3.0)

| Pin | Net | Connected to |
|-----|-----|--------------|
| VCC | `3V3_AUX` | Switched LDO output; cut during sleep |
| VBAT | `VCOIN+` | Coin cell (BT1) ∥ C19 (0.1 µF, 0402) |

The VBAT line is **not** connected to the always-on core 3.3 V rail.

## Remaining Vulnerability

The 0.1 µF cap on VBAT cannot sustain the RTC indefinitely. Two failure modes remain:

1. **Depleted or missing coin cell.** If VBAT has no source, C19 discharges in microseconds and the RTC resets.
2. **Cold-temperature coin cell brownout.** Standard CR1220 cells (Li-MnO₂) are rated to −20°C. Below that, internal resistance rises sharply and the cell may not be able to maintain VBAT above the DS3231M's minimum (2.3 V). This was the cause of observed clock failures in subarctic deployments.

Cold-temperature mitigation (available since v3.0): replace the CR1220 with a Panasonic BR1220 (rated −30°C) or BR1225A (rated −40°C, 2.5 mm thick — fits the Linx BAT-HLD-012-SMT holder). See the README for details.

## Proposed Future Improvement — VBAT Spoof (Issue #43)

To address both failure modes, v3.1 may route the always-on core 3.3 V rail to DS3231M VBAT through a Schottky diode (~0.3 V drop → ~3.0 V on VBAT, within the DS3231M's 2.3–5.5 V spec), with the existing C19 retained as the decoupling cap.

The coin cell would remain as a last-resort backup if the main pack is exhausted; the Schottky diode prevents backfeed into the core rail if the coin cell voltage ever exceeds it.

Pros and cons are discussed in [Issue #43](https://github.com/NorthernWidget/Project-Margay/issues/43).
