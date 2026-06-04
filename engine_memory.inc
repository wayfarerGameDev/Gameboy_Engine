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

; iENGINE_MEMORY_STACK_POINTER_SET
; - Destroys: Registers SP
; - Usage: iENGINE_MEMORY_STACK_POINTER_SET [$Addr]
; - Description: Sets the stack pointer. Defaults to cENGINE_MEMORY_STACK_TOP.
MACRO iENGINE_MEMORY_STACK_POINTER_SET
IF _NARG > 0
ld sp, \1
ELSE
ld sp, cENGINE_MEMORY_STACK_TOP
ENDC
ENDM

; iENGINE_MEMORY_COPY_SUBROUTINE
; - Destroys: Registers A, B, C, D, E, and Flags
; - Usage: call Engine_Memory_Copy
; - Description: Copies a block of memory from DE to HL. 
;   Requires length in BC.
MACRO iENGINE_MEMORY_COPY_SUBROUTINE
Engine_Memory_Copy:
ld a, [de]
ld [hli], a
inc de
dec bc
ld a, b
or c
jr nz, Engine_Memory_Copy
ret
ENDM
