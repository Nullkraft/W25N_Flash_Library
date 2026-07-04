# Memory Layout Strategy

## Goal

Use the `W25N01GVZEIG` in a way that:

- keeps runtime address calculations simple
- uses fixed logical addresses for lookup tables
- relies on the chip's internal BBM to preserve logical continuity
- supports the expected two-write life cycle
- allows later service checks on aging chips

Note:
    "Fixed logical addresses for lookup tables" means your software should treat each
    frequency step table as living at a fixed address in the chip’s normal address space,
    regardless of which physical NAND block actually holds it underneath.

  Example idea:

  - 1 kHz table starts at logical address A
  - 3 kHz table starts at logical address B
  - 10 kHz table starts at logical address C

  The intent is not to force generic multiplication into the runtime path.
  The intent is to keep the table bases fixed so the remaining address math can be chosen
  to be cheap, such as:

  - base address plus direct offset
  - base address plus shift-derived offset
  - base address plus shift/add offset

  For example, if the entry size is a power of two, scaling can be done with a left shift
  instead of a general multiply.

  So “fixed logical address” means:

  - the address your code uses is fixed by your image format
  - it does not move around from chip to chip
  - it does not depend on where bad physical blocks happened to be

  Why that can still work:

  - if the chip remaps a bad block through BBM, the original logical block number still works
  - the chip internally redirects that logical block to a replacement physical block

  So your code still reads the same logical address, even though the chip may be pulling the data from a different physical block internally.

  Short version:

  - logical address = the stable address your program uses
  - physical block = where the data actually lives inside NAND
  - BBM is meant to hide physical relocation so your logical addresses stay fixed


## Expected Write Life Cycle

This design assumes the chip is normally written only twice:

1. initial programming with uncalibrated data
2. final programming with calibrated data

After that, normal operation is read-only.

Because the chip is not expected to be rewritten often, the main service concern is not normal wear from heavy rewrite traffic. The main concern is long-term aging, detection of newly bad blocks, and whether enough BBM/LUT capacity remains for continued safe use.

## Core Model

Use a **fixed logical image layout**.

That means:

- each lookup table has a fixed logical start address
- binary boundaries are part of the image format
- runtime code uses those fixed addresses directly
- runtime code does not need to understand physical block remapping

The chip's internal Bad Block Management is expected to keep logical block addresses usable even when specific physical blocks are replaced.

## Why This Simpler Model Works

The W25N01GV provides an internal BBM LUT that remaps a bad logical block to a good physical block.

Practical meaning:

- your programming utility still talks in logical block/page addresses
- a remapped block still appears at the original logical address
- your image layout can remain fixed even if the underlying physical storage changes

So long as BBM capacity is still available, your image does not need to care about physical fragmentation.

## Recommended Image Layout

Define the image once using fixed logical areas.

The `W25N01GV` has a 1-Gbit main array: `134,217,728` bytes (`128 MiB`). The
older `WN2A_FlashProgrammer` project contains separate 64 MiB and 128 MiB
layouts. Its 64 MiB 1-3-10 ladder is useful design history, but it is not the
capacity of this chip and should not be treated as the final layout for this
project.

The first fixed area is now defined as follows:

| Area | Start | Entry count | Entry size | Last byte used | First free byte |
| --- | ---: | ---: | ---: | ---: | ---: |
| `table_1khz` | `0x00000000` | `3,000,001` | `8` bytes | `0x016E3607` | `0x016E3608` |

This table covers `0 MHz` through `3000 MHz`, inclusive. Entry `i` begins at:

`i << 3`

The old 1-3-10 layout placed `table_3khz` at `0x02000000`. If that boundary is
retained, the complete `table_1khz` area is `0x00000000` through `0x01FFFFFF`,
and the unused tail after the table contains `9,554,424` bytes. That next-table
boundary remains a layout decision to confirm, not a chip requirement.

Planned logical areas include:

- `table_1khz`
- `table_3khz`
- `table_10khz`
- `table_30khz`
- `device_metadata`
- `reserved_future`

Each area should have:

- fixed logical start address
- fixed maximum size
- fixed interpretation

