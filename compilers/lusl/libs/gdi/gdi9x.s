border: ; (x, y, w, h, pat)
    pop %bp
    pop %di
    pop %dc
    pop %bs
    pop %dt
    pop %ac
    mul %dt 640
    add %dt %ac
    jsr border_asm
    psh 0
    psh %bp
ret
border_asm:
    psh %dt
.bloop:
    lda *%di
    cmp %ac $10
    jme .fill
    cmp %ac $20
    jme .middle
    cmp %ac $30
    jme .eline
    cmp %ac $F0
    jme .end
    int $C
    inx %dt
    inx %di
    jmp .bloop

.fill:
    inx %di
    lda *%di
    ldc %bs
    dex %cn
.floop:
    int $C
    inx %dt
    loop .floop
    inx %di
    jmp .bloop

.eline:
    pop %dt
    add %dt 640
    psh %dt
    inx %di
    jmp .bloop

.middle:
    inx %di
    psh %di
    ldc %dc
    dex %cn
.mloop:
    lda *%di
    cmp %ac $10
    jme .mfill
    cmp %ac $F0
    jme .mend
    int $C
    inx %dt
    inx %di
    jmp .mloop
.mend:
    lda %di
    pop %di
    pop %dt
    add %dt 640
    psh %dt
    psh %di
    loop .mloop
    pop %di
    ldi %ac
    inx %di
    jmp .bloop

.mfill:
    add %dt %bs
    inx %di
    jmp .mloop

.end:
    pop %ac
    ret
taskbar:
    pop %bp
    ldt 58480
    lda 7
    ldc 640
.floop:
    int $C
    inx %dt
    loop .floop
    lda 15
    ldc 640
.sloop:
    int $C
    inx %dt
    loop .sloop
    lda 7
    ldc 5440
.tloop:
    int $C
    inx %dt
    loop .tloop
    psh %bp
ret
