.section .text
q2:
   movq $0x0,  %rcx
        movq $0x0,  %rax 
        movq $0x0,  %rdx
        subq $1,    %rsp
        movq %rsp,  %r9  
        #gather input:
        movl num,  %r8d
        movq $source, %rax
        movq $destination , %rdx
        
CPSTK:  # copy info to stack 
        # copy next byte:
        movb (%rax, %rcx, 1), %bl
        movb  %bl, (%rsp)
        add  $1, %rcx
        subq $1, %rsp      
        cmp %rcx, %r8 # end of source
        jne CPSTK    
        
        movq %r9, %rsp # start copying from top of stack
        movq $0, %rcx
CPDEST: # copy to dest from stack
        # copy next byte:
        movb (%rsp), %bl
        movb %bl, (%rdx, %rcx, 1)
        addq $1, %rcx
        subq $1, %rsp       
        cmp  %rcx, %r8  # end of source
        jne CPDEST
        
        movq %r9, %rsp
        addq $1,  %rsp
	ret		#use ret to return from your code (like "return" in c\c++)