The remaining addresses must be fixed by the image specification, not by
whatever physical block placement happened on a given chip.

## Recommendation For Binary Boundaries

Keep the binary-boundary rule.

That is still a good idea because it keeps runtime math simple.

For example:

- the base address of `table_1khz` is fixed
- the base address of `table_3khz` is fixed
- each table entry size and alignment are chosen to support cheap address math

Then runtime lookup is simple:

- base logical address
- plus direct offset or shift/add-derived offset

Examples of the intended style:

- `base + offset`
- `base + (index << n)`
- `base + ((major_index << a) + (minor_index << b))`

Bitwise OR may replace addition only when the base address and the complete
shifted offset cannot have any set bits in common:

- `base | (index << n)`

The 1 kHz table has a zero base, so `index << 3` is its complete byte address.
For later tables, each selected binary boundary must be checked against that
table's maximum shifted offset before using OR.

Avoid designing the layout so runtime needs a general multiply if that is what you are
trying to eliminate.

No runtime bad-block translation layer is needed if the chip's BBM has already preserved the logical address space.

## Recommendation For "Top Of 1 kHz Area"

Do not define "top of the 1 kHz area" by whatever physical space happens to remain.

Instead:

- reserve a fixed logical tail section inside the `table_1khz` area
- give that tail section a defined start offset and size
- treat it as part of the logical image format

With the current 1 kHz table definition, lookup data ends at `0x016E3607`.
If the next area starts at `0x02000000`, any additional information must be
assigned a fixed address within `0x016E3608` through `0x01FFFFFF`; its address
must not be calculated from physical bad-block placement.

Example concept:

- `table_1khz.data`
- `table_1khz.tail_info`

This keeps the extra data location stable even if one or more physical blocks backing that logical area are remapped by BBM.

## What BBM Changes And What It Does Not

BBM changes:

- which physical block actually stores a given logical block

BBM does not change:

- the logical block address used by your software
- the fixed logical image layout
- your runtime lookup math

So for your layout strategy, BBM is expected to hide physical relocation from the normal programming and read cycle.

## Service Strategy For Aging Chips

If a board has been in service for many years and starts showing trouble:

1. inspect the current BBM LUT usage
2. scan for newly bad blocks
3. decide whether the chip still has enough remaining remap capacity
4. if needed, update BBM
5. erase and reprogram from the authoritative original calibration image
6. verify the final image
7. restore full-array protection

This is the safest model because BBM updates and aging checks should be treated as service actions, not as something the runtime application tries to manage on the fly.

## Source Of Truth

The flash chip should not be treated as the only copy of the important data.

Keep an off-chip golden image as the source of truth:

- uncalibrated image for production flow if needed
- calibrated image for final programming

Then if service work is required later, the utility can rebuild the flash contents from the known-good source image after BBM or bad-block maintenance.

## BBM Capacity Policy

The datasheet does not guarantee how many LUT entries remain unused on a new chip.

So the design should assume:

- some LUT entries may already be consumed by the manufacturer
- remaining LUT capacity must be measured per chip
- low remaining capacity should be treated as a service warning

Recommended policy:

- read LUT usage during programming and service
- define a minimum acceptable remaining-entry threshold
- warn the user when the chip is approaching replacement time

## Spare Area Policy

Do not use the spare area as normal application payload storage.

Reasons:

- bad-block markers live there
- internal ECC uses it when `ECC-E=1`
- it is not the right place for normal lookup-table content

Normal application data should live in the main `2048`-byte page area.

## Runtime Model

Runtime code should be simple:

- use fixed logical addresses
- read tables from their defined locations
- remain read-only in normal operation

Runtime code should not:

- scan for bad blocks
- manage BBM updates
- relocate data

Those are programming or service-utility responsibilities.

## Bottom Line

The recommended simple model is:

- fixed logical image layout
- binary boundaries kept in the image specification
- extra data stored in reserved logical subsections, not leftover physical space
- chip BBM used to preserve logical continuity
- golden image kept off-chip for reprogramming after service checks

With that model, your image layout stays simple and stable, and BBM handles the physical remapping underneath it until the chip ages far enough that replacement is the better answer.
