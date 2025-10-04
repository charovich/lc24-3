ldt $49FF00
lda $03
storb %ac

ldt $4A0002
lda $7FFF
storw %ac
jmp main

vputs:
  ldi $4F0000
  push %dt
    ldt vputscur
    lodsw
    mov %dc %ac
  pop %dt
  mul %ac 2
  add %di %ac
  ldb 1
.lp:
  lda *%dt
  cmp %ac 0
  jme .end
  cmp %ac 10
  jme .newline
  inx %dc
  stgrb %ac
  inx %di
  stgrb %bs
  inx %di
  inx %dt
  jmp .lp
.newline:
  push %bs
  ldb %dc
  sub %di $4F0000
  div %di 2
  add %di 79
  div %di 80
  mul %di 80
  ldd %di
  mul %di 2
  add %di $4F0000
  inx %dt
  pop %bs
  jmp .lp
.end:
  ldt vputscur
  lda %dc
  storb %ac
  ret

vputscur: bytes $00$00

main:
  ldt hw
  jsr vputs
  int $11
  int 1
  push 0
  int $0

hw: bytes "Hellost World!$^@"
