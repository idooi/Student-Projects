#include <linux/kernel.h>
#include "sched/sched.h"
#include <linux/sched.h>


asmlinkage long sys_hello(void) {
    printk("Hello, World!\n");
    return 0;
}

asmlinkage long get_vruntime(void){
    return current->se.vruntime;
}

asmlinkage long increment_vruntime(long delta){
    if (delta < 0){
        return -EINVAL;
    }
    deactivate_task(this_rq(), current, 0);
    current->se.vruntime += delta;
    activate_task(this_rq(), current, 0);
    return 0;
}

