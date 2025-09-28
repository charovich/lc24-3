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

s2i:
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


buf: reserve 64 bytes
main:
  mov %bp 3
  sub %sp %bp
  mov %bp %sp
  inx %bp
  mov %ac 1
  mov %dt 0
  add %dt %bp
  storw %ac
  psh %bp
  mov %ac str0
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
  mov %ac str1
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
  mov %ac buf
  psh %ac
  call puts
  pop %ac
  pop %bp
  psh %bp
  mov %ac str2
  psh %ac
  call puts
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
str0: bytes "Hello, World!^@"
str1: bytes "# ^@"
str2: bytes "$^@"
