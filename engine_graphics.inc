; Load tileset
; Usage: ENGINE_GRAPHICS_TILESET_LOAD StartLabel, EndLabel
MACRO ENGINE_GRAPHICS_TILESET_LOAD
ld hl, $8000
ld de, \1
ld bc, \2 - \1
ENDM

; Load tilemap
; Usage: ENGINE_GRAPHICS_TILEMAP_LOAD StartLabel, EndLabel
MACRO ENGINE_GRAPHICS_TILEMAP_LOAD
ld HL, $9800
ld DE, \1 
ld BC, \2 - \1
ENDM