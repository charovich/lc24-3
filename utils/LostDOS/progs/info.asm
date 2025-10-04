start: jmp infomain
; puts - Output string to stdout(DevConsole).
; dt - str addr
puts:
  cmp *%dt $00
  re
  push *%dt
  int $02
  inx %dt
  jmp puts
infomain:
  ldt OS_RELEASE
  call puts
  ret

OS_RELEASE: bytes "^[[96m"
            extern "imports/info.ans"
            bytes "^[[0m^@"
