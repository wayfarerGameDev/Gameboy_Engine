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
DEF _rBGP                                               EQU $FF47 ; Background Palette (DMG)

; CGB background palette register constants
DEF _rBCPS                                              EQU $FF68 ; Background Color Palette Specification (CGB)
DEF _rBCPD                                              EQU $FF69 ; Background Color Palette Data (CGB)

; CGB OAM palette register constants
DEF _rOCPS                                              EQU $FF6A ; Object Color Palette Specification (CGB)
DEF _rOCPD                                              EQU $FF6B ; Object Color Palette Data (CGB)

; CGB OAM palette register constants
DEF _rSCY                                               EQU $FF42 ; Y scroll register
DEF _rSCX                                               EQU $FF43 ; X scroll register

; DMG constants
DEF _cENGINE_GRAPHICS_PALETTE_DEFAULT                   EQU %11100100 ; EQU %11100100 ; Default 4-color palette mapping

; Constants
DEF cENGINE_GRAPHICS_TILESET0                           EQU $8000 ; Start of Block 0: Standard Sprite/BG tiles (0-127): Gameplay (Sprites & Level)
DEF cENGINE_GRAPHICS_TILESET1                           EQU $8800 ; Start of Block 1: Shared/Signed BG tiles (128-255): "Bridge"
DEF cENGINE_GRAPHICS_TILESET2                           EQU $9000 ; Start of Block 2: BG-only tiles (256-383): UI & Fonts
DEF cENGINE_GRAPHICS_TILEMAP0                           EQU $9800 ; Start of Background Map 0
DEF cENGINE_GRAPHICS_TILEMAP1                           EQU $9C00 ; Start of Background Map 1
DEF cENGINE_GRAPHICS_TILEMAP_SIGNED_OFFSET              EQU $80 ; Offset for 0-indexed maps when BG is in Signed mode (often used to reach shared tiles at $8800)
DEF cENGINE_GRAPHICS_OAM_START                          EQU $FE00 ; Start of Object Attribute Memory (OAM): Holds rendering data for up to 40 sprites (4 bytes per sprite)

; Flags
DEF fENGINE_GRAPHICS_OAM_PRIORITY_BG                    EQU %10000000 ; 1 = BG/Window over OBJ
DEF fENGINE_GRAPHICS_OAM_Y_FLIP                         EQU %01000000 ; 1 = Vertical Flip
DEF fENGINE_GRAPHICS_OAM_X_FLIP                         EQU %00100000 ; 1 = Horizontal Flip
DEF fENGINE_GRAPHICS_OAM_PALETTE_CGB                    EQU %00000111 ; CGB Palette flag
DEF fENGINE_GRAPHICS_OAM_PALETTE_DMG                    EQU %00010000 ; 1 = DMG Palette 1, 0 = DMG Palette 0
DEF fENGINE_GRAPHICS_OAM_PALETTE_TARGET_BACKGROUND      EQU %00000000 ; Background pallet target flag
DEF fENGINE_GRAPHICS_OAM_PALETTE_TARGET_OAM             EQU %00000001 ; OAM pallet target flag

; CGB Palette offset constants
DEF cENGINE_GRAPHICS_PALETTE_ADDRESS_0                  EQU $80 + 0
DEF cENGINE_GRAPHICS_PALETTE_ADDRESS_1                  EQU $80 + 8
DEF cENGINE_GRAPHICS_PALETTE_ADDRESS_2                  EQU $80 + 16
DEF cENGINE_GRAPHICS_PALETTE_ADDRESS_3                  EQU $80 + 24
DEF cENGINE_GRAPHICS_PALETTE_ADDRESS_4                  EQU $80 + 32
DEF cENGINE_GRAPHICS_PALETTE_ADDRESS_5                  EQU $80 + 40
DEF cENGINE_GRAPHICS_PALETTE_ADDRESS_6                  EQU $80 + 48
DEF cENGINE_GRAPHICS_PALETTE_ADDRESS_7                  EQU $80 + 56

; dENGINE_GRAPHICS_PALETTE_DATA
; - Usage: dENGINE_GRAPHICS_PALETTE_DATA R,G,B, R,G,B, R,G,B, R,G,B
; - Description: Define a full 4-color palette.
MACRO dENGINE_GRAPHICS_PALETTE_DATA
dw ((\1) | (\2 << 5) | (\3 << 10)) & $7FFF
dw ((\4) | (\5 << 5) | (\6 << 10)) & $7FFF
dw ((\7) | (\8 << 5) | (\9 << 10)) & $7FFF
dw ((\10) | (\11 << 5) | (\12 << 10)) & $7FFF
ENDM

; iENGINE_GRAPHICS_SCROLL_X_SET
; - Usage: iENGINE_GRAPHICS_SCROLL_X_SET Address
; - Registers: Adress -> A
; - Description: Sets the horizontal scroll register (_rSCX).
MACRO iENGINE_GRAPHICS_SCROLL_X_SET
ld a, [\1]
ld [_rSCX], a
ENDM

; iENGINE_GRAPHICS_SCROLL_Y_SET
; - Usage: iENGINE_GRAPHICS_SCROLL_Y_SET Address
; - Registers: Address -> A
; - Description: Sets the vertical scroll register (_rSCY).
MACRO iENGINE_GRAPHICS_SCROLL_Y_SET
ld a, [\1]
ld [_rSCY], a
ENDM

; iENGINE_GRAPHICS_PALETTE_DMG_DEFAULT
; - Usage: iENGINE_GRAPHICS_PALETTE_DMG_DEFAULT
; - Registers: Default palette value -> A
; - Description: Sets the default DMG background palette.
MACRO iENGINE_GRAPHICS_PALETTE_DMG_DEFAULT
ld a, _cENGINE_GRAPHICS_PALETTE_DEFAULT
ldh [_rBGP], a
ENDM

; iENGINE_GRAPHICS_PALETTE_OAM_CGB_CLEAR
; - Usage: iENGINE_GRAPHICS_PALETTE_BACKGROUND_CGB_CLEAR
; - Registers: Index/Auto-inc -> A, Count -> C
; - Description: Clears all CGB background palette data to zero.
MACRO iENGINE_GRAPHICS_PALETTE_BACKGROUND_CGB_CLEAR
ld a, $80          ; Start at Index 0 with auto-increment
ldh [_rBCPS], a
ld c, 64           ; Clear 64 bytes
xor a              ; A = 0
.loop\@:
ldh [_rBCPD], a
dec c
jr nz, .loop\@
ENDM

; iENGINE_GRAPHICS_PALETTE_CGB_SET_BACKGROUND_SUBROUTINE_INCLUDE
; - Usage: call Engine_Graphics_Palette_Background_CGB_Set
; - Registers: PaletteAddress -> A, PaletteAddress -> HL
; - Description: Adds "Engine_Graphics_Palette_Background_CGB_Set" subroutine
MACRO iENGINE_GRAPHICS_PALETTE_BACKGROUND_CGB_SET_SUBROUTINE_INCLUDE
Engine_Graphics_Palette_Background_CGB_Set:
ldh [_rBCPS], a
ld b, 8            ; A palette is 8 bytes (4 colors * 2 bytes)
.loop\@:
ld a, [hli]        ; Get byte from data
ldh [_rBCPD], a    ; Write to palette RAM
dec b
jr nz, .loop\@
ret
ENDM

; iENGINE_GRAPHICS_PALETTE_BACKGROUND_CGB_SET_SUBROUTINE_CALL
; - Usage: iENGINE_GRAPHICS_PALETTE_BACKGROUND_CGB_SET_SUBROUTINE_CALL PaletteAddress, SourceAddress, [SourceAddressOffset]
; - Registers: PaletteAddress -> A, SourceAddress -> HL
; - Description: Sets CGB background palette via calling "Engine_Graphics_Palette_Background_CGB_Set" subroutine.
MACRO iENGINE_GRAPHICS_PALETTE_BACKGROUND_CGB_SET_SUBROUTINE_CALL
; SourceAddress
ld hl, \2
; SourceAddress (offseted 16bit) a
IF _NARG == 3
IF STRCMP("\3", "a") == 0
ld e, a
ld d, 0
add hl, de
; SourceAddress (offseted 16bit) b
ELSE
ld a, [\3]
ld e, a
ld d, 0
add hl, de
ENDC
ENDC
; Palette address
ld a, \1
call Engine_Graphics_Palette_Background_CGB_Set
ENDM

; iENGINE_GRAPHICS_PALETTE_BACKGROUND_CGB_CLEAR
; - Usage: iENGINE_GRAPHICS_PALETTE_OAM_CGB_CLEAR
; - Registers: Index/Auto-inc -> A, Count -> C
; - Description: Clears all CGB OAM palette data to zero.
MACRO iENGINE_GRAPHICS_PALETTE_OAM_CGB_CLEAR
ld a, $80          ; Start at Index 0 with auto-increment
ldh [_rOCPS], a
ld c, 64           ; Clear 64 bytes
xor a              ; A = 0
.loop\@:
ldh [_rOCPD], a
dec c
jr nz, .loop\@
ENDM

