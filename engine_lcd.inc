; Register constants
DEF rLY                                   EQU $FF44 ; LCD Y-Coordinate (The current scanline 0-153)
DEF rLCDC                                 EQU $FF40 ; LCD Control (The Master Switchboard)

; LCD mode Flags
DEF fENGINE_LCD_MODE_OFF                   EQU %00000000 ; Shutdown LCD
DEF fENGINE_LCD_MODE_BG_SIGNED             EQU %10000001 ; Signed Mode: Use for UI protection/Large maps
DEF fENGINE_LCD_MODE_BG_UNSIGNED           EQU %10010001 ; Unsigned Mode: Use for sharing tiles with Sprites
DEF fENGINE_LCD_MODE_BG_SPRITE_SIGNED      EQU %10000011 ; BG at $8800, Sprites ON
DEF fENGINE_LCD_MODE_BG_SPRITE_UNSIGNED    EQU %10010011 ; BG at $8000, Sprites ON

; Wait for vblank
; Loops until the LCD enters the vertical blanking period.
MACRO ENGINE_LCD_VBLANK_WAIT
.vBlankWait\@
ldh a, [rLY]
cp 144
jr c, .vBlankWait\@
ENDM

; Wait for vblank
; Loops until the LCD exits the vertical blanking period.
MACRO ENGINE_LCD_VBLANK_END
.vBlankEnd\@
ldh a, [rLY]
cp 144
jr nc, .vBlankEnd\@
ENDM

; Turn LCD on
; Usage: ENGINE_LCD_ON ENGINE_LCD_MODE_SIGNED (Use $8800 for Backgrounds - Good for UI/Large Maps) : Bit 4 = 0: BG Tile Data at $8800-$97FF
; Usage: ENGINE_LCD_ON ENGINE_LCD_MODE_UNSIGNED (Use $8000 for Backgrounds - Good for Shared Sprite/BG tiles) :  ; Bit 4 = 1: BG Tile Data at $8000-$8FFF
MACRO ENGINE_LCD_MODE
ld A, \1
ldh [rLCDC], A
ENDM