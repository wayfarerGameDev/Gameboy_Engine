; ==============================================================================
; HELLO WORLD DEMONSTRATION
; ==============================================================================
; A basic implementation showing engine initialization, palette swapping, 
; and input handling.

INCLUDE "engine_audio.inc"
INCLUDE "engine_graphics.inc"
INCLUDE "engine_input.inc"
INCLUDE "engine_lcd.inc"
INCLUDE "engine_memory.inc"
INCLUDE "engine_system.inc"

; Work ram
sENGINE_SYSTEM_WRAM
sENGINE_INPUT_WRAM
SECTION "game_wram", WRAM0
wScrollY::ds 1
wPaletteBackgroundIndex::ds 1

; Header
sENGINE_SYSTEM_HEADER_CGB

; Subroutines
iENGINE_GRAPHICS_PALETTE_BACKGROUND_CGB_SET_SUBROUTINE
iENGINE_GRAPHICS_PALETTE_OAM_CGB_SET_SUBROUTINE
iENGINE_MEMORY_COPY_SUBROUTINE

; Begin
Begin:
   ; Set stack pointer to top of WRAM
   iENGINE_MEMORY_STACK_POINTER_SET
   ; Store system type
   iENGINE_SYSTEM_TYPE_STORE
   ; Turn off audio to prevent "Screech of Death" when first loading up rom
   iENGINE_AUDIO_SHUTDOWN
   ; Begin input
   iENGINE_INPUT_BEGIN
   ; Wait for VBlank (vertical blanking interval)
   ; VRAM, OAM, and LCD control registers are unsafe to  
   ; modify while the LCD is actively drawing scanlines.
   iENGINE_LCD_VBLANK_WAIT
   ; Disable the LCD controller.
   ; This immediately halts rendering and allows unrestricted
   ; access to VRAM and OAM memory.
   ; Turning off outside of VBlank can permanently damage original gameboy hardware
   iENGINE_LCD_MODE fENGINE_LCD_MODE_BASE_OFF
   ; Load defafault palettes : Default to DMG and upgrade if CGB is supported
   iENGINE_GRAPHICS_PALETTE_DMG_DEFAULT
   iENGINE_SYSTEM_TYPE_IF_CGB .palette_end
   iENGINE_GRAPHICS_PALETTE_BACKGROUND_CGB_CLEAR
   iENGINE_GRAPHICS_PALETTE_BACKGROUND_CGB_SET Palette_Red
   iENGINE_GRAPHICS_PALETTE_OAM_CGB_CLEAR
   iENGINE_GRAPHICS_PALETTE_OAM_CGB_SET Palette_Blue, cENGINE_GRAPHICS_PALETTE_0_OFFSET
   iENGINE_GRAPHICS_PALETTE_OAM_CGB_SET Palette_Orange, cENGINE_GRAPHICS_PALETTE_1_OFFSET
   .palette_end
   ; Set tileset/map (Background)
   ; The tileset is loaded into the $8800 memory block (Signed Mode). 
   ; Since our raw tilemap data is 0-indexed, we must apply the signed 
   ; offset ($80) so the hardware correctly points to the tiles at 
   ; $8800 rather than looking for them at $9000.
   iENGINE_GRAPHICS_TILESET_LOAD dTilesetStart_Background_HelloWorld, dTilesetEnd_Background_HelloWorld, cENGINE_GRAPHICS_TILESET1, Engine_Memory_Copy
   iENGINE_GRAPHICS_TILEMAP_LOAD dTilemapStart_Background_HelloWorld, dTilemapEnd_Background_HelloWorld, cENGINE_GRAPHICS_TILEMAP0, Engine_Memory_Copy, cENGINE_GRAPHICS_TILEMAP_SIGNED_OFFSET
   ; Set tileset/map (Objects)
   iENGINE_GRAPHICS_TILESET_LOAD dTilesetStart_Objects_HelloWorld, dTilesetEnd_Objects_HelloWorld, cENGINE_GRAPHICS_TILESET0, Engine_Memory_Copy
   ; Turn the LCD controller back on
   ; At this point:
   ; - Palette is set
   ; - Tiles are in VRAM
   ; - Tilemap is configured
   iENGINE_LCD_MODE fENGINE_LCD_MODE_BASE_BG_SPRITE_SIGNED
   ; Turn on audio
   iENGINE_AUDIO_STARTUP
   ; Initalize game data
   ld a, 0
   ld [wScrollY], a
   ld [wPaletteBackgroundIndex], a

