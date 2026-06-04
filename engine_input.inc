; ==============================================================================
; MODULE DESCRIPTION
; ==============================================================================
; Provides an interface for physical Joypad polling and 
; state management. This module handles hardware multiplexing, delta 
; calculation, and provides branching tools for game logic integration.

; ==============================================================================
; TODO
; ==============================================================================
; [ ] Implement optional buffering layer (iINPUT_BUFFER_UPDATE) 
;     for delayed button processing (e.g., jump forgiveness, combo input).
; [ ] Add support for standardized "hold-time" tracking on specific buttons.

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

; Register constants
DEF _rJOYP                                      EQU $FF00 ; Joypad Register (Multiplexer & Button State)

; Input Flags
DEF fENGINE_INPUT_CLEAR                         EQU %00110000 ; 1=OFF. Deselects both Action (Bit 5) and D-Pad (Bit 4)
DEF fENGINE_INPUT_SELECT_DPAD                   EQU %00100000 ; 0=ON. Selects Directional buttons by pulling Bit 4 low
DEF fENGINE_INPUT_SELECT_ACTION                 EQU %00010000 ; 0=ON. Selects Action buttons by pulling Bit 5 low
DEF fENGINE_INPUT_MASK_LOWER_NIBBLE             EQU %00001111 ; Isolates the physical button data by dropping the top 4 hardware bits, we can only read half controller at a time
DEF fENGINE_INPUT_STATE_UP                      EQU %00000000 ; State: Up = 0
DEF fENGINE_INPUT_STATE_DOWN                    EQU %00000001 ; State: Down = 1
DEF fENGINE_INPUT_STATE_PRESSED                 EQU %00000010 ; State: Pressed = 2
DEF fENGINE_INPUT_STATE_RELEASED                EQU %00000011 ; State: Released = 3
DEF fENGINE_INPUT_BUTTON_A                      EQU %00000001 ; Button: A
DEF fENGINE_INPUT_BUTTON_B                      EQU %00000010 ; Button: B
DEF fENGINE_INPUT_BUTTON_SELECT                 EQU %00000100 ; Button: Select
DEF fENGINE_INPUT_BUTTON_START                  EQU %00001000 ; Button: Start
DEF fENGINE_INPUT_BUTTON_RIGHT                  EQU %00010000 ; Button: Right
DEF fENGINE_INPUT_BUTTON_LEFT                   EQU %00100000 ; Button: Left
DEF fENGINE_INPUT_BUTTON_UP                     EQU %01000000 ; Button: Up
DEF fENGINE_INPUT_BUTTON_DOWN                   EQU %10000000 ; Button: Down

; sENGINE_INPUT_WRAM
; - Usage: sENGINE_INPUT_WRAM
; - Description: Declares the "engine_input_wram" section in WRAM0. This 
;   macro reserves the necessary memory addresses for the input system 
;   variables: current state, previous state, and the edge-triggered 
;   pressed/released states.
MACRO sENGINE_INPUT_WRAM
SECTION "engine_input_wram", WRAM0
wEngineInputCurrent::ds 1
wEngineInputPrevious::ds 1
wEngineInputPressed::ds 1
wEngineInputReleased::ds 1
ENDM

; _iENGINE_INPUT_DELAY [Private] 
; - Destroys: Registers A
; - Usage: _iENGINE_INPUT_DELAY
; - Description: Forces a brief CPU pause to allow the hardware Joypad lines to stabilize after switching the multiplexer, as physical signal capacitance prevents an instant, reliable reading.
; - The standard way to do this on the Game Boy is simply to read from [_rJOYP] into your accumulator (a) multiple times in a row, twice is usually the sweet spot
MACRO _iENGINE_INPUT_DELAY
ld a, [_rJOYP]
ld a, [_rJOYP]
ENDM

; iENGINE_INPUT_BUTTON_IF
; - Destroys: Registers A
; - Usage: iENGINE_INPUT_BUTTON_IF [wInputState], fButtonMask, .false
; - Usage Example: iENGINE_INPUT_BUTTON_IF [wEngineInputPressed], fENGINE_INPUT_BUTTON_A, .jump
; - Description: Checks if a specific button bit is set within the provided input state variable. 
;   Jumps to the specified label if the button is active.
MACRO iENGINE_INPUT_BUTTON_IF
ld a, \1
and \2
jr z, \3
ENDM

