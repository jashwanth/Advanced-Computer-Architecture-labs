#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
struct tcs {
        unsigned long enclave_rsp_heap;
        unsigned long p_saved_regs_e;
        bool saved;
        unsigned long res1;
        unsigned long flags;
        unsigned long ossa;
        unsigned int cssa;
        unsigned int nssa;
        unsigned long oentry; // entry point
        unsigned long res2;
        unsigned long ofsbasgx;
        unsigned long ogsbasgx;
        unsigned int fslimit;
        unsigned int gslimit;
        char res3[4024];
};

struct tcs* p_tcs;

void enclave_program() {
   void *return_address;
   asm volatile ( "mov %%rcx, %0\n\t"
                  : "=r" (return_address) ::
                );
   printf("We are now in the enclave, cheers!\n");
   asm volatile ("mov %0, %%rbx \n\t"
                 "movl $0x04 , %%eax\n\t" // EEXIT
                 ".byte 0x0f,0x01,0xd7"
                 :
                 : "r" (return_address));
}

int main() {
        p_tcs = (struct tcs *)malloc(sizeof(struct tcs));
        p_tcs->oentry = (unsigned long)enclave_program;
        asm volatile ("mov %0, %%rbx\n\t"
        "movl $0x02, %%eax\n\t" // EENTER, instruction type
        ".byte 0x0f,0x01,0xd7\n\t" // call opcode 01d7
        :
        : "r" (p_tcs)
        : "eax", "rbx"
        );
   printf("return from Enclave program!\n");
   return 0;
}
