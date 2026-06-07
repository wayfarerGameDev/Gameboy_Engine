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

; System variables (WRAM)
MACRO sENGINE_SYSTEM_WRAM
SECTION "engine_system_wram", WRAM0
wEngineSystemType:: ds 1
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

; sENGINE_SYSTEM_HEADER_CGB
; - Usage: sENGINE_SYSTEM_HEADER_CGB [EntryLabel]
; - Description: Configures the CGB-only ROM header. Defaults to "Begin".
MACRO sENGINE_SYSTEM_HEADER_CGB
SECTION "header", ROM0[$100]
IF _NARG == 0
jp Begin
ELSE
jp \1
ENDC
; Set CGB Flag to C0, CGB Required (Will not run on DMG)
ds $143 - @, 0
db $C0
; Pad remaining header area ($144-$14F)
ds $150 - @, 0
ENDM

; sENGINE_SYSTEM_HEADER_DMG_CGB
; - Usage: sENGINE_SYSTEM_HEADER_DMG_CGB [EntryLabel]
; - Description: Configures the CGB-compatible ROM header. Defaults to "Begin".
MACRO sENGINE_SYSTEM_HEADER_DMG_CGB
SECTION "header", ROM0[$100]
IF _NARG == 0
jp Begin
ELSE
jp \1
ENDC
; Set CGB Flag to $80, CGB features allowed, DMG compatible
ds $143 - @, 0
db $80
; Pad remaining header area ($144-$14F)
ds $150 - @, 0
ENDM

; iENGINE_SYSTEM_TYPE_STORE
; - Usage: iENGINE_SYSTEM_TYPE_STORE
; - Registers: System type -> A
; - Description: Persists the system hardware type into WRAM.
MACRO iENGINE_SYSTEM_TYPE_STORE
ld [wEngineSystemType], a
ENDM

; iENGINE_SYSTEM_TYPE_IF_CGB
; - Usage: iENGINE_SYSTEM_TYPE_IF_CGB TargetLabel
; - Registers: System type -> A
; - Description: Conditional jump to label if system is NOT CGB hardware.
MACRO iENGINE_SYSTEM_TYPE_IF_CGB
ld a, [wEngineSystemType]
cp $11
jp nz, \1
ENDM

; iENGINE_SYSTEM_TYPE_IF_DMG
; - Usage: iENGINE_SYSTEM_TYPE_IF_DMG TargetLabel
; - Registers: System type -> A
; - Description: Conditional jump to label if system is NOT DMG hardware.
MACRO iENGINE_SYSTEM_TYPE_IF_DMG
ld a, [wEngineSystemType]
cp $01
jp nz, \1
ENDM