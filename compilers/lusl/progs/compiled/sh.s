; libs/std.s
  jsr main

  ldt endmsg
  psh %dt
  jsr puts
  pop %ac

  ; exit code left on stack
  int $0
endmsg: bytes "Destroying Lusl runtime data...$^@"
; Lusl Standard Library
puti:
  pop %bp
  pop %ac
  cmp %ac 0
  jme .zer
  ldb 10000
  ldi 0
.loop:
  ldc %ac
  div %ac %bs
  div %ac 10
  cmp %dc %di
  jme .next
  add %dc $30
  ldi %dc ; so that cmp fails
  psh %dc
  int 2
.next:
  lda %cn
  cmp %bs 1
  jme .end
  div %bs 10
  jmp .loop
.zer:
  psh $30
  int 2
.end:
  psh 0
  psh %bp
  ret

puts:
  pop %bp
  pop %dt
.loop:
  cmp *%dt 0
  jme .end
  psh *%dt
  int 2
  inx %dt
  jmp .loop
.end:
  psh %dt
  psh %bp
  ret

gets:
  pop %bp
  pop %dt
  ldi %dt
.loop:
  int 1
  pop %ac
  cmp %ac $1B
  jme .esc
  cmp %ac 8
  jme .bs
  cmp %ac 127
  jme .bs
  psh %ac
  int 2
  cmp %ac 10
  jme .end
  storb %ac
  inx %dt
  jmp .loop
.esc:
  lda 94
  psh %ac
  int 2
  storb %ac
  inx %dt

  lda '['
  psh %ac
  int 2
  storb %ac
  inx %dt
jmp .loop
.bs:
  cmp %dt %di
  jme .loop
  lda 0
  storb %ac
  dex %dt
  lda *%dt
  div %ac 64
  cmp %ac 2
  jme .bs
  psh 8
  int $2
  psh 32
  int $2
  psh 8
  int $2
  jmp .loop
.end:
  lda 0
  storb %ac
  psh %dt
  psh %bp
  ret
exit:
  pop %bp
  int $0
  hlt

memcmp:
  pop %bp
  pop %dt
  pop %di
  pop %cn
  dex %cn
.loop:
  lda *%dt
  ldb *%di
  cmp %ac %bs
  jmne .fail
  inx %dt
  inx %di
  loop .loop
  psh 1
  psh %bp
  ret
.fail:
  psh 0
  psh %bp
  ret
scmp:
  pop %bp
  pop %dt
  pop %di
.loop:
  lda *%dt
  ldb *%di
  cmp %ac %bs
  jmne .fail
  cmp %bs 0
  jme .ok
  inx %dt
  inx %di
  jmp .loop
.fail:
  psh 0
  psh %bp
  ret
.ok:
  psh 1
  psh %bp
  ret

itoa:
    pop %bp
    pop %ac
    ldi itoa_buf
    add %di $04
.loop:
    div %ac 10
    add %dc 48
    ldt %di
    storb %dc
    dex %di
    cmp %ac $00
    jmne .loop
    psh 1
    psh %bp
    ret
itoa_buf: reserve 5 bytes
itoa_clr:
    ldc $4
    ldd $00
    ldt itoa_buf
.loop:
    storb %dc
    inx %dt
    loop .loop
ret

atoi:
  pop %bp
  pop %dt
  lda 0
.loop:
  ldb *%dt
  cmp %bs 0
  jme .end
  sub %bs $30
  mul %ac 10
  add %ac %bs
  inx %dt
  jmp .loop
.end:
  psh %ac
  psh %bp
  ret

putc:
  pop %bp
  int 2
  psh 0
  psh %bp
  ret

getch:
  pop %bp
  int 1
  psh %bp
ret
msleep:
  pop %bp
  pop %dc
  int $22
  psh 0
  psh %bp
  ret

rand:
  pop %bp
  int $21
  psh %dc
  psh %bp
ret

; libs/gdi/gui.s
vputs: ; (x,y,c,t)
    pop %bp
    pop %di
    pop %ac
    pop %dc
    pop %dt
    mul %dc 640
    add %dt %dc
    jsr vputs-
    psh %dc
    psh %bp
ret

vputs5: ; (x,y,c,t)
    pop %bp
    pop %di
    pop %ac
    pop %dc
    pop %dt
    mul %dc 640
    add %dt %dc
    jsr vputs5-
    psh %dc
    psh %bp
ret

vputs8: ; (x,y,c,t)
    pop %bp
    pop %di
    pop %ac
    pop %dc
    pop %dt
    mul %dc 640
    add %dt %dc
    jsr vputs8-
    psh %dc
    psh %bp
ret

vputs8t: ; (x,y,c,t)
    pop %bp
    pop %di
    pop %ac
    pop %dc
    pop %dt
    mul %dc 640
    add %dt %dc
    jsr vputs8t-
    psh %dc
    psh %bp
ret

vputs16: ; (x,y,c,t)
    pop %bp
    pop %di
    pop %ac
    pop %dc
    pop %dt
    mul %dc 640
    add %dt %dc
    jsr vputs16-
    psh %dc
    psh %bp
ret

vputsgui: ; (x,y,c,t)
    pop %bp
    pop %di
    pop %ac
    pop %dc
    pop %dt
    mul %dc 640
    add %dt %dc
    jsr vputsgui-
    psh %dc
    psh %bp
ret

vputsas10: ; (x,y,c,t)
    pop %bp
    pop %di
    pop %ac
    pop %dc
    pop %dt
    mul %dc 640
    add %dt %dc
    jsr vputsas10-
    psh %dc
    psh %bp
ret

vputsas16: ; (x,y,c,t)
    pop %bp
    pop %di
    pop %ac
    pop %dc
    pop %dt
    mul %dc 640
    add %dt %dc
    jsr vputsas16-
    psh %dc
    psh %bp
ret

cls: ; (c)
    pop %bp
    pop %ac
    jsr cls-
    psh 0
    psh %bp
ret

box: ; (x,y,w,h,c)
    pop %bp
    pop %ac
    pop %dc
    pop %bs
    pop %cn
    pop %dt
    mul %cn 640
    add %dt %cn
    jsr box-
    psh 0
    psh %bp
ret

vflush:
    pop %bp
    int $11
    psh 0
    psh %bp
ret

spr: ; (x, y, c, data)
    pop %bp
    pop %di
    pop %ac
    pop %dt
    pop %dc
    mul %dt 640
    add %dt %dc
    jsr spr-
    psh 0
    psh %bp
ret

load_pal:
    mov %dt Palitro
    mov %di $4A0000
    mov %cn 255
.l:
    lw %dt %dc
    sw %di %dc
    loop .l

    mov %dt $49FF00
    mov %dc $01
    sb %dt %dc

    pop %dc psh 0 psh %dc rts

gic: ; (x, y, c, data)
    pop %bp
    pop %di
    pop %ac
    pop %dt
    pop %dc
    mul %dt 640
    add %dt %dc
    jsr gic-
    psh 0
    psh %bp
ret

getx:
  pop %ac
  mov %dt $480000
  lw %dt %dt
  psh %dt
  psh %ac
  rts

gety:
  pop %ac
  mov %dt $480002
  lw %dt %dt
  psh %dt
  psh %ac
  rts

getm:
  pop %ac
  mov %dt $480004
  lw %dt %dt
  psh %dt
  psh %ac
  rts

getmousepos:
  mov %dt $480000
  lw %dt %dt
  mov %di $480002
  lw %di %di
  mul %di 640
  add %di %dt
  add %di $400000
  mov %dt %di
  rts

vputs-: ; G: char
    psh %dt
.line:
    psh %di
    lodgb
    cmp %di 0
    jme .end
    cmp %di 10
    jme .ln
    cmp %di 27
    jme .undr
    cmp %di $D0
    jme .gu8_d0
    cmp %di $D1
    jme .gu8_d1
.pchar:
    div %di 128
    add %di res.fontas12
    lodgb
    dex %di
    mul %di 4224
    add %di 513
    add %di res.fontas12
    mul %dc 33
    add %di %dc
    psh %bs
	jsr spr16-
	pop %bs
    sub %di 33
    ldd *%di
    sub %dt 10240
    add %dt %dc
    pop %di
    inx %di
jmp .line
.gu8_d0:
    pop %di
    inx %di
    psh %di
    lodgb
    add %di $380
jmp .pchar
.gu8_d1:
    pop %di
    inx %di
    psh %di
    lodgb
    add %di $3C0
jmp .pchar
.ln:
    pop %di
    pop %dt
    add %dt 5120
    inx %di
jmp vputs-
.undr:
    ldi res.underline
    jsr spr-
    sub %dt 5120
    pop %di
    inx %di
jmp .line
.end:
    pop %di
    ldd %dt
    pop %dt
ret

vputs5-: ; G: char
    psh %dt
.line:
    psh %di
    lodgb
    cmp %di 0
    jme .end
    cmp %di 10
    jme .ln
    cmp %di 27
    jme .undr
    cmp %di $D0
    jme .gu8_d0
    cmp %di $D1
    jme .gu8_d1
    sub %di $20
.pchar:
    mul %di 9
    ldd res.font
    add %di %dc
    jsr spr-
    ldd *%di
    sub %dt 5120
    ;add %dt %dc
    add %dt 6
    pop %di
    inx %di
jmp .line
.gu8_d0:
    pop %di
    inx %di
    psh %di
    lodgb
    sub %di $20
jmp .pchar
.gu8_d1:
    pop %di
    inx %di
    psh %di
    lodgb
    add %di $20 ; $40 - $20
jmp .pchar
.ln:
    pop %di
    pop %dt
    add %dt 5120
    inx %di
jmp vputs5-
.undr:
    ldi res.underline
    jsr spr-
    sub %dt 5120
    pop %di
    inx %di
jmp .line
.end:
    pop %di
    ldd %dt
    pop %dt
ret


vputs8-: ; G: char
    psh %dt
.line:
    psh %di
    lodgb
    cmp %di 0
    jme .end
    cmp %di 10
    jme .ln
    cmp %di 27
    jme .undr
    cmp %di $D0
    jme .gu8_d0
    cmp %di $D1
    jme .gu8_d1
.pchar:
    div %di 128
    add %di res.font8
    lodgb
    dex %di
    mul %di 4224
    add %di 513
    add %di res.font8
    mul %dc 33
    add %di %dc
    psh %bs
	jsr spr16-
	pop %bs
    sub %di 33
    ldd *%di
    sub %dt 10240
    add %dt %dc
    pop %di
    inx %di
jmp .line
.gu8_d0:
    pop %di
    inx %di
    psh %di
    lodgb
    add %di $380
jmp .pchar
.gu8_d1:
    pop %di
    inx %di
    psh %di
    lodgb
    add %di $3C0
