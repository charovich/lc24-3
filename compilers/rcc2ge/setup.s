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
ret_i: bytes $00
my_char: bytes $00 $00
geti:
  mov %ac ret_i
  mov %bs 0
  sb %ac %bs
  mov %ac my_char
  mov %bs 48
  sb %ac %bs
.loop0:
  mov %ac my_char
  lb %ac %ac
  dex %ac
  mov %bs 10
  sub %ac %bs
  cmp %ac 0
  jme .lend0
  mov %ac ret_i
  mov %bs my_char
  lb %bs %bs
  dex %bs
  mov %cn 48
  sub %bs %cn
  mov %cn 10
  mov %dc ret_i
  lb %dc %dc
  dex %dc
  mul %cn %dc
  add %bs %cn
  sb %ac %bs
  mov %ac my_char
  push %ac
  call getchar
  pop %bs
  pop %ac
  sb %ac %bs
  mov %ac my_char
  push %ac
  call puts
  pop %ac
  jmp .loop0
.lend0:
  pop %ac push 0 push %ac
  ret
main:
  mov %ac .str0
  push %ac
  call puts
  pop %ac
  call date
  pop %ac
  push %ac
  call puti
  pop %ac
  mov %ac .str1
  push %ac
  call puts
  pop %ac
  call cls
  pop %ac
  mov %ac ret_i
  mov %bs 0
  sb %ac %bs
.loop0:
  mov %ac ret_i
  lb %ac %ac
  dex %ac
  mov %bs 255
  sub %ac %bs
  cmp %ac 0
  jme .lend0
  mov %ac 4194304
  push %ac
  call rand
  pop %bs
  pop %ac
  add %ac %bs
  mov %bs 15
  sb %ac %bs
  mov %ac ret_i
  mov %bs 1
  mov %cn ret_i
  lb %cn %cn
  dex %cn
  add %bs %cn
  sb %ac %bs
  jmp .loop0
.lend0:
  call flip
  pop %ac
  mov %ac .str2
  push %ac
  call puts
  pop %ac
  call getchar
  pop %ac
  mov %ac .str3
  push %ac
  call puts
  pop %ac
  call geti
  pop %ac
  mov %ac .str4
  push %ac
  call puts
  pop %ac
  mov %ac 500
  push %ac
  call sleep
  pop %ac
  mov %ac .str5
  push %ac
  call puts
  pop %ac
  mov %ac 500
  push %ac
  call sleep
  pop %ac
  mov %ac .str6
  push %ac
  call puts
  pop %ac
  mov %ac 500
  push %ac
  call sleep
  pop %ac
  mov %ac .str7
  push %ac
  call puts
  pop %ac
  mov %ac 10
  mov %bs ret_i
  lb %bs %bs
  dex %bs
  mul %ac %bs
  push %ac
  call puti
  pop %ac
  mov %ac .str8
  push %ac
  call puts
  pop %ac
  mov %ac 10
  mov %bs ret_i
  lb %bs %bs
  dex %bs
  mul %ac %bs
  push %ac
  call beep
  pop %ac
  pop %ac push 0 push %ac
  ret
.str0: bytes "GovnDate: ^@" 
.str1: bytes "$^@" 
.str2: bytes "Press any key (NOT THE SHUTDOWN ONE)...^@" 
.str3: bytes "$Freq/10? ^@" 
.str4: bytes "Beep in 3^@" 
.str5: bytes "^H2^@" 
.str6: bytes "^H1^@" 
.str7: bytes "^H^H^H^HNOW with ^@" 
.str8: bytes "Hz$^@" 
