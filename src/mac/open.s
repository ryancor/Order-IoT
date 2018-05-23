.text
.global _verify

// Create if(open()) gcc -S open.c

_verify:
  push    %rbp
  mov     %rsp, %rbp
  sub     $16, %rsp
  lea     _file(%rip), %rdi
  xor     %esi, %esi
  movl    $0, -4(%rbp)
  // We would add a 1 :: mov $1, %esi for WRONLY, but we are only doing RDONLY
  movb    $0, %al
  call    _open
  mov     %eax, -8(%rbp)
  cmpl    $0, -8(%rbp)  // rax == 0x3 if equal, 0xffffffffffffffff	-1 if not
  jge     CALIT         // jump if rax > rdx

  // Create file
  mov     $85, %rax     // sys_create
  syscall

  // File created sucessfully?
  mov     $0, %rdx
  cmp     %rdx, %rax
  jmp     EX

CALIT:
  lea     _string(%rip), %rdi
  movb    $0, %al
  call    _printf
  mov     %eax, -16(%rbp)
  mov     -4(%rbp), %eax
  add     $16, %rsp
  pop     %rbp
  ret

EX:
  // sys_exit(return_code)
  mov     $60, %rax        	// sys_exit
  mov     $1, %rdi        	// exit 1 (fail)
  call    _exit


.data
_file:    .asciz "receipt.txt"		// File to write
_string:  .asciz "Receipt Created Successfully.\n\n"
