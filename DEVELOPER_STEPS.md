# W25N01GVZEIG Driver Steps

Target device: `W25N01GVZEIG`

Assumption: distributor suffix `TR` is packaging, not a different command set. The electrical and protocol behavior here is the `W25N01GVxxxG` device from the datasheet, which powers up with `BUF=1`.

## Operating Model

- [ ] Treat the runtime driver as read-only in normal product operation.
- [ ] Keep erase/program flows out of the normal runtime API and put them in a separate flash-programming utility.
- [ ] Use internal ECC during the full programming cycle so the calibrated data is programmed and verified with the chip's ECC engine enabled.
- [ ] Add a separate verify-only utility that compares chip contents against the original programming image without modifying the device.
- [ ] After programming is complete, use register-based full-array protection as the main safeguard against inadvertent writes.

## Datasheet Facts To Preserve

- [ ] Keep the geometry constants together:
    `1024` blocks,
    `64` pages per block,
    `2048` bytes main area per page,
    `64` bytes spare area per page,
    `2112` total bytes in the internal data buffer,
    `128KB` erase block size.
- [ ] Treat page address as `16-bit` and column address as `16-bit input with CA[11:0] effective`.
- [ ] Keep the JEDEC ID constants together: manufacturer `0xEF`, device ID bytes `0xAA 0x21`.
- [ ] Record the important timing targets in one place: `tRD1=25us`, `tRD2=60us`, `tPP=250us typ / 700us max`, `tBE=2ms typ / 10ms max`, `tRST=5us to 500us`.
- [ ] Record the power-up defaults that affect behavior: `BUF=1` for `xxxG`, `ECC-E=1`, block-protect bits default to protecting the full array, `WEL=0`.

## File Split

- [ ] Create a small public device API file for high-level operations only, for example `include/device.h`.
- [ ] Create a transport interface file that hides raw SPI and chip-select details, for example `include/spi_transport.h`.
- [ ] Create `include/flash_config.h` for manufacturer-stated hardware values only.
- [ ] Put the datasheet-defined hardware facts in `flash_config.h`: JEDEC IDs, page size, spare size, full buffer size, pages per block, block count, total capacity, erase-block size, address widths, default read mode, ECC default, and timing constants stated by the manufacturer.
- [ ] Keep `flash_config.h` free of behavior and protocol helpers; it should be configuration data only.
- [ ] Create a separate constants file for protocol items such as opcodes and register addresses, for example `include/constants.h`.
- [ ] Create a register/bit-definition file for `SR-1`, `SR-2`, and `SR-3`, for example `include/registers.h`.
- [ ] Create one implementation file for transport-independent device logic, for example `src/device.cpp`.
- [ ] Create one implementation file for optional bad-block and spare-area helpers, for example `src/bad_blocks.cpp`.
- [ ] Create a separate programming utility source file for erase/program/update operations, for example `tools/program_flash.cpp`.
- [ ] Create a separate verify-only utility source file for golden-image comparison, for example `tools/verify_flash.cpp`.
- [ ] Keep test code separate from driver code from the start, for example under `tests/`.

## Transport Layer

- [ ] Define a minimal SPI transport contract that can transmit command-only, command-plus-write-data, and command-plus-read-data transactions.
- [ ] Include an explicit busy-wait hook or delay hook so the core driver can poll status and respect erase/program/read transfer timing.
- [ ] Keep the transport focused on standard 1-bit serial SPI because the board is not wired for Dual/Quad operation.

## Bring-Up And Identification

- [ ] Implement `reset()` with opcode `0xFF`.
- [ ] After reset, poll status register `SR-3` until `BUSY=0` before accepting the device as ready.
- [ ] Implement `read_jedec_id()` with opcode `0x9F` plus the required dummy byte, and verify `EF AA 21`.
- [ ] Implement `read_status(register_address)` using opcode `0x0F` or `0x05`.

## Initialization

- [ ] Implement an `initialize()` sequence that performs reset, JEDEC-ID check, checks BP[3:0]/TB for full-array protection and status-register snapshot.
- [ ] Do not clear array protection during normal runtime initialization; the datasheet default full-array protection is acceptable for a read-only runtime model.
- [ ] Do not change ECC policy during normal runtime initialization; ECC setup belongs to the dedicated programming utility.
- [ ] Keep `WP-E=0` unless there is a specific reason to change it; on this board `/WP` and `/HOLD` are held high and are not the primary write-protection mechanism.
- [ ] Keep `BUF=1` as the default first implementation path for the `G` part, and treat continuous-read support as a later feature.

## Read Path

- [ ] Implement `page_data_read(page_address)` with opcode `0x13`, then poll `SR-3.BUSY` until clear.
- [ ] Implement `read_data(column_address, dst, len)` with opcode `0x03`.
- [ ] In buffer-read mode (`BUF=1`), require the caller to supply the starting column address and stop at the end of the loaded buffer.
- [ ] Build a high-level `read_page(page_address, dst)` API as: `0x13` page load, wait for ready, then `0x03` buffer read.
- [ ] Add support for reading spare bytes explicitly so bad-block markers and ECC/spare metadata can be inspected.
- [ ] After reads with internal ECC enabled, inspect `SR-3.ECC[1:0]` and surface three states to the caller: clean, corrected, uncorrectable.
- [ ] If continuous read is added later, implement it as a separate path because `BUF=0` changes read semantics and ECC reporting behavior.

## Programming Utility Path

- [ ] Keep all erase/program/update commands in the dedicated programming utility rather than the normal runtime driver surface.
- [ ] Implement `write_status(register_address, value)` using opcode `0x1F` or `0x01` inside the programming utility support layer.
- [ ] Implement `write_enable()` with opcode `0x06`, then verify `SR-3.WEL=1`.
- [ ] Implement `write_disable()` with opcode `0x04`, then verify `SR-3.WEL=0`.
- [ ] At the start of the programming cycle, clear full-array protection in `SR-1` because the datasheet powers up with the full array protected.
- [ ] Explicitly program with `ECC-E=1` during the full-chip programming cycle so the device generates and stores ECC parity as pages are written.
- [ ] Implement `load_program_data(column_address, src, len)` with opcode `0x02`.
- [ ] Implement `random_load_program_data(column_address, src, len)` with opcode `0x84` for patching portions of the buffer without clearing untouched bytes.
- [ ] For each program operation, require `write_enable()` first and confirm `WEL=1`.
- [ ] Implement `program_execute(page_address)` with opcode `0x10`, then poll `BUSY` until clear.
- [ ] After program completion, inspect `SR-3.P-FAIL` and treat a set bit as a hard failure.
- [ ] Enforce datasheet limits in the programming utility: only program previously erased locations, only `4` partial page programs per page maximum, and only program pages in ascending order within a block.
- [ ] Build a high-level `program_page(page_address, column_address, src, len)` helper for the programming utility as: `0x06`, `0x02/0x84`, `0x10`, wait, check `P-FAIL`.
- [ ] Implement `block_erase(page_address_in_block)` with opcode `0xD8`; the device takes a page address, not a raw block number command.
- [ ] For each erase operation, require `write_enable()` first and confirm `WEL=1`.
- [ ] After erase completion, inspect `SR-3.E-FAIL` and treat a set bit as a hard failure.
- [ ] Build a high-level erase helper in the programming utility that converts block index to the first page in that block before issuing erase.
- [ ] After each programmed page or block-sized chunk, read back and compare against the source image before moving on.
- [ ] Finish the programming cycle with a full-chip verification pass against the calibration image.
- [ ] At the end of the programming cycle, restore `SR-1` protection bits to protect the full array against inadvertent program/erase operations during normal use.

## Bad Blocks And NAND-Specific Rules

- [ ] Never erase the original factory marker information before the initial scan has been captured.
- [ ] On first-use tooling, scan factory bad-block markers before any erase/program pass.
- [ ] For each block, inspect page `0` byte `0` of the main area and byte `0` of the spare area; non-`0xFF` means bad block.
- [ ] Keep a software bad-block table even if the hardware BBM LUT is used.
- [ ] Expose the BBM LUT status bit `SR-3.LUT-F`.
- [ ] Implement optional `read_bbm_lut()` with opcode `0xA5`.
- [ ] Implement optional `bad_block_management(lba, pba)` with opcode `0xA1` only after the basic driver is stable.

## API Design

- [ ] Keep raw command wrappers separate from user-facing read APIs and from the separate programming utility APIs.
- [ ] Use distinct types or helper functions for page address, block index, and column offset so NOR-style flat addressing does not leak into the NAND layer.
- [ ] Make ECC result, program failure, erase failure, timeout, protected-region rejection, and JEDEC mismatch visible in the returned status.
- [ ] Keep spare-area access explicit instead of silently folding spare bytes into normal page reads.

## Tests And Bring-Up Sequence

- [ ] Add unit tests for opcode packing, register-bit decoding, block-to-page conversion, and page/column bounds checks.
- [ ] Add a runtime integration test sequence for a known-good page: reset, identify, read page, verify payload, verify spare-area access, report ECC state.
- [ ] Add a programming-utility integration test sequence: reset, identify, scan bad blocks, unprotect, erase block, program page, read back, verify payload, verify spare-area access.
- [ ] Add a test that confirms `WEL` clears after program and erase in the programming utility path.
- [ ] Add a test that confirms protected-array writes fail until protection is cleared in the programming utility path.
- [ ] Add a test that checks ECC status propagation after read operations.
- [ ] Add a test that scans and records factory bad blocks without modifying the device.
- [ ] Add a verify-only utility test that compares chip contents against a known image and reports the first mismatch clearly.

## Nice-To-Have After First Working Driver

- [ ] Add fast-read (`0x0B`) support after plain `0x03` reads are stable.
- [ ] Add continuous-read mode (`BUF=0`) as a separate feature with its own tests.
- [ ] Add OTP/parameter-page helpers only after main-array read/program/erase is proven.
