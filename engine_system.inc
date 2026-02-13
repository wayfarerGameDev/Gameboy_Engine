; System variables (WRAM)
MACRO ENGINE_SYSTEM_WRAM
SECTION "engine_system_wram", WRAM0[$C000]
wEngineSystemType:
ds 1
ENDM

; Header
; Does not check hardware, DMG is set by default
; The Game Boy CPU begins execution at address $0100 after boot ROM finishes.
; Usage: ENGINE_HEADER Label
MACRO ENGINE_SYSTEM_HEADER_DMG
SECTION "header", ROM0[$100]
jp \1
; Set CGB Flag adress to 0 to stay in DMG mode
ds $143 - @, 0
db $00
; Pad remaining header area ($144-$14F) to reach the start of code at $150
ds $150 - @, 0
ENDM

; Header
; Forces Game Boy Color Only
; Usage: ENGINE_HEADER_CGB_ONLY Label
MACRO ENGINE_SYSTEM_HEADER_CGB
SECTION "header", ROM0[$100]
jp \1
; Set CGB Flag to C0, CGB Required (Will not run on DMG)
ds $143 - @, 0
db $C0
; Pad remaining header area ($144-$14F) to reach the start of code at $150
ds $150 - @, 0
ENDM

; Header
; Checks hardware, can be DMG or CGB
; The Game Boy CPU begins execution at address $0100 after boot ROM finishes.
; Usage: ENGINE_HEADER Label
MACRO ENGINE_SYSTEM_HEADER_DMG_CGB
SECTION "header", ROM0[$100]
jp \1
; Set CGB Flag adress to $80 which allows CGB features but remains compatible with DMG
ds $143 - @, 0
db $80
; Pad remaining header area ($144-$14F) to reach the start of code at $150
ds $150 - @, 0
ENDM

; Type Check
MACRO ENGINE_SYSTEM_TYPE_STORE
ld [wEngineSystemType], a
ENDM

; Type cgb if
; Usage: ENGINE_SYSTEM_SKIP_IF_NOT_CGB .label
MACRO ENGINE_SYSTEM_TYPE_IF_CGB
ld a, [wEngineSystemType]
cp $11
jp nz, \1
ENDM

; Type dmg if
; Usage: ENGINE_SYSTEM_SKIP_IF_NOT_DMG .label
MACRO ENGINE_SYSTEM_TYPE_IF_DMG
ld a, [wEngineSystemType]
cp $01
jp nz, \1
ENDM