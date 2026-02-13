; Set staack pointer
; Usage: ENGINE_SYSTEM_STACK_POINTER_SET         ; Defaults to $E000
; Usage: ENGINE_SYSTEM_STACK_POINTER_SET $D000   ; Manually set
MACRO ENGINE_MEMORY_STACK_POINTER_SET
IF _NARG > 0
ld sp, \1
ELSE
ld sp, $E000
ENDC
ENDM

MACRO ENGINE_MEMORY_COPY_ROUTINE
Engine_Memory_Copy:
ld a, [de]
ld [hli], a
inc de
dec bc
ld a, b
or c
jr nz, Engine_Memory_Copy
ret
ENDM
