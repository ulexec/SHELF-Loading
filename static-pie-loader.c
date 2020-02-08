/* Minimalistic User Land Exec for STATIC-PIE ELF64 Binaries by @ulexec */

#include "include/embedded.h"
#include "include/static-pie-loader.h"

_Alignas(char) struct ST_stack_template {
	Elf64_auxv_t auxv[6];
} __attribute__((packed)) stack_template = {
	.auxv = {
		[0].a_type = AT_PHDR,
		[0].a_un.a_val = 0,
		[1].a_type = AT_PHNUM,
		[1].a_un.a_val = G_AT_PHNUM,
		[2].a_type = AT_ENTRY,
		[2].a_un.a_val = G_IMAGEBASE + G_AT_ENTRY,
		[3].a_type = AT_RANDOM,
		[3].a_un.a_val = G_IMAGEBASE,
		[4].a_type = AT_PHENT,
		[4].a_un.a_val = sizeof(Elf64_Phdr),
		[5].a_type = AT_NULL
	}
};

int load_embedded_file(uint8_t *buff, size_t buff_size) {
   void *dest_address;
   Elf64_Phdr *_phdr;
   Elf64_Ehdr *ehdr;
   uint8_t *mapped;
   register uint8_t *stack_top; 
   uint64_t entry_offset;
   uint64_t delta;
   uint64_t _rip;

   _phdr = (Elf64_Phdr*)phdr;
   dest_address = (void *)((uint8_t*)PAGEALIGN(_phdr->p_vaddr) + G_IMAGEBASE);
   delta = sizeof(Elf64_Ehdr)+(G_AT_PHNUM*sizeof(Elf64_Phdr));

   if((mapped = mmap(dest_address, PAGEALIGNUP(_phdr->p_memsz) + STACK_SZ, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED, -1, 0)) == MAP_FAILED) {
       	return false;
   }
   memcpy((uint8_t*)mapped+delta, buff, _phdr->p_filesz);

   if(mprotect(mapped, _phdr->p_memsz + STACK_SZ, _phdr->p_flags)) {
       return false;
   }

   stack_top = mapped + (PAGEALIGNUP(_phdr->p_memsz) + STACK_SZ);
   entry_offset = G_IMAGEBASE + G_AT_ENTRY;

   stack_template.auxv[0].a_un.a_val = PIC_RESOLVE_ADDR(phdr);
   memcpy(stack_top-sizeof(stack_template), &stack_template, sizeof(stack_template));

   asm volatile (
		"push %0\n"
		"pop %%rsp\n"
		"mov %1, %%eax\n"
		"xor %%edx, %%edx\n"
		"jmp *%%rax\n"
		:
		: "irm"(stack_top - (sizeof(stack_template) + (sizeof(long) * 3))),
		  "irm"(entry_offset)
		: "rax", "rdx"
   );
}

__attribute__((naked)) void _start(int argc, char **argv, char **envp) {
   load_embedded_file(test, test_len);
}
