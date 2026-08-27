#ifndef MACH_ARM64_PMAP_H
#define MACH_ARM64_PMAP_H
#include <mach/types.h>

typedef struct pv_entry {
	/* Linked list to the next mapping of the physical page. */
	struct pv_entry *pve_next;

	/* Pointer to the page table entry for this mapping. */
	ULONGLONG *pve_ptep[2];
} pv_entry_t;

typedef struct {
	pv_entry_t *list;
	uint32 count;
} pv_free_list_t;

struct pmap_cpu_data {
	unsigned int cpu_number;
	bool copywindow_strong_sync[4];
	pv_free_list_t pv_free;
	pv_entry_t *pv_free_spill_marker;
};

__enum_closed_decl(vm_page_q_state_t, uint8_t, {
	VM_PAGE_NOT_ON_Q                = 0,    /* page is not present on any queue, nor is it wired... mainly a transient state */
	VM_PAGE_IS_WIRED                = 1,    /* page is currently wired */
	VM_PAGE_USED_BY_COMPRESSOR      = 2,    /* page is in use by the compressor to hold compressed data */
	VM_PAGE_ON_FREE_Q               = 3,    /* page is on the main free queue */
	VM_PAGE_ON_FREE_LOCAL_Q         = 4,    /* page is on one of the per-CPU free queues */
	VM_PAGE_ON_FREE_LOPAGE_Q        = 5,    /* page is on the lopage pool free list *//
	VM_PAGE_ON_PAGEOUT_Q            = 6,    /* page is on one of the pageout queues (internal/external) awaiting processing */
	VM_PAGE_ON_ACTIVE_Q             = 7,   /* page is in global active queue */
});
#define VM_PAGE_Q_STATE_LAST_VALID_VALUE  13    /* we currently use 4 bits for the state... don't let this go beyond 15 */

__enum_closed_decl(vm_page_specialq_t, uint8_t, {
	VM_PAGE_SPECIAL_Q_EMPTY         = 0,
	VM_PAGE_SPECIAL_Q_BG            = 1,
	VM_PAGE_SPECIAL_Q_DONATE        = 2,
	VM_PAGE_SPECIAL_Q_FG            = 3,
});

struct page_arm64 {

  	struct {
		vm_page_q_state_t       vmp_q_state:4;      /* which q is the page on (P) */
		vm_page_specialq_t      vmp_on_specialq:2;
		uint8                 vmp_lopage:1;
		uint8                 vmp_canonical:1;    /* this page is a canonical kernel page (immutable) */
	};
	struct {
		uint8                 vmp_gobbled:1;      /* page used internally (P) */
		uint8                 vmp_laundry:1;      /* page is being cleaned now (P)*/
		uint8                 vmp_no_cache:1;     /* page is not to be cached and should */
		                                            /* be reused ahead of other pages (P) */
		uint8                 vmp_reference:1;    /* page has been used (P) */
		uint8                 vmp_realtime:1;     /* page used by realtime thread (P) */
		uint8                 vmp_iopl_wired:1;   /* page has been wired for I/O UPL (O&P) */
		uint8                 vmp_unmodified_ro:1;/* Tracks if an anonymous page is modified after a decompression (O&P).*/
		uint8                 __vmp_reserved1:1;
#endif
		uint8                 __vmp_reserved2:1;
	};
	unsigned int    vmp_busy:1,           /* page is in transit (O) */
	    vmp_wanted:1,                     /* someone is waiting for page (O) */
	    vmp_tabled:1,                     /* page is in VP table (O) */
	    vmp_hashed:1,                     /* page is in vm_page_buckets[] (O) + the bucket lock */
	__vmp_unused : 1,
	vmp_clustered:1,                      /* page is not the faulted page (O) or (O-shared AND pmap_page) */
	    vmp_pmapped:1,                    /* page has at some time been entered into a pmap (O) or */
	                                      /* (O-shared AND pmap_page) */
	    vmp_xpmapped:1,                   /* page has been entered with execute permission (O) or */
	                                      /* (O-shared AND pmap_page) */
	    vmp_wpmapped:1,                   /* page has been entered at some point into a pmap for write (O) */
	    vmp_free_when_done:1,             /* page is to be freed once cleaning is completed (O) */
	    vmp_absent:1,                     /* Data has been requested, but is not yet available (O) */
	    vmp_error:1,                      /* Data manager was unable to provide data due to error (O) */
	    vmp_dirty:1,                      /* Page must be cleaned (O) */
	    vmp_cleaning:1,                   /* Page clean has begun (O) */
	    vmp_precious:1,                   /* Page is precious; data must be returned even if clean (O) */
	    vmp_overwriting:1,                /* Request to unlock has been made without having data. (O) */
	                                      /* [See vm_fault_page_overwrite] */
	    vmp_restart:1,                    /* Page was pushed higher in shadow chain by copy_call-related pagers */
	                                      /* start again at top of chain */
	    vmp_unusual:1,                    /* Page is absent, error, restart or page locked */
	    vmp_cs_validated:VMP_CS_BITS,     /* code-signing: page was checked */
	    vmp_cs_tainted:VMP_CS_BITS,       /* code-signing: page is tainted */
	    vmp_cs_nx:VMP_CS_BITS,            /* code-signing: page is nx */
	    vmp_reusable:1,
	    vmp_written_by_kernel:1;          /* page was written by kernel (i.e. decompressed) */
};

#endif
