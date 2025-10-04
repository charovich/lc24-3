  jmp main
; puts - Output string to stdout(DevConsole).
; dt - str addr
puts:
  cmp *%dt $00
  re
  push *%dt
  int $02
  inx %dt
  jmp puts
main:
  ldt str0
  jsr puts
  int $0
str0:            bytes "Hello, World From kernel!$^@"
