#include <sys/mman.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <sys/syscall.h>
#include <elf.h>

#define STACK_SZ (1024 * 32)
#define PAGEALIGN(k)    (((k)+((0x1000)-1))&(~((0x1000)-1)))
#define PAGEALIGNDOWN(k)    (((k)+((0x1000)-1))&(~((0x1000)-1))) - 0x1000
#define PAGEALIGNUP(k)    (((k)+((0x1000)-1))&(~((0x1000)-1))) + 0x1000
#define O_ACCMODE       00000003
#define O_RDONLY        00000000
#define O_WRONLY        00000001
#define O_RDWR          00000002
#define NULL	0
#define offsetof(type, member) (__builtin_offsetof(type, member))
#define RLONG(base, offset) ((long)base[offset + 7] << (64 - 8) | (long)base[offset + 6] << (64 - 8 * 2) | (long)base[offset + 5] << (64 - 8 * 3) | (long)base[offset + 4] << (64 - 8 * 4) | (long)base[offset + 3] << (64 - 8 * 5) | (long)base[offset + 2] << (64 - 8 * 6) | (long)base[offset + 1] << (64 - 8 * 7) | (long)base[offset + 0] << (64 - 8 * 8))
#define RSHORT(base, offset) ((short)base[offset + 1] << (16 - 8) | (short)base[offset + 0] << (16 - 8 * 2))
#define PIC_RESOLVE_ADDR(target) (get_rip() - ((char *)&get_rip_label - (char *)target))
extern unsigned long get_rip_label;

unsigned long get_rip(void) {
	unsigned long ret;

	asm volatile(
		"call get_rip_label     \n"
		".globl get_rip_label   \n"
		"get_rip_label:         \n"
		"pop %%rax              \n"
		"mov %%rax, %0" : "=r"(ret)
	);

        return ret;
}

asm (
    ".text\n"
    ".global mmap\n"
        ".type mmap,@function\n"
        "mmap:\n\t"
		"mov    %rcx,%r10\n\t"
		"mov    $0x9,%eax\n\t"
		"syscall \n\t"
		"cmp    $0xfffffffffffff001,%rax\n\t"
		"jae    .Lx1\n\t"
		".Lx2:\n\t"
		"retq   \n\t"
		".Lx1:\n\t"
		"mov    2623486(%rip),%rcx\n\t"
		"xor    %edx,%edx\n\t"
		"sub    %rax,%rdx\n\t"
		"mov    %edx,%fs:(%rcx)\n\t"
		"or     $0xffffffffffffffff,%rax\n\t"
		"jmp    .Lx2\n"
    );

int mprotect(void *addr, unsigned long len, int prot) {
	long ret;
	asm volatile ("syscall" : "=a" (ret) : "a" (__NR_mprotect),
		      "D" (addr), "S" (len), "d" (prot) :
		      "cc", "memory", "rcx",
		      "r8", "r9", "r10", "r11" );
	if (ret < 0) {
		ret = -1;
	}
	return (int) ret;
}

void * memcpy(void *dest, const void *src, unsigned long n) {
	unsigned long i;
	unsigned char *d = (unsigned char *)dest;
	unsigned char *s = (unsigned char *)src;

	for (i = 0; i < n; ++i)
		d[i] = s[i];

	return dest;
}

