.data
  _string: .asciz "Welcome "

# Actual Code
.text
.global show
.type show, @function

show:
  sub  $8, %rsp
  mov  $1, %rax
  lea  _string(%rip), %rdi
  call printf
  add  $8, %rsp         
  ret