; Update
Update:

   
   ; Update game state
   ; --> Entity logic, physics, collision, and camera movement happen here.
   ; --> Construct your sprite data and write it to the Shadow OAM buffer in standard WRAM here.
   ; --> Do NOT write directly to the hardware OAM ($FE00) during this phase.
   iENGINE_INPUT_UPDATE
   iENGINE_MEMORY_ADD_HL wScrollY, -1

   iENGINE_LCD_VBLANK_WAIT
   ; --> Safe to write to palette in VRAM
   ; --> Safe to load new dynamic tiles into memory ($8000-$97FF) here.
   ; --> Safe to update Background/Window maps ($9800-$9FFF) here.
   ; --> Execute your hardware DMA transfer here to blast the Shadow OAM buffer into $FE00.
   ; --> Safe to push hardware register updates (like background scrolls) here.

   ; Select pallet
   iENGINE_INPUT_BUTTON_IF wEngineInputPressed, fENGINE_INPUT_BUTTON_SELECT, .input_none
   ; Set pallet index clamped 0-7 and get current palette adress offset, 8 bytes per palette
   ld a, [wPaletteBackgroundIndex]
   inc a
   cp a, 7
   jr c, .select_pallet_index_ok          
   xor a
   .select_pallet_index_ok
   ld [wPaletteBackgroundIndex], a
   add a, a             ; Multiply by 2
   add a, a             ; Multiply by 4
   add a, a             ; Multiply by 8
   ; Get pallet adress
   ld l, a              ; Put the 8-bit offset into L
   ld h, 0              ; Clear H
   ld bc, Palette_Red   ; Load the base address into BC
   add hl, bc           ; Add the offset to the base address
   ;
   ld a, $80            ; target pallet 0
   call Engine_Graphics_Palette_Background_CGB_Set
   .input_none:
   

   ; Write OAM / sprite updates
   ; We do not need and should not to do this each frame but its here for simplicity (Only write when object data should change)
   iENGINE_GRAPHICS_OAM_SINGLE_SET_FULL_DYNAMIC 0, 16, 6, 1
   iENGINE_GRAPHICS_OAM_SINGLE_SET_FULL_DYNAMIC 1, 40, 80, 5, fENGINE_GRAPHICS_OAM_X_FLIP | fENGINE_GRAPHICS_OAM_Y_FLIP
   iENGINE_GRAPHICS_OAM_SINGLE_SET_POSITION 1, 50, 90
   iENGINE_GRAPHICS_OAM_SINGLE_SET_TILE_INDEX 1, 3
   iENGINE_GRAPHICS_SCROLL_Y_SET wScrollY

   iENGINE_LCD_VBLANK_END
   jp Update

SECTION "data_game", ROM0

Palette_Red:
dENGINE_GRAPHICS_PALETTE_DATA 30,28,26, 26,16,16, 18,4,4, 0,0,0
Palette_Green:
dENGINE_GRAPHICS_PALETTE_DATA 30,28,26, 18,24,16, 6,14,6, 0,0,0
Palette_Blue:
dENGINE_GRAPHICS_PALETTE_DATA 30,28,26, 16,18,26, 4,6,18, 0,0,0
Palette_Yellow:
dENGINE_GRAPHICS_PALETTE_DATA 30,28,26, 28,24,10, 20,14,2, 0,0,0
Palette_Purple:
dENGINE_GRAPHICS_PALETTE_DATA 30,28,26, 22,16,24, 12,4,14, 0,0,0
Palette_Orange:
dENGINE_GRAPHICS_PALETTE_DATA 30,28,26, 28,18,8, 20,8,2, 0,0,0
Palette_Pink:
dENGINE_GRAPHICS_PALETTE_DATA 30,28,26, 28,18,22, 20,8,12, 0,0,0
 
