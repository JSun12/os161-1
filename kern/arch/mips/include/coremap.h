#ifndef _MIPS_VM_COREMAP_
#define _MIPS_VM_COREMAP_

#define INVALID_PADDR ((paddr_t)0x0)
#define INVALID_TLB_IX -1

#define COREMAP_TO_PADDR(ix) (((paddr_t)PAGE_SIZE)*((ix)+cm_stats.cms_base))
#define PADDR_TO_COREMAP(addr)(((addr)/PAGE_SIZE) - cm_stats.cms_base)

#define LOCK_COREMAP() (spinlock_acquire(&slk_coremap))
#define UNLOCK_COREMAP() (spinlock_release( &slk_coremap))

#define COREMAP_IS_LOCKED() (KASSERT(spinlock_do_i_hold( &slk_coremap )))
#define COREMAP_NO_VMP_LOCKS() (KASSERT( curthread->t_vmp_count == 0 ))

struct coremap_stats {
	uint32_t		cms_total_frames;	/* what we physically manage */
	uint32_t		cms_kpages;		/* kernel pages */
	uint32_t		cms_upages;		/* user pages */
	uint32_t		cms_free;		/* free pages */
	uint32_t		cms_wired;		/* wired pages */
	uint32_t		cms_base;		/* base frame */
};

struct coremap_entry {
	struct vm_page		*cme_page;		/* who currently resides here? */
	int			cme_tlb_ix : 7;		/* index in the tlb */
	
	unsigned 		cme_kernel : 1,		/* is it a kernel page? */
				cme_last : 1,		/* is this the last of a multi-page allocation? */
				cme_alloc: 1,		/* are we allocated? */
				cme_wired: 1,		/* are we wired? */
				cme_cpu : 5;
};

void			coremap_bootstrap( void );
void			coremap_wire( paddr_t );
void			coremap_unwire( paddr_t );
void			coremap_zero( paddr_t );
void			coremap_clone( paddr_t, paddr_t );
paddr_t			coremap_alloc( struct vm_page *, bool );
void			coremap_free( paddr_t, bool );
void			mark_pages_as_allocated( int, int, bool, bool);
bool			coremap_is_wired( paddr_t );

extern struct coremap_entry		*coremap;
extern struct spinlock			slk_coremap;
extern struct wchan			*wc_wire;
extern struct coremap_stats		cm_stats;
extern struct wchan			*wc_shootdown;

#endif
