; _start
call main int 0 hlt
; C library
puts: pop %di pop %dt call .native push 0 push %di ret
  .native: lb %dt %ac cmp %ac 0 re push %ac int 2 jmp .native
getchar: pop %ac int 1 push %ac ret
beep: pop %ac int $23 push 0 push %ac ret
sleep: pop %ac pop %dc int $22 push 0 push %ac ret
date: pop %ac int $3 push %dc push %ac ret
rand: pop %ac int $21 push %dc push %ac ret
puti: pop %di pop %ac mov %bs 10000000 .loop: div %ac %bs add %ac '0' push %ac int 2 mov %ac %dc div %bs 10 cmp %dc 1 jmne .loop push 0 push %di ret
flip: pop %ac int $11 push 0 push %ac ret
cls: pop %ac int $12 push 0 push %ac ret
main:
  call flip
  pop %ac
.loop0:
  mov %ac 1
  cmp %ac 0
  jme .lend0
  mov %ac .str0
  push %ac
  call puts
  pop %ac
  mov %ac 4718597
  push %ac
  call puti
  pop %ac
  mov %ac .str1
  push %ac
  call puts
  pop %ac
  mov %ac 4718598
  push %ac
  call puti
  pop %ac
  mov %ac .str2
  push %ac
  call puts
  pop %ac
  mov %ac 4718599
  push %ac
  call puti
  pop %ac
  mov %ac .str3
  push %ac
  call puts
  pop %ac
  mov %ac 4718600
  push %ac
  call puti
  pop %ac
  mov %ac .str4
  push %ac
  call puts
  pop %ac
  mov %ac 4718601
  push %ac
  call puti
  pop %ac
  mov %ac .str5
  push %ac
  call puts
  pop %ac
  jmp .loop0
.lend0:
  pop %ac push 0 push %ac
  ret
.str0: bytes "HID: ^@" 
.str1: bytes " ^@" 
.str2: bytes " ^@" 
.str3: bytes " ^@" 
.str4: bytes " ^@" 
.str5: bytes " ^@" 
