.extern transpose_data_chunk
.global isCorrect, find_index_of_errored_row, fix_data_block
.data  
row_index: .int -1
col_index: .int -1
transposed: .byte 0, 0, 0, 0, 0, 0, 0, 0
#.bss
 #   .lcomm transposed, 8
.text

isCorrect:
    mov $0x1, %rax
    add $0x0, %rdi
    jp  isCorrect_end
    mov $0x0, %rax
isCorrect_end:   
    ret
    
    #  mov $data, %rdi
    # mov $row_index, %rsi
find_index_of_errored_row:
    pushq %rbx # curr row index
    pushq %r12 # data
    pushq %r13 # curr row
    pushq %r14 # last error index
    pushq %r15 # num of errors
    mov %rdi, %r12
    mov $-1, %rbx
    mov $0x0, %r15
    mov $-1,  %r14
    
    
find_index_of_errored_row_loop:
    inc %rbx
    cmp $6, %rbx
    je  find_index_of_errored_row_end
    mov (%r12, %rbx, 1), %r13
    mov  %r13, %rdi
    call isCorrect
    cmp $1, %rax
    je  find_index_of_errored_row_loop
    mov %rbx, %r14
    inc %r15
    jmp find_index_of_errored_row_loop
find_index_of_errored_row_end:
    mov %r15, %rax
    mov %r14d, (%rsi)
    popq %r15 
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    ret
    
    #mov $data, %rdi
fix_data_block:
    pushq %rdi
    mov  $row_index, %rsi
    call find_index_of_errored_row
    popq %rdi
    cmp  $1, %rax 
    jg  fix_data_block_no_fix
    cmp  $0, %rax
    je fix_data_block_fixed
    #we have some work to do
    pushq %rdi
    mov  $8, %rsi
    mov  $transposed, %rdx
    call transpose_data_chunk
    # get second index
    popq  %rdi
    pushq %rdi
    mov  $transposed, %rdi
    mov  $col_index, %rsi
    call find_index_of_errored_row
    popq %rdi
    cmp  $1, %rax 
    jg   fix_data_block_no_fix
    cmp  $0, %rax
    je   fix_data_block_fixed
    #actually fix
    mov $0x0, %r8
    mov $0x0, %rcx
    mov row_index, %r8d
    mov col_index, %ecx
    mov $0x0, %r10
    movb  (%rdi, %r8, 1), %r10b
    movb $0b10000000, %r9b
    shr   %cl, %r9b
    xorb  %r9b, %r10b
    movb  %r10b,(%rdi, %r8, 1)
    mov  $2, %rax
    jmp fix_data_block_end
    
fix_data_block_no_fix:
    mov $0, %rax
    jmp fix_data_block_end
    
fix_data_block_fixed:
    mov $1, %rax
    jmp fix_data_block_end
    
fix_data_block_end:
    ret
