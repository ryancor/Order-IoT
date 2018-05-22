.data
  _string: .asciz "Welcome "

# Actual Code
.text
.global _show

_show:
  sub  $8, %rsp           // align rsp to 16B boundary
  mov  $1, %rax
  lea  _string(%rip), %rdi // load address of format string
  call _printf
  add  $8, %rsp           // restore rsp
  ret
