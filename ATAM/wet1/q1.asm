
.section .text
q1:
        movq $0, %rax
        movq $0, %rdx
        movq $0, %rbx
        # gather input:
        movl a, %eax
        movl b, %ebx
        cdq 
        
        idiv %ebx # actual division
        # gather output:
        movl %eax, d
        movl %edx, r 
	ret		#use ret to return from your code (like "return" in c\c++)


############### remove the following code before submission
