#ifndef MACH__VM_MAP_H
#define MACH__VM_MAP_H

typedef struct {
	unsigned long val;
} swp_entry_t;
struct anon_vma {
	struct anon_vma *root;		
atomic_t refcount;
	unsigned long num_children;
		unsigned long num_active_vmas;
	struct anon_vma *parent;	/* Parent of this anon_vma */

	struct rb_root_cached rb_root;
};

#include <mach/types.h>
#include <kern/kobject.h>
#include <linux/fs.h>
#include <linux/rmap.h>
#include <linux/mm_ops.h>
#include <mach/__vm_object.h>
#include <linux/rbtree.h>
#include <linux/refcount_types.h>

typedef struct { unsigned long v; } freeptr_t;

typedef union vm_map_object {
	struct vm_object	*vm_object;	/* object object */
	struct vm_map		*sub_map;	/* belongs to another map */
} vm_map_object_t;

struct vm_map_links {
	struct vm_map_entry	*prev;		/* previous entry */
	struct vm_map_entry	*next;		/* next entry */
	vm_offset_t		start;		/* start address */
	vm_offset_t		end;		/* end address */
};

struct vm_map_entry {
	struct vm_map_links	links;		/* links to other entries */
#define vme_prev		links.prev
#define vme_next		links.next
#define vme_start		links.start
#define vme_end			links.end
	union vm_map_object	object;		/* object I point to */
	vm_offset_t		offset;		/* offset into object */
	unsigned int
	/* boolean_t */		is_shared:1,	/* region is shared */
	/* boolean_t */		is_sub_map:1,	/* Is "object" a submap? */
	/* boolean_t */		in_transition:1, /* Entry being changed */
	/* boolean_t */		needs_wakeup:1,  /* Waiters on in_transition */
	/* vm_behavior_t */	behavior:2,	/* user paging behavior hint */
		/* Only used when object is a vm_object: */
	/* boolean_t */		needs_copy:1,	/* object need to be copied? */
		/* Only in task maps: */
	/* vm_prot_t */		protection:3,	/* protection code */
	/* vm_prot_t */		max_protection:3,/* maximum protection */
	/* vm_inherit_t */	inheritance:2;	/* inheritance */
	unsigned short		wired_count;	/* can be paged if = 0 */
	unsigned short		user_wired_count; /* for vm_wire */
};

struct __VM_REGION {
  	struct rb_node	vm_rb;		/* link in global region tree */
	uint32	vm_flags;	/* VMA vm_flags */
	ULONGLONG	vm_start;	/* start address of region */
	ULONGLONG	vm_end;		/* region initialised to here */
  ULONGLONG vm_top;		/* region allocated to here */
	ULONGLONG	vm_pgoff;	/* the offset in vm_file corresponding to vm_start */
	struct vm_object	*vm_object;	/* the backing file or NULL */

	int		vm_usage;	/* region usage count (access under nommu_region_sem) */
	bool		vm_icache_flushed : 1; /* true if the icache has been flushed for
						* this region */
};

struct vm_map {
	/* The first cache line has the info for VMA tree walking. */

	union {
		struct {
			/* VMA covers [vm_start; vm_end) addresses within mm */
			unsigned long vm_start;
			unsigned long vm_end;
		};
		freeptr_t vm_freeptr; /* Pointer used by SLAB_TYPESAFE_BY_RCU */
	};
	struct pmap *vm_pmap;
	vm_prot_t vm_page_prot;          /* Access permissions of this VMA. */

	/*
	 * Flags, see mm.h.
	 * To modify use vm_flags_{init|reset|set|clear|mod} functions.
	 * Preferably, use vma_flags_xxx() functions.
	 */
	union {
		/* Temporary while VMA flags are being converted. */
		const vm_flags_t vm_flags;
		vma_flags_t flags;
	};

  unsigned int vm_lock_seq;

struct anon_vma *anon_vma;	/* Serialized by page_table_lock */

	/* Function pointers to deal with this struct. */
	const struct vm_operations_struct *vm_ops;

	/* Information about our backing store: */
	unsigned long vm_pgoff;		/* Offset (within vm_file) in PAGE_SIZE
					   units */
	struct vm_map_object * vm_object;		/* File we map to (can be NULL). */
	void * vm_private_data;		/* was vm_pte (shared mem) */
  refcount_t vm_refcnt ____cacheline_aligned_in_smp;
  	struct {
		struct rb_node rb;
		unsigned long rb_subtree_last;
	} shared;
  	lock_t			lock;		/* uni- and smp-lock */
	struct vm_map_header	hdr;		/* Map entry header */
#define min_offset		hdr.links.start	/* start of range */
#define max_offset		hdr.links.end	/* end of range */
	pmap_t			pmap;		/* Physical map */
	vm_size_t		size;		/* virtual size */
	int			ref_count;	/* Reference count */
#if	TASK_SWAPPER
	int			res_count;	/* Residence count (swap) */
	int			sw_state;	/* Swap state */
#endif	/* TASK_SWAPPER */
	decl_mutex_data(,	s_lock)		/* Lock ref, res, hint fields */
	vm_map_entry_t		hint;		/* hint for quick lookups */
	vm_map_entry_t		first_free;	/* First free space hint */
	boolean_t		wait_for_space;	/* Should callers wait
						   for space? */
	boolean_t		wiring_required;/* All memory wired? */
	boolean_t		no_zero_fill;	/* No zero fill absent pages */
	unsigned int		timestamp;	/* Version number */

};

#endif
