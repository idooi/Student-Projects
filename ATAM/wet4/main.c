/* Code sample: using ptrace for simple tracing of a child process.
**
** Note: this was originally developed for a 32-bit x86 Linux system; some
** changes may be required to port to x86-64.
**
** Eli Bendersky (http://eli.thegreenplace.net)
** This code is in the public domain.
*/
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <signal.h>
#include <syscall.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#define MAX_REGS 24
#define MAX_VAR_LEN 257
#define START_BLOCK 0
#define END_BLOCK 1
#define MAX_REG_LEN 8

typedef struct varReg_t {
  char *name;
  char *reg;
  unsigned long long int oldValue;
  unsigned long long int newValue;
} *VarReg;

pid_t run_target(const char *programname, char **args) {
  pid_t pid;

  pid = fork();

  if (pid > 0) {
    return pid;

  } else if (pid == 0) {
    /* Allow tracing of this process */
    if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0) {
      perror("ptrace");
      exit(1);
    }
    /* Replace this process's image with the given program */
    execv(programname, args);

  } else {
    // fork error
    perror("fork");
    exit(1);
  }
  //should not get hear
  return pid;
}
void saveValReg(VarReg *mapping, struct user_regs_struct *regs, int location_status) {
  ///int n = 1;
  /// printf("staret saving reg %d \n", n);
  for (int i = 0; i < MAX_REGS; i++) {
    if (mapping[i] != NULL && mapping[i]->reg != NULL) {
      if (strcmp((mapping[i]->reg), "rax") == 0 || strcmp((mapping[i]->reg), "eax") == 0 ||
          strcmp((mapping[i]->reg), "ax") == 0 || strcmp((mapping[i]->reg), "ah") == 0 ||
          strcmp((mapping[i]->reg), "al") == 0) {
        if (location_status == START_BLOCK) {
          mapping[i]->oldValue = regs->rax;
          ///  printf("try to  save rax %d \n", n);
        } else {///END_BLOCK
          mapping[i]->newValue = regs->rax;
        }

      } else if (strcmp((mapping[i]->reg), "rbx") == 0 || strcmp((mapping[i]->reg), "ebx") == 0 ||
                 strcmp((mapping[i]->reg), "bx") == 0 || strcmp((mapping[i]->reg), "bh") == 0 ||
                 strcmp((mapping[i]->reg), "bl") == 0) {
        if (location_status == START_BLOCK) {
          mapping[i]->oldValue = regs->rbx;
        } else {///END_BLOCK
          mapping[i]->newValue = regs->rbx;
        }
      } else if (strcmp((mapping[i]->reg), "rcx") == 0 || strcmp((mapping[i]->reg), "ecx") == 0 ||
                 strcmp((mapping[i]->reg), "cx") == 0 || strcmp((mapping[i]->reg), "ch") == 0 ||
                 strcmp((mapping[i]->reg), "cl") == 0) {
        if (location_status == START_BLOCK) {
          mapping[i]->oldValue = regs->rcx;
        } else {///END_BLOCK
          mapping[i]->newValue = regs->rcx;
        }
      } else if(strcmp((mapping[i]->reg), "rdx") == 0 || strcmp((mapping[i]->reg), "edx") == 0 ||
                strcmp((mapping[i]->reg), "dx") == 0 || strcmp((mapping[i]->reg), "dh") == 0 ||
                strcmp((mapping[i]->reg), "dl") == 0) {
        if (location_status == START_BLOCK) {
          mapping[i]->oldValue = regs->rdx;
        } else {///END_BLOCK
          mapping[i]->newValue = regs->rdx;
        }
      } else {
        if (location_status == START_BLOCK) {
          mapping[i]->oldValue = regs->rsi;
        } else {///END_BLOCK
          mapping[i]->newValue = regs->rsi;
        }
      }
    }
    ///something is wrong
    ///printf("we save non reg val");
    /// n++;
  }
}
void printChangedVar(VarReg *mapping) {
  int num_reg = 0;

  for(int i = 0 ; i < MAX_REGS ; i++) {
    if(mapping[i] != NULL) {
      num_reg++;
    } else {
      break;
    }
  }

  //sorting by var name
  for(int i = 0 ; i < num_reg - 1; i++) {
    for(int j = 0 ; j < num_reg - 1; j++) {
      if (strcmp(mapping[j]->name, mapping[j + 1]->name) > 0) {
        VarReg temp = mapping[j];
        mapping[j] = mapping[j + 1];
        mapping[j + 1] = temp;

      }
    }
  }

  ///print only if val change
  for(int i = 0 ; i < num_reg ; i++) {
    if(strcmp((mapping[i]->reg), "rax") == 0 || strcmp((mapping[i]->reg), "rbx") == 0 ||
        strcmp((mapping[i]->reg), "rcx") == 0 || strcmp((mapping[i]->reg), "rdx") == 0 ||
        strcmp((mapping[i]->reg), "rsi") == 0) {
      signed long long int old = (signed long long int)mapping[i]->oldValue;
      signed long long int new = (signed long long int)mapping[i]->newValue;
      if(old != new ) {
        printf("PRF:: %s: %lld->%lld\n", mapping[i]->name, old, new);
      }
    } else if(strcmp((mapping[i]->reg), "eax") == 0 || strcmp((mapping[i]->reg), "ebx") == 0 ||
              strcmp((mapping[i]->reg), "ecx") == 0 || strcmp((mapping[i]->reg), "edx") == 0 ||
              strcmp((mapping[i]->reg), "esi") == 0) {

      unsigned long long int old = mapping[i]->oldValue;
      unsigned long long int new = mapping[i]->newValue;
      void *ptrOld = ((void *) &old);
      void *ptrNew = ((void *) &new);
      signed int old_val = *(signed int *)ptrOld;
      signed int new_val = *(signed int *)ptrNew;

      if( old_val != new_val) {
        printf("PRF:: %s: %d->%d\n", mapping[i]->name, old_val, new_val);
      }
    } else if(strcmp((mapping[i]->reg), "ax") == 0 || strcmp((mapping[i]->reg), "bx") == 0 ||
              strcmp((mapping[i]->reg), "cx") == 0 || strcmp((mapping[i]->reg), "dx") == 0 ||
              strcmp((mapping[i]->reg), "si") == 0) {
      unsigned long long int old = mapping[i]->oldValue;
      unsigned long long int new = mapping[i]->newValue;

      void *ptrOld = ((void *) &old );
      void *ptrNew = ((void *) &new );

      signed short old_val = *(signed short *) ptrOld;
      signed short new_val = *(signed short *) ptrNew;
      if (old_val != new_val) {
        printf("PRF:: %s: %hi->%hi\n", mapping[i]->name, old_val, new_val);
      }
    } else if(strcmp((mapping[i]->reg), "ah") == 0 || strcmp((mapping[i]->reg), "bh") == 0 ||
              strcmp((mapping[i]->reg), "ch") == 0 || strcmp((mapping[i]->reg), "dh") == 0 ) {

      unsigned long long int old = mapping[i]->oldValue;
      unsigned long long int new = mapping[i]->newValue;

      void *ptrOld = ((void *) &old ) + 1;
      void *ptrNew = ((void *) &new ) + 1;

      signed char old_val = *(signed char *)ptrOld;
      signed char new_val = *(signed char *)ptrNew;
      if((int)old_val != (int)new_val) {
        printf("PRF:: %s: %d->%d\n", mapping[i]->name, (int)old_val, (int)new_val);
      }
    } else if(strcmp((mapping[i]->reg), "al") == 0 || strcmp((mapping[i]->reg), "bl") == 0 ||
              strcmp((mapping[i]->reg), "cl") == 0 || strcmp((mapping[i]->reg), "dl") == 0 ||
              strcmp((mapping[i]->reg), "sil") == 0 ) {

      unsigned long long int old = mapping[i]->oldValue;
      unsigned long long int new = mapping[i]->newValue;

      void *ptrOld = ((void *) &old) ;
      void *ptrNew = ((void *) &new );

      signed char old_val = *(signed char *)ptrOld;
      signed char new_val = *(signed char *)ptrNew;
      if((int)old_val != (int)new_val) {
        printf("PRF:: %s: %d->%d\n", mapping[i]->name, (int)old_val, (int)new_val);
      }
    } else {
      printf("save non reg in map");
      exit(1);
    }
  }


}
void add_breakpoint1(pid_t child_pid, unsigned long addr1, unsigned long *data1 ) {
  *data1 = ptrace(PTRACE_PEEKTEXT, child_pid, (void *) addr1, NULL);


  /* Write the trap instruction 'int 3' into the address */
  unsigned long data_trap1 = (*data1 & 0xFFFFFF00) | 0xCC;


  ptrace(PTRACE_POKETEXT, child_pid, (void *) addr1, (void *) data_trap1);




}

