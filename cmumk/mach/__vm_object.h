#ifndef MACH__VM_OBJECT_H
#define MACH__VM_OBJECT_H

 #include <mach/mach_types.h>
 # include <mach/machine/pmap.h>

struct vm_object_fault_info {
	int             interruptible;
	uint32        user_tag;
	vm_size_t       cluster_size;
	vm_behavior_t   behavior;
	vm_object_offset_t lo_offset;
	vm_object_offset_t hi_offset;
	unsigned int
	/* boolean_t */ no_cache:1,
	/* boolean_t */ stealth:1,
	/* boolean_t */ io_sync:1,
	/* boolean_t */ cs_bypass:1,
	/* boolean_t */ csm_associated:1,
	/* boolean_t */ mark_zf_absent:1,
	/* boolean_t */ batch_pmap_op:1,
	/* boolean_t */ resilient_media:1,
	/* boolean_t */ no_copy_on_read:1,
	/* boolean_t */ fi_imtiredtomakekernels_user_debug:1,
	/* boolean_t */ fi_used_for_tpro:1,
	/* boolean_t */ fi_change_wiring:1,
	/* boolean_t */ fi_no_sleep:1,
	    __vm_object_fault_info_unused_bits:19;
	int             pmap_options;
};
struct vm_object {
	/*
	 * on 64 bit systems we pack the pointers hung off the memq.
	 * those pointers have to be able to point back to the memq.
	 * the packed pointers are required to be on a 64 byte boundary
	 * which means 2 things for the vm_object...  (1) the memq
	 * struct has to be the first element of the structure so that
	 * we can control its alignment... (2) the vm_object must be
	 * aligned on a 64 byte boundary... for static vm_object's
	 * this is accomplished via the 'aligned' attribute... for
	 * vm_object's in the zone pool, this is accomplished by
	 * rounding the size of the vm_object element to the nearest
	 * 64 byte size before creating the zone.
	 */
	vm_page_queue_head_t    memq;           /* Resident memory - must be first */
	lck_rw_t                Lock;           /* Synchronization */

	union {
		vm_object_size_t  vou_size;     /* Object size (only valid if internal) */
		int               vou_cache_pages_to_scan;      /* pages yet to be visited in an
		                                                 * external object in cache
		                                                 */
	} vo_un1;

	struct vm_page          *memq_hint;
	os_ref_atomic_t         ref_count;        /* Number of references */
	unsigned int            resident_page_count;
	/* number of resident pages */
	unsigned int            wired_page_count; /* number of wired pages
	                                           *  use VM_OBJECT_WIRED_PAGE_UPDATE macros to update */
	unsigned int            reusable_page_count;

	struct vm_object        *vo_copy;       /* Object that should receive
	                                         * a copy of my changed pages,
	                                         * for copy_delay, or just the
	                                         * temporary object that
	                                         * shadows this object, for
	                                         * copy_call.
	                                         */
	uint32_t                vo_copy_version;
	uint32_t                vo_inherit_copy_none:1,
	    __vo_unused_padding:31;
	struct vm_object        *shadow;        /* My shadow */
	memory_object_t         pager;          /* Where to get data */

	union {
		vm_object_offset_t vou_shadow_offset;   /* Offset into shadow */
		clock_sec_t     vou_cache_ts;   /* age of an external object
		                                 * present in cache
		                                 */
		task_t          vou_owner;      /* If the object is purgeable
		                                 * or has a "ledger_tag", this
		                                 * is the task that owns it.
		                                 */
	} vo_un2;

	vm_object_offset_t      paging_offset;  /* Offset into memory object */
	memory_object_control_t pager_control;  /* Where data comes back */

	memory_object_copy_strategy_t
	    copy_strategy;                      /* How to handle data copy */

	/*
	 * Some user processes (mostly VirtualMachine software) take a large
	 * number of UPLs (via IOMemoryDescriptors) to wire pages in large
	 * VM objects and overflow the 16-bit "activity_in_progress" counter.
	 * Since we never enforced any limit there, let's give them 32 bits
	 * for backwards compatibility's sake.
	 */
	uint16_t                paging_in_progress;
	uint16_t                vo_size_delta;
	uint32_t                activity_in_progress;

	/* The memory object ports are
	 * being used (e.g., for pagein
	 * or pageout) -- don't change
	 * any of these fields (i.e.,
	 * don't collapse, destroy or
	 * terminate)
	 */

	unsigned int
	/* boolean_t array */ all_wanted:7,     /* Bit array of "want to be
	                                         * awakened" notations.  See
	                                         * VM_OBJECT_EVENT_* items
	                                         * below */
	/* boolean_t */ pager_created:1,        /* Has pager been created? */
	/* boolean_t */ pager_initialized:1,    /* Are fields ready to use? */
	/* boolean_t */ pager_ready:1,          /* Will pager take requests? */

	/* boolean_t */ pager_trusted:1,        /* The pager for this object
	                                         * is trusted. This is true for
	                                         * all internal objects (backed
	                                         * by the default pager)
	                                         */
	/* boolean_t */ can_persist:1,          /* The kernel may keep the data
	                                         * for this object (and rights
	                                         * to the memory object) after
	                                         * all address map references
	                                         * are deallocated?
	                                         */
	/* boolean_t */ internal:1,             /* Created by the kernel (and
	                                         * therefore, managed by the
	                                         * default memory manger)
	                                         */
	/* boolean_t */ private:1,              /* magic device_pager object,
	                                        * holds private pages only */
	/* boolean_t */ pageout:1,              /* pageout object. contains
	                                         * private pages that refer to
	                                         * a real memory object. */
	/* boolean_t */ alive:1,                /* Not yet terminated */

	/* boolean_t */ purgable:2,             /* Purgable state.  See
	                                         * VM_PURGABLE_*
	                                         */
	/* boolean_t */ purgeable_only_by_kernel:1,
	/* boolean_t */ purgeable_when_ripe:1,         /* Purgeable when a token
	                                                * becomes ripe.
	                                                */
	/* boolean_t */ shadowed:1,             /* Shadow may exist */
	/* boolean_t */ true_share:1,
	/* This object is mapped
	 * in more than one place
	 * and hence cannot be
	 * coalesced */
	/* boolean_t */ terminating:1,
	/* Allows vm_object_lookup
	 * and vm_object_deallocate
	 * to special case their
	 * behavior when they are
	 * called as a result of
	 * page cleaning during
	 * object termination
	 */
	/* boolean_t */ named:1,                /* An enforces an internal
	                                         * naming convention, by
	                                         * calling the right routines
	                                         * for allocation and
	                                         * destruction, UBC references
	                                         * against the vm_object are
	                                         * checked.
	                                         */
	/* boolean_t */ shadow_severed:1,
	/* When a permanent object
	 * backing a COW goes away
	 * unexpectedly.  This bit
	 * allows vm_fault to return
	 * an error rather than a
	 * zero filled page.
	 */
	/* boolean_t */ phys_contiguous:1,
	/* Memory is wired and
	 * guaranteed physically
	 * contiguous.  However
	 * it is not device memory
	 * and obeys normal virtual
	 * memory rules w.r.t pmap
	 * access bits.
	 */
	/* boolean_t */ nophyscache:1,
	/* When mapped at the
	 * pmap level, don't allow
	 * primary caching. (for
	 * I/O)
	 */
	/* boolean_t */ for_realtime:1,
	/* Might be needed for realtime code path */
	/* vm_object_destroy_reason_t */ no_pager_reason:3,
	/* differentiate known and unknown causes */
#if FBDP_DEBUG_OBJECT_NO_PAGER
	/* boolean_t */ fbdp_tracked:1;
#else /* FBDP_DEBUG_OBJECT_NO_PAGER */
	__object1_unused_bits:1;
#endif /* FBDP_DEBUG_OBJECT_NO_PAGER */

	queue_chain_t           cached_list;    /* Attachment point for the
	                                         * list of objects cached as a
	                                         * result of their can_persist
	                                         * value
	                                         */
	/*
	 * the following fields are not protected by any locks
	 * they are updated via atomic compare and swap
	 */
	vm_object_offset_t      last_alloc;     /* last allocation offset */
	vm_offset_t             cow_hint;       /* last page present in     */
	                                        /* shadow but not in object */
	int32_t                 sequential;     /* sequential access size */

	uint32_t                pages_created;
	uint32_t                pages_used;
	/* hold object lock when altering */
	unsigned        int
	    wimg_bits:8,                /* cache WIMG bits         */
	    code_signed:1,              /* pages are signed and should be
	                                 *  validated; the signatures are stored
	                                 *  with the pager */
	    transposed:1,               /* object was transposed with another */
	    mapping_in_progress:1,      /* pager being mapped/unmapped */
	    phantom_isssd:1,
	    volatile_empty:1,
	    volatile_fault:1,
	    all_reusable:1,
	    blocked_access:1,
	    set_cache_attr:1,
	    object_is_shared_cache:1,
	    purgeable_queue_type:2,
	    purgeable_queue_group:3,
	    io_tracking:1,
	    no_tag_update:1,            /*  */
#if CONFIG_SECLUDED_MEMORY
	    eligible_for_secluded:1,
	    can_grab_secluded:1,
#else /* CONFIG_SECLUDED_MEMORY */
	__object3_unused_bits:2,
#endif /* CONFIG_SECLUDED_MEMORY */
#if VM_OBJECT_ACCESS_TRACKING
	    access_tracking:1,
#else /* VM_OBJECT_ACCESS_TRACKING */
	__unused_access_tracking:1,
#endif /* VM_OBJECT_ACCESS_TRACKING */
	vo_ledger_tag:3,
	    vo_no_footprint:1;

	uint32_t        access_tracking_reads;
	uint32_t        access_tracking_writes;

	uint8                 scan_collisions;

	/* This value is used for selecting a chead in the compressor for internal objects.
	 * see rdar://140849693 for a possible way to implement the chead_hint functionality
	 * in a way that doesn't require these bits */
	uint8_t vo_chead_hint:COMPRESSOR_PAGEOUT_CHEADS_BITS;
	uint8 __object4_unused_bits:8 - COMPRESSOR_PAGEOUT_CHEADS_BITS;
	vm_tag_t                wire_tag;

	uint32                phantom_object_id;

	queue_head_t            uplq;           /* List of outstanding upls */


#define VM_PIP_DEBUG_STACK_FRAMES       25      /* depth of each stack trace */
#define VM_PIP_DEBUG_MAX_REFS           10      /* track that many references */
	struct __pip_backtrace {
		void *pip_retaddr[VM_PIP_DEBUG_STACK_FRAMES];
	} pip_holders[VM_PIP_DEBUG_MAX_REFS];

	queue_chain_t           objq;      /* object queue - currently used for purgable queues */
	queue_chain_t           task_objq; /* objects owned by task - protected by task lock */

#if !VM_TAG_ACTIVE_UPDATE
	queue_chain_t           wired_objq;
#endif /* !VM_TAG_ACTIVE_UPDATE */

#if DEBUG
	void *purgeable_owner_bt[16];
	task_t vo_purgeable_volatilizer; /* who made it volatile? */
	void *purgeable_volatilizer_bt[16];
#endif /* DEBUG */

	/*
	 * If this object is backed by anonymous memory, this represents the ID of
	 * the vm_map that the memory originated from (i.e. this points backwards in
	 * shadow chains). Note that an originator is present even if the object
	 * hasn't been faulted into the backing pmap yet.
	 */
	const void vmo_provenance;
};
