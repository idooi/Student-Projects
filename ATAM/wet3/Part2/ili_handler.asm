.extern what_to_do
.global my_ili_handler
.data
rdiReg: .quad -1
raxReg: .quad -1
.text
.align 4, 0x90
my_ili_handler:
  movq %rdi, rdiReg 
  movq %rax, raxReg 
  movq (%rsp), %rdi # get instruction address
  movzxb 1(%rdi), %rdi # get instruction
  pushq %rsi
  pushq %rdx
  pushq %rcx
  pushq %r8	
  pushq %r9	
  pushq %r10
  pushq %r11
  
 
  call  what_to_do
  cmp   $0, %al
  je    orig
  # else - just continue 
  popq %r11
  popq %r10
  popq %r9
  popq %r8
  popq %rcx
  popq %rdx
  popq %rsi
  
  # make rip point to the next instruction
  popq %rdi
  addq $2, %rdi
  pushq %rdi
  # restore rdi
  movq rdiReg, %rdi
  iretq
   
orig:
  popq %r11
  popq %r10
  popq %r9
  popq %r8
  popq %rcx
  popq %rdx
  popq %rsi
  movq rdiReg, %rdi
  movq raxReg, %rax
  
   jmp *old_ili_handler
   iretq

