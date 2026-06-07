; ==============================================================================
; MODULE DESCRIPTION
; ==============================================================================
; Provides low-level memory management utilities. This module handles 
; stack pointer initialization and block memory copy subroutines for 
; transferring data between memory regions.

; ==============================================================================
; TODO
; ==============================================================================
; [ ] Implement memory clearing (memset) routine.
; [ ] Add optimized copy routines for VBlank-sensitive data.

; ==============================================================================
; LICENSE
; ==============================================================================
; Copyright (c) 2026 [Nicholas Gennady Korta]. All rights reserved.
;
; This source code is free software: you can redistribute it and/or modify
; it under the terms of the GNU Lesser General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
; GNU Lesser General Public License for more details.

; Constants
DEF cENGINE_MEMORY_STACK_TOP               EQU $E000 ; Default stack pointer top
DEF cENGINE_MEMORY_HRAM_START              EQU $FF80 ; Start of high speed ram

; iENGINE_MEMORY_STACK_POINTER_SET
; - Usage: iENGINE_MEMORY_STACK_POINTER_SET [$Addr]
; - Registers: Stack Pointer -> SP
; - Description: Sets the stack pointer. Defaults to cENGINE_MEMORY_STACK_TOP.
MACRO iENGINE_MEMORY_STACK_POINTER_SET
IF _NARG > 0
ld sp, \1
ELSE
ld sp, cENGINE_MEMORY_STACK_TOP
ENDC
ENDM

; iENGINE_MEMORY_WRITE_MULTIPLE_WRAM
; - Usage: iENGINE_MEMORY_WRITE_MULTIPLE_WRAM value, address_1, [address_2], [address_3]...
; - Registers: value -> A -> Address(es)
; - Description: Loads a value into the A register and stores it into any number of provided addresses.
MACRO iENGINE_MEMORY_WRITE_MULTIPLE_WRAM
; load
ld a, \1
; store
REPT _NARG - 1
SHIFT
ld [\1], a
ENDR
ENDM

; iENGINE_GRAPHICS_LOAD
; - Usage: iENGINE_GRAPHICS_LOAD SourceStart, SourceEnd, Destination,[MemoryCopyOffset]
; - Usage: iENGINE_GRAPHICS_LOAD SourceStart, SourceEnd, Destination,[MemoryCopyOffset]
; - Registers: Destination -> HL, SourceStart -> DE, (SourceEnd - SourceStart) -> BC, Offset -> A
; - Description: Loads source data block into VRAM.
MACRO iENGINE_MEMORY_COPY_SUBROUTINE_CALL
ld hl, \3
ld de, \1
ld bc, \2 - \1
IF _NARG == 4
ld a, \4
ELSE
ld a, 0
ENDC
call Engine_Memory_Copy
ENDM

; iENGINE_MEMORY_COPY_SUBROUTINE_INCLUDE
; - Source: https://gbdev.io/gb-asm-tutorial/part2/functions.html
; - Source modified: Added offset support
; - Usage: call Engine_Memory_Copy
; - Registers: Source address -> DE, Destination address -> HL, Length -> BC, Offset -> HRAM
; - Description: Copies a block of memory from DE to HL, applying an offset to each byte.
MACRO iENGINE_MEMORY_COPY_SUBROUTINE_INCLUDE
Engine_Memory_Copy:
ldh [cENGINE_MEMORY_HRAM_START], a
Engine_Memory_Copy_Loop:
ld a, [de]
; Offset
push bc
ld b, a
ldh a, [cENGINE_MEMORY_HRAM_START]
add a, b
pop bc
;
ld [hli], a
inc de
dec bc
ld a, b
or a, c
jr nz, Engine_Memory_Copy_Loop
ret
ENDM
