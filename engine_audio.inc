; ==============================================================================
; MODULE DESCRIPTION
; ==============================================================================

; ==============================================================================
; TODO
; ==============================================================================
; [ ] ALOT lol.

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
DEF _rNR10                                      EQU $FF10 ; CH1 Sweep
DEF _rNR11                                      EQU $FF11 ; CH1 Sound Length/Wave Pattern Duty
DEF _rNR12                                      EQU $FF12 ; CH1 Envelope
DEF _rNR13                                      EQU $FF13 ; CH1 Frequency Lo
DEF _rNR14                                      EQU $FF14 ; CH1 Frequency Hi
DEF _rNR50                                      EQU $FF24 ; Master Volume/Panning
DEF _rNR51                                      EQU $FF25 ; Sound Pan Enable
DEF _rNR52                                      EQU $FF26 ; Sound On/Off

; iENGINE_AUDIO_STARTUP
; - Destroys: Registers A
; - Usage: iENGINE_AUDIO_STARTUP
; - Description: Powers on the APU (Bit 7). Required before any sound registers can be set.
MACRO iENGINE_AUDIO_STARTUP
ld a, %10000000
ldh [_rNR52], a
ENDM
 
; iENGINE_AUDIO_SHUTDOWN
; - Destroys: Registers A
; - Usage: iENGINE_AUDIO_SHUTDOWN
; - Description: Powers off the APU (Clears all bits). Prevents audio artifacts during boot.
MACRO iENGINE_AUDIO_SHUTDOWN
xor a
ldh [_rNR52], a
ENDM