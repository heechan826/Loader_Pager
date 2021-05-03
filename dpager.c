#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <elf.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include "dpager.h"

Elf64_Phdr ph_table[PHDR_TABLE_SIZE];
int8_t *stack_ptr, *stack_top, *arg_start, *env_start;
int fd, real_table_size;

int main(int argc, char *argv[], char *envp[])
{
	struct sigaction act;
	Elf64_Ehdr elf_header;
	int envc = 0;

	if (argc < 2) 
	{
		fprintf(stderr, "./apager elf_file\n");
		exit(EXIT_FAILURE);
	}

	if ((fd = open(argv[1], O_RDWR)) < 0) 
	{
		fprintf(stderr, "Fail to open test file\n");
		exit(EXIT_FAILURE);
	}

	if (read(fd, &elf_header, sizeof(Elf64_Ehdr)) < 0) 
	{
		fprintf(stderr, "read error %d\n", __LINE__);
		exit(EXIT_FAILURE);
	}
	
	int i = 0;
	while(envp[i]) envc = ++i;
	
	memset(&act, 0, sizeof(act));
	act.sa_handler = segv_handler;
	act.sa_flags = SA_SIGINFO | SA_RESTART;
	if(sigaction(SIGSEGV, &act, NULL) < 0) fprintf (stderr, "signal(SIGSEGV) error\N");
	
	load_elf(&elf_header);
	init_stack(argc, argv, envc, envp);
	build_stack(argc, envc, envp, &elf_header);
	clean_and_jump(elf_header.e_entry);

	exit (EXIT_SUCCESS);
}

int load_elf(Elf64_Ehdr *ep)
{	
	Elf64_Phdr phdr;
	unsigned long elf_bss = 0, elf_brk = 0;
	int bss_prot = 0;

	// 1. seek in file
	lseek(fd, ep->e_phoff, SEEK_SET);
	real_table_size = ep->e_phnum;
	
	printf("There are %u entries for program header table\n", ep->e_phnum);
	printf("[Construct Program Header table first]\n", ep->e_phnum);
	
	// 2. read fd and fill program header table
	int i;
	for (i = 0; i < ep->e_phnum; i++) 
	{
		if (read(fd, &ph_table[i], sizeof(Elf64_Phdr)) < 0) 
		{
			fprintf(stderr, "read erorr on phdr\n");
			return -1;
		}
		printf("Segment type(phdr.p_type:[%d]): %u \n", i, ph_table[i].p_type); //PT_LOAD == 1
		printf("ph_table[%d].p_vaddr: %p \n", i, ph_table[i].p_vaddr);
	}
	
	// 3. traverse through program header tables
	printf("\n[Now, load one loadable segment from program header table]\n");
	//int i;
	for (i = 0; i < ep->e_phnum; i++) 
	{
		int elf_prot = 0, elf_flags;
		unsigned long k;
		 
		// 4. skip if not loadble segment
		printf("Segment type(phdr.p_type:[%d]): %u ", i, phdr.p_type); //PT_LOAD == 1
		if (ph_table[i].p_type != PT_LOAD)
		{
			printf("-->Not Loadable segment(PT_LOAD!=1)\n", i, phdr.p_type);
			continue;
		}
		printf("-->Loadable segment(PT_LOAD==1)\n", i, phdr.p_type);
		
		// 5. set prot, flags for mapping
		if (ph_table[i].p_flags & PF_R)
			elf_prot |= PROT_READ;
		if (ph_table[i].p_flags & PF_W)
			elf_prot |= PROT_WRITE;
		if (ph_table[i].p_flags & PF_X)
			elf_prot |= PROT_EXEC;
		
		elf_flags = MAP_PRIVATE | MAP_FIXED | MAP_EXECUTABLE;

		// 6. map segment
		if (elf_map(ph_table[i].p_vaddr, elf_prot, elf_flags, &phdr) < 0) 
		{
			fprintf(stderr, "error on elf_mapping\n");
			return -1;
		}
		
		// 7. set bss and brk
		k = ph_table[i].p_vaddr + ph_table[i].p_filesz;
		printf("elf_bss[%d]: %p\t", i, k);
		if (k > elf_bss)
			elf_bss = k;

		k = ph_table[i].p_vaddr + ph_table[i].p_memsz;
		printf("elf_brk[%d]: %p\n", i, k);
		if (k > elf_brk) 
		{
			bss_prot = elf_prot;
			elf_brk = k;
		}
		
		// 8. If memsz greater than filesz, allocate space for bss
		if(ph_table[i].p_memsz > ph_table[i].p_filesz)
		{
			unsigned long nbyte;
			
			printf("-->brk bigger than bss\n");
			
			if(do_bss(elf_bss, elf_brk, bss_prot) < 0 )
			{
				fprintf(stderr, "error on bss mapping\n");
				return -1;
			}
			
			nbyte = ELF_PAGEOFFSET(elf_bss);
			if (nbyte) 
			{
				nbyte = ELF_MIN_ALIGN - nbyte;
				memset((void *) elf_bss, 0, nbyte);
			}
		}
		
		// 9. Break as the segment was loadable
		if (ph_table[i].p_type == PT_LOAD) break;
		
	}
	// 10. close file
	close(fd);
	return 0;
}

