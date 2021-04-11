#include <asm/desc.h>
void my_store_idt(struct desc_ptr *idtr) {
    asm ("sidt %0;"
    : "=m" (*idtr)
    :
    :"%rax"
    );
}

void my_load_idt(struct desc_ptr *idtr) {
    asm ("lidt %0;"
    :
    :"m" (*idtr)
    :
    );
}

void my_set_gate_offset(gate_desc *gate, unsigned long addr) {
    gate->offset_low    = (unsigned short)addr;
    gate->offset_middle = (unsigned short)(addr >> 16) ;
    gate->offset_high   = (unsigned int)(addr >> 32) ;
}

unsigned long my_get_gate_offset(gate_desc *gate) {
    return (unsigned long)gate_offset(gate);
}