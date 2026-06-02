# KiCAD "Update PCB from Schematic" — Known Warnings

Running **Update PCB from Schematic** in KiCAD consistently produces 8 warnings of the form "No net found for symbol X pin Y." All are benign and carry through from the original Eagle design.

## The 8 warnings

| Component | Pin | Reason |
|-----------|-----|--------|
| J4 (USB-C, GCT USB4105-GF-A) | M1 | Mechanical mounting tab — no electrical net by design |
| J4 (USB-C, GCT USB4105-GF-A) | M2 | Mechanical mounting tab — no electrical net by design |
| J4 (USB-C, GCT USB4105-GF-A) | M3 | Mechanical mounting tab — no electrical net by design |
| J4 (USB-C, GCT USB4105-GF-A) | M4 | Mechanical mounting tab — no electrical net by design |
| U6 (TPS797 LDO) | 3 (EN) | EN pin not modeled in Eagle-imported symbol; pad exists on footprint but has no symbol pin. EN floating = always on — intentional. |
| U10 (TLV3691 comparator) | 5 (HYST) | Hysteresis pin intentionally left floating — no external hysteresis resistor used |
| U11 (MIC9409 load switch) | 3 (FLAG) | Open-drain fault flag output intentionally unused |

## Why they can't be easily suppressed

- **J4 M1–M4**: The M pins exist in the Eagle-imported symbol but are not broken out visibly on the schematic sheet. No-connect markers cannot be placed on invisible pins.
- **U6 pin 3**: The EN pin is absent from the Eagle-imported TPS797 symbol entirely. Adding it would require editing the symbol and re-wiring the schematic instance.
- **U10 pin 5, U11 pin 3**: Pins exist in the symbol but are not visible on the schematic sheet. No-connect markers could suppress these if the pins are exposed.

## What to do

Nothing — these warnings do not affect manufacturing output, DRC, or board function. They are accepted as known informational noise from the Eagle→KiCAD migration.
