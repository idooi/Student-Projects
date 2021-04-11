
.section .text
q5:
        # gather input
        movq b, %rbx 
        movq a, %rax
        movq $a, %rcx
        cmp %rax, %rbx # get greater number to r15 and smaller to r14
        jg  bGreater
        # a > b
        movq %rax, %r15 
        movq %rbx, %r14
        jmp endGreater
bGreater:
        # b > a
        movq %rax, %r14 
        movq %rbx, %r15
        jmp endGreater
endGreater:
        cmp  $0, %r14 # if one of the inputs is zero return zero quotient
        je   endOfDivision
        jmp  mainLoop
mainLoop: 
        # prepare for division
        movq %r14,  %rbx
        movl %r15d, %eax
        movq %r15,  %rdx
        shr  $32,   %rdx
        # actual division
        div  %rbx
        # next iteration
        movq %r14, %r15
        movq %rdx, %r14
        cmp  $0,   %r14 # reminder is zero
        jne  mainLoop
        jmp  endOfDivision
        
endOfDivision:
        movq %r15, c
	ret		#use ret to return from your code (like "return" in c\c++)


############### remove the following code before submission
