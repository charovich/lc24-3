; Boot.asm - Lost Bootloader.
  jmp boot

; puts - Output string to stdout(DevConsole).
; dt - str addr
puts:
  cmp *%dt $00
  re
  push *%dt
  int $02
  inx %dt
  jmp puts

; scans - Scan a string from stdin
; dt - buf addr
scans:
  int $01
  pop %dc
  cmp %dc $7F
  jme .backspace
  cmp %dc $08
  jme .backspace
  push %dc
  int $02
  ldi *qptr
  add %dt %di
  storb %dc
  sub %dt %di
  inx *qptr
  cmp %dc $0A
  re
  jmp scans
.backspace:
  ldi *qptr
  cmp %di $00
  jme scans
.backspace_strict:
  dex *qptr
  psh %dt
  ldt bs_seq
  jsr puts
  pop %dt
  jmp scans

; strcmp - Check if two strings are equal
; Arguments:
; ax - first string address
; bx - second string address
; Returns:
; ax - status
strcmp:
  ldt *%ac
  ldi *%bs
  cmp %dt %di
  jmne .fail
  cmp %dt $00
  jme .eq
  inx %ac
  inx %bs
  jmp strcmp
.eq:
  lda $00
  ret
.fail:
  lda $01
  ret

; strcpy - Copy one string into another location
; Arguments:
; ac - Target
; bs - Destination
strcpy:
  ldt *%ac
  ldi %bs
  cmp %dt $00 ; Target has no more bytes to copy
  re
  stgrb %dt
  inx %ac
  inx %bs
  jmp strcpy

; dstrsubset - Find a specific string in the disk
; Arguments:
; dt - address{disk}
; di - address to the string{mem}
; At the end, si should point to the end of that string on the disk
dstrsubset:
  ldb *%di

  call dstrtok ; Load si with the address to the first character (stored in ax)
    cmp %bs $01
    jme .fnf
  push %di
  call dstrcmp ; Compare and store the status into ax
    cmp %ac $00 ; We found the substring (address in si)
  jme .end
  pop %di
  dex %dt
  jmp dstrsubset
.end:
  pop %x
  ldb $00
  ret
.fnf:
  ldt fnf_msg
  call puts
  ldb $01
  ret

; dstrcmp - Check if two strings are equal (first pointer being on the disk)
; Arguments:
; dt - first string address{disk}
; di - second string address
; Returns:
; ac - status
dstrcmp:
  ldds
  ldb *%di

  inx %dt
  inx %di

  cmp %ac %bs
  jmne .fail
  cmp %bs $00
  jme .eq
  jmp dstrcmp
.eq:
  lda $00
  ret
.fail:
  lda $01
  ret

; dstrtok - Progress the string pointer{disk} until character
; Arguments:
; dt - string address
; bs - end character
; (also affects dynptr)
dstrtok:
  ldds
  cmp %ac $F7 ; End of the disk
  jme .error
  cmp %ac %bs
  jme .done
  inx %dt
  jmp dstrtok
.error:
  ldb $01
  ret
.done:
  ldb $00
  ret

; dbstrtok - Progress back the string pointer{disk} until character
; Arguments:
; dt - string address
; bs - end character
; (also affects dynptr)
dbstrtok:
  ldds
  cmp %ac $F7 ; End of the disk
  jme .error
  cmp %ac %bs
  jme .done
  dex %dt
  jmp dstrtok
.error:
  ldb $01
  ret
.done:
  ldb $00
  ret

; gfs_read_signature - Read the signature of the
; drive (GovnFS filesystem)
; Returns:
; magic_byte: magic byte
; drive_letter: drive letter
; disk_size: disk size
; gfs_sign_sernum: serial number
gfs_read_signature:
  ldt $0000 ; magic byte address (disk)
  ldds
  ldt magic_byte
  storb %ac

  ldt $0011 ; drive letter address (disk)
  ldds
  ldt drive_letter
  storb %ac

  ldt $0010 ; disk size (in sectors) address (disk)
  ldds
  ldt disk_size
  storb %ac

  ldt $000C
  ldds
  ldt gfs_sign_sernum
  storb %ac

  ldt $000D
  ldds
  ldt gfs_sign_sernum
  inx %dt
  storb %ac

  ldt $000E
  ldds
  ldt gfs_sign_sernum
  add %dt $02
  storb %ac

  ldt $000F
  ldds
  ldt gfs_sign_sernum
  add %dt $03
  storb %ac

  lda %cn
  ret

; gfs_read_file - Read the file in the drive (GovnFS filesystem) and
; copy the file contents into an address
; dc - tag (UPDATE LATER)
; di - filename
; dt - address to store data from a file
gfs_read_file:
  ldt com_file_full
  str $F1 ; Load $F1 into com_file_full[0]
  ldb %dt
  call strcpy ; Load filename into com_file_full
  lda com_file_sign ; Load file signature into com_file_full
  call strcpy
  ldt %bs
  str $00 ; Load $00 into com_file_full so it doesn't include the
          ; past query

  ldt $001F
  ldi com_file_full
  call dstrsubset
    cmp %bs $01 ; Check for disk end
    re

  ldi %dc                ; load the file into *%dx
  call flcpy
  ldb $00
  ret
gfs_disk_space:
  ldt $0020
  ldb $F7
  call dstrtok
  lda %dt
  inx %ac
  ret