jmp .pchar
.ln:
    pop %di
    pop %dt
    add %dt 5120
    inx %di
jmp vputs8-
.undr:
    ldi res.underline
    jsr spr-
    sub %dt 5120
    pop %di
    inx %di
jmp .line
.end:
    pop %di
    ldd %dt
    pop %dt
ret

vputs8t-: ; G: char
    psh %dt
.line:
    psh %di
    lodgb
    cmp %di 0
    jme .end
    cmp %di 10
    jme .ln
    cmp %di 27
    jme .undr
    cmp %di $D0
    jme .gu8_d0
    cmp %di $D1
    jme .gu8_d1
.pchar:
    div %di 128
    add %di res.font8t
    lodgb
    dex %di
    mul %di 4224
    add %di 513
    add %di res.font8t
    mul %dc 33
    add %di %dc
    psh %bs
	jsr spr16-
	pop %bs
    sub %di 33
    ldd *%di
    sub %dt 10240
    add %dt %dc
    pop %di
    inx %di
jmp .line
.gu8_d0:
    pop %di
    inx %di
    psh %di
    lodgb
    add %di $380
jmp .pchar
.gu8_d1:
    pop %di
    inx %di
    psh %di
    lodgb
    add %di $3C0
jmp .pchar
.ln:
    pop %di
    pop %dt
    add %dt 5120
    inx %di
jmp vputs8t-
.undr:
    ldi res.underline
    jsr spr-
    sub %dt 5120
    pop %di
    inx %di
jmp .line
.end:
    pop %di
    ldd %dt
    pop %dt
ret

vputs16-: ; G: char
    psh %dt
.line:
    psh %di
    lodgb
    cmp %di 0
    jme .end
    cmp %di 10
    jme .ln
    cmp %di 27
    jme .undr
    cmp %di $D0
    jme .gu8_d0
    cmp %di $D1
    jme .gu8_d1
.pchar:
    div %di 128
    add %di res.font16
    lodgb
    dex %di
    mul %di 4224
    add %di 513
    add %di res.font16
    mul %dc 33
    add %di %dc
    psh %bs
	jsr spr16-
	pop %bs
    sub %di 33
    ldd *%di
    sub %dt 10240
    add %dt %dc
    pop %di
    inx %di
jmp .line
.gu8_d0:
    pop %di
    inx %di
    psh %di
    lodgb
    add %di $380
jmp .pchar
.gu8_d1:
    pop %di
    inx %di
    psh %di
    lodgb
    add %di $3C0
jmp .pchar
.ln:
    pop %di
    pop %dt
    add %dt 5120
    inx %di
jmp vputs16-
.undr:
    ldi res.underline
    jsr spr-
    sub %dt 5120
    pop %di
    inx %di
jmp .line
.end:
    pop %di
    ldd %dt
    pop %dt
ret

vputsgui-: ; G: char
    psh %dt
.line:
    psh %di
    lodgb
    cmp %di 0
    jme .end
    cmp %di 10
    jme .ln
    cmp %di 27
    jme .undr
    cmp %di $D0
    jme .gu8_d0
    cmp %di $D1
    jme .gu8_d1
.pchar:
    div %di 128
    add %di res.fontgui
    lodgb
    dex %di
    mul %di 4224
    add %di 513
    add %di res.fontgui
    mul %dc 33
    add %di %dc
    psh %bs
	jsr spr16-
	pop %bs
    sub %di 33
    ldd *%di
    sub %dt 10240
    add %dt %dc
    pop %di
    inx %di
jmp .line
.gu8_d0:
    pop %di
    inx %di
    psh %di
    lodgb
    add %di $380
jmp .pchar
.gu8_d1:
    pop %di
    inx %di
    psh %di
    lodgb
    add %di $3C0
jmp .pchar
.ln:
    pop %di
    pop %dt
    add %dt 5120
    inx %di
jmp vputsgui-
.undr:
    ldi res.underline
    jsr spr-
    sub %dt 5120
    pop %di
    inx %di
jmp .line
.end:
    pop %di
    ldd %dt
    pop %dt
ret

vputsas10-: ; G: char
    psh %dt
.line:
    psh %di
    lodgb
    cmp %di 0
    jme .end
    cmp %di 10
    jme .ln
    cmp %di 27
    jme .undr
    cmp %di $D0
    jme .gu8_d0
    cmp %di $D1
    jme .gu8_d1
.pchar:
    div %di 128
    add %di res.fontas10
    lodgb
    dex %di
    mul %di 4224
    add %di 513
    add %di res.fontas10
    mul %dc 33
    add %di %dc
    psh %bs
	jsr spr16-
	pop %bs
    sub %di 33
    ldd *%di
    sub %dt 10240
    add %dt %dc
    pop %di
    inx %di
jmp .line
.gu8_d0:
    pop %di
    inx %di
    psh %di
    lodgb
    add %di $380
jmp .pchar
.gu8_d1:
    pop %di
    inx %di
    psh %di
    lodgb
    add %di $3C0
jmp .pchar
.ln:
    pop %di
    pop %dt
    add %dt 5120
    inx %di
jmp vputsas10-
.undr:
    ldi res.underline
    jsr spr-
    sub %dt 5120
    pop %di
    inx %di
jmp .line
.end:
    pop %di
    ldd %dt
    pop %dt
ret

vputsas16-: ; G: char
    psh %dt
.line:
    psh %di
    lodgb
    cmp %di 0
    jme .end
    cmp %di 10
    jme .ln
    cmp %di 27
    jme .undr
    cmp %di $D0
    jme .gu8_d0
    cmp %di $D1
    jme .gu8_d1
.pchar:
    div %di 128
    add %di res.fontas16
    lodgb
    dex %di
    mul %di 4224
    add %di 513
    add %di res.fontas16
    mul %dc 33
    add %di %dc
    psh %bs
	jsr spr16-
	pop %bs
    sub %di 33
    ldd *%di
    sub %dt 10240
    add %dt %dc
    pop %di
    inx %di
jmp .line
.gu8_d0:
    pop %di
    inx %di
    psh %di
    lodgb
    add %di $380
jmp .pchar
.gu8_d1:
    pop %di
    inx %di
    psh %di
    lodgb
    add %di $3C0
jmp .pchar
.ln:
    pop %di
    pop %dt
    add %dt 5120
    inx %di
jmp vputsas16-
.undr:
    ldi res.underline
    jsr spr-
    sub %dt 5120
    pop %di
    inx %di
jmp .line
.end:
    pop %di
    ldd %dt
    pop %dt
ret

; Gravno Display Interface 16
cls-: ; A: color
    mov %dt $450000
    sb %dt %ac
    int $12
ret
box-: ; A: color, B: width, D: height, S: start
    ldc %bs
    dex %cn
.pix:
    int $C
    inx %dt
    loop .pix
    sub %dt %bs
    add %dt 640
    dex %dc
    cmp %dc 0
    jmne box-
ret

spr-: ; A: color, G: sprite data
    ldb 8
.line:
    psh %di
    lodgb
    ldc 7
.pix:
    div %di 2
    cmp %dc 0
    jme .pix_next
    int $C
    ;inx %dt
    ;int $C
    ;dex %dt
.pix_next:
    inx %dt
    loop .pix

    pop %di
    inx %di

    add %dt 632

    dex %bs
    cmp %bs 0
    jmne .line
ret

spr16-: ; A: color, G: sprite data
    ldb 16
.line:
    psh %di
    ; lodgb
    lw %di %dc
    mov %di %dc
    ldc 15
.pix:
    div %di 2
    cmp %dc 0
    jme .pix_next
    int $C
    ;inx %dt
    ;int $C
    ;dex %dt
.pix_next:
    inx %dt
    loop .pix

    pop %di
    add %di 2

    add %dt 624

    dex %bs
    cmp %bs 0
    jmne .line
ret

gic-: ; G: sprite data, S: location
    lw %di %cn
    lw %di %bs
    mov %dc %ac

.line:
    psh %cn
    dex %cn
.pix:
    lda *%di
    cmp %ac %dc
    jme .np
    int $C
.np:
    inx %dt
    inx %di
    loop .pix
    pop %cn
    sub %dt %cn
    add %dt 640

    dex %bs
    cmp %bs 0
    jmne .line
ret

; libs/gdi/font.s
res:
.underline: bytes $00 $00 $00 $00 $00 $00 $00 $FF
.font: extern "libs/gdi/fonts/unscii5x7.gf1"
.fontas10: extern "libs/gdi/fonts/ArkSans10.gf2"
.fontas12: extern "libs/gdi/fonts/ArkSans12.gf2"
.fontas16: extern "libs/gdi/fonts/ArkSans16.gf2"
.fontgui: extern "libs/gdi/fonts/GovnUI.gf2"
.font16: extern "libs/gdi/fonts/Unscii16.gf2"
.font8: extern "libs/gdi/fonts/Unscii8.gf2"
.font8t: extern "libs/gdi/fonts/Unscii8Thin.gf2"

gweb2: extern "libs/gdi/icons/gweb2.gic"
gweb2_mini: extern "libs/gdi/icons/gweb2_mini.gic"
shutapp: extern "libs/gdi/icons/shutapp.gic"
shutapp_mini: extern "libs/gdi/icons/shutapp_mini.gic"
wall: extern "libs/gdi/icons/wall.gic"
g10: extern "libs/gdi/icons/g10.gic"
my_pc: extern "libs/gdi/icons/my_pc.gic"
max: extern "libs/gdi/icons/max.gic"
max_mini: extern "libs/gdi/icons/max_mini.gic"
cursor: extern "libs/gdi/icons/cursor.gic"

cursor:
  bytes $12 $12 $00 $00 $00 $00 $00 $00
  bytes $12 $0F $12 $00 $00 $00 $00 $00
  bytes $12 $0F $0F $12 $00 $00 $00 $00
  bytes $12 $0F $0F $0F $12 $00 $00 $00
  bytes $12 $0F $0F $0F $0F $12 $00 $00
  bytes $12 $0F $0F $0F $12 $12 $00 $00
  bytes $12 $0F $12 $0F $12 $00 $00 $00
  bytes $12 $12 $12 $0F $12 $00 $00 $00

