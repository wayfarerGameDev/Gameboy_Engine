; ==============================================================================
; MODULE DESCRIPTION
; ==============================================================================
; Provides an interface for LCD controller management. This module handles 
; display timing synchronization (VBlank) and configuration of the LCD 
; control registers.

; ==============================================================================
; TODO
; ==============================================================================
; [] Add support for window layer positioning registers.

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
DEF _rLCDC                                     EQU $FF40 ; LCD Control (The Master Switchboard)
DEF _rLY                                       EQU $FF44 ; LCD Y-Coordinate (The current scanline 0-153)

; LCD control mode Flags
DEF fENGINE_LCD_MODE_BASE_OFF                  EQU %00000000 ; Shutdown LCD
DEF fENGINE_LCD_MODE_BASE_BG_SIGNED            EQU %10000001 ; Signed Mode: Use for UI protection/Large maps
DEF fENGINE_LCD_MODE_BASE_BG_UNSIGNED          EQU %10010001 ; Unsigned Mode: Use for sharing tiles with Sprites
DEF fENGINE_LCD_MODE_BASE_BG_SPRITE_SIGNED     EQU %10000011 ; BG at $8800, Sprites ON
DEF fENGINE_LCD_MODE_BASE_BG_SPRITE_UNSIGNED   EQU %10010011 ; BG at $8000, Sprites ON
DEF fENGINE_LCD_MODE_MODIFIER_MAP1             EQU %00001000 ; Bit 3 controls current tilemap, turn it on for map 1

; Constants
DEF cENGINE_LCD_VBLANK                         EQU 144  ; V-Blank begins at scanline 144

; iENGINE_LCD_VBLANK_WAIT
; - Usage: iENGINE_LCD_VBLANK_WAIT
; - Registers: _rLY -> A
; - Description: Loops until the LCD enters the vertical blanking period.
MACRO iENGINE_LCD_VBLANK_WAIT
.vBlankWait\@:
ldh a, [_rLY]
cp cENGINE_LCD_VBLANK
jr c, .vBlankWait\@
ENDM

; iENGINE_LCD_VBLANK_END
; - Usage: iENGINE_LCD_VBLANK_END
; - Registers: _rLY -> A
; - Description: Loops until the LCD exits the vertical blanking period.
MACRO iENGINE_LCD_VBLANK_END
.vBlankEnd\@:
ldh a, [_rLY]
cp cENGINE_LCD_VBLANK
jr nc, .vBlankEnd\@
ENDM

; iENGINE_LCD_MODE
; - Usage: iENGINE_LCD_MODE ModeFlag
; - Registers: ModeFlag -> A
; - Description: Updates the LCD control register to the specified display mode.
MACRO iENGINE_LCD_MODE
ld a, \1
ldh [_rLCDC], a
ENDM