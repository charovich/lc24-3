ldt $49FF00
lda $03
storb %ac

ldt $4A0002
lda $7FFF
storw %ac
jmp main

main:
  ldt buf
  jsr vgetc
  int $0

vgets:
  psh %ac
  ldb $00
  lda $00
.lp:
  jsr vgetc
  cmp %ac $7F
  jme .back
  jsr vputc
  cmp %ac $0A
  jme .end
  sb %dt %ac
  inx %ac
  jmp vgets.lp
.back:
  cmp %ac $00
  jme vgets.lp
.back_strict:
  psh %dt
  ldt bs_seq
  jsr vgets
  pop %dt
  sb %dt %bs
  sub %dt 2
  dex %ac
  jmp vgets.lp
.end:
  lda $00
  sb %dt %ac
  pop %ac
  rts

vgetc:
  psh %bs psh %cn psh %dt psh %di
  mov %di 0 ; 0 - cell clear
  jsr .invert
  mov %bs 0 ; 0 - wait for press, >=1 - key, wait for unpress
  mov %cn 0
.loop:
  jsr getnorkey
  cmp %bs 0
  jme .waiter
  cmp %ac 0
  jmne .achk
  cmp %di 0
  jme .dontclear
  jsr .invert
.dontclear:
  mov %ac %bs
  pop %di pop %dt pop %cn pop %bs
  rts
.waiter:
  cmp %ac 0
  jme .achk
  cmp %ac $39
  jl .txtkey
  sub %ac $39
  mov %dt .extra
  add %dt %ac
  lb %dt %bs
  jmp .achk
.txtkey:
  sub %ac 4
  jsr getmodkey
  mul %dc 53
  mov %dt .layout
  add %dt %dc
  add %dt %ac
  lb %dt %bs
.achk:
  mov %dc 33
  int $22
  inx %cn
  cmp %cn 15
  jmne .loop

.blink:
  psh .loop
.invert:
  mov %cn *cur.posh
  add %cn %cn
  add %cn $4F0001
  lb %cn %ac
  mov %dc %ac
  not %ac
  dex %cn
  sb %cn %ac
  int $11
  not %di
  mov %cn 0
  rts
.layout: bytes "abcdefghijklmnopqrstuvwxyz1234567890" $0A $1B $7F $09 " -=[]\`;'`,./"
.shiftd: bytes "ABCDEFGHIJKLMNOPQRSTUVWXYZ!@#^$%"$5E"&*()" $0A $1B $7F $09 " _+{}|~:\"~<>?"
.extra:  bytes $13 $E0 $E1 $E2 $E3 $E4 $E5 $E6 $E7 $E8 $E9 $EA $EB $14 $17 $12 $07 $C3 $C2 $FE $B4 $C1 $10 $11 $1F $1E "^@^@^@^@^@^@^@^@^@^@^@^@^@^@^@^@^@^@^@^@^@^@^@^@^@^@^@"
.ctrld:  bytes "^A^B^C^D^E^F^G^H^I^J^K^L^M^N^O^P^Q^R^S^T^U^V^W^X^Y^Z" ; Ctrl+A-Z
  bytes $AD  ; Ctrl+1 -- Reversed !
  bytes $FD  ; Ctrl+2 -- Superscript 2
  bytes $FC  ; Ctrl+3 -- Superscript n
  bytes $AB  ; Ctrl+4 -- 1/2
  bytes $AC  ; Ctrl+5 -- 1/4
  bytes "^^" ; Ctrl+6 -- Up arrow
  bytes $A8  ; Ctrl+7 -- Reversed ?
  bytes $F8  ; Ctrl+8 -- Degree sign
  bytes $AE  ; Ctrl+9 -- Opening double quote
  bytes $AF  ; Ctrl+0 -- Closing double quote

  bytes $F7  ; Ctrl+Enter -- Approximately equal
  bytes $FE  ; Ctrl+Esc -- Small rectangle
  bytes $F1  ; Ctrl+Bksp -- Plus-minus
  bytes $EC  ; Ctrl+Tab -- Infinity
  bytes $F0  ; Ctrl+Space -- Identically equal sign

  bytes $FB  ; Ctrl+- -- Square root sign
  bytes $F6  ; Ctrl+= -- Division sign

  bytes "^[" ; Ctrl+[ -- Small left arrow
  bytes "^]" ; Ctrl+] -- Left-right arrow
  bytes $1C  ; Ctrl+\ -- Right angle sign
  bytes "%"  ; Ctrl+NoNuSlash -- idk
  bytes $FA  ; Ctrl+; -- Rouble sign
  bytes $9F  ; Ctrl+' -- Function sign
  bytes $9C  ; Ctrl+` -- Pound sign
  bytes $F4  ; Ctrl+, -- Upper half integral
  bytes $F5  ; Ctrl+. -- Lower half integral
  bytes $9B  ; Ctrl+/ -- Copyright sign

getmodkey: ; %dc - Mod bitmask
  mov %dt $480005
.sloop:
  lb %dt %dc
; Shift only for now
  cmp %dc $E1
  jme .setshift
  cmp %dc $E0
  jme .setctrl
  cmp %dc $E5
  jme .setshift
  cmp %dc $E4
  jme .setctrl
  cmp %dt $48000B
  jmne .sloop
  mov %dc 0
  rts
.setshift:
  mov %dc 1
  rts
.setctrl:
  mov %dc 3
  rts

getnorkey: ; %ac - Key
  mov %ac 0
  mov %dt $480005
.loop:
  lb %dt %ac
  cmp %ac 4
  jl .aloop
; *Lock keys
  cmp %ac $39
  jme .aloop
  cmp %ac $47
  jme .aloop
  cmp %ac $53
  jme .end
.aloop:
  cmp %dt $48000B
  jmne .loop
  rts
.end:
  rts

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

vputc:
  mov %dt %ac
  jsr vputs
  ret

vputscur: bytes $00$00
cur: .posh: reserve 2 bytes .c: reserve 1 bytes
bs_seq: bytes "^H ^H^@"
buf: reserve 255 bytes
