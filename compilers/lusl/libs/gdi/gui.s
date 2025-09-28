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
