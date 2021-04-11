
.section .text
q3:
        cmp  $0, n  # checks for a null array
        je   nullArr
        movq $0x0, %rcx # counter
        movl $0x1, %r8d  # current length
        movl $0x0, %r9d  # current begin index
        movq $arr, %r10  #prev address
        movl n, %r11d
        dec  %r11d
       # movq $arr, %r11  #curr address
        movl 0(%r10), %eax    # prev element
        movl 4(%r10), %ebx  # current element  
        
loop1:
        # compare last 2 elements 
        cmp %eax, %ebx
        jg greater
        jmp notGreater
greater:
        # ascending
        inc %r8d
        jmp continue
        
notGreater:
        # not ascending
        cmp len, %r8d # new length record
        jg  newMax
        jmp newSequence
        
newMax:        
        # output the new max values
        movl %r9d, begin
        movl %r8d, len
        jmp newSequence
        
newSequence:
        # resets current sequence values
        movl $1 ,  %r8d
        movl %ecx, %r9d
        inc  %r9d
        jmp continue
        
continue:
        # go to next element
        add $1, %ecx
        cmp %ecx, %r11d # the last sequence
        je  notGreater 
        cmp %ecx, n     # end of loop
        je end        
        movl %ebx, %eax
        movl 4(%r10, %rcx, 4), %ebx 
        jmp loop1
nullArr:
        movl $0, len
        movl $0, begin
        jmp end

end:	
        ret		#use ret to return from your code (like "return" in c\c++)


############### remove the following code before submission
