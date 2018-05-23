;;section .text
global _read

_read:
  mov   rax, 1
  mov   rdi, 1
  mov   rsi, startmsg
  mov   rdx, sizestart
  syscall

  ;using sys_read
  mov   ax, 0
  mov   rdi, 0
  mov   rsi, key
  mov   rdx, 2
  syscall

  mov   rbx, 0x1b
  cmp   bl, key
  je _exit

_exit:
  mov   rax, 60
  mov   rdi, 0
  syscall

;;section .bss
; key reserves word of 1 but no .bss in .SO files
;;key resw 1

section .data
key         equ 1
startmsg    db 'Press a Key', 10
sizestart   equ $-startmsg
