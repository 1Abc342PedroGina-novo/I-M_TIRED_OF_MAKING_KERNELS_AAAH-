/* SPDX License Indentifier: GPL-2.0 WITH Linux-Syscall-Note */

#ifndef MACH_VM_PROT_H
#define MACH_VM_PROT_H

#include <mach/types.h> 

#ifndef _AC
#ifdef __ASSEMBLY__
#define _AC(X,Y)	X
#else
#define _AC(X,Y)	(X##Y)
#endif
#endif

typedef int		vm_prot_t;
#define	VM_PROT_NONE	((vm_prot_t) 0x00)

#define VM_PROT_READ	((vm_prot_t) 0x01)	/* read permission */
#define VM_PROT_WRITE	((vm_prot_t) 0x02)	/* write permission */
#define VM_PROT_EXECUTE	((vm_prot_t) 0x04)	/* execute permission */

#define VM_PROT_DEFAULT	(VM_PROT_READ|VM_PROT_WRITE)

#define VM_PROT_ALL	(VM_PROT_READ|VM_PROT_WRITE|VM_PROT_EXECUTE)

#define VM_PROT_NO_CHANGE	((vm_prot_t) 0x08)

struct vm_statistics {
	integer_t	pagesize;		/* page size in bytes */
	integer_t	free_count;		/* # of pages free */
	integer_t	active_count;		/* # of pages active */
	integer_t	inactive_count;		/* # of pages inactive */
	integer_t	wire_count;		/* # of pages wired down */
	integer_t	zero_fill_count;	/* # of zero fill pages */
	integer_t	reactivations;		/* # of pages reactivated */
	integer_t	pageins;		/* # of pageins */
	integer_t	pageouts;		/* # of pageouts */
	integer_t	faults;			/* # of faults */
	integer_t	cow_faults;		/* # of copy-on-writes */
	integer_t	lookups;		/* object cache lookups */
	integer_t	hits;			/* object cache hits */
};

typedef struct vm_statistics	*vm_statistics_t;
typedef struct vm_statistics	vm_statistics_data_t;

struct pmap_statistics {
	integer_t		resident_count;	/* # of pages mapped (total)*/
	integer_t		wired_count;	/* # of pages wired */
};

typedef struct pmap_statistics	*pmap_statistics_t;

typedef unsigned long	pteval_t;
typedef unsigned long	pmdval_t;
typedef unsigned long	pudval_t;
typedef unsigned long	p4dval_t;
typedef unsigned long	pgdval_t;
typedef unsigned long	pgprotval_t;

typedef struct { pteval_t pte; } pte_t;
typedef struct { pmdval_t pmd; } pmd_t;

/*
 * PGDIR_SHIFT determines what a top-level page table entry can map
 */
#define PGDIR_SHIFT	pgdir_shift
#define PTRS_PER_PGD	512

/*
 * 4th level page in 5-level paging case
 */
#define P4D_SHIFT		39
#define MAX_PTRS_PER_P4D	512
#define PTRS_PER_P4D		ptrs_per_p4d
#define P4D_SIZE		(_AC(1, UL) << P4D_SHIFT)
#define P4D_MASK		(~(P4D_SIZE - 1))

#define MAX_POSSIBLE_PHYSMEM_BITS	52

/*
 * 3rd level page
 */
#define PUD_SHIFT	30
#define PTRS_PER_PUD	512

/*
 * PMD_SHIFT determines the size of the area a middle-level
 * page table can map
 */
#define PMD_SHIFT	21
#define PTRS_PER_PMD	512

/*
 * entries per page directory level
 */
#define PTRS_PER_PTE	512

#define PMD_SIZE	(_AC(1, UL) << PMD_SHIFT)
#define PMD_MASK	(~(PMD_SIZE - 1))
#define PUD_SIZE	(_AC(1, UL) << PUD_SHIFT)
#define PUD_MASK	(~(PUD_SIZE - 1))
#define PGDIR_SIZE	(_AC(1, UL) << PGDIR_SHIFT)
#define PGDIR_MASK	(~(PGDIR_SIZE - 1))

/*
 * See Documentation/arch/x86/x86_64/mm.rst for a description of the memory map.
 *
 * Be very careful vs. KASLR when changing anything here. The KASLR address
 * range must not overlap with anything except the KASAN shadow area, which
 * is correct as KASAN disables KASLR.
 */
#define MAXMEM			(1UL << MAX_PHYSMEM_BITS)

#define GUARD_HOLE_PGD_ENTRY	-256UL
#define GUARD_HOLE_SIZE		(16UL << PGDIR_SHIFT)
#define GUARD_HOLE_BASE_ADDR	(GUARD_HOLE_PGD_ENTRY << PGDIR_SHIFT)
#define GUARD_HOLE_END_ADDR	(GUARD_HOLE_BASE_ADDR + GUARD_HOLE_SIZE)

#define LDT_PGD_ENTRY		-240UL
#define LDT_BASE_ADDR		(LDT_PGD_ENTRY << PGDIR_SHIFT)
#define LDT_END_ADDR		(LDT_BASE_ADDR + PGDIR_SIZE)

#define __VMALLOC_BASE_L4	0xffffc90000000000UL
#define __VMALLOC_BASE_L5 	0xffa0000000000000UL

#define VMALLOC_SIZE_TB_L4	32UL
#define VMALLOC_SIZE_TB_L5	12800UL

#define __VMEMMAP_BASE_L4	0xffffea0000000000UL
#define __VMEMMAP_BASE_L5	0xffd4000000000000UL

# define VMALLOC_START		vmalloc_base
# define VMALLOC_SIZE_TB	(pgtable_l5_enabled() ? VMALLOC_SIZE_TB_L5 : VMALLOC_SIZE_TB_L4)
# define VMEMMAP_START		vmemmap_base

#endif
