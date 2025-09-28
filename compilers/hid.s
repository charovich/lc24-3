jmp main

getmousepos:
  psh %di
  mov %dt $480000
  lw %dt %dt
  mov %di $480002
  lw %di %di
  mul %di 640
  add %di %dt
  add %di $400000
  mov %dt %di
  pop %di
  rts

draw_mouse:
  jsr getmousepos
  int $13
  int $11
  int $12
  mov %dc 16
  int $22

main:
  mov %di mouse
  jsr draw_mouse
  jmp main

mouse:
  bytes $12 $12 $00 $00 $00 $00 $00 $00
  bytes $12 $0F $12 $00 $00 $00 $00 $00
  bytes $12 $0F $0F $12 $00 $00 $00 $00
  bytes $12 $0F $0F $0F $12 $00 $00 $00
  bytes $12 $0F $0F $0F $0F $12 $00 $00
  bytes $12 $0F $0F $0F $12 $12 $00 $00
  bytes $12 $0F $12 $0F $12 $00 $00 $00
  bytes $12 $12 $12 $0F $12 $00 $00 $00