Palitro: bytes $00 $00 $00 $50 $80 $02 $40 $51 $14 $00 $14 $50 $94 $02 $94 $52 $4A $29 $4A $7D $EA $2B $EA $7F $5F $29 $5F $7D $FF $2B $FF $7F $00 $00 $06 $00 $0C $00 $12 $00 $18 $00 $1F $00 $C0 $00 $C6 $00 $CC $00 $D2 $00 $D8 $00 $DF $00 $80 $01 $86 $01 $8C $01 $92 $01 $98 $01 $9F $01 $40 $02 $46 $02 $4C $02 $52 $02 $58 $02 $5F $02 $00 $03 $06 $03 $0C $03 $12 $03 $18 $03 $1F $03 $E0 $03 $E6 $03 $EC $03 $F2 $03 $F8 $03 $FF $03 $00 $18 $06 $18 $0C $18 $12 $18 $18 $18 $1F $18 $C0 $18 $C6 $18 $CC $18 $D2 $18 $D8 $18 $DF $18 $80 $19 $86 $19 $8C $19 $92 $19 $98 $19 $9F $19 $40 $1A $46 $1A $4C $1A $52 $1A $58 $1A $5F $1A $00 $1B $06 $1B $0C $1B $12 $1B $18 $1B $1F $1B $E0 $1B $E6 $1B $EC $1B $F2 $1B $F8 $1B $FF $1B $00 $30 $06 $30 $0C $30 $12 $30 $18 $30 $1F $30 $C0 $30 $C6 $30 $CC $30 $D2 $30 $D8 $30 $DF $30 $80 $31 $86 $31 $8C $31 $92 $31 $98 $31 $9F $31 $40 $32 $46 $32 $4C $32 $52 $32 $58 $32 $5F $32 $00 $33 $06 $33 $0C $33 $12 $33 $18 $33 $1F $33 $E0 $33 $E6 $33 $EC $33 $F2 $33 $F8 $33 $FF $33 $00 $48 $06 $48 $0C $48 $12 $48 $18 $48 $1F $48 $C0 $48 $C6 $48 $CC $48 $D2 $48 $D8 $48 $DF $48 $80 $49 $86 $49 $8C $49 $92 $49 $98 $49 $9F $49 $40 $4A $46 $4A $4C $4A $52 $4A $58 $4A $5F $4A $00 $4B $06 $4B $0C $4B $12 $4B $18 $4B $1F $4B $E0 $4B $E6 $4B $EC $4B $F2 $4B $F8 $4B $FF $4B $00 $60 $06 $60 $0C $60 $12 $60 $18 $60 $1F $60 $C0 $60 $C6 $60 $CC $60 $D2 $60 $D8 $60 $DF $60 $80 $61 $86 $61 $8C $61 $92 $61 $98 $61 $9F $61 $40 $62 $46 $62 $4C $62 $52 $62 $58 $62 $5F $62 $00 $63 $06 $63 $0C $63 $12 $63 $18 $63 $1F $63 $E0 $63 $E6 $63 $EC $63 $F2 $63 $F8 $63 $FF $63 $00 $7C $06 $7C $0C $7C $12 $7C $18 $7C $1F $7C $C0 $7C $C6 $7C $CC $7C $D2 $7C $D8 $7C $DF $7C $80 $7D $86 $7D $8C $7D $92 $7D $98 $7D $9F $7D $40 $7E $46 $7E $4C $7E $52 $7E $58 $7E $5F $7E $00 $7F $06 $7F $0C $7F $12 $7F $18 $7F $1F $7F $E0 $7F $E6 $7F $EC $7F $F2 $7F $F8 $7F $FF $7F $00 $00 $42 $08 $63 $0C $84 $10 $A5 $14 $E7 $1C $08 $21 $29 $25 $4A $29 $6B $2D $AD $35 $CE $39 $EF $3D $10 $42 $31 $46 $73 $4E $94 $52 $B5 $56 $D6 $5A $18 $63 $39 $67 $5A $6B $7B $6F $9C $73

buf: reserve 32 bytes
draw_mouse:
  mov %bp 0
  sub %sp %bp
  mov %bp %sp
  inx %bp
  jsr getmousepos
  mov %di cursor
  int $13
  int $11
  mov %dc 16
  int $22
  psh 0
.ret:
  pop %dc
  mov %ac 0
  add %sp %ac
  pop %ac
  mov %bp 0
  add %sp %bp
  psh %dc
  psh %ac
  ret
rfib:
  mov %bp 0
  sub %sp %bp
  mov %bp %sp
  inx %bp
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  mov %bs 1
  cmp %ac %bs
  mov %ac 0
  jmne .notone2
  mov %ac 1
.notone2:
  cmp %ac 0
  jmne .last1
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  mov %bs 0
  cmp %ac %bs
  mov %ac 0
  jmne .notone3
  mov %ac 1
.notone3:
  cmp %ac 0
  jmne .last1
.last1:
  cmp %ac 0
  jme .ifnot1
  mov %ac 1
  psh %ac
  jmp .ret
  jmp .ifend1
.ifnot1:
.ifend1:
  psh %bp
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  sub %ac 1
  psh %ac
  call rfib
  pop %ac
  pop %bp
  psh %bp
  psh %ac
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  sub %ac 2
  psh %ac
  call rfib
  pop %bs
  pop %ac
  pop %bp
  add %ac %bs
  psh %ac
  jmp .ret
  psh 0
.ret:
  pop %dc
  mov %ac 0
  add %sp %ac
  pop %ac
  mov %bp 3
  add %sp %bp
  psh %dc
  psh %ac
  ret
sfib:
  mov %bp 6
  sub %sp %bp
  mov %bp %sp
  inx %bp
  mov %ac 1
  mov %dt 3
  add %dt %bp
  storw %ac
  mov %ac 1
  mov %dt 0
  add %dt %bp
  storw %ac
.loop1:
  mov %dt 9
  add %dt %bp
  lh %dt %ac
  cmp %ac 0
  jme .loopend1
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  mov %dt 3
  add %dt %bp
  xchg %ac %bs
  lh %dt %ac
  xchg %ac %bs
  add %ac %bs
  mov %dt 3
  add %dt %bp
  storw %ac
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  mov %dt 3
  add %dt %bp
  xchg %ac %bs
  lh %dt %ac
  xchg %ac %bs
  add %ac %bs
  mov %dt 0
  add %dt %bp
  storw %ac
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  mov %dt 3
  add %dt %bp
  xchg %ac %bs
  lh %dt %ac
  xchg %ac %bs
  sub %ac %bs
  mov %dt 3
  add %dt %bp
  storw %ac
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  mov %dt 3
  add %dt %bp
  xchg %ac %bs
  lh %dt %ac
  xchg %ac %bs
  sub %ac %bs
  mov %dt 0
  add %dt %bp
  storw %ac
  mov %dt 9
  add %dt %bp
  lh %dt %ac
  sub %ac 1
  mov %dt 9
  add %dt %bp
  storw %ac
  jmp .loop1
.loopend1:
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  psh %ac
  jmp .ret
  psh 0
.ret:
  pop %dc
  mov %ac 6
  add %sp %ac
  pop %ac
  mov %bp 3
  add %sp %bp
  psh %dc
  psh %ac
  ret
getint:
  mov %bp 6
  sub %sp %bp
  mov %bp %sp
  inx %bp
  mov %ac 0
  mov %dt 3
  add %dt %bp
  storw %ac
  mov %ac 0
  mov %dt 0
  add %dt %bp
  storw %ac
.loop1:
  mov %dt 9
  add %dt %bp
  lh %dt %ac
  mov %dt 0
  add %dt %bp
  xchg %ac %bs
  lh %dt %ac
  xchg %ac %bs
  mov %dt %bs
  add %dt %ac
  mov %ac *%dt
  cmp %ac 0
  jme .loopend1
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  mul %ac 10
  mov %dt 9
  add %dt %bp
  xchg %ac %bs
  lh %dt %ac
  xchg %ac %bs
  mov %dt 0
  add %dt %bp
  xchg %ac %cn
  lh %dt %ac
  xchg %ac %cn
  mov %dt %cn
  add %dt %bs
  mov %bs *%dt
  sub %bs 48
  add %ac %bs
  mov %dt 3
  add %dt %bp
  storw %ac
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  add %ac 1
  mov %dt 0
  add %dt %bp
  storw %ac
  jmp .loop1
.loopend1:
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  psh %ac
  jmp .ret
  psh 0
.ret:
  pop %dc
  mov %ac 6
  add %sp %ac
  pop %ac
  mov %bp 3
  add %sp %bp
  psh %dc
  psh %ac
  ret
fibcalc:
  mov %bp 0
  sub %sp %bp
  mov %bp %sp
  inx %bp
  psh %bp
  mov %ac str0
  psh %ac
  call puts
  pop %ac
  pop %bp
  psh %bp
  mov %ac buf
  psh %ac
  call gets
  pop %ac
  pop %bp
  psh %bp
  mov %ac str1
  psh %ac
  call puts
  pop %ac
  pop %bp
  psh %bp
  psh %bp
  psh %bp
  mov %ac buf
  psh %ac
  call getint
  pop %ac
  pop %bp
  psh %ac
  call rfib
  pop %ac
  pop %bp
  psh %ac
  call puti
  pop %ac
  pop %bp
  psh %bp
  mov %ac str2
  psh %ac
  call puts
  pop %ac
  pop %bp
  psh 0
.ret:
  pop %dc
  mov %ac 0
  add %sp %ac
  pop %ac
  mov %bp 0
  add %sp %bp
  psh %dc
  psh %ac
  ret
lost_cpu_mem: reserve 400 bytes
lost_editor:
  mov %bp 9
  sub %sp %bp
  mov %bp %sp
  inx %bp
  psh %bp
  mov %ac str3
  psh %ac
  call puts
  pop %ac
  pop %bp
  mov %ac lost_cpu_mem
  mov %dt 6
  add %dt %bp
  storw %ac
  mov %ac 1
  mov %dt 3
  add %dt %bp
  storw %ac
.loop1:
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  cmp %ac 0
  jme .loopend1
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  mov %dt 0
  add %dt %bp
  storw %ac
  psh %bp
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  psh %ac
  call gets
  pop %ac
  pop %bp
  mov %dt 6
  add %dt %bp
  storw %ac
  mov %ac 10
  mov %dt 6
  add %dt %bp
  xchg %ac %bs
  lh %dt %ac
  xchg %ac %bs
  mov %cn 0
  mov %dt %cn
  add %dt %bs
  storb %ac
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  mov %dt 0
  add %dt %bp
  xchg %ac %bs
  lh %dt %ac
  xchg %ac %bs
  add %bs 1
  cmp %ac %bs
  mov %ac 0
  jmne .notone1
  mov %ac 1
.notone1:
  cmp %ac 0
  jme .ifnot1
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  mov %bs 0
  mov %dt %bs
  add %dt %ac
  mov %ac *%dt
  mov %bs 46
  cmp %ac %bs
  mov %ac 0
  jmne .notone2
  mov %ac 1
