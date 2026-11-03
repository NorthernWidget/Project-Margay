# Hardware/Margay_2 — Eagle-to-KiCAD port, current working version (work in progress)

This is the most current KiCAD design for the Margay v3.0, representing the active Eagle-to-KiCAD port.

**Current state:** schematic is split across three hierarchical sheets (`Margay_1/2/3.kicad_sch`) imported from Eagle; every symbol and footprint now comes from the shared `nw-cold-silicon` library, and the importer's `Margay-eagle-import.kicad_sym` and `Margay.pretty` have been retired. PCB layout (`Margay.kicad_pcb`) is also present. Both are works in progress and have not been verified against the v3.0 production Gerbers.

Tracking issue: https://github.com/NorthernWidget/Project-Margay/issues/41
