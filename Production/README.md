# Production

This folder contains bills of materials, Gerber packages, and assembly files for each Margay hardware version.

## Contents

### Bills of Materials

| File | Version | Date | Notes |
|------|---------|------|-------|
| `Margay_BoM_2v2_-2021-05-10.csv` | v2.2 | 2021-05-10 | |
| `Margay_BoM_2v3_-_2023-03-16.csv` | v3.0 day 1 | 2023-03-16 | "v2.3" was a one-day working name; board became v3.0 |
| `Margay_BoM_3v0_-_2023-03-17.csv` | v3.0 | 2023-03-17 | Final v3.0 BoM used for production |

### Gerber and assembly packages

| File | Version | Date | Notes |
|------|---------|------|-------|
| `Margay_3v0_proto_2023-05-01.zip` | v3.0 proto | 2023-05-01 | Prototype Gerber run; detailed top silkscreen; same copper as production |
| `Margay_3v0_2023-08-01.zip` | v3.0 production | 2023-08-01 | Production Gerbers used for Caltronics assembly run |
| `Margay_3v0.mnt` | v3.0 | — | Pick-and-place / centroid file for SMT assembly |

### Quotes

| File | Date | Notes |
|------|------|-------|
| `CaltronicsQuote_2023-08-01.zip` | 2023-08-01 | Assembly quote from Caltronics for the v3.0 production run |

---

## Submitting to a fab house

Each Gerber ZIP contains a `CAMOutputs/` folder exported from Eagle with the following structure:

```
CAMOutputs/
  Assembly/
    Margay.txt          — BoM (Eagle-exported)
    PnP_Margay_front.txt — Pick-and-place, top side
    PnP_Margay_back.txt  — Pick-and-place, bottom side
  DrillFiles/
    drill_1_16.xln      — Excellon drill file
  GerberFiles/
    copper_top.gbr / copper_bottom.gbr
    silkscreen_top.gbr / silkscreen_bottom.gbr
    soldermask_top.gbr / soldermask_bottom.gbr
    solderpaste_top.gbr / solderpaste_bottom.gbr
    profile.gbr         — Board outline
    gerber_job.gbrjob   — Job file (KiCAD/Gerber viewer metadata)
```

Submit the full ZIP to the PCB fabricator. For assembly, additionally provide the pick-and-place files and the production BoM CSV.

> **Note:** When these packages are restructured into versioned subdirectories, each subfolder will contain its own README with version-specific notes.