; Source: https://gbdev.io/rgbds-live/
; Modified: To scroll on y properly
dTilemapStart_Background_HelloWorld:
db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $01, $02, $03, $01, $04, $03, $01, $05, $00, $01, $05, $00, $06, $04, $07, $00, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $08, $09, $0a, $0b, $0c, $0d, $0b, $0e, $0f, $08, $0e, $0f, $10, $11, $12, $13, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $14, $15, $16, $17, $18, $19, $1a, $1b, $0f, $14, $1b, $0f, $14, $1c, $16, $1d, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $1e, $1f, $20, $21, $22, $23, $24, $22, $25, $1e, $22, $25, $26, $22, $27, $1d, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $01, $28, $29, $2a, $2b, $2c, $2d, $2b, $2e, $2d, $2f, $30, $2d, $31, $32, $33, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $08, $34, $0a, $0b, $11, $0a, $0b, $35, $36, $0b, $0e, $0f, $08, $37, $0a, $38, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $14, $39, $16, $17, $1c, $16, $17, $3a, $3b, $17, $1b, $0f, $14, $3c, $16, $1d, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $1e, $3d, $3e, $3f, $22, $27, $21, $1f, $20, $21, $22, $25, $1e, $22, $40, $1d, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $00, $41, $42, $43, $44, $30, $33, $41, $45, $43, $41, $30, $43, $41, $30, $33, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $01, $02, $03, $01, $04, $03, $01, $05, $00, $01, $05, $00, $06, $04, $07, $00, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $08, $09, $0a, $0b, $0c, $0d, $0b, $0e, $0f, $08, $0e, $0f, $10, $11, $12, $13, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $14, $15, $16, $17, $18, $19, $1a, $1b, $0f, $14, $1b, $0f, $14, $1c, $16, $1d, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $1e, $1f, $20, $21, $22, $23, $24, $22, $25, $1e, $22, $25, $26, $22, $27, $1d, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $01, $28, $29, $2a, $2b, $2c, $2d, $2b, $2e, $2d, $2f, $30, $2d, $31, $32, $33, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $08, $34, $0a, $0b, $11, $0a, $0b, $35, $36, $0b, $0e, $0f, $08, $37, $0a, $38, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $14, $39, $16, $17, $1c, $16, $17, $3a, $3b, $17, $1b, $0f, $14, $3c, $16, $1d, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $1e, $3d, $3e, $3f, $22, $27, $21, $1f, $20, $21, $22, $25, $1e, $22, $40, $1d, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $00, $41, $42, $43, $44, $30, $33, $41, $45, $43, $41, $30, $43, $41, $30, $33, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00,  0,0,0,0,0,0,0,0,0,0,0,0
dTilemapEnd_Background_HelloWorld:

; Source: https://gbdev.io/rgbds-live/
dTilesetStart_Background_HelloWorld:
dw `22222222, `22222222, `22222222, `22222222, `22222222, `22222222, `22222222, `22222222 ; $00
dw `22222222, `20000000, `20000000, `20000000, `20000000, `20000000, `20000000, `20000000 ; $01
dw `22222222, `02222220, `02222220, `02222220, `02222220, `02222220, `02222220, `02222220 ; $02
dw `22222222, `00000002, `00000002, `00000002, `00000002, `00000002, `00000002, `00000002 ; $03
dw `22222222, `00000000, `00000000, `00000000, `00000000, `00000000, `00000000, `00000000 ; $04
dw `22222222, `02222222, `02222222, `02222222, `02222222, `02222222, `02222222, `02222222 ; $05
dw `22222222, `22222211, `22222000, `22220000, `22200000, `22100000, `22000000, `21000000 ; $06
dw `22222222, `11222222, `00022222, `00002222, `00000222, `00000122, `00000022, `00000012 ; $07
dw `20000000, `20000000, `21111111, `20000000, `20000000, `21111111, `21111111, `20000000 ; $08
dw `02222220, `02323230, `13232321, `02323230, `03232320, `11111111, `11111111, `00000000 ; $09
dw `00000002, `00000002, `11111113, `00000002, `00000003, `11111112, `11111113, `00000002 ; $0a
dw `20000000, `30000000, `21111111, `30000000, `20000000, `31111111, `21111111, `30000000 ; $0b
dw `02222222, `02323232, `13232323, `02323232, `03232323, `11111111, `11111111, `00000000 ; $0c
dw `22222222, `32323232, `23232323, `32323232, `23232323, `11111232, `11111323, `00000232 ; $0d
dw `02222222, `02323232, `13232323, `02323232, `03232323, `12323232, `13232323, `02323232 ; $0e
dw `22222222, `32222222, `22222222, `32222222, `22222222, `32222222, `22222222, `32222222 ; $0f
dw `21000000, `20000000, `21111111, `20000000, `20000000, `21111111, `21111111, `20000000 ; $10
dw `00222200, `02222230, `12222321, `02223230, `02232320, `12323231, `12232321, `02323230 ; $11
dw `00000012, `00000002, `11111113, `00000002, `00000003, `11111112, `11111113, `00000002 ; $12
dw `22222222, `32222222, `23232222, `32323222, `23232222, `32323222, `23232322, `32323222 ; $13
dw `21111111, `21111111, `21111111, `21111111, `21111111, `21111111, `21111111, `21111111 ; $14
dw `11111111, `11111111, `11111111, `12323231, `13232321, `12323231, `13232321, `12323231 ; $15
dw `11111113, `11111112, `11111113, `11111112, `11111113, `11111112, `11111113, `11111112 ; $16
dw `21111111, `31111111, `21111111, `31111111, `21111111, `31111111, `21111111, `31111111 ; $17
dw `11111111, `11111111, `11111111, `12323232, `13232323, `12323232, `13232323, `12323232 ; $18
dw `11111222, `11111222, `11111222, `32222222, `22222222, `32323232, `23232323, `32323232 ; $19
dw `21111111, `21111111, `21111111, `21111111, `21111111, `31111111, `21111111, `31111111 ; $1a
dw `13232323, `12323232, `13232323, `12323232, `13232323, `12323232, `13232323, `12323232 ; $1b
dw `13232321, `12323231, `13232321, `12323231, `13232321, `12323231, `13232321, `11323211 ; $1c
dw `23232322, `32323232, `23232322, `32323232, `23232322, `32323232, `23232322, `32323232 ; $1d
dw `21111111, `21111111, `21111111, `21111111, `21111111, `21111111, `21111111, `22222222 ; $1e
dw `13232321, `12323231, `13232321, `12323231, `13232321, `12323231, `13232321, `22323232 ; $1f
dw `11111113, `11111112, `11111113, `11111112, `11111113, `11111112, `11111113, `32222222 ; $20
dw `21111111, `31111111, `21111111, `31111111, `21111111, `31111111, `21111111, `32323232 ; $21
dw `11111111, `11111111, `11111111, `11111111, `11111111, `11111111, `11111111, `22323232 ; $22
dw `11111113, `11111112, `11111113, `11111112, `11111112, `11111112, `11111112, `32222222 ; $23
dw `21111111, `31111111, `21111111, `21111111, `21111111, `21111111, `21111111, `22222222 ; $24
dw `11111112, `11111112, `11111112, `11111112, `11111112, `11111112, `11111112, `32222222 ; $25
dw `22111111, `22111111, `22111111, `22211111, `22211111, `22221111, `22222211, `22222222 ; $26
dw `11111123, `11111122, `11111123, `11111222, `11111223, `11112222, `11222223, `32222232 ; $27
dw `23232323, `02323230, `03232320, `02323230, `03232320, `02323230, `03232320, `02222220 ; $28
dw `22222223, `00000002, `00000003, `00000002, `00000003, `00000002, `00000003, `00000002 ; $29
dw `23232322, `32323110, `23230000, `32300000, `23100000, `32000000, `21000000, `21000000 ; $2a
dw `23232323, `00000000, `00000000, `00000000, `00000000, `00000000, `00000000, `00000000 ; $2b
dw `23232323, `01123232, `00002323, `00000232, `00000323, `00000032, `00000013, `00000012 ; $2c
dw `23232322, `30000000, `20000000, `30000000, `20000000, `30000000, `20000000, `20000000 ; $2d
dw `23232323, `00123232, `00001323, `00000132, `00000023, `00000012, `00000003, `00000002 ; $2e
dw `23232323, `02323232, `03232323, `02323232, `03232323, `02323232, `03232323, `02222222 ; $2f
dw `23232323, `32323232, `23232323, `32323232, `23232323, `32323232, `23232323, `22222222 ; $30
dw `22232323, `00000000, `00000000, `00000000, `00000000, `00000000, `00000000, `00000000 ; $31
dw `23232323, `01123232, `00001323, `00000132, `00000023, `00000012, `00000013, `00000002 ; $32
dw `23232322, `32323222, `23232322, `32323222, `23232222, `32322222, `23222222, `22222222 ; $33
dw `02222220, `02323230, `13232321, `02323230, `03232320, `12321231, `13211321, `02300230 ; $34
dw `02222200, `02323230, `13232321, `02323230, `03232300, `11111111, `11111111, `00000000 ; $35
dw `00000002, `00000002, `11111113, `00000012, `00000113, `11111132, `11111323, `00003232 ; $36
dw `02222200, `02323230, `13232321, `02323230, `03232320, `12323231, `13232321, `02323230 ; $37
dw `22222222, `32322222, `23232222, `32323222, `23232322, `32323222, `23232322, `32323232 ; $38
dw `13211121, `12111131, `13111121, `12111111, `11111111, `11111111, `11112111, `11123111 ; $39
dw `11111111, `11111111, `11111111, `12323211, `13232321, `12323231, `13232321, `12323231 ; $3a
dw `11111223, `11111122, `11111123, `11111112, `11111113, `11111112, `11111113, `11111112 ; $3b
dw `13232321, `12323231, `13232321, `12323231, `13232321, `12323231, `13232321, `12323211 ; $3c
dw `11132111, `11123211, `11232311, `11323211, `11232321, `12323231, `13232321, `22323232 ; $3d
dw `11111113, `11111112, `11111113, `11111112, `11111113, `11111112, `11111113, `32322232 ; $3e
dw `23111111, `32111111, `23111111, `32311111, `23211111, `32321111, `23232311, `32323232 ; $3f
dw `11111113, `11111122, `11111123, `11111122, `11111223, `11112222, `11222223, `32222232 ; $40
dw `23232323, `22323232, `23232323, `22323232, `23232323, `22323232, `23232323, `22222222 ; $41
dw `23222323, `32322232, `23222223, `32222232, `23222223, `32222222, `22222223, `22222222 ; $42
dw `23232322, `32323232, `23232322, `32323232, `23232322, `32323232, `23232322, `22222222 ; $43
dw `22232323, `22323232, `22232323, `22223232, `22232323, `22223232, `22222223, `22222222 ; $44
dw `22222223, `32222222, `22222223, `32222222, `22222223, `32222222, `22222223, `22222222 ; $45
dTilesetEnd_Background_HelloWorld:

dTilesetStart_Objects_HelloWorld:
dw `00000000, `00000000, `00000000, `00000000, `00000000, `00000000, `00000000, `00000000 ; $00
dw `00222200, `02000020, `20200202, `20000002, `20200202, `20022002, `02000020, `00222200 ; $02
dw `20202020, `02020202, `20202020, `02020202, `20202020, `02020202, `20202020, `02020202 ; $02
dw `22222222, `20022002, `20022002, `22222222, `22222222, `20022002, `20022002, `22222222 ; $03
dw `00222200, `02222220, `22222222, `22222222, `22222222, `22222222, `02222220, `00222200 ; $04
dw `00022000, `00222200, `02200220, `22000022, `22000022, `02200220, `00222200, `00022000 ; $05
dTilesetEnd_Objects_HelloWorld: