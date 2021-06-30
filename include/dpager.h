#ifndef __PAGER_H__
#define __PAGER_H__

#define OPTIMAL 4
#define PHDR_MAX 20
#define PAGE_SIZE	0x1000
#define STACK_SIZE	(PAGE_SIZE * 10) //same as 0xa000
#define STACK_START	0xc0000000	
#define STACK_TOP	(STACK_START + STACK_SIZE)
#define ELF_MIN_ALIGN	PAGE_SIZE
#define MAX_ARG_STRLEN	PAGE_SIZE
#define AT_VECTOR_SIZE_BASE	20
#define AT_VECTOR_SIZE (2*(AT_VECTOR_SIZE_BASE + 1))

#define ELF_PAGESTART(_v) ((_v) & ~(unsigned long)(ELF_MIN_ALIGN-1))
#define ELF_PAGEOFFSET(_v) ((_v) & (ELF_MIN_ALIGN-1))
#define ELF_PAGEALIGN(_v) (((_v) + ELF_MIN_ALIGN - 1) & ~(ELF_MIN_ALIGN - 1))
#define STACK_ADD(sp, items) ((Elf64_Addr *)(sp) - (items))
#define STACK_ROUND(sp, items) \
	(((unsigned long) ((Elf64_Addr *) (sp) - (items))) &~ 15UL)

//int load_elf(Elf64_Ehdr *ep);
int load_elf();
void view_phdr();
void *elf_map(Elf64_Addr addr, int prot, int type, Elf64_Phdr *phdr);
int do_bss(unsigned long start, unsigned long end, int prot);
int build_stack(int argc, int envc, char *envp[]);
int init_stack(int argc, char *argv[], int envc, char *envp[]);
int clean_and_jump(Elf64_Addr elf_entry);
int closest_index(int size, int currentLocation, int * array);
int farthest_index(int size, int currentLocation, int * array);
void predict_map(int next_index);
void segv_handler(int signo, siginfo_t *siginfo, void* context);

#endif
