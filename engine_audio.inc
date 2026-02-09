 ; Startup audio
MACRO ENGINE_AUDIO_STARTUP
ld a, %10000000
ldh [$FF26], a
ENDM
 
; Shutdown audio
MACRO ENGINE_AUDIO_SHUTDOWN
xor a
ldh [$FF26], a
ENDM