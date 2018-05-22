.text
.global verify
.type verify, @function

verify:
  push    %rbp
  mov     %rsp, %rbp
  sub     $16, %rsp
  lea     _file(%rip), %rdi
  xor     %esi, %esi
  movl    $0, -4(%rbp)
  movb    $0, %al
  call    open
  mov     %eax, -8(%rbp)
  cmpl    $0, -8(%rbp)
  jge     CALIT

  mov     $85, %rax
  syscall

  mov     $0, %rdx
  cmp     %rdx, %rax
  jmp     EX

CALIT:
  lea     _string(%rip), %rdi
  movb    $0, %al
  call    printf
  mov     %eax, -16(%rbp)
  mov     -4(%rbp), %eax
  add     $16, %rsp
  pop     %rbp
  ret

EX:
  mov     $60, %rax
  mov     $1, %rdi
  call    exit


.data
_file:    .asciz "receipt.txt"
_string:  .asciz "File Created Successfully.\n\n"
