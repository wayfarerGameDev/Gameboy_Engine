; ==============================================================================
; MODULE DESCRIPTION
; ==============================================================================
; Provides an interface for graphics management, including tile loading, 
; tilemap management, and color palette configuration for both DMG and CGB 
; hardware.

; ==============================================================================
; TODO
; ==============================================================================
; [ ] Implement OAM (Sprite) shadow buffer system.
; [ ] Add scrolling support for Background and Window layers.

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

; DMG register constants
DEF _rBGP                                       EQU $FF47 ; Background Palette (DMG)

; CGB register constants
DEF _rBCPS                                      EQU $FF68 ; Background Color Palette Specification (CGB)
DEF _rBCPD                                      EQU $FF69 ; Background Color Palette Data (CGB)

; DMG constants
DEF _cENGINE_GRAPHICS_PALLET_DEFAULT            EQU %11100100 ; EQU %11100100 ; Default 4-color palette mapping

; Memory constants
DEF cENGINE_GRAPHICS_TILESET0                   EQU $8000 ; Start of Block 0: Standard Sprite/BG tiles (0-127): Gameplay (Sprites & Level)
DEF cENGINE_GRAPHICS_TILESET1                   EQU $8800 ; Start of Block 1: Shared/Signed BG tiles (128-255): "Bridge"
DEF cENGINE_GRAPHICS_TILESET2                   EQU $9000 ; Start of Block 2: BG-only tiles (256-383): UI & Fonts
DEF cENGINE_GRAPHICS_TILEMAP0                   EQU $9800 ; Start of Background Map 0
DEF cENGINE_GRAPHICS_TILEMAP1                   EQU $9C00 ; Start of Background Map 1
DEF cENGINE_GRAPHICS_TILEMAP_SIGNED_OFFSET      EQU $80 ; Offset for 0-indexed maps when BG is in Signed mode (often used to reach shared tiles at $8800)

; dENGINE_GRAPHICS_PALLET_DATA
; - Usage: dENGINE_GRAPHICS_PALLET_DATA R,G,B, R,G,B, R,G,B, R,G,B
; - Description: Define a full 4-color palette.
MACRO dENGINE_GRAPHICS_PALLET_DATA
dw ((\1) | (\2 << 5) | (\3 << 10)) & $7FFF
dw ((\4) | (\5 << 5) | (\6 << 10)) & $7FFF
dw ((\7) | (\8 << 5) | (\9 << 10)) & $7FFF
dw ((\10) | (\11 << 5) | (\12 << 10)) & $7FFF
ENDM

; iENGINE_GRAPHICS_TILESET_LOAD
; - Destroys: Registers A, C,  HL, DE, BC
; - Usage: iENGINE_GRAPHICS_TILESET_LOAD SourceStart, SourceEnd, Destination, MemoryCopyRoutine, MemoryCopyOffset[Optinal]
; - Description: Loads a tileset block into VRAM.
MACRO iENGINE_GRAPHICS_TILESET_LOAD
ld hl, \3
ld de, \1
ld bc, \2 - \1
IF _NARG == 5
ld a, \5
ELSE
ld a, 0
ENDC
call \4
ENDM

; iENGINE_GRAPHICS_TILEMAP_LOAD
; - Destroys: Registers A, HL, DE, BC
; - Usage: iENGINE_GRAPHICS_TILEMAP_LOAD SourceStart, SourceEnd, Destination, MemoryCopyRoutine, MemoryCopyOffset[Optinal]
; - Description: Loads a tilemap block into VRAM.
MACRO iENGINE_GRAPHICS_TILEMAP_LOAD
ld hl, \3
ld de, \1 
ld bc, \2 - \1
IF _NARG == 5
ld a, \5
ELSE
ld a, 0
ENDC
call \4
ENDM

; iENGINE_GRAPHICS_PALLET_DMG_DEFAULT
; - Destroys: Registers A
; - Usage: iENGINE_GRAPHICS_PALLET_DMG_DEFAULT
; - Description: Sets the default DMG background palette.
MACRO iENGINE_GRAPHICS_PALLET_DMG_DEFAULT
ld a, _cENGINE_GRAPHICS_PALLET_DEFAULT
ldh [_rBGP], a
ENDM

; iENGINE_GRAPHICS_PALLET_CGB_SET
; - Destroys: Registers A, HL
; - Usage: iENGINE_GRAPHICS_PALLET_CGB_SET DataPointer
; - Description: Sets CGB palette; also sets DMG default to avoid blank screen.
MACRO iENGINE_GRAPHICS_PALLET_CGB_SET
iENGINE_GRAPHICS_PALLET_DMG_DEFAULT
ld hl, \1
call Engine_Graphics_Pallet_CGB_Set
ENDM

; iENGINE_GRAPHICS_PALLET_CGB_SET_SUBROUTINE
; - Destroys: Registers A, B, HL, Flags
; - Usage: call Engine_Graphics_Pallet_CGB_Set
; - Description: Subroutine to load palette data from HL to CGB palette RAM.
MACRO iENGINE_GRAPHICS_PALLET_CGB_SET_SUBROUTINE
Engine_Graphics_Pallet_CGB_Set:
ld a, $80          ; Start at Index 0 with auto-increment
ldh [_rBCPS], a
ld b, 8            ; A palette is 8 bytes (4 colors * 2 bytes)
.loop:
ld a, [hli]        ; Get byte from data
ldh [_rBCPD], a     ; Write to palette RAM
dec b
jr nz, .loop
ret
ENDM