void *elf_map(Elf64_Addr addr, int prot, int type, Elf64_Phdr *phdr)
{
	size_t size = phdr->p_filesz + ELF_PAGEOFFSET(phdr->p_vaddr);
	size_t off = phdr->p_offset - ELF_PAGEOFFSET(phdr->p_vaddr);
	addr = ELF_PAGESTART(addr);
	size = ELF_PAGEALIGN(size);

	if (!size)
		return (void *) addr;
	
	return mmap((void *) addr, size, prot, type, fd, off);
}

int do_bss(unsigned long start, unsigned long end, int prot)
{
	int flags = MAP_FIXED | MAP_ANONYMOUS | MAP_PRIVATE;

	start = ELF_PAGEALIGN(start);
	end = ELF_PAGEALIGN(end);
	
	if (end <= start) return -1;

	return mmap((void *) start, end - start, prot, flags, -1, 0);
}

int init_stack(int argc, char *argv[], int envc, char *envp[])
{
	size_t len; 
	int prot = PROT_READ | PROT_WRITE, 
	    flags = MAP_FIXED | MAP_ANONYMOUS | MAP_GROWSDOWN | MAP_PRIVATE;
	
	// 1. map a new space starting from STACK_START
	if ( (stack_ptr = mmap((void *) STACK_START, STACK_SIZE, prot, flags, -1 ,0)) < 0 ) {
		fprintf(stderr, "mmap error in creating stack space n");
		exit(EXIT_FAILURE);
	}
	// 2. point to top of stack
	stack_ptr = (Elf64_Addr) STACK_TOP;
	
	int i;
	// 3. envp to stack
	for (i = envc - 1; i >= 0; i--) {
		len = strnlen(envp[i], MAX_ARG_STRLEN);
		stack_ptr -= (len + 1);
		if (i == 0) env_start = stack_ptr;
		memcpy(stack_ptr, envp[i], len + 1);
	}
	
	// 4. argv to stack 
	for (i = argc - 1; i >= 0; i--) {
		len = strnlen(argv[i], MAX_ARG_STRLEN);
		stack_ptr -= (len + 1);
		if (i == 0) arg_start = stack_ptr;
		memcpy(stack_ptr, argv[i], len + 1);
	}
	
	return 0;

}