.notone2:
  cmp %ac 0
  jme .ifnot2
  mov %ac 0
  mov %dt 0
  add %dt %bp
  xchg %ac %bs
  lh %dt %ac
  xchg %ac %bs
  mov %cn 0
  mov %dt %cn
  add %dt %bs
  storb %ac
  mov %ac 0
  mov %dt 3
  add %dt %bp
  storw %ac
  jmp .ifend2
.ifnot2:
.ifend2:
  jmp .ifend1
.ifnot1:
.ifend1:
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  add %ac 1
  mov %dt 6
  add %dt %bp
  storw %ac
  jmp .loop1
.loopend1:
  psh 0
.ret:
  pop %dc
  mov %ac 9
  add %sp %ac
  pop %ac
  mov %bp 0
  add %sp %bp
  psh %dc
  psh %ac
  ret
lost_emulator:
  mov %bp 9
  sub %sp %bp
  mov %bp %sp
  inx %bp
  psh %bp
  mov %ac str4
  psh %ac
  call puts
  pop %ac
  pop %bp
  mov %ac lost_cpu_mem
  mov %dt 6
  add %dt %bp
  storw %ac
  mov %ac 0
  mov %dt 3
  add %dt %bp
  storw %ac
.loop1:
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  mov %bs 0
  mov %dt %bs
  add %dt %ac
  mov %ac *%dt
  cmp %ac 0
  jme .loopend1
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  mov %bs 0
  mov %dt %bs
  add %dt %ac
  mov %ac *%dt
  mov %bs 10
  cmp %ac %bs
  mov %ac 0
  jmne .notone1
  mov %ac 1
.notone1:
  cmp %ac 0
  jme .ifnot1
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  add %ac 1
  mov %dt 6
  add %dt %bp
  storw %ac
  jmp .ifend1
.ifnot1:
  psh %bp
  psh 6
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %ac str5
  psh %ac
  call memcmp
  pop %ac
  pop %bp
  cmp %ac 0
  jme .ifnot2
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  add %ac 6
  mov %dt 0
  add %dt %bp
  storw %ac
.loop2:
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  mov %bs 0
  mov %dt %bs
  add %dt %ac
  mov %ac *%dt
  mov %bs 10
  cmp %ac %bs
  mov %ac 0
  jme .notone2
  mov %ac 1
.notone2:
  cmp %ac 0
  jme .loopend2
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  add %ac 1
  mov %dt 0
  add %dt %bp
  storw %ac
  jmp .loop2
.loopend2:
  mov %ac 0
  mov %dt 0
  add %dt %bp
  xchg %ac %bs
  lh %dt %ac
  xchg %ac %bs
  mov %cn 0
  mov %dt %cn
  add %dt %bs
  storb %ac
  psh %bp
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  add %ac 6
  psh %ac
  call puts
  pop %ac
  pop %bp
  psh %bp
  mov %ac str6
  psh %ac
  call puts
  pop %ac
  pop %bp
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  cmp %ac 0
  jme .ifnot3
  psh %bp
  mov %ac str7
  psh %ac
  call puts
  pop %ac
  pop %bp
  psh %bp
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  add %ac 6
  psh %ac
  call puts
  pop %ac
  pop %bp
  psh %bp
  mov %ac str8
  psh %ac
  call puts
  pop %ac
  pop %bp
  mov %ac 0
  mov %dt 3
  add %dt %bp
  storw %ac
  jmp .ifend3
.ifnot3:
.ifend3:
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  add %ac 1
  mov %dt 6
  add %dt %bp
  storw %ac
  jmp .ifend2
.ifnot2:
  psh %bp
  psh 6
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %ac str9
  psh %ac
  call memcmp
  pop %ac
  pop %bp
  cmp %ac 0
  jme .ifnot4
  psh %bp
  mov %ac str10
  psh %ac
  call puts
  pop %ac
  pop %bp
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  add %ac 6
  mov %dt 6
  add %dt %bp
  storw %ac
  jmp .ifend4
.ifnot4:
  psh %bp
  psh 12
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %ac str11
  psh %ac
  call memcmp
  pop %ac
  pop %bp
  cmp %ac 0
  jme .ifnot5
  psh %bp
  mov %ac str12
  psh %ac
  call puts
  pop %ac
  pop %bp
  psh %bp
  mov %ac buf
  psh %ac
  call gets
  pop %ac
  pop %bp
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  add %ac 13
  mov %dt 6
  add %dt %bp
  storw %ac
  jmp .ifend5
.ifnot5:
  psh %bp
  psh 3
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %ac str13
  psh %ac
  call memcmp
  pop %ac
  pop %bp
  cmp %ac 0
  jme .ifnot6
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  add %ac 3
  mov %dt 0
  add %dt %bp
  storw %ac
.loop3:
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  mov %bs 0
  mov %dt %bs
  add %dt %ac
  mov %ac *%dt
  mov %bs 10
  cmp %ac %bs
  mov %ac 0
  jme .notone3
  mov %ac 1
.notone3:
  cmp %ac 0
  jme .loopend3
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  add %ac 1
  mov %dt 0
  add %dt %bp
  storw %ac
  jmp .loop3
.loopend3:
  mov %ac 0
  mov %dt 0
  add %dt %bp
  xchg %ac %bs
  lh %dt %ac
  xchg %ac %bs
  mov %cn 0
  mov %dt %cn
  add %dt %bs
  storb %ac
  psh %bp
  mov %ac buf
  psh %ac
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  add %ac 3
  psh %ac
  call scmp
  pop %ac
  pop %bp
  mov %bs 0
  cmp %ac %bs
  mov %ac 0
  jmne .notone4
  mov %ac 1
.notone4:
  cmp %ac 0
  jme .ifnot7
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  mov %dt 6
  add %dt %bp
  storw %ac
.loop4:
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  mov %bs 0
  mov %dt %bs
  add %dt %ac
  mov %ac *%dt
  mov %bs 10
  cmp %ac %bs
  mov %ac 0
  jme .notone5
  mov %ac 1
.notone5:
  cmp %ac 0
  jme .loopend4
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  add %ac 1
  mov %dt 6
  add %dt %bp
  storw %ac
  jmp .loop4
.loopend4:
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  add %ac 1
  mov %dt 6
  add %dt %bp
  storw %ac
  jmp .ifend7
.ifnot7:
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  add %ac 1
  mov %dt 6
  add %dt %bp
  storw %ac
  mov %ac 1
  mov %dt 3
  add %dt %bp
  storw %ac
.ifend7:
  jmp .ifend6
.ifnot6:
  psh %bp
  mov %ac str14
  psh %ac
  call puts
  pop %ac
  pop %bp
  psh %bp
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  psh %ac
  call puts
  pop %ac
  pop %bp
  mov %ac 0
  psh %ac
  jmp .ret
.ifend6:
.ifend5:
.ifend4:
.ifend2:
.ifend1:
  jmp .loop1
.loopend1:
  psh 0
.ret:
  pop %dc
  mov %ac 9
  add %sp %ac
  pop %ac
  mov %bp 0
  add %sp %bp
  psh %dc
  psh %ac
  ret
gui:
  mov %bp 0
  sub %sp %bp
  mov %bp %sp
  inx %bp
  psh %bp
  psh 0
  psh 0
  psh 15
  mov %ac str15
  psh %ac
  call vputs
  pop %ac
  pop %bp
  psh %bp
  call vflush
  pop %ac
  pop %bp
  psh %bp
  psh 2000
  call msleep
  pop %ac
  pop %bp
  psh %bp
  psh 0
  call cls
  pop %ac
  pop %bp
  psh %bp
  call vflush
  pop %ac
  pop %bp
  psh %bp
  psh 0
  psh 10
  psh 15
  mov %ac str16
  psh %ac
  call vputs8
  pop %ac
  pop %bp
  psh %bp
  call vflush
  pop %ac
  pop %bp
  psh %bp
  psh 2000
  call msleep
  pop %ac
  pop %bp
  psh %bp
  psh 0
  call cls
  pop %ac
  pop %bp
  psh %bp
  call vflush
  pop %ac
  pop %bp
  psh %bp
  psh 0
  psh 20
  psh 15
  mov %ac str17
  psh %ac
  call vputs8t
  pop %ac
  pop %bp
  psh %bp
  call vflush
  pop %ac
  pop %bp
  psh %bp
  psh 2000
  call msleep
  pop %ac
  pop %bp
  psh %bp
  psh 0
  call cls
  pop %ac
  pop %bp
  psh %bp
  call vflush
  pop %ac
  pop %bp
  psh %bp
  psh 0
  psh 30
  psh 15
  mov %ac str18
  psh %ac
  call vputs16
  pop %ac
  pop %bp
  psh %bp
  call vflush
  pop %ac
  pop %bp
  psh %bp
  psh 2000
  call msleep
  pop %ac
  pop %bp
  psh %bp
  psh 0
  call cls
  pop %ac
  pop %bp
  psh %bp
  call vflush
  pop %ac
  pop %bp
  psh %bp
  psh 0
  psh 40
  psh 15
  mov %ac str19
  psh %ac
  call vputsas16
  pop %ac
  pop %bp
  psh %bp
  call vflush
  pop %ac
  pop %bp
  psh %bp
  psh 2000
  call msleep
  pop %ac
  pop %bp
  psh %bp
  psh 0
  call cls
  pop %ac
  pop %bp
  psh %bp
  call vflush
  pop %ac
  pop %bp
  psh %bp
  psh 0
  psh 50
  psh 15
  mov %ac str20
  psh %ac
  call vputsgui
  pop %ac
  pop %bp
  psh %bp
  call vflush
  pop %ac
  pop %bp
  psh %bp
  psh 2000
  call msleep
  pop %ac
  pop %bp
  psh %bp
  psh 0
  call cls
  pop %ac
  pop %bp
  psh %bp
  call vflush
  pop %ac
  pop %bp
  psh %bp
  mov %ac str21
  psh %ac
  call puts
  pop %ac
  pop %bp
  psh 0
.ret:
  pop %dc
  mov %ac 0
  add %sp %ac
  pop %ac
  mov %bp 0
  add %sp %bp
  psh %dc
  psh %ac
  ret