; flcpy - Copy the file contents into memory (assuming
; %dt is already loaded with the disk address to the file)
; Arguments:
; dt - file contents address{disk}
; di - address where the file will be loaded{mem}
flcpy:
  ldds
  cmp %ac $F1
  re
  cmp %ac $E0
  jme .strange
.normie:
  stgrb %ac
  jmp .next
.strange:
  inx %dt
  ldds
  add %ac $E0
  stgrb %ac
  jmp .next
.next:
  inx %dt
  inx %di
  jmp flcpy

; Boot lostos
boot:
  ldt st_msg
  jsr puts
  ; hlt
boot_start:
  ldd $00
  cpuid
  cmp %dc $1C
  jme .lc24_cpu
  jmp .unk_cpu
.lc24_cpu:
  ldt proc_00_msg
  jsr puts
  ldd $01
  cpuid
  cmp %dc $01
  jmne .livecd_warn
  jmp .shell
.unk_cpu:
  ldt proc_unk_msg
  jsr puts
  ldt kp_0_0msg
  jmp fail
.livecd_warn:
  ldt livecd_msg
  jsr puts
.shell:
  call lostos

fail:
  call puts
  hlt

lostos:
  ldt welcome_msg
  jsr puts

  jsr gfs_read_signature

  ldt env_PS
  add %dt 5
  ldb *drive_letter
  storb %bs

  jmp .prompt
.prompt:
  ldt env_PS
  jsr puts
.input:
  ldt cline
  jsr scans
  jmp .process
.term:
  push $00
  int $00
.process:
  ldt cline
  ldi *qptr
  add %dt %di
  dex %dt
  ldd $00
  storb %dc ; Load $00 (NUL) instead of $0A (Enter)
  ldt qptr
  storb %dc

  ; drive
  lda cline
  ldb instFULL_drve
  jsr strcmp
  cmp %ac $00
  jme EXEC_drive

  ; exit
  lda cline
  ldb instFULL_exit
  jsr strcmp
  cmp %ac $00
  jme EXEC_exit

  ; Load the file from the disk
  lda cline
  ldd $3000
  mov %fx com_file_sign
  call gfs_read_file
    cmp %bs $01
    jme .aftexec
  call $3000
.aftexec:
  jmp .prompt
EXEC_exit:
  ldt exit_msg
  jsr puts
  int $0
EXEC_drive:
  ldd $01
  cpuid
  cmp %dc $00
  jme EXEC_driveDSC
EXEC_driveCNN:
  ldt drvCNN_msg00
  call puts
  ldt *drive_letter
  push %dt
  int $02
  ldt drvCNN_msg01
  jsr puts
  jmp lostos.aftexec
EXEC_driveDSC:
  ldt drvDSC_msg
  jsr puts
  jmp lostos.aftexec

; Msg's
st_msg:        bytes "Lost Bootloader.$^@"
welcome_msg:   bytes "Load the kernel by typing `kernel.bin`$^@"
livecd_msg:    bytes "^[[91mLoaded from \"Live Floppy\" image$"
               bytes "Some commands using the GovnFS driver might not work^[[0m$^@"
fnf_msg:       bytes "Bad command or file name.$^@"
exit_msg:      bytes "Exiting...$^@"

; Kernel panic
kp_0_0msg:     bytes "[CE] Kernel panic: Unable to find processor type(0,0)$^@"
kp_1_0msg:     bytes "[CE] Kernel panic: Unknown filesystem(0,1)$^@"
kp_1_1msg:     bytes "[CE] Kernel panic: Could not read disk(1,0)$^@"
kp_6_0msg:     bytes "[CE] Kernel panic: Triggered halt(1,1)$^@"
kp_41_0msg:    bytes "[CE] Kernel panic: Kernel error(1,2)$^@"

; CPU types
proc_00_msg:   bytes "[0000] CPU: ^[[32mLost Core 24^[[0m$$^@"
proc_unk_msg:  bytes "[0000] CPU: ^[[31mUnknown^[[0m$$^@"
proc_00M:      bytes "Lost Core 24$^@"
proc_unkM:     bytes "Unknown :($^@"

; disk msg's
drvCNN_msg00:  bytes "Disk connected as ^@"
drvCNN_msg01:  bytes "/$^@"
drvDSC_msg:    bytes "Disk disconnected, A/ is an empty byte stream.$"
               bytes "Loading without a disk can have serious issues for commands that use GovnFS filesystem$^@"

; GovnFS signatures
com_file_sign: bytes $F2 "com/" $F2 $00
txt_file_sign: bytes $F2 "txt/" $F2 $00
com_file_full: reserve 96 bytes
dir_tag:       bytes $F2 "com/" $F2 $00

; Control sequences
bs_seq:        bytes "^H ^H^@"
cls_seq:       bytes "^[[H^[[2J^@"

; GovnFS Signatures
gfs_sign_sernum: reserve 4 bytes
magic_byte:      reserve 1 bytes
disk_size:       reserve 1 bytes
drive_letter:    reserve 1 bytes

; Environment variables
; 11 bytes
env_PS:        bytes "^[[93m />^[[0m ^@"

; cmds
instFULL_drve: bytes "drive^@"
instFULL_exit: bytes "exit^@"
instFULL_help: bytes "help^@"

; Buffers
cline:         reserve 64 bytes
qptr:          reserve 1 bytes
txtbut:        reserve 255 bytes

bse:           bytes $AA $55
