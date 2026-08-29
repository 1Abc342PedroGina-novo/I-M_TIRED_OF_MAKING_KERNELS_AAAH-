#ifndef MACH__VM_PAGE_H
#define MACH__VM_PAGE_H

#include <mach/types.h>

typedef struct {
	ULONGLONG f;
} memdesc_flags_t;

#include <mach/vm_definitions.h>
#include <mach/vm_prot.h>
#include <mach/__vm_object.h>
#include <mach/__vm_map.h>
#include <mach/machine/pmap.h>
#include <kern/zalloc.h>
#include <kern/queue.h>
#include <linux/fs.h>
#include <net/page_pool/types.h>

struct vm_page {
    memdesc_flags_t flags;
    struct vm_page	*next;		/* VP bucket link (O) */

   union {
		struct {	/* Page cache and anonymous pages */
			union {
        queue_chain_t lru;

				/* Or, free page */
				queue_chain_t buddy_list;
				queue_chain_t pcp_list;
				queue_chain_t pcp_llist;
    	  queue_chain_t	pageq;		/* queue info for FIFO
					 * queue or free list (P) */
	      queue_chain_t	listq;		/* all pages in same object (O) */
			};

      struct address_space *mapping;
			union {
				uint64 __folio_index;		/* Our offset within mapping. */
				unsigned long share;	/* share count for fsdax */
			};

      unsigned long private;

  struct {	/* page_pool used by netstack */
			/**
			 * @pp_magic: magic value to avoid recycling non
			 * page_pool allocated pages.
			 */
			unsigned long pp_magic;
			struct vm_page *pp;
			unsigned long _pp_mapping_pad;
			unsigned long dma_addr;
			atomic_long_t pp_ref_count;
		};

struct {	/* Tail pages of compound page */
			unsigned long compound_info;	/* Bit zero is set */
		};
		struct {	/* ZONE_DEVICE pages */
			/*
			 * The first word is used for compound_info or folio
			 * pgmap
			 */
			void *_unused_pgmap_compound_info;
			void *zone_device_data;
			/*
			 * ZONE_DEVICE private pages are counted as being
			 * mapped so the next 3 words hold the mapping, index,
			 * and private fields from the source anonymous or
			 * page cache page while the page is migrated to device
			 * private memory.
			 * ZONE_DEVICE MEMORY_DEVICE_FS_DAX pages also
			 * use the mapping, index, and private fields when
			 * pmem backed DAX files are mapped.
			 */
		};

		/** @rcu_head: You can use this to free a page by RCU. */
		struct rcu_head rcu_head;
	};

	union {		/* This union is 4 bytes in size. */
		unsigned int page_type;
		 */
		atomic_t _mapcount;
	};
	atomic_t _refcount;

	unsigned long memcg_data;
	unsigned long _unused_slab_obj_exts;
#endif
	void *virtual;			/* Kernel virtual address (NULL if
					   not kmapped, ie. highmem) */

	int _last_cpupid;


	struct vm_page *kmsan_shadow;
	struct vm_page *kmsan_origin;

vm_object_t	object;		/* which object am I in (O&P) */
	vm_offset_t	offset;		/* offset into that object (O,P) */

	unsigned int	wire_count:16,	/* how many wired down maps use me?
					   (O&P) */
	/* boolean_t */	inactive:1,	/* page is in inactive list (P) */
			active:1,	/* page is in active list (P) */
			laundry:1,	/* page is being cleaned now (P)*/
			free:1,		/* page is on free list (P) */
			reference:1,	/* page has been used (P) */
			limbo:1,	/* page prepped then stolen (P) */
			pageout:1,	/* page wired & busy for pageout (P) */
			gobbled:1,      /* page used internally (P) */
			:0;		/* (force to 'long' boundary) */
#ifdef	ns32000
	int		pad;		/* extra space for ns32000 bit ops */
#endif	/* ns32000 */

	unsigned int
	/* boolean_t */	busy:1,		/* page is in transit (O) */
			wanted:1,	/* someone is waiting for page (O) */
			tabled:1,	/* page is in VP table (O) */
			fictitious:1,	/* Physical page doesn't exist (O) */
			private:1,	/* Page should not be returned to
					 *  the free list (O) */
			absent:1,	/* Data has been requested, but is
					 *  not yet available (O) */
			error:1,	/* Data manager was unable to provide
					 *  data due to error (O) */
			dirty:1,	/* Page must be cleaned (O) */
			cleaning:1,	/* Page clean has begun (O) */
			precious:1,	/* Page is precious; data must be
					 *  returned even if clean (O) */
			clustered:1,	/* page is not the faulted page (O) */
			overwriting:1,  /* Request to unlock has been made
					 * without having data. (O)
					 * [See vm_fault_page_overwrite] */
			restart:1,	/* Page was pushed higher in shadow
					   chain by copy_call-related pagers;
					   start again at top of chain */
			lock_supplied:1,/* protection supplied by pager (O) */
	/* vm_prot_t */	page_lock:3,	/* Uses prohibited by pager (O) */
	/* vm_prot_t */	unlock_request:3,/* Outstanding unlock request (O) */
			unusual:1,	/* Page is absent, error, restart or
					   page locked */
	  		discard_request:1,/* a memory_object_discard_request()
					   * has been sent */
			:0;

	vm_offset_t	phys_addr;	/* Physical address of page, passed
					 *  to pmap_enter (read-only) */
	kern_return_t	page_error;

	union {
	  struct {
	   unsigned int	prep:16,	/* page prep count */
			pin:16;		/* page pin pount */
	  } pp_counts;
	  unsigned int	pp_both;	/* used to test for both zero */
	} prep_pin_u;

  struct vm_map    *map;
  struct zone      *z_pmap;
};


#define VM_PAGE_NULL		((vm_page_t) 0)
#define NEXT_PAGE(m)    ((vm_page_t) (m)->pageq.next)

/*
 * XXX	The unusual bit should not be necessary.  Most of the bit
 * XXX	fields above really want to be masks.
 */

/*
 *	For debugging, this macro can be defined to perform
 *	some useful check on a page structure.
 */

#define VM_PAGE_CHECK(mem)


#define VM_PAGE_QUEUES_REMOVE(mem)					\
	if (mem->active) {					\
		assert(!mem->inactive);				\
		queue_remove(&vm_page_queue_active,		\
			mem, vm_page_t, pageq);			\
		mem->active = FALSE;				\
		if (!mem->fictitious)				\
			vm_page_active_count--;			\
	}							\
								\
	if (mem->inactive) {					\
		assert(!mem->active);				\
		queue_remove(&vm_page_queue_inactive,		\
			mem, vm_page_t, pageq);			\
		mem->inactive = FALSE;				\
		if (!mem->fictitious)				\
			vm_page_inactive_count--;		\
	}							\

/*
 * Data structure for automatic physical memory reservation.  Declared
 * here, defined in vm_resident.c.
 *
 * The following data structure is used to specify contiguous ranges
 * of physical memory that should be allocated before virtual memory
 * initialization.  Each element is a pair of addresses--the address
 * of the size of the chunk that should be allocated, the second the
 * location at which to place the pointer to that section when it is
 * allocated.
 *
 * The actual mechanism for allocating the contiguous physical memory
 * is machine dependent, and occurs during the machine dependent VM
 * initialization.
 *
 * This is defined as a pointer rather than a dimensionless array
 * because the PGI (intel 860) compiler can't handle dimensionless
 * arrays. 
 */

extern struct pmem_reserve {
  unsigned long *pmem_size;		/* In  */
  vm_offset_t *pmem_addr;		/* Out  */
} *pmem_reserve_ctl;

#endif
