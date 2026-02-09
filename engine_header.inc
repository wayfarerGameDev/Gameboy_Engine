; Header
; The Game Boy CPU begins execution at address $0100 after boot ROM finishes.
MACRO ENGINE_HEADER
SECTION "header", ROM0[$100]
jp Main
ds $150 -@, 0
ENDM