; iENGINE_GRAPHICS_PALETTE_OAM_CGB_SET_SUBROUTINE_CALL
; - Usage: iENGINE_GRAPHICS_PALETTE_OAM_CGB_SET_SUBROUTINE_CALL PaletteAddress, SourceAddress, [SourceAddressOffset]
; - Registers: PaletteAddress -> A, SourceAddress -> HL
; - Description: Sets CGB OAM palette via calling "Engine_Graphics_Palette_Oam_CGB_Set" subroutine
MACRO iENGINE_GRAPHICS_PALETTE_OAM_CGB_SET_SUBROUTINE_CALL
; SourceAddress
ld hl, \2
; SourceAddress (offseted 16bit) a
IF _NARG == 3
IF STRCMP("\3", "a") == 0
ld e, a
ld d, 0
add hl, de
; SourceAddress (offseted 16bit) address
ELSE
ld a, [\3]
ld e, a
ld d, 0
add hl, de
ENDC
ENDC
; Palette adress
ld a, \1
call Engine_Graphics_Palette_Oam_CGB_Set
ENDM

; iENGINE_GRAPHICS_PALETTE_OAM_CGB_SET_SUBROUTINE
; - Usage: call Engine_Graphics_Palette_Oam_CGB_Set
; - Registers: PaletteAddress -> A, SourceAddress -> HL
; - Description: Subroutine to load palette data from HL to CGB oam palette RAM.
MACRO iENGINE_GRAPHICS_PALETTE_OAM_CGB_SET_SUBROUTINE_INCLUDE
Engine_Graphics_Palette_Oam_CGB_Set:
ldh [_rOCPS], a
ld b, 8            ; A palette is 8 bytes (4 colors * 2 bytes)
.loop\@:
ld a, [hli]        ; Get byte from data
ldh [_rOCPD], a    ; Write to palette RAM
dec b
jr nz, .loop\@
ret
ENDM

; iENGINE_GRAPHICS_OAM_SINGLE_SET_FULL_DYNAMIC
; - Usage: iENGINE_GRAPHICS_OAM_SINGLE_SET_FULL_DYNAMIC OamIndex, Y, X, [TileIndex], [AttributeFlags]
; - Registers: Memory destination -> HL, Y/X/Tile/Attr -> A
; - Description: Sets OAM data
MACRO iENGINE_GRAPHICS_OAM_SINGLE_SET_FULL_DYNAMIC
; Calculate memory destination
ld hl, cENGINE_GRAPHICS_OAM_START + (\1 * 4)
; Always write Y and X (Required)
ld a, \2
ld [hl+], a
ld a, \3
ld [hl+], a
; Only write Tile Index if provided
IF _NARG >= 4
ld a, \4
ld [hl+], a
ELSE
inc hl ; Skip tile index byte
ENDC
; Only write Attributes if provided
IF _NARG >= 5
ld a, \5
ld [hl], a
ELSE
xor a  ; Default to 0 if not provided
ld [hl], a
ENDC
ENDM

; iENGINE_GRAPHICS_OAM_SINGLE_SET_POSITION
; - Usage: iENGINE_GRAPHICS_OAM_SINGLE_SET_POSITION OamIndex, Y, X
; - Registers: Memory destination -> HL, Y/X -> A
; - Description: Sets OAM data yx
MACRO iENGINE_GRAPHICS_OAM_SINGLE_SET_POSITION
; Calculate memory destination
ld hl, cENGINE_GRAPHICS_OAM_START + (\1 * 4)
ld a, \2
ld [hl+], a
ld a, \3
ld [hl+], a
ENDM

; iENGINE_GRAPHICS_OAM_SINGLE_SET_TILE_INDEX
; - Usage: iENGINE_GRAPHICS_OAM_SINGLE_SET_TILE_INDEX OamIndex, TileIndex
; - Registers: Memory destination -> HL, TileIndex -> A
; - Description: Sets OAM data tile index
MACRO iENGINE_GRAPHICS_OAM_SINGLE_SET_TILE_INDEX
ld hl, cENGINE_GRAPHICS_OAM_START + (\1 * 4) + 2
ld a, \2
ld [hl], a
ENDM

; iENGINE_GRAPHICS_OAM_SINGLE_SET_ATTRIBUTES
; - Usage: iENGINE_GRAPHICS_OAM_SINGLE_SET_ATTRIBUTES OamIndex, AttributeFlags
; - Registers: Memory destination -> HL, AttributeFlags -> A
; - Description: Sets OAM data attribute flags
MACRO iENGINE_GRAPHICS_OAM_SINGLE_SET_ATTRIBUTES
ld hl, cENGINE_GRAPHICS_OAM_START + (\1 * 4) + 3
ld a, \2
ld [hl], a
ENDM