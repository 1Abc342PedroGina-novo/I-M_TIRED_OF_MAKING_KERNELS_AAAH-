/* SPDX License Indentifier : GPL-2.0 WITH Linux-Syscalls-Note */
/* FIle: mach/vm_definitions.h */
/* 26/08/26 Pedro Emanuel
 * add x86_64 and ARM-support
 */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
 * All Rights Reserved.
 */

#ifndef MACH_VM_DEFINITIONS_H
#define MACH_VM_DEFINITIONS_H

#include <mach/types.h>

#define KERNEL_STACK_SIZE	(1*PAGE_SIZE)
#define INTSTACK_SIZE		(1*PAGE_SIZE)


#define atop(x)		(((vm_size_t)(x)) >> PAGE_SHIFT)
#define ptoa(x)		((vm_offset_t)((x) << PAGE_SHIFT))

/*
 *	Round off or truncate to the nearest page.  These will work
 *	for either addresses or counts.  (i.e. 1 byte rounds to 1 page
 *	bytes.
 */

#define round_page(x)	((vm_offset_t)((((vm_offset_t)(x)) + PAGE_MASK) & ~PAGE_MASK))
#define trunc_page(x)	((vm_offset_t)(((vm_offset_t)(x)) & ~PAGE_MASK))


/*
 *	Types defined:
 *
 *	vm_inherit_t	inheritance codes.
 */

typedef int		vm_inherit_t;	/* might want to change this */

/*
 *	Enumeration of valid values for vm_inherit_t.
 */

#define	VM_INHERIT_SHARE	((vm_inherit_t) 0)	/* share with child */
#define	VM_INHERIT_COPY		((vm_inherit_t) 1)	/* copy into child */
#define VM_INHERIT_NONE		((vm_inherit_t) 2)	/* absent from child */

#define VM_INHERIT_DEFAULT	VM_INHERIT_COPY

/*
 *	Types of machine-dependent attributes
 */
typedef unsigned int	vm_machine_attribute_t;

#define	MATTR_CACHE		1	/* cachability */
#define MATTR_MIGRATE		2	/* migrability */
#define	MATTR_REPLICATE		4	/* replicability */

/*
 *	Values for the above, e.g. operations on attribute
 */
typedef int		vm_machine_attribute_val_t;

#define MATTR_VAL_OFF		0	/* (generic) turn attribute off */
#define MATTR_VAL_ON		1	/* (generic) turn attribute on */
#define MATTR_VAL_GET		2	/* (generic) return current value */

#define MATTR_VAL_CACHE_FLUSH	6	/* flush from all caches */
#define MATTR_VAL_DCACHE_FLUSH	7	/* flush from data caches */
#define MATTR_VAL_ICACHE_FLUSH	8	/* flush from instruction caches */


#define KASAN_STACK_ORDER 1


#define THREAD_SIZE_ORDER	(2 + KASAN_STACK_ORDER)
#define THREAD_SIZE  (PAGE_SIZE << THREAD_SIZE_ORDER)

#define EXCEPTION_STACK_ORDER (1 + KASAN_STACK_ORDER)
#define EXCEPTION_STKSZ (PAGE_SIZE << EXCEPTION_STACK_ORDER)

#define IRQ_STACK_ORDER (2 + KASAN_STACK_ORDER)
#define IRQ_STACK_SIZE (PAGE_SIZE << IRQ_STACK_ORDER)

#define	IST_INDEX_DF		0
#define	IST_INDEX_NMI		1
#define	IST_INDEX_DB		2
#define	IST_INDEX_MCE		3
#define	IST_INDEX_VC		4

#endif
