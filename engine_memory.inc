MACRO ENGINE_MEMORY_COPY
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