; iENGINE_INPUT_AXIS_IF
; - Destroys: Registers A
; - Usage: iENGINE_INPUT_AXIS_IF [wInputState], .up, .down, .left, .right, .false
; - Description: Evaluates a snapshot of the input state to determine which 
;   directional button is active. It uses a fixed priority order (Up -> Down -> 
;   Left -> Right). Upon finding a match, it performs a conditional jump to the 
;   corresponding provided label. If no directional button is active, it 
;   executes an unconditional jump to the final label (.false).
MACRO iENGINE_INPUT_AXIS_IF
; Up
ld a, \1
and fENGINE_INPUT_BUTTON_UP
jr nz, \2
; Down
ld a, \1
and fENGINE_INPUT_BUTTON_DOWN
jr nz, \3
; Left
ld a, \1
and fENGINE_INPUT_BUTTON_LEFT
jr nz, \4
; Right
ld a, \1
and fENGINE_INPUT_BUTTON_RIGHT
jr nz, \5
; False
jr \6
ENDM

; iENGINE_INPUT_BEGIN
; - Destroys: Registers A
; - Usage: iENGINE_INPUT_BEGIN
; - Description: Initializes input worker memory
MACRO iENGINE_INPUT_BEGIN
ld a, 0
ld [wEngineInputCurrent], a
ld [wEngineInputPrevious], a
ld [wEngineInputPressed], a
ld [wEngineInputReleased], a
ENDM

; iENGINE_INPUT_UPDATE
; - Destroys: Registers A, B, and C.
; - Usage: iENGINE_INPUT_UPDATE
; - Writes: wEngineInputCurrent, wEngineInputPrevious, wEngineInputPressed, and wEngineInputReleased
; - Description: Reads the physical joypad and constructs a unified 8-bit input state.
;   Because the hardware shares data lines, it must poll the D-Pad and Action buttons in two separate passes.
;   We must clear _rJOYP at the end of the operation to disconnect both input columns, 
;   which prevents ghost inputs, ensures consistent interrupt behavior, and is 
;   mandatory if the system utilizes the HALT instruction.
MACRO iENGINE_INPUT_UPDATE
; Copy current into previous
ld a, [wEngineInputCurrent]
ld [wEngineInputPrevious], a
; Select dpad, clear top 4 bit garbage, move to b and swap 00001111 to 11110000
ld a, fENGINE_INPUT_SELECT_DPAD
ld [_rJOYP], a
_iENGINE_INPUT_DELAY
and a, fENGINE_INPUT_MASK_LOWER_NIBBLE
ld b, a 
swap b
; Select action, clear top 4 bit garbage
ld a, fENGINE_INPUT_SELECT_ACTION
ld [_rJOYP], a
_iENGINE_INPUT_DELAY
and a, fENGINE_INPUT_MASK_LOWER_NIBBLE
; Merge dpad and action, normalize
or a, b 
cpl
; Current
ld [wEngineInputCurrent], a    
; Delta = (Previous ^ Current)
ld a, [wEngineInputCurrent]
ld b, a
ld a, [wEngineInputPrevious]
xor a, b
ld c, a
; Pressed = Delta & Current
ld a, [wEngineInputCurrent]
and a, c
ld [wEngineInputPressed], a
; Released = Delta & Previous
ld a, [wEngineInputPrevious]
and a, c
ld [wEngineInputReleased], a
; Clear
ld a, fENGINE_INPUT_CLEAR
ld [_rJOYP], a
ENDM

; iENGINE_INPUT_UPDATE_SUBROUTINE
; - Destroys: Registers A, B, and C.
; - Usage: call iEngine_Input_Update
; - Writes: wEngineInputCurrent, wEngineInputPrevious, wEngineInputPressed, and wEngineInputReleased
; - Description: Reads the physical joypad and constructs a unified 8-bit input state.
;   Because the hardware shares data lines, it must poll the D-Pad and Action buttons in two separate passes.
;   We must clear _rJOYP at the end of the operation to disconnect both input columns, 
;   which prevents ghost inputs, ensures consistent interrupt behavior, and is 
;   mandatory if the system utilizes the HALT instruction.
MACRO iENGINE_INPUT_UPDATE_SUBROUTINE
Engine_Input_Update:
iENGINE_INPUT_UPDATE
ret
ENDM