int build_stack(int argc, int envc, char *envp[], Elf64_Ehdr *ep)
{
	int items, ei_index = 0; 
	int8_t *ptr;
	size_t len;
	Elf64_auxv_t elf_info[AT_VECTOR_SIZE], *auxv;

	// copy auxv_t to elf_info
	while (*envp++ != NULL) ;
	for (auxv = (Elf64_auxv_t *) envp; auxv->a_type != AT_NULL; auxv++, ei_index++) 
	{
		elf_info[ei_index] = *auxv;
		if (auxv->a_type == AT_PHDR) elf_info[ei_index].a_un.a_val = 0;
		else if (auxv->a_type == AT_ENTRY) elf_info[ei_index].a_un.a_val = ep->e_entry;
		else if (auxv->a_type == AT_PHNUM) elf_info[ei_index].a_un.a_val = ep->e_phnum;
	}
	
	// 1. Advance past the AT_NULL entry.
	ei_index += 2;
	stack_ptr = STACK_ADD(stack_ptr, 2 * ei_index);
	
	items = (argc + 1) + (envc + 1) + 1;
	stack_ptr = STACK_ROUND(stack_ptr, items);
	stack_top = stack_ptr;
	
	// 2. Now, let's put argc (and argv, envp if appropriate) on the stack
	*((unsigned long *) stack_ptr) = (unsigned long) argc - 1;
	stack_ptr += 0x8;
	
	// 3. Populate list of argv pointers back to argv strings.
	ptr = arg_start;
	int i;
	for (i = 0; i < argc - 1; i++) 
	{
		*((unsigned long *) stack_ptr) = (unsigned long) ptr;
		len = strnlen(ptr, MAX_ARG_STRLEN);
		stack_ptr += 0x8;
		ptr += len + 1;
	}
	*((unsigned long *) stack_ptr) = NULL;
	stack_ptr += 0x8;

	// 4. Populate list of envp pointers back to envp strings.
	ptr = env_start;
	for (i = 0; i < envc; i++) 
	{
		*((unsigned long *) stack_ptr) = (unsigned long) ptr;
		len = strnlen(ptr, MAX_ARG_STRLEN);
		stack_ptr += 0x8;
		ptr += len;
	}
	*((unsigned long *) stack_ptr) = NULL;
	stack_ptr += 0x8;

	// 5. Put the elf_info on the stack in the right place.
	memcpy(stack_ptr, elf_info, sizeof(Elf64_auxv_t) * ei_index);

	return 0;
}

int clean_and_jump(Elf64_Addr elf_entry)
{

	asm("movq $0, %rax");
	asm("movq $0, %rbx");
	asm("movq $0, %rcx");
	asm("movq $0, %rdx");
	asm("movq %0, %%rsp" : : "r" (stack_top));
	asm("jmp *%0" : : "c" (elf_entry));

	return 0;
}


void segv_handler(int signo, siginfo_t *siginfo ,void* context)
{
	unsigned long elf_brk = 0, elf_bss = 0, k;
	int elf_prot = 0, elf_flags, bss_prot ,unvalid_addr = 1;
	Elf64_Addr siaddr = (Elf64_Addr) siginfo->si_addr;
	Elf64_Phdr *phdr;
	int i;

	// 1. find segment according to its address, from phdr table
	for (i = 0; i < real_table_size; i++) 
	{
		phdr = &ph_table[i];
		
		if (phdr->p_type != PT_LOAD)
			continue;

		if ( (phdr->p_vaddr <= siaddr) && (siaddr <= (phdr->p_vaddr + phdr->p_memsz)) ) 
		{
			unvalid_addr = -1;
			break;
		}
	}

	// 2. exit if address was invalid 
	if (unvalid_addr > 0) 
	{
		fprintf(stderr, "Unvalid memory reference\n");
		exit(EXIT_FAILURE);
	}

	// 3. set prot, flags for mapping
	if (phdr->p_flags & PF_R)
		elf_prot |= PROT_READ;
	if (phdr->p_flags & PF_W)
		elf_prot |= PROT_WRITE;
	if (phdr->p_flags & PF_X)
		elf_prot |= PROT_EXEC;
		
	elf_flags = MAP_PRIVATE | MAP_FIXED | MAP_EXECUTABLE;
	
	// 4. map segment
	if (elf_map(phdr->p_vaddr, elf_prot, elf_flags, &phdr) < 0) 
	{
		fprintf(stderr, "error on elf_mapping\n");
		exit(EXIT_FAILURE);
	}
	
	// 5. set bss and brk
	k = phdr->p_vaddr + phdr->p_filesz;
	if (k > elf_bss)
		elf_bss = k;

	k = phdr->p_vaddr + phdr->p_memsz;
	if (k > elf_brk) 
	{
		bss_prot = elf_prot;
		elf_brk = k;
	}
	
	// 6. If memsz greater than filesz, allocate space for bss
	if(phdr->p_memsz > phdr->p_filesz)
	{
		unsigned long nbyte;
		
		if(do_bss(elf_bss, elf_brk, bss_prot) < 0 )
		{
			fprintf(stderr, "error on bss mapping\n");
			exit(EXIT_FAILURE);
		}
		
		nbyte = ELF_PAGEOFFSET(elf_bss);
		if (nbyte) 
		{
			nbyte = ELF_MIN_ALIGN - nbyte;
			memset((void *) elf_bss, 0, nbyte);
		}
	}
	
	return 0;
}
