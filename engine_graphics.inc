; DMG register constants
DEF rBGP                                  EQU $FF47 ; Background Palette (DMG)

; CGB register constants
DEF rBCPS                                 EQU $FF68 ; Background Color Palette Specification (CGB)
DEF rBCPD                                 EQU $FF69 ; Background Color Palette Data (CGB)

; Memory constants
DEF vENGINE_GRAPHICS_TILESET0             EQU $8000 ; Start of Block 0: Standard Sprite/BG tiles (0-127): Gameplay (Sprites & Level)
DEF vENGINE_GRAPHICS_TILESET1             EQU $8800 ; Start of Block 1: Shared/Signed BG tiles (128-255): "Bridge"
DEF vENGINE_GRAPHICS_TILESET2             EQU $9000 ; Start of Block 2: BG-only tiles (256-383): UI & Fonts
DEF vENGINE_GRAPHICS_TILEMAP0             EQU $9800 ; Start of Background Map 0
DEF vENGINE_GRAPHICS_TILEMAP1             EQU $9C00 ; Start of Background Map 1

; RGB helper macro
; Define a full 4-color palette on one line
; Usage: ENGINE_GRAPHICS_PALLET_DATA R,G,B, R,G,B, R,G,B, R,G,B
MACRO ENGINE_GRAPHICS_PALLET_DATA
dw ((\1) | (\2 << 5) | (\3 << 10)) & $7FFF
dw ((\4) | (\5 << 5) | (\6 << 10)) & $7FFF
dw ((\7) | (\8 << 5) | (\9 << 10)) & $7FFF
dw ((\10) | (\11 << 5) | (\12 << 10)) & $7FFF
ENDM

; Load tileset
; Usage: ENGINE_GRAPHICS_TILESET_LOAD SourceStart, SoureEnd, Destination, MemoryCopyRoutine
MACRO ENGINE_GRAPHICS_TILESET_LOAD
ld hl, \3
ld de, \1
ld bc, \2 - \1
call \4
ENDM

; Load tilemap
; Usage: ENGINE_GRAPHICS_TILEMAP_LOAD SourceStart, SoureEnd, Destination, MemoryCopyRoutine
MACRO ENGINE_GRAPHICS_TILEMAP_LOAD
ld HL, \3
ld DE, \1 
ld BC, \2 - \1
call \4
ENDM

; Set background pallet: default
MACRO ENGINE_GRAPHICS_PALLET_DMG_DEFAULT
ld A, %11100100
ld [rBGP], A
ENDM

MACRO ENGINE_GRAPHICS_PALLET_CGB_SET
; We needd to still set dmg pallet or we will get blank screen
ENGINE_GRAPHICS_PALLET_DMG_DEFAULT
; Set pallet
ld hl, \1
call EngineGraphicsPalletCGBSet
ENDM

; Load a palette from a data pointer
; Input: HL = Address of the 8-byte palette data
; Uses: A, HL
MACRO ENGINE_GRAPHICS_PALLET_CGB_SET_ROUTINE
EngineGraphicsPalletCGBSet:
ld a, $80          ; Start at Index 0 with auto-increment
ldh [rBCPS], a
ld b, 8            ; A palette is 8 bytes (4 colors * 2 bytes)
.loop:
ld a, [hli]        ; Get byte from data
ldh [rBCPD], a     ; Write to palette RAM
dec b
jr nz, .loop
ret
ENDM