presa:
  mov %bp 3
  sub %sp %bp
  mov %bp %sp
  inx %bp
  psh %bp
  psh 15
  call cls
  pop %ac
  pop %bp
  psh %bp
  psh 10
  psh 10
  psh 3
  mov %ac str22
  psh %ac
  call vputs8t
  pop %ac
  pop %bp
  mov %dt 0
  add %dt %bp
  storw %ac
  psh %bp
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  psh %ac
  psh 0
  psh 0
  mov %ac str23
  psh %ac
  call vputs8t
  pop %ac
  pop %bp
  psh %bp
  psh 10
  psh 20
  psh 0
  mov %ac str24
  psh %ac
  call vputs8t
  pop %ac
  pop %bp
  psh %bp
  psh 10
  psh 44
  mov %ac 8
  mul %ac 15
  psh %ac
  mov %ac 8
  mul %ac 15
  psh %ac
  psh 7
  call box
  pop %ac
  pop %bp
  psh %bp
  psh 10
  psh 44
  psh 0
  mov %ac str25
  psh %ac
  call vputs8t
  pop %ac
  pop %bp
  psh %bp
  psh 140
  psh 20
  psh 0
  mov %ac str26
  psh %ac
  call vputs8t
  pop %ac
  pop %bp
  psh %bp
  psh 140
  psh 36
  psh 0
  mov %ac str27
  psh %ac
  call vputs8t
  pop %ac
  pop %bp
  psh %bp
  psh 140
  psh 44
  mov %ac 8
  mul %ac 15
  psh %ac
  mov %ac 8
  mul %ac 15
  psh %ac
  psh 7
  call box
  pop %ac
  pop %bp
  psh %bp
  psh 140
  psh 44
  psh 0
  mov %ac str28
  psh %ac
  call vputs8t
  pop %ac
  pop %bp
  psh %bp
  call vflush
  pop %ac
  pop %bp
  psh 0
.ret:
  pop %dc
  mov %ac 3
  add %sp %ac
  pop %ac
  mov %bp 0
  add %sp %bp
  psh %dc
  psh %ac
  ret
meme:
  mov %bp 3
  sub %sp %bp
  mov %bp %sp
  inx %bp
  psh %bp
  psh 15
  call cls
  pop %ac
  pop %bp
  mov %ac 0
  mov %dt 0
  add %dt %bp
  storw %ac
.loop1:
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  mov %bs 16
  cmp %ac %bs
  mov %ac 0
  jme .notone1
  mov %ac 1
.notone1:
  cmp %ac 0
  jme .loopend1
  psh %bp
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  mul %ac 8
  psh %ac
  psh 0
  psh 8
  psh 8
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  psh %ac
  call box
  pop %ac
  pop %bp
  psh %bp
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  mul %ac 8
  psh %ac
  psh 0
  mov %ac 15
  mov %dt 0
  add %dt %bp
  xchg %ac %bs
  lh %dt %ac
  xchg %ac %bs
  sub %ac %bs
  psh %ac
  mov %ac str29
  mov %dt 0
  add %dt %bp
  xchg %ac %bs
  lh %dt %ac
  xchg %ac %bs
  mul %bs 2
  add %ac %bs
  psh %ac
  call vputs
  pop %ac
  pop %bp
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  add %ac 1
  mov %dt 0
  add %dt %bp
  storw %ac
  jmp .loop1
.loopend1:
  psh %bp
  psh 10
  psh 10
  psh 2
  mov %ac str30
  psh %ac
  call vputs8t
  pop %ac
  pop %bp
  psh %bp
  psh 10
  psh 18
  psh 0
  mov %ac str31
  psh %ac
  call vputs8t
  pop %ac
  pop %bp
  psh %bp
  psh 0
  psh 182
  psh 0
  mov %ac str32
  psh %ac
  call vputs8t
  pop %ac
  pop %bp
  psh %bp
  call vflush
  pop %ac
  pop %bp
  psh 0
.ret:
  pop %dc
  mov %ac 3
  add %sp %ac
  pop %ac
  mov %bp 0
  add %sp %bp
  psh %dc
  psh %ac
  ret
geometry:
  mov %bp 12
  sub %sp %bp
  mov %bp %sp
  inx %bp
  psh %bp
  psh 1
  call cls
  pop %ac
  pop %bp
  psh %bp
  psh 0
  psh 0
  psh 340
  psh 190
  psh 11
  call box
  pop %ac
  pop %bp
  mov %ac 35
  mov %dt 9
  add %dt %bp
  storw %ac
  mov %ac 0
  mov %dt 9
  add %dt %bp
  xchg %ac %bs
  lh %dt %ac
  xchg %ac %bs
  sub %ac %bs
  mov %dt 6
  add %dt %bp
  storw %ac
  mov %ac 0
  mov %dt 9
  add %dt %bp
  xchg %ac %bs
  lh %dt %ac
  xchg %ac %bs
  sub %ac %bs
  mov %dt 3
  add %dt %bp
  storw %ac
.loop1:
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  mov %dt 9
  add %dt %bp
  xchg %ac %bs
  lh %dt %ac
  xchg %ac %bs
  cmp %ac %bs
  mov %ac 0
  jme .notone1
  mov %ac 1
.notone1:
  cmp %ac 0
  jme .loopend1
.loop2:
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  mov %dt 9
  add %dt %bp
  xchg %ac %bs
  lh %dt %ac
  xchg %ac %bs
  cmp %ac %bs
  mov %ac 0
  jme .notone2
  mov %ac 1
.notone2:
  cmp %ac 0
  jme .loopend2
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  mov %dt 6
  add %dt %bp
  xchg %ac %bs
  lh %dt %ac
  xchg %ac %bs
  mul %ac %bs
  mov %dt 3
  add %dt %bp
  xchg %ac %bs
  lh %dt %ac
  xchg %ac %bs
  mov %dt 3
  add %dt %bp
  xchg %ac %cn
  lh %dt %ac
  xchg %ac %cn
  mul %bs %cn
  add %ac %bs
  mov %dt 9
  add %dt %bp
  xchg %ac %bs
  lh %dt %ac
  xchg %ac %bs
  mov %dt 9
  add %dt %bp
  xchg %ac %cn
  lh %dt %ac
  xchg %ac %cn
  mul %bs %cn
  cmp %ac %bs
  mov %ac 0
  jg .notone3
  mov %ac 1
.notone3:
  cmp %ac 0
  jme .ifnot1
  psh %bp
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  add %ac 270
  psh %ac
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  add %ac 85
  psh %ac
  psh 1
  psh 1
  psh 5
  call box
  pop %ac
  pop %bp
  jmp .ifend1
.ifnot1:
.ifend1:
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  add %ac 1
  mov %dt 6
  add %dt %bp
  storw %ac
  jmp .loop2
.loopend2:
  mov %ac 0
  mov %dt 9
  add %dt %bp
  xchg %ac %bs
  lh %dt %ac
  xchg %ac %bs
  sub %ac %bs
  mov %dt 6
  add %dt %bp
  storw %ac
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  add %ac 1
  mov %dt 3
  add %dt %bp
  storw %ac
  jmp .loop1
.loopend1:
  psh %bp
  psh 10
  psh 50
  psh 70
  psh 70
  psh 5
  call box
  pop %ac
  pop %bp
  mov %ac 160
  mov %dt 6
  add %dt %bp
  storw %ac
  mov %ac 50
  mov %dt 3
  add %dt %bp
  storw %ac
  mov %ac 1
  mov %dt 0
  add %dt %bp
  storw %ac
.loop3:
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  mov %bs 120
  cmp %ac %bs
  mov %ac 0
  jg .notone4
  mov %ac 1
.notone4:
  cmp %ac 0
  jme .loopend3
  psh %bp
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  psh %ac
  psh 1
  psh 5
  call box
  pop %ac
  pop %bp
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  add %ac 1
  mov %dt 3
  add %dt %bp
  storw %ac
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  sub %ac 1
  mov %dt 6
  add %dt %bp
  storw %ac
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  add %ac 2
  mov %dt 0
  add %dt %bp
  storw %ac
  jmp .loop3
.loopend3:
  psh %bp
  call vflush
  pop %ac
  pop %bp
  psh 0
.ret:
  pop %dc
  mov %ac 12
  add %sp %ac
  pop %ac
  mov %bp 0
  add %sp %bp
  psh %dc
  psh %ac
  ret
govndate_inx:
  mov %bp 0
  sub %sp %bp
  mov %bp %sp
  inx %bp
  psh %bp
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  psh %ac
  psh 200
  psh 100
  psh 8
  call box
  pop %ac
  pop %bp
  psh %bp
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  add %ac 200
  sub %ac 1
  psh %ac
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  psh %ac
  psh 1
  psh 100
  psh 8
  call box
  pop %ac
  pop %bp
  psh %bp
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  add %ac 100
  sub %ac 1
  psh %ac
  psh 200
  psh 1
  psh 8
  call box
  pop %ac
  pop %bp
  psh %bp
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  psh %ac
  psh 1
  psh 100
  psh 8
  call box
  pop %ac
  pop %bp
  psh %bp
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  add %ac 1
  psh %ac
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  add %ac 1
  psh %ac
  psh 198
  psh 98
  psh 0
  call box
  pop %ac
  pop %bp
  psh %bp
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  add %ac 8
  psh %ac
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  add %ac 8
  psh %ac
  psh 15
  mov %ac str33
  psh %ac
  call vputsas16
  pop %ac
  pop %bp
  psh %bp
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  add %ac 8
  psh %ac
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  add %ac 24
  psh %ac
  psh 15
  mov %ac str34
  psh %ac
  call vputs
  pop %ac
  pop %bp
  psh %bp
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  add %ac 8
  psh %ac
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  add %ac 40
  psh %ac
  psh 15
  mov %ac str35
  psh %ac
  call vputs8t
  pop %ac
  pop %bp
  psh %bp
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  add %ac 8
  psh %ac
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  add %ac 48
  psh %ac
  psh 6
  mov %ac str36
  psh %ac
  call vputs8t
  pop %ac
  pop %bp
  psh %bp
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  add %ac 8
  psh %ac
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  add %ac 56
  psh %ac
  psh 9
  mov %ac str37
  psh %ac
  call vputs8t
  pop %ac
  pop %bp
  psh %bp
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  add %ac 8
  psh %ac
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  add %ac 64
  psh %ac
  psh 15
  mov %ac str38
  psh %ac
  call vputs8t
  pop %ac
  pop %bp
  psh %bp
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  add %ac 8
  psh %ac
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  add %ac 72
  psh %ac
  psh 10
  mov %ac str39
  psh %ac
  call vputs8t
  pop %ac
  pop %bp
  psh %bp
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  add %ac 200
  sub %ac 9
  sub %ac 49
  sub %ac 8
  psh %ac
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  add %ac 100
  sub %ac 9
  sub %ac 16
  psh %ac
  mov %ac 49
  add %ac 8
  psh %ac
  psh 16
  psh 8
  call box
  pop %ac
  pop %bp
  psh %bp
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  add %ac 200
  sub %ac 9
  sub %ac 49
  sub %ac 8
  add %ac 4
  psh %ac
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  add %ac 100
  sub %ac 9
  sub %ac 16
  add %ac 1
  psh %ac
  psh 15
  mov %ac str40
  psh %ac
  call vputs
  pop %ac
  pop %bp
  psh 0
.ret:
  pop %dc
  mov %ac 0
  add %sp %ac
  pop %ac
  mov %bp 6
  add %sp %bp
  psh %dc
  psh %ac
  ret
notify:
  mov %bp 0
  sub %sp %bp
  mov %bp %sp
  inx %bp
  psh %bp
  mov %dt 15
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %dt 12
  add %dt %bp
  lh %dt %ac
  psh %ac
  psh 200
  psh 100
  psh 8
  call box
  pop %ac
  pop %bp
  psh %bp
  mov %dt 15
  add %dt %bp
  lh %dt %ac
  add %ac 200
  sub %ac 1
  psh %ac
  mov %dt 12
  add %dt %bp
  lh %dt %ac
  psh %ac
  psh 1
  psh 100
  psh 8
  call box
  pop %ac
  pop %bp
  psh %bp
  mov %dt 15
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %dt 12
  add %dt %bp
  lh %dt %ac
  add %ac 100
  sub %ac 1
  psh %ac
  psh 200
  psh 1
  psh 8
  call box
  pop %ac
  pop %bp
  psh %bp
  mov %dt 15
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %dt 12
  add %dt %bp
  lh %dt %ac
  psh %ac
  psh 1
  psh 100
  psh 8
  call box
  pop %ac
  pop %bp
  psh %bp
  mov %dt 15
  add %dt %bp
  lh %dt %ac
  add %ac 1
  psh %ac
  mov %dt 12
  add %dt %bp
  lh %dt %ac
  add %ac 1
  psh %ac
  psh 198
  psh 98
  psh 0
  call box
  pop %ac
  pop %bp
  psh %bp
  mov %dt 15
  add %dt %bp
  lh %dt %ac
  add %ac 8
  psh %ac
  mov %dt 12
  add %dt %bp
  lh %dt %ac
  add %ac 8
  psh %ac
  psh 15
  mov %dt 9
  add %dt %bp
  lh %dt %ac
  psh %ac
  call vputsas16
  pop %ac
  pop %bp
  psh %bp
  mov %dt 15
  add %dt %bp
  lh %dt %ac
  add %ac 8
  psh %ac
  mov %dt 12
  add %dt %bp
  lh %dt %ac
  add %ac 24
  psh %ac
  psh 15
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  psh %ac
  call vputs
  pop %ac
  pop %bp
  psh %bp
  mov %dt 15
  add %dt %bp
  lh %dt %ac
  add %ac 200
  sub %ac 9
  sub %ac 49
  sub %ac 8
  psh %ac
  mov %dt 12
  add %dt %bp
  lh %dt %ac
  add %ac 100
  sub %ac 9
  sub %ac 16
  psh %ac
  mov %ac 49
  add %ac 8
  psh %ac
  psh 16
  psh 8
  call box
  pop %ac
  pop %bp
  psh %bp
  mov %dt 15
  add %dt %bp
  lh %dt %ac
  add %ac 200
  sub %ac 9
  sub %ac 49
  sub %ac 8
  add %ac 4
  psh %ac
  mov %dt 12
  add %dt %bp
  lh %dt %ac
  add %ac 100
  sub %ac 9
  sub %ac 16
  add %ac 1
  psh %ac
  psh 15
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  psh %ac
  call vputs
  pop %ac
  pop %bp
  psh 0
.ret:
  pop %dc
  mov %ac 0
  add %sp %ac
  pop %ac
  mov %bp 15
  add %sp %bp
  psh %dc
  psh %ac
  ret
govnweb:
  mov %bp 6
  sub %sp %bp
  mov %bp %sp
  inx %bp
  psh %bp
  mov %dt 12
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %dt 9
  add %dt %bp
  lh %dt %ac
  psh %ac
  psh 340
  psh 190
  mov %ac str41
  psh %ac
  call window
  pop %ac
  pop %bp
  psh %bp
  mov %dt 12
  add %dt %bp
  lh %dt %ac
  add %ac 2
  psh %ac
  mov %dt 9
  add %dt %bp
  lh %dt %ac
  add %ac 1
  psh %ac
  psh 13
  mov %ac gweb2_mini
  psh %ac
  call gic
  pop %ac
  pop %bp
  mov %dt 12
  add %dt %bp
  lh %dt %ac
  add %ac 1
  mov %dt 3
  add %dt %bp
  storw %ac
  mov %dt 9
  add %dt %bp
  lh %dt %ac
  add %ac 17
  mov %dt 0
  add %dt %bp
  storw %ac
  psh %bp
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  psh %ac
  psh 8
  psh 8
  psh 6
  call box
  pop %ac
  pop %bp
  psh %bp
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  psh %ac
  psh 15
  mov %ac str42
  psh %ac
  call vputs8t
  pop %ac
  pop %bp
  psh %bp
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  add %ac 8
  psh %ac
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  psh %ac
  psh 8
  psh 8
  psh 8
  call box
  pop %ac
  pop %bp
  psh %bp
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  add %ac 8
  psh %ac
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  psh %ac
  psh 7
  mov %ac str43
  psh %ac
  call vputs8t
  pop %ac
  pop %bp
  psh %bp
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  add %ac 16
  psh %ac
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %ac 340
  sub %ac 16
  psh %ac
  psh 8
  psh 0
  call box
  pop %ac
  pop %bp
  psh %bp
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  add %ac 8
  psh %ac
  psh 340
  psh 8
  psh 12
  call box
  pop %ac
  pop %bp
  psh %bp
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  add %ac 340
  mov %bs 8
  mul %bs 24
  sub %ac %bs
  psh %ac
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  psh %ac
  psh 15
  mov %ac str44
  psh %ac
  call vputs8t
  pop %ac
  pop %bp
  psh %bp
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  add %ac 8
  psh %ac
  psh 15
  mov %ac str45
  psh %ac
  call vputs8t
  pop %ac
  pop %bp
  psh %bp
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  add %ac 16
  psh %ac
  mov %ac 7
  mul %ac 8
  psh %ac
  psh 8
  psh 15
  call box
  pop %ac
  pop %bp
  psh %bp
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  add %ac 16
  psh %ac
  psh 0
  mov %ac str46
  psh %ac
  call vputs8t
  pop %ac
  pop %bp
  psh %bp
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  mov %bs 6
  mul %bs 8
  add %ac %bs
  psh %ac
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  add %ac 16
  psh %ac
  psh 8
  mov %ac str47
  psh %ac
  call vputs8t
  pop %ac
  pop %bp
  psh %bp
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  mov %bs 7
  mul %bs 8
  add %ac %bs
  psh %ac
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  add %ac 16
  psh %ac
  mov %ac 340
  mov %bs 7
  mul %bs 8
  sub %ac %bs
  psh %ac
  psh 8
  psh 7
  call box
  pop %ac
  pop %bp
  psh %bp
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  add %ac 24
  psh %ac
  psh 340
  psh 8
  psh 15
  call box
  pop %ac
  pop %bp
  psh %bp
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  add %ac 24
  psh %ac
  psh 0
  mov %ac str48
  psh %ac
  call vputs8t
  pop %ac
  pop %bp
  psh %bp
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  mov %bs 15
  mul %bs 8
  add %ac %bs
  psh %ac
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  add %ac 24
  psh %ac
  psh 8
  mov %ac str49
  psh %ac
  call vputs8t
  pop %ac
  pop %bp
  psh %bp
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  add %ac 32
  psh %ac
  psh 340
  mov %ac 190
  sub %ac 32
  psh %ac
  psh 15
  call box
  pop %ac
  pop %bp
  psh %bp
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  mov %bs 340
  mov %cn 3
  mul %cn 8
  sub %bs %cn
  div %bs 3
  add %ac %bs
  psh %ac
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  add %ac 32
  psh %ac
  psh 0
  mov %ac str50
  psh %ac
  call vputs16
  pop %ac
  pop %bp
  psh %bp
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  add %ac 48
  psh %ac
  psh 8
  mov %ac str51
  psh %ac
  call vputsas16
  pop %ac
  pop %bp
  psh %bp
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  add %ac 64
  psh %ac
  psh 0
  mov %ac str52
  psh %ac
  call vputs8t
  pop %ac
  pop %bp
  psh 0
.ret:
  pop %dc
  mov %ac 6
  add %sp %ac
  pop %ac
  mov %bp 6
  add %sp %bp
  psh %dc
  psh %ac
  ret
window:
  mov %bp 3
  sub %sp %bp
  mov %bp %sp
  inx %bp
  mov %ac 15
  mov %dt 0
  add %dt %bp
  storw %ac
  psh %bp
  mov %dt 18
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %dt 15
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %dt 12
  add %dt %bp
  lh %dt %ac
  add %ac 2
  psh %ac
  psh 1
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  psh %ac
  call box
  pop %ac
  pop %bp
  psh %bp
  mov %dt 18
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %dt 15
  add %dt %bp
  lh %dt %ac
  mov %dt 9
  add %dt %bp
  xchg %ac %bs
  lh %dt %ac
  xchg %ac %bs
  add %ac %bs
  add %ac 17
  psh %ac
  mov %dt 12
  add %dt %bp
  lh %dt %ac
  add %ac 2
  psh %ac
  psh 1
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  psh %ac
  call box
  pop %ac
  pop %bp
  psh %bp
  mov %dt 18
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %dt 15
  add %dt %bp
  lh %dt %ac
  add %ac 1
  psh %ac
  psh 1
  mov %dt 9
  add %dt %bp
  lh %dt %ac
  add %ac 16
  psh %ac
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  psh %ac
  call box
  pop %ac
  pop %bp
  psh %bp
  mov %dt 18
  add %dt %bp
  lh %dt %ac
  mov %dt 12
  add %dt %bp
  xchg %ac %bs
  lh %dt %ac
  xchg %ac %bs
  add %ac %bs
  add %ac 1
  psh %ac
  mov %dt 15
  add %dt %bp
  lh %dt %ac
  add %ac 1
  psh %ac
  psh 1
  mov %dt 9
  add %dt %bp
  lh %dt %ac
  add %ac 16
  psh %ac
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  psh %ac
  call box
  pop %ac
  pop %bp
  psh %bp
  mov %dt 18
  add %dt %bp
  lh %dt %ac
  add %ac 1
  psh %ac
  mov %dt 15
  add %dt %bp
  lh %dt %ac
  add %ac 1
  psh %ac
  mov %dt 12
  add %dt %bp
  lh %dt %ac
  psh %ac
  psh 16
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  psh %ac
  call box
  pop %ac
  pop %bp
  psh %bp
  mov %dt 18
  add %dt %bp
  lh %dt %ac
  add %ac 1
  psh %ac
  mov %dt 15
  add %dt %bp
  lh %dt %ac
  add %ac 18
  psh %ac
  mov %dt 12
  add %dt %bp
  lh %dt %ac
  psh %ac
  mov %dt 9
  add %dt %bp
  lh %dt %ac
  psh %ac
  psh 15
  call box
  pop %ac
  pop %bp
  psh %bp
  mov %dt 18
  add %dt %bp
  lh %dt %ac
  add %ac 20
  psh %ac
  mov %dt 15
  add %dt %bp
  lh %dt %ac
  add %ac 1
  psh %ac
  psh 0
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  psh %ac
  call vputs
  pop %ac
  pop %bp
  psh 0
.ret:
  pop %dc
  mov %ac 3
  add %sp %ac
  pop %ac
  mov %bp 15
  add %sp %bp
  psh %dc
  psh %ac
  ret
task:
  mov %bp 0
  sub %sp %bp
  mov %bp %sp
  inx %bp
  mov %dt 3
  add %dt %bp
  lh %dt %ac
  cmp %ac 0
  jme .ifnot1
  psh %bp
  mov %ac 32
  mov %bs 160
  mov %dt 6
  add %dt %bp
  xchg %ac %cn
  lh %dt %ac
  xchg %ac %cn
  mul %bs %cn
  add %ac %bs
  psh %ac
  mov %ac 480
  sub %ac 32
  psh %ac
  psh 160
  mov %ac 32
  sub %ac 2
  psh %ac
  psh 8
  call box
  pop %ac
  pop %bp
  psh %bp
  mov %ac 32
  mov %bs 160
  mov %dt 6
  add %dt %bp
  xchg %ac %cn
  lh %dt %ac
  xchg %ac %cn
  mul %bs %cn
  add %ac %bs
  psh %ac
  mov %ac 480
  sub %ac 2
  psh %ac
  psh 160
  psh 2
  psh 10
  call box
  pop %ac
  pop %bp
  jmp .ifend1
.ifnot1:
  psh %bp
  mov %ac 34
  mov %bs 160
  mov %dt 6
  add %dt %bp
  xchg %ac %cn
  lh %dt %ac
  xchg %ac %cn
  mul %bs %cn
  add %ac %bs
  psh %ac
  mov %ac 480
  sub %ac 2
  psh %ac
  psh 156
  psh 2
  psh 7
  call box
  pop %ac
  pop %bp
.ifend1:
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  mov %bs 0
  cmp %ac %bs
  mov %ac 0
  jmne .notone1
  mov %ac 1
.notone1:
  cmp %ac 0
  jme .ifnot2
  psh %bp
  mov %ac 32
  add %ac 4
  mov %bs 160
  mov %dt 6
  add %dt %bp
  xchg %ac %cn
  lh %dt %ac
  xchg %ac %cn
  mul %bs %cn
  add %ac %bs
  psh %ac
  mov %ac 480
  sub %ac 32
  add %ac 8
  psh %ac
  psh 13
  mov %ac gweb2_mini
  psh %ac
  call gic
  pop %ac
  pop %bp
  psh %bp
  mov %ac 32
  add %ac 4
  add %ac 16
  add %ac 4
  mov %bs 160
  mov %dt 6
  add %dt %bp
  xchg %ac %cn
  lh %dt %ac
  xchg %ac %cn
  mul %bs %cn
  add %ac %bs
  psh %ac
  mov %ac 480
  sub %ac 32
  add %ac 8
  psh %ac
  psh 15
  mov %ac str53
  psh %ac
  call vputs
  pop %ac
  pop %bp
  jmp .ifend2
.ifnot2:
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  mov %bs 1
  cmp %ac %bs
  mov %ac 0
  jmne .notone2
  mov %ac 1
.notone2:
  cmp %ac 0
  jme .ifnot3
  psh %bp
  mov %ac 32
  add %ac 4
  mov %bs 160
  mov %dt 6
  add %dt %bp
  xchg %ac %cn
  lh %dt %ac
  xchg %ac %cn
  mul %bs %cn
  add %ac %bs
  psh %ac
  mov %ac 480
  sub %ac 32
  add %ac 8
  psh %ac
  psh 13
  mov %ac shutapp_mini
  psh %ac
  call gic
  pop %ac
  pop %bp
  psh %bp
  mov %ac 32
  add %ac 4
  add %ac 16
  add %ac 4
  mov %bs 160
  mov %dt 6
  add %dt %bp
  xchg %ac %cn
  lh %dt %ac
  xchg %ac %cn
  mul %bs %cn
  add %ac %bs
  psh %ac
  mov %ac 480
  sub %ac 32
  add %ac 8
  psh %ac
  psh 15
  mov %ac str54
  psh %ac
  call vputs
  pop %ac
  pop %bp
  jmp .ifend3
.ifnot3:
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  mov %bs 2
  cmp %ac %bs
  mov %ac 0
  jmne .notone3
  mov %ac 1
.notone3:
  cmp %ac 0
  jme .ifnot4
  psh %bp
  mov %ac 32
  add %ac 4
  mov %bs 160
  mov %dt 6
  add %dt %bp
  xchg %ac %cn
  lh %dt %ac
  xchg %ac %cn
  mul %bs %cn
  add %ac %bs
  psh %ac
  mov %ac 480
  sub %ac 32
  add %ac 8
  psh %ac
  psh 13
  mov %ac my_pc
  psh %ac
  call gic
  pop %ac
  pop %bp
  psh %bp
  mov %ac 32
  add %ac 4
  add %ac 16
  add %ac 4
  mov %bs 160
  mov %dt 6
  add %dt %bp
  xchg %ac %cn
  lh %dt %ac
  xchg %ac %cn
  mul %bs %cn
  add %ac %bs
  psh %ac
  mov %ac 480
  sub %ac 32
  add %ac 8
  psh %ac
  psh 15
  mov %ac str55
  psh %ac
  call vputs
  pop %ac
  pop %bp
  jmp .ifend4
.ifnot4:
  psh %bp
  mov %ac 32
  add %ac 4
  mov %bs 160
  mov %dt 6
  add %dt %bp
  xchg %ac %cn
  lh %dt %ac
  xchg %ac %cn
  mul %bs %cn
  add %ac %bs
  psh %ac
  mov %ac 480
  sub %ac 32
  add %ac 8
  psh %ac
  psh 16
  psh 16
  psh 3
  call box
  pop %ac
  pop %bp
.ifend4:
.ifend3:
.ifend2:
  psh 0
.ret:
  pop %dc
  mov %ac 0
  add %sp %ac
  pop %ac
  mov %bp 6
  add %sp %bp
  psh %dc
  psh %ac
  ret
lost10:
  mov %bp 3
  sub %sp %bp
  mov %bp %sp
  inx %bp
  mov %ac 1
  mov %dt 0
  add %dt %bp
  storw %ac
  psh %bp
  call load_pal
  pop %ac
  pop %bp
.loop1:
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  cmp %ac 0
  jme .loopend1
  psh %bp
  psh 0
  psh 0
  psh 13
  mov %ac wall
  psh %ac
  call gic
  pop %ac
  pop %bp
  psh %bp
  psh 8
  mov %ac 480
  sub %ac 24
  psh %ac
  psh 13
  mov %ac g10
  psh %ac
  call gic
  pop %ac
  pop %bp
  psh %bp
  mov %ac 640
  mov %bs 6
  mul %bs 8
  sub %ac %bs
  psh %ac
  mov %ac 480
  sub %ac 29
  psh %ac
  psh 15
  mov %ac str56
  psh %ac
  call vputs
  pop %ac
  pop %bp
  psh %bp
  mov %ac 640
  mov %bs 6
  mul %bs 8
  sub %ac %bs
  psh %ac
  mov %ac 480
  sub %ac 16
  psh %ac
  psh 15
  mov %ac str57
  psh %ac
  call vputs
  pop %ac
  pop %bp
  psh %bp
  psh 8
  mov %ac 8
  add %ac 32
  psh %ac
  psh 15
  mov %ac str58
  psh %ac
  call vputs
  pop %ac
  pop %bp
  psh %bp
  mov %ac 8
  add %ac 8
  psh %ac
  psh 8
  psh 13
  mov %ac gweb2
  psh %ac
  call gic
  pop %ac
  pop %bp
  psh %bp
  psh 8
  mov %ac 8
  add %ac 32
  add %ac 48
  psh %ac
  psh 15
  mov %ac str59
  psh %ac
  call vputs
  pop %ac
  pop %bp
  psh %bp
  mov %ac 8
  add %ac 8
  psh %ac
  mov %ac 8
  add %ac 48
  psh %ac
  psh 13
  mov %ac shutapp
  psh %ac
  call gic
  pop %ac
  pop %bp
  psh %bp
  psh 0
  psh 1
  call task
  pop %ac
  pop %bp
  psh %bp
  psh 1
  psh 0
  call task
  pop %ac
  pop %bp
  psh %bp
  psh 2
  psh 0
  call task
  pop %ac
  pop %bp
  psh %bp
  psh 160
  psh 8
  call govnweb
  pop %ac
  pop %bp
  psh %bp
  mov %ac 640
  sub %ac 200
  sub %ac 8
  psh %ac
  mov %ac 480
  sub %ac 100
  sub %ac 32
  sub %ac 8
  psh %ac
  call govndate_inx
  pop %ac
  pop %bp
  psh %bp
  call draw_mouse
  pop %ac
  pop %bp
  jmp .loop1
.loopend1:
  psh 0
.ret:
  pop %dc
  mov %ac 3
  add %sp %ac
  pop %ac
  mov %bp 0
  add %sp %bp
  psh %dc
  psh %ac
  ret
shell:
  mov %bp 3
  sub %sp %bp
  mov %bp %sp
  inx %bp
  mov %ac 1
  mov %dt 0
  add %dt %bp
  storw %ac
  psh %bp
  mov %ac str60
  psh %ac
  call puts
  pop %ac
  pop %bp
  psh %bp
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  psh %ac
  call puti
  pop %ac
  pop %bp
  psh %bp
  mov %ac str61
  psh %ac
  call puts
  pop %ac
  pop %bp
.loop1:
  mov %dt 0
  add %dt %bp
  lh %dt %ac
  cmp %ac 0
  jme .loopend1
  psh %bp
  mov %ac str62
  psh %ac
  call puts
  pop %ac
  pop %bp
  psh %bp
  mov %ac buf
  psh %ac
  call gets
  pop %ac
  pop %bp
  mov %ac *buf
  mov %bs 0
  cmp %ac %bs
  mov %ac 0
  jmne .notone1
  mov %ac 1
.notone1:
  cmp %ac 0
  jme .ifnot1
  jmp .ifend1
.ifnot1:
  psh %bp
  mov %ac buf
  psh %ac
  mov %ac str63
  psh %ac
  call scmp
  pop %ac
  pop %bp
  cmp %ac 0
  jme .ifnot2
  psh %bp
  mov %ac str64
  psh %ac
  call puts
  pop %ac
  pop %bp
  jmp .ifend2
.ifnot2:
  psh %bp
  mov %ac buf
  psh %ac
  mov %ac str65
  psh %ac
  call scmp
  pop %ac
  pop %bp
  cmp %ac 0
  jme .ifnot3
  psh %bp
  mov %ac str66
  psh %ac
  call puts
  pop %ac
  pop %bp
  jmp .ifend3
.ifnot3:
  psh %bp
  mov %ac buf
  psh %ac
  mov %ac str67
  psh %ac
  call scmp
  pop %ac
  pop %bp
  cmp %ac 0
  jme .ifnot4
  psh %bp
  call fibcalc
  pop %ac
  pop %bp
  jmp .ifend4
.ifnot4:
  psh %bp
  mov %ac buf
  psh %ac
  mov %ac str68
  psh %ac
  call scmp
  pop %ac
  pop %bp
  cmp %ac 0
  jme .ifnot5
  psh %bp
  psh 0
  call cls
  pop %ac
  pop %bp
  psh %bp
  call vflush
  pop %ac
  pop %bp
  jmp .ifend5
.ifnot5:
  psh %bp
  mov %ac buf
  psh %ac
  mov %ac str69
  psh %ac
  call scmp
  pop %ac
  pop %bp
  cmp %ac 0
  jme .ifnot6
  psh %bp
  call meme
  pop %ac
  pop %bp
  jmp .ifend6
.ifnot6:
  psh %bp
  mov %ac buf
  psh %ac
  mov %ac str70
  psh %ac
  call scmp
  pop %ac
  pop %bp
  cmp %ac 0
  jme .ifnot7
  psh %bp
  call presa
  pop %ac
  pop %bp
  jmp .ifend7
.ifnot7:
  psh %bp
  mov %ac buf
  psh %ac
  mov %ac str71
  psh %ac
  call scmp
  pop %ac
  pop %bp
  cmp %ac 0
  jme .ifnot8
  psh %bp
  call geometry
  pop %ac
  pop %bp
  jmp .ifend8
.ifnot8:
  psh %bp
  mov %ac buf
  psh %ac
  mov %ac str72
  psh %ac
  call scmp
  pop %ac
  pop %bp
  cmp %ac 0
  jme .ifnot9
  psh %bp
  call lost10
  pop %ac
  pop %bp
  jmp .ifend9
.ifnot9:
  psh %bp
  mov %ac buf
  psh %ac
  mov %ac str73
  psh %ac
  call scmp
  pop %ac
  pop %bp
  cmp %ac 0
  jme .ifnot10
  psh %bp
  call gui
  pop %ac
  pop %bp
  jmp .ifend10
.ifnot10:
  psh %bp
  mov %ac buf
  psh %ac
  mov %ac str74
  psh %ac
  call scmp
  pop %ac
  pop %bp
  cmp %ac 0
  jme .ifnot11
  psh %bp
  call lost_editor
  pop %ac
  pop %bp
  psh %bp
  call lost_emulator
  pop %ac
  pop %bp
  jmp .ifend11
.ifnot11:
  psh %bp
  mov %ac buf
  psh %ac
  mov %ac str75
  psh %ac
  call scmp
  pop %ac
  pop %bp
  cmp %ac 0
  jme .ifnot12
  psh %bp
  mov %dt 6
  add %dt %bp
  lh %dt %ac
  add %ac 1
  psh %ac
  call shell
  pop %ac
  pop %bp
  jmp .ifend12
.ifnot12:
  psh %bp
  mov %ac buf
  psh %ac
  mov %ac str76
  psh %ac
  call scmp
  pop %ac
  pop %bp
  cmp %ac 0
  jme .ifnot13
  psh %bp
  psh 0
  call exit
  pop %ac
  pop %bp
  jmp .ifend13
.ifnot13:
  psh %bp
  mov %ac buf
  psh %ac
  mov %ac str77
  psh %ac
  call scmp
  pop %ac
  pop %bp
  cmp %ac 0
  jme .ifnot14
  psh %bp
  mov %ac str78
  psh %ac
  call puts
  pop %ac
  pop %bp
  mov %ac 0
  mov %dt 0
  add %dt %bp
  storw %ac
  jmp .ifend14
.ifnot14:
  psh %bp
  mov %ac str79
  psh %ac
  call puts
  pop %ac
  pop %bp
.ifend14:
.ifend13:
.ifend12:
.ifend11:
.ifend10:
.ifend9:
.ifend8:
.ifend7:
.ifend6:
.ifend5:
.ifend4:
.ifend3:
.ifend2:
.ifend1:
  jmp .loop1
.loopend1:
  psh %bp
  mov %ac str80
  psh %ac
  call puts
  pop %ac
  pop %bp
  psh 0
.ret:
  pop %dc
  mov %ac 3
  add %sp %ac
  pop %ac
  mov %bp 3
  add %sp %bp
  psh %dc
  psh %ac
  ret
main:
  mov %bp 0
  sub %sp %bp
  mov %bp %sp
  inx %bp
  psh %bp
  psh 0
  call shell
  pop %ac
  pop %bp
  psh 0
.ret:
  pop %dc
  mov %ac 0
  add %sp %ac
  pop %ac
  mov %bp 0
  add %sp %bp
  psh %dc
  psh %ac
  ret
str0: bytes "Number> ^@"
str1: bytes "Result: ^@"
str2: bytes "$^@"
str3: bytes "Lost Core Input Mode$Type code, end with . on a seperate line$^@"
str4: bytes "^[[9mLox^[[0m Lost Core Starting...$^@"
str5: bytes "print ^@"
str6: bytes "$^@"
str7: bytes "[SYS] Condition met for print: ^@"
str8: bytes "$^@"
str9: bytes "print$^@"
str10: bytes "$^@"
str11: bytes "input_string^@"
str12: bytes "Lost -> ^@"
str13: bytes "if ^@"
str14: bytes "^[[31m ERROR: UNKNOWN SHIT ^[[0m^@"
str15: bytes "123321^@"
str16: bytes "123321^@"
str17: bytes "123321^@"
str18: bytes "123321^@"
str19: bytes "123321^@"
str20: bytes "123321^@"
str21: bytes "$123321 <- Must output this$^@"
str22: bytes "Govn^@"
str23: bytes "Py Conf 2025^@"
str24: bytes "Версия 1^@"
str25: bytes "push 2$push 2$push 2$pop %bx$pop %ax$mul %ax %bx$push %ax$pop %bx$pop %ax$add %ax %bx$push %ax^@"
str26: bytes "Version 2$11 lines^@"
str27: bytes "50% shorter^@"
str28: bytes "lda 2$ldb 2$mul %bx 2$add %ax %bx$push %ax^@"
str29: bytes "0^@1^@2^@3^@4^@5^@6^@7^@8^@9^@a^@b^@c^@d^@e^@f^@"
str30: bytes "Geraldip Pavlov^@"
str31: bytes "Коллега, подскажите, пожалуйста, когда будет завершен этот$эксперимент? В том плане, что, я полагаю, изучение ОС началось$из-за некого рода интереса к теме, была ли поставлена цель,$какое-то логическое завершение, или это просто будет длиться$до тех пор, пока энтузиазм не угаснет?^@"
str32: bytes "LostGram. Powered by LUSL(TM)^@"
str33: bytes "GovnDate by Ксэ816^@"
str34: bytes "С инкрементацией!^@"
str35: bytes "^$ diff gd.old gd.new^@"
str36: bytes "1c1^@"
str37: bytes "< GovnDate=28429^@"
str38: bytes "---^@"
str39: bytes "> GovnDate=28430^@"
str40: bytes "Скотчать^@"
str41: bytes "GovnWeb 2 (Alpha)^@"
str42: bytes "1^@"
str43: bytes "2^@"
str44: bytes "  100kg/32MiB|08-23-2025^@"
str45: bytes "GovnWeb 2^@"
str46: bytes "Gnkui^@"
str47: bytes "x^@"
str48: bytes "gnkui.github.io^@"
str49: bytes "/^@"
str50: bytes "Фанфик про шлюкси^@"
str51: bytes "Персонажи: Люкси и Гнкуи^@"
str52: bytes "Часть 1$Пасмурдный день апреля, Гнкуи спокойно спа$ла. Но вдруг телефон завибрировал.Гнкуи н$ехотя поднялась с кровати, и проверила тел$ефон. Это был Люксидев. Гнкуи пробормотала$ под нос. -И чего ему надо. В сообщении го$ворилось.-Привет.Не хочешь прогулятся?.Гн$куи насупилась и напечатала.-Извини за воп$рос.Но зачем писать в такую рань? Не долго$думая, Люксидев ответил.-Как можно так до$лго спать.Здоровые люди уже на ногах, а ты$спишь.Гнкуи не предала значения его словам,$и быстро ответила.-Ладно, говори во скольк$о.Люксидев быстро написал.-9:00, я буду жд$ать около твоего дома.Гнкуи посмотрела на$часы.8:40.^@"
str53: bytes "GovnWeb 2^@"
str54: bytes "Рика Фуруде - ShutApp^@"
str55: bytes "ПО `мой пк`^@"
str56: bytes "00:00:00^@"
str57: bytes "09.19.25^@"
str58: bytes "GovnWeb2^@"
str59: bytes "ShutApp^@"
str60: bytes "GovnShell.py [Version 25.2.8]$(c) 2025 Govn Industries & t.me/pyproman$$You are ^@"
str61: bytes " levels away from boot$^@"
str62: bytes "^[[33msh.py>^[[0m ^@"
str63: bytes "help^@"
str64: bytes "Welcome to a shell compiled with Lusl 🤑!$help             Show help$dir              List tags$fib              Calculate fibbonaci$newsh            Start a new shell$shutdown         int^$0(0)$exit             Exit shell$scls             Clear Screen$lost             Enter shedevrolostcore emulator$graphics         XOR Graphics demo$render meme      Render meme to screen$render presa     Render presa to screen$render geometry  Render geometry to screen$render lost10    Render lost10 to screen$^@"
str65: bytes "dir^@"
str66: bytes "Your disk is cooked bro$^@"
str67: bytes "fib^@"
str68: bytes "scls^@"
str69: bytes "render meme^@"
str70: bytes "render presa^@"
str71: bytes "render geometry^@"
str72: bytes "render lost10^@"
str73: bytes "graphics^@"
str74: bytes "lost^@"
str75: bytes "newsh^@"
str76: bytes "shutdown^@"
str77: bytes "exit^@"
str78: bytes "Deleting C:/System32...$ok$^@"
str79: bytes "Your command is bad... just like that apple$^@"
str80: bytes "Goodbye!$^@"
