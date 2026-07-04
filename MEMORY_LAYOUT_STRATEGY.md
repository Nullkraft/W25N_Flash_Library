# Memory Layout Strategy

## Goal

Use the `W25N01GVZEIG` in a way that:

- keeps runtime address calculations simple
- uses fixed logical addresses for lookup tables
- relies on the chip's internal (Bad Block Mgmt) BBM to preserve logical continuity
- supports the expected two-write life cycle
- allows later service checks on aging chips

## Expected Write Life Cycle

This design assumes the chip is normally written only twice:

1. initial programming with uncalibrated data
2. final programming with calibrated data

After that, normal operation is read-only.

Because the chip is not expected to be rewritten often, the main service concern
is not normal wear from heavy rewrite traffic. The main concern is long-term
aging, detection of newly bad blocks, and whether enough BBM/LUT capacity remains
for continued safe use.

## Core Model

Use a **fixed logical image layout**.

- each lookup table has a fixed logical start address
- binary boundaries are part of the image format
- runtime code uses those fixed addresses directly
- runtime code does not need to understand physical bad-block remapping

The W25N01GV BBM LUT redirects a bad logical block to a good physical block. The
programming utility and runtime code continue using the original logical address,
so physical relocation does not alter the image layout.

## Image Layout

Define the image once using fixed logical areas.

The `W25N01GV` has a 1-Gbit main array: `134,217,728` bytes (`128 MiB`). The image
uses the following layout. Each table covers `0 MHz` through `3000 MHz`, inclusive,
and each entry is one 8-byte tuple.
TABLE 1: Address Assignment Table
|  Step   | Base address | Base page | Entry count | Last byte used |
|  ---:   | -----------: | --------: | ----------: | -------------: |
|   1 kHz | `0x00000000` |       `0` | `3,000,001` |  `0x016E3607`  |
|   2 kHz | `0x02000000` |  `16,384` | `1,500,001` |  `0x02B71B07`  |
|   3 kHz | `0x03000000` |  `24,576` | `1,000,001` |  `0x037A1207`  |
|   5 kHz | `0x03800000` |  `28,672` |   `600,001` |  `0x03C93E07`  |
|  10 kHz | `0x04000000` |  `32,768` |   `300,001` |  `0x04249F07`  |
|  20 kHz | `0x04400000` |  `34,816` |   `150,001` |  `0x04524F87`  |
|  50 kHz | `0x04600000` |  `35,840` |    `60,001` |  `0x04675307`  |
| 100 kHz | `0x04680000` |  `36,096` |    `30,001` |  `0x046BA987`  |
| 200 kHz | `0x046C0000` |  `36,224` |    `15,001` |  `0x046DD4C7`  |
| 500 kHz | `0x046E0000` |  `36,288` |     `6,001` |  `0x046EBB87`  |
|   1 MHz | `0x046F0000` |  `36,320` |     `3,001` |  `0x046F5DC7`  |

The table area is reserved through `0x046F7FFF`. The configuration and technician-
string area begins at `0x07000000`; its record format and allocation rules must be
specified as part of the image format.

The 11 tables contain `53,312,088` bytes (`50.84 MiB`) of tuple data. Their
fixed-address span, rounded through `0x046F7FFF`, occupies `74,416,128` bytes
(`70.97 MiB`) and touches 568 erase blocks. Those complete blocks occupy
`71 MiB`. The tables therefore fit comfortably within the first 1,000 blocks,
which the manufacturer may use BBM entries to keep logically continuous.

The datasheet guarantees at least 1,004 valid blocks, equal to `125.5 MiB`, but
BBM does not simply shorten the top of the logical address space. A bad logical
block is redirected to a separate good physical block.
Replacement blocks must therefore come from logical space that the image does
not use.

This project reserves blocks 1000 through 1023, the final `3 MiB`, as the
replacement-block pool. The range from `0x046F8000` through `0x06FFFFFF`
remains unassigned. The configuration and technician-string area beginning at
`0x07000000` is limited to blocks 896 through 999, ending at `0x07CFFFFF`, which
provides `13 MiB` without entering the replacement-block pool.

## Tuple Format

Each tuple contains two little-endian 32-bit words and occupies 8 bytes:

- word 0 contains the LO1 settings
- word 1 contains the LO2 settings
- each word stores `F[31:20]`, `M[19:8]`, `N[7:1]`, and `Ref[0]`
- `Ref` bits `0:0` select Ref1
- `Ref` bits `0:1` select Ref2
- `Ref` bits `1:0` select LO3 270 MHz
- `Ref` bits `1:1` select LO3 360 MHz
- `word0 == 0x00000000` is the special-command sentinel

The tuple field definitions and sentinel are part of the image format. Changing
them requires regenerating the programming image and updating every reader.

## Address Calculation

Use fixed binary boundaries for all table bases.

The fixed binary-aligned bases and 8-byte tuples allow address scaling with a
left shift instead of a general multiplication.

For tuple index `i`, the runtime calculation is:

- byte address: `base | (i << 3)`
- page address: `byte_address >> 11`
- column address: `byte_address & 0x07FF`

An 8-byte tuple and 2,048-byte main page produce exactly 256 tuples per page.
A 64-page erase block holds 16,384 tuples.

Bitwise OR may replace addition only when the base address and the complete
shifted offset cannot have any set bits in common:

- `base | (index << n)`

The 1 kHz table has a zero base, so `i << 3` is its complete byte address. The
bases for all 11 tables have no set bits in common with the maximum shifted tuple
offset for that table, making OR equivalent to addition throughout this layout.

All table bases are page-aligned, but they are not all erase-block-aligned. The
500 kHz and 1 MHz tables share the erase block beginning at `0x046E0000`.
Independent table erasure is therefore not supported by this image map;
that is consistent with the full-image programming and service-rewrite model.

No runtime bad-block translation layer is needed.

## Additional Information Placement

The 1 kHz tuple data ends at `0x016E3607`, and its allocated area ends immediately
before the 2 kHz base at `0x02000000`. That leaves `9,554,424` padding bytes. The
image reserves `0x07000000` as the configuration and technician-string base.

Use the fixed configuration area for additional information rather than placing
new data in table padding. This keeps table allocation gaps reserved, avoids
creating undocumented subregions, and follows the existing image design. The
configuration record format, size, versioning, and integrity checks still need
to be specified.

## BBM Boundary

BBM changes only the physical block holding the data. It does not change the
logical address, fixed image layout, or runtime lookup math.

## Service Strategy For Aging Chips

If a board has been in service for many years and starts showing trouble:

1. inspect the current BBM LUT usage
2. scan for newly bad blocks
3. decide whether the chip still has enough remaining remap capacity
4. if needed, update BBM
5. erase and reprogram from the authoritative original calibration image
6. verify the final image
7. restore full-array protection

BBM updates and aging checks are service actions; the runtime application does not
manage them on the fly.

## Source Of Truth

The flash chip is not the only copy of the important data.

Keep an off-chip golden image as the source of truth:

- uncalibrated image for production flow if needed
- calibrated image for final programming

Then if service work is required later, the utility can rebuild the flash contents
from the known-good source image after BBM or bad-block maintenance.

## BBM Capacity Policy

The datasheet does not guarantee how many LUT entries remain unused on a new chip.

Design assumptions:

- some LUT entries may already be consumed by the manufacturer
- remaining LUT capacity must be measured per chip
- low remaining capacity is a service warning

Policy:

- read LUT usage during programming and service
- treat a LUT slot as consumed if it is no longer reported as available,
  including an invalidated link that cannot be reused
- warn the user whenever inspection finds 18 or more consumed LUT slots
- warn the user before and after adding a link that raises the consumed count
  to 18, 19, or 20
- report that 18 consumed slots leave two hardware slots available but exhaust
  the project's acceptable operating reserve
- report that 19 consumed slots leave one hardware slot available
- report that 20 consumed slots make the LUT full and leave no further BBM
  capability
- treat any count of 18 or greater as a chip-replacement warning

## Spare Area Policy

Do not use the spare area as normal application payload storage.

Reasons:

- bad-block markers live there
- internal ECC uses it when `ECC-E=1`
- it is not the right place for normal lookup-table content

Normal application data lives in the main `2048`-byte page area.

## Runtime Model

Runtime code:

- use fixed logical addresses
- read tables from their defined locations
- remain read-only in normal operation

Runtime code does not:

- scan for bad blocks
- manage BBM updates
- relocate data

Those are programming or service-utility responsibilities.

## Bottom Line

The model is:

- fixed logical image layout
- binary boundaries kept in the image specification
- extra data stored in reserved logical subsections, not leftover physical space
- chip BBM used to preserve logical continuity
- golden image kept off-chip for reprogramming after service checks

With that model, your image layout stays simple and stable, and BBM handles the
physical remapping underneath it until the chip ages far enough that replacement
is the better answer.
