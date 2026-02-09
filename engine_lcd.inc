; Wait for vblank
; Loops until the LCD enters the vertical blanking period.
MACRO ENGINE_LCD_VBLANK_WAIT
.vBlankWait\@
ldh a, [$FF44]
cp 144
jr c, .vBlankWait\@
ENDM

; Wait for vblank
; Loops until the LCD exits the vertical blanking period.
MACRO ENGINE_LCD_VBLANK_END
.vBlankEnd\@
ldh a, [$FF44]
cp 144
jr nc, .vBlankEnd\@
ENDM

; Turn LCD off
MACRO ENGINE_LCD_OFF
xor a
ldh [$FF40], a
ENDM

; Turn LCD on
MACRO ENGINE_LCD_ON
ld A, %10010001
ld [$FF40], A
ENDM

; Set background pallet (default)
MACRO ENGINE_LCD_PALLET_DEFAULT
ld A, %11100100
ld [$FF47], A
ENDM