void add_breakpoint2(pid_t child_pid, unsigned long addr2, unsigned long *data2 ) {

  *data2 = ptrace(PTRACE_PEEKTEXT, child_pid, (void *) addr2, NULL);

  /* Write the trap instruction 'int 3' into the address */

  unsigned long data_trap2 = (*data2 & 0xFFFFFF00) | 0xCC;


  ptrace(PTRACE_POKETEXT, child_pid, (void *) addr2, (void *) data_trap2);



}

/*void print_status(int wait_status, int line ) {

  return;
  printf("%d  ", line);
  if(WIFEXITED(wait_status)) {
    printf("Exit status %d\n", WEXITSTATUS(wait_status));
  } else if(WIFSIGNALED(wait_status)) {
    printf("Terminated by signal %d (%s)%s\n",
           WTERMSIG(wait_status),
           strsignal(WTERMSIG(wait_status)),
           WCOREDUMP(wait_status) ? " (core dumped)" : "");
  } else if(WIFSTOPPED(wait_status)) {
    printf("Stopped by signal %d (%s)\n",
           WSTOPSIG(wait_status),

           strsignal(WSTOPSIG(wait_status)));
    usleep(5000);
  } else if(WIFCONTINUED(wait_status)) {
    printf("Continued\n");
  }

}*/

void run_breakpoint_debugger(pid_t child_pid, unsigned long addr1, unsigned long addr2, VarReg *mapping) {
  int wait_status = 1;
  struct user_regs_struct regs;
  int location_status = START_BLOCK;
  unsigned long data1;
  unsigned long data2;
  /// wait(&wait_status);
  /* Wait for child to stop on its first instruction */
  //    wait(&wait_status);
  /* Look at the word at the address we're interested in */
  add_breakpoint1(child_pid, addr1, &data1);

  ptrace(PTRACE_CONT, child_pid, NULL, NULL);

  while (1) { ///|| !WIFSIGNALED(wait_status) || !WIFSTOPPED(wait_status)

    wait(&wait_status);
    //print_status(wait_status, __LINE__);


    if(WIFEXITED(wait_status) ||  WIFSIGNALED(wait_status)) {
      break;
    }

    ptrace(PTRACE_GETREGS, child_pid, 0, &regs);

    saveValReg( mapping, &regs, location_status);
    ptrace(PTRACE_POKETEXT, child_pid, (void *) addr1, (void *) data1);
    regs.rip -= 1;
    ptrace(PTRACE_SETREGS, child_pid, 0, &regs);
    location_status = END_BLOCK;

    /* The child can continue running now */

    add_breakpoint2(child_pid, addr2, &data2);
    ptrace(PTRACE_CONT, child_pid, NULL, NULL);

    wait(&wait_status);
    //print_status(wait_status, __LINE__);
    if(WIFEXITED(wait_status) ||  WIFSIGNALED(wait_status)) {
      break;
    }

    ptrace(PTRACE_POKETEXT, child_pid, (void *) addr2, (void *) data2);
    ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
    regs.rip -= 1;
    ptrace(PTRACE_SETREGS, child_pid, 0, &regs);

    /* Make the child execute another instruction */
    if (ptrace(PTRACE_SINGLESTEP, child_pid, NULL, NULL) < 0) {
      perror("ptrace");
      exit(1);
    }

    /* Wait for child to stop on its next instruction */
    wait(&wait_status);
    //print_status(wait_status, __LINE__);

    if(WIFEXITED(wait_status) || WIFSIGNALED(wait_status)) {
      break;
    }

    ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
    saveValReg( mapping, &regs, location_status);
    printChangedVar( mapping);
    location_status = START_BLOCK;

    add_breakpoint1(child_pid, addr1, &data1);
    ptrace(PTRACE_CONT, child_pid, NULL, NULL);
  }

}
void freeMapping(VarReg* mapping){
    for(int i = 0 ;i < MAX_REGS;i++)
    {
        if(mapping[i]){
            free(mapping[i]->name);
            free(mapping[i]->reg);
            free(mapping[i]);
        }
    }
    free(mapping);
}

int main(int argc, char **argv) {

  VarReg *mapping = malloc(sizeof(mapping) * MAX_REGS); //need to init
  for(int i = 0; i < MAX_REGS; i++) {
    mapping[i] = NULL;
  }
  // get variable list from the user
  char *name = malloc(sizeof(char) * MAX_VAR_LEN);
  char *reg = malloc(sizeof(char) * MAX_REG_LEN);

  scanf("%s", name);
  scanf("%s", reg);
  int j = 0;
    char *nameToSave;
    char *regToSave;
    while ((strcmp(name, "run") != 0 || strcmp(reg, "profile") != 0 ) && j < MAX_REGS) {

    VarReg newVar = malloc(sizeof(*newVar));

    nameToSave = malloc(sizeof(char) * MAX_VAR_LEN);
    regToSave= malloc(sizeof(char) * MAX_REG_LEN);
    strcpy(nameToSave, name);
    strcpy(regToSave, reg);
    newVar->name = nameToSave;
    newVar->reg = regToSave;
    newVar->oldValue = 0;
    newVar->newValue = 0;
    mapping[j] = newVar;
    j++;

    scanf("%s", name);
    scanf("%s", reg);
  }
    free(name);
free(reg);
  unsigned long startAddr;
  unsigned long endAddr;
  startAddr = strtoul(argv[1], NULL, 16);
  endAddr   = strtoul(argv[2],    NULL, 16);

  pid_t child_pid;
  int wait_status = 0;
  int location_status = START_BLOCK;
  child_pid = run_target(argv[3], argv + 3);
  wait(&wait_status);
  // run specific "debugger"
  run_breakpoint_debugger( child_pid, startAddr, endAddr, mapping);
  wait(&wait_status);
  /// relases mem
  freeMapping(mapping);

  return 0;
}
