# Learning to Plan Driver development


**Planning moves from**:

      Purpose to Boundaries
              ↓
    Boundaries to Architecture
              ↓
    Architecture to Milestones
              ↓
       Milestones to Files


## Purpose
    The purpose of this driver is to provide a c-language interface for talking to
    and controlling the state of the W25N01GVZEIG 1 Gbit Flash chip. First for
    **initialization and communication** followed by **programming and data verification**
    and later for **providing read-access** for an application.

## Non-goals
  This driver is not responsible for data formatting, memory management or type checking.

## Hardware assumptions
  SPI transfer rate (clock) should be configured for the highest possible setting. SPI is set for single, or standard. Not dual or quad. WP and HOLD pins are disabled by board voltages. The control for the CS pin is handled by the controller firmware.

## Runtime policy <-- what's this?
    - Are we talking about complexity?
    - Access memory by incrementing addresses rather than using a for-loop
    - Choose bit manipulation over mathematical algorithms over programming structures

## Driver-owned responsibilities
  - read
  - write with ECC
  - verify
  - erase
  - block address start variable 
  - current page address

## Application-owned responsibilities
  - memory management
  - data formatting
  - CS pin select/deselect
  - memory address management

## First milestone
  - initialize and read/confirm chip JEDEC ID

## Deferred capabilities
  - erase page
  - erase block
  - erase chip
  - write, read, and verify a single address
  - write, read, and verify full page
  - write, read, and verify partial page
  - write, read, and verify 1kHz step table

## Likely file boundaries <-- Do you mean what goes in each file and what files?
