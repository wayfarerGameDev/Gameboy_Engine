; ==============================================================================
; MODULE DESCRIPTION
; ==============================================================================

; ==============================================================================
; TODO
; ==============================================================================

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

; Bool constants
DEF cMATH_FALSE                                   EQU 0
DEF cMATH_TRUE                                    EQU 1

; Bound mode constants
DEF cMATH_BOUND_MODE_WRAP                         EQU 0
DEF cMATH_BOUND_MODE_CLAMP                        EQU 1

; iENGINE_MATH_ADD_WRAM
; - Usage: iENGINE_MATH_ADD_WRAM address, value, load_flag, store_flag
; - Registers: Address value -> A
; - Description: Adds a value to the contents of a WRAM address.
MACRO iENGINE_MATH_ADD_WRAM
; load
IF \3
ld a, [\1]
ENDC
; do
add a, \2
; store
IF \4
ld [\1], a
ENDC
ENDM

; iENGINE_MATH_ADD_HRAM
; - Usage: iENGINE_MATH_ADD_HRAM address, value, load_flag, store_flag
; - Registers: Address value -> A
; - Description: Adds a value to the contents of an HRAM address.
MACRO iENGINE_MATH_ADD_HRAM
; load
IF \3
ldh a, \1
ENDC
; do
add a, \2
; store
IF \4
ldh \1, a
ENDC
ENDM

; iENGINE_MATH_SUB_WRAM
; - Usage: iENGINE_MATH_SUB_WRAM address, value, load_flag, store_flag
; - Registers: Address value -> A
; - Description: Subtracts a value from the contents of a WRAM address.
MACRO iENGINE_MATH_SUB_WRAM
; load
IF \3
ld a, [\1]
ENDC
; do
sub a, \2
; store
IF \4
ld [\1], a
ENDC
ENDM

; iENGINE_MATH_SUB_HRAM
; - Usage: iENGINE_MATH_SUB_HRAM address, value, load_flag, store_flag
; - Registers: Address value -> A
; - Description: Subtracts a value from the contents of an HRAM address.
MACRO iENGINE_MATH_SUB_HRAM
; load
IF \3
ldh a, \1
ENDC
; do
sub a, \2
; store
IF \4
ldh \1, a
ENDC
ENDM

; iENGINE_MATH_INC_WRAM
; - Usage: iENGINE_MATH_INC_WRAM address, load_flag, store_flag
; - Registers: Address value -> A
; - Description: Increments the contents of a WRAM address.
MACRO iENGINE_MATH_INC_WRAM
; load
IF \2
ld a, [\1]
ENDC
; do
inc a
; store
IF \3
ld [\1], a
ENDC
ENDM

; iENGINE_MATH_INC_HRAM
; - Usage: iENGINE_MATH_INC_HRAM address, load_flag, store_flag
; - Registers: Address value -> A
; - Description: Increments the contents of an HRAM address.
MACRO iENGINE_MATH_INC_HRAM
; load
IF \2
ldh a, \1
ENDC
; do
inc a
; store
IF \3
ldh \1, a
ENDC
ENDM

; iENGINE_MATH_DEC_WRAM
; - Usage: iENGINE_MATH_DEC_WRAM address, load_flag, store_flag
; - Registers: Address value -> A
; - Description: Decrements the contents of a WRAM address.
MACRO iENGINE_MATH_DEC_WRAM
; load
IF \2
ld a, [\1]
ENDC
; do
dec a
; store
IF \3
ld [\1], a
ENDC
ENDM

; iENGINE_MATH_DEC_HRAM
; - Usage: iENGINE_MATH_DEC_HRAM address, load_flag, store_flag
; - Registers: Address value -> A
; - Description: Decrements the contents of an HRAM address.
MACRO iENGINE_MATH_DEC_HRAM
; load
IF \2
ldh a, \1
ENDC
; do
dec a
; store
IF \3
ldh \1, a
ENDC
ENDM

; iENGINE_MATH_BOUND_MAX_WRAM
; - Usage: iENGINE_MATH_BOUND_MAX_WRAM address, min, max, mode, load_flag, store_flag
; - Registers: Address value -> A
; - Description: Bounds value (wraps or clamps based on mode) if > max + 1.
;   * Note on Implementation Logic: To save CPU cycles, the checks evaluate 
;     the impossible boundaries (max + 1). If the value is 
;     safe, the comparison requires a borrow, sets the Carry flag, 
;     and safely jumps OVER the fix.
MACRO iENGINE_MATH_BOUND_MAX_WRAM
; load
IF \5
ld a, [\1]
ENDC
; max
cp a, \3 + 1
jr c, .end\@
; wrap
IF \4 == cMATH_BOUND_MODE_WRAP
ld a, \2
; clamp
ELIF \4 == cMATH_BOUND_MODE_CLAMP
ld a, \3
ENDC
; store
.end\@:
IF \6
ld [\1], a
ENDC
ENDM

; iENGINE_MATH_BOUND_MIN_WRAM
; - Usage: iENGINE_MATH_BOUND_MIN_WRAM address, min, max, mode, load_flag, store_flag
; - Registers: Address value -> A
; - Description: Bounds value (wraps or clamps based on mode) if < min - 1.
;   * Note on Implementation Logic: To save CPU cycles, the min check evaluates 
;     (min - 1) to force an underflow. If the value is safe, the comparison 
;     requires a borrow, sets the Carry flag, and safely jumps OVER the fix.
MACRO iENGINE_MATH_BOUND_MIN_WRAM
; load
IF \5
ld a, [\1]
ENDC
; min
cp a, \2 - 1
jr c, .end\@
; wrap
IF \4 == cMATH_BOUND_MODE_WRAP
ld a, \3
; clamp
ELIF \4 == cMATH_BOUND_MODE_CLAMP
ld a, \2
ENDC
; store
.end\@:
IF \6
ld [\1], a
ENDC
ENDM

; iENGINE_MATH_BOUND_MIN_MAX_WRAM
; - Usage: iENGINE_MATH_BOUND_MIN_MAX_WRAM address, min, max, mode, load_flag, store_flag
; - Registers: Address value -> A
; - Description: Bounds value (wraps or clamps based on mode) within min - 1 and max - 1
;   * Note on Implementation Logic: To save CPU cycles, the checks evaluate 
;     the impossible boundaries (min - 1) and (max + 1). If the value is 
;     safe, the comparison requires a borrow, sets the Carry flag, 
;     and safely jumps OVER the fix.
MACRO iENGINE_MATH_BOUND_MIN_MAX_WRAM
; load
IF \5
ld a, [\1]
ENDC
; min
cp a, \2 - 1
jr c, .max\@
; wrap
IF \4 == cMATH_BOUND_MODE_WRAP
ld a, \3
; clamp
ELIF \4 == cMATH_BOUND_MODE_CLAMP
ld a, \2
ENDC
; max
.max\@:
cp a, \3 + 1
jr c, .end\@
; wrap
IF \4 == cMATH_BOUND_MODE_WRAP
ld a, \2
; clamp
ELIF \4 == cMATH_BOUND_MODE_CLAMP
ld a, \3
ENDC
; store
.end\@:
IF \6
ld [\1], a
ENDC
ENDM
 
; iENGINE_MATH_SHIFT_LEFT_WRAM
; - Usage: iENGINE_MATH_SHIFT_LEFT_WRAM times, [store_address]
; - Registers: A(times) -> A
; - Description: Shifts A to the left for the specified number of times.
MACRO iENGINE_MATH_SHIFT_LEFT_WRAM
; load
REPT \1
add a, a
; store
ENDR
IF _NARG == 2
ld [\2], a
ENDC
ENDM

; iENGINE_MATH_SHIFT_RIGHT_WRAM
; - Usage: iENGINE_MATH_SHIFT_RIGHT_WRAM times, [store_address]
; - Registers: A(times) -> A
; - Description: Shifts A to the right for the specified number of times.
MACRO iENGINE_MATH_SHIFT_RIGHT_WRAM
; load
REPT \1
srl a
; store
ENDR
IF _NARG == 2
ld [\2], a
ENDC
ENDM