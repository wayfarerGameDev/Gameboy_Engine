; ==============================================================================
; MODULE DESCRIPTION
; ==============================================================================
; Provides core system initialization, ROM header configuration, and 
; hardware identification logic. This module manages system state and 
; provides branching tools for platform-specific (DMG/CGB) logic.

; ==============================================================================
; TODO
; ==============================================================================
; [ ] Implement full header padding including title and checksum calculations.
; [ ] Add interrupt vector initialization.

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

; sENGINE_SYSTEM_WRAM
; - Usage: sENGINE_SYSTEM_WRAM
; - Description: Declares the "engine_system_wram" section in WRAM0.
MACRO sENGINE_SYSTEM_WRAM
SECTION "engine_system_wram", WRAM0
wEngineSystemType:: ds 1
ENDM

; sENGINE_SYSTEM_HEADER_DMG
; - Usage: sENGINE_SYSTEM_HEADER_DMG [EntryLabel]
; - Description: Configures the DMG-only ROM header. Defaults to "Begin".
MACRO sENGINE_SYSTEM_HEADER_DMG
SECTION "header", ROM0[$100]
IF _NARG == 0
jp Begin
ELSE
jp \1
ENDC
; Set CGB Flag address to 0 to stay in DMG mode
ds $143 - @, 0
db $00
; Pad remaining header area ($144-$14F)
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
; - Destroys: Registers A
; - Usage: iENGINE_SYSTEM_TYPE_STORE
; - Description: Persists the system hardware type into WRAM.
MACRO iENGINE_SYSTEM_TYPE_STORE
ld [wEngineSystemType], a
ENDM

; iENGINE_SYSTEM_TYPE_IF_CGB
; - Destroys: Registers A
; - Usage: iENGINE_SYSTEM_TYPE_IF_CGB .label
; - Description: Conditional jump to label if system is NOT CGB hardware.
MACRO iENGINE_SYSTEM_TYPE_IF_CGB
ld a, [wEngineSystemType]
cp $11
jp nz, \1
ENDM

; iENGINE_SYSTEM_TYPE_IF_DMG
; - Destroys: Registers A
; - Usage: iENGINE_SYSTEM_TYPE_IF_DMG .label
; - Description: Conditional jump to label if system is NOT DMG hardware.
MACRO iENGINE_SYSTEM_TYPE_IF_DMG
ld a, [wEngineSystemType]
cp $01
jp nz, \1
ENDM