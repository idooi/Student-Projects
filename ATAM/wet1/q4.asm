
.section .text
q4:
        # %r8 = 1st adress, r9 = 1st prev ,%r10 = 2nd adress, r11 = 2nd prev 
        movq head,    %rbx  # current adress
        movq $0x0   , %rdx  # found counter
        cmp $0x0, %rbx      # NULL end of list
        je  swap
        movq 8(%rbx), %rcx    # next node adress
        movq (%rbx) ,    %rax # current value
        movq $head,   %r15    # prev adress
        sub  $8,  %r15 
        
mainLoop:
        # check for the required elements
        cmp %rax, src
        je  match
        cmp %rax, dst
        je  match
        jmp goNext
match:
        cmp $0, %rdx
        je  loop1
        jmp loop2
loop1:
        #store the first found element
        movq %rbx, %r8 
        movq %r15, %r9
        inc  %rdx
        jmp goNext
        
loop2:
        #store the second found element
        movq %rbx, %r10
        movq %r15, %r11
        inc %rdx
        jmp goNext

goNext:
        cmp $0x0, %rcx  # NULL end of list
        je  swap
        movq %rbx, %r15     # prev adress = current adress
        movq %rcx,    %rbx  # current adress = next adress
        movq 8(%rbx), %rcx  # next node adress = current adress + 8
        movq (%rbx),  %rax  # current value = *(current adress)
        jmp  mainLoop
                 
swap:   # after loop
        # %r8 = 1st adress, r9 = 1st prev ,%r10 = 2nd adress, r11 = 2nd prev 
        cmp $2 ,%rdx # an element not found
        jne end
        movq %r10, 8(%r9) # prev1->next = 2
        movq %r8,  8(%r11)# prev2->next = 1
        movq 8(%r8) ,%r14 # temp = 1->next
        movq 8(%r10), %rax  # 1->next = 2->next
        movq %rax, 8(%r8)
        movq %r14, 8(%r10)  # 2->next = 1->next
        
end:	
        ret		#use ret to return from your code (like "return" in c\c++)


############### remove the following code before submission



