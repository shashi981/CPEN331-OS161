#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <spl.h>
#include <spinlock.h>
#include <proc.h>
#include <current.h>
#include <mips/tlb.h>
#include <addrspace.h>
#include <vm.h>

/* Enum for representing page states in the coremap */
enum page_state {
    PAGE_FREE,  // Page is free
    PAGE_USED   // Page is in use
};

/* Structure for each entry in the coremap */
struct coremap_entry {
    paddr_t paddr;         // Physical address of the page
    enum page_state state; // Current state of the page
};

/* Global spinlock for memory allocation synchronization */
static struct spinlock stealmem_lock = SPINLOCK_INITIALIZER;

/* 
 * vm_bootstrap: Initializes the virtual memory system.
 * This function is responsible for setting up any structures and
 * synchronization primitives needed for managing virtual memory.
 */
void vm_bootstrap(void) {
    // VM system initialization logic goes here
    // ...
}

/* 
 * vm_fault: Handles memory faults, called by the trap code.
 * @param faulttype: Type of the fault (read, write, read-only)
 * @param faultaddress: The virtual address where the fault occurred
 * @return: Status code indicating success or type of error
 */
int vm_fault(int faulttype, vaddr_t faultaddress) {
    faultaddress &= PAGE_FRAME; // Align the fault address to page boundary

    switch (faulttype) {
        case VM_FAULT_READONLY:
            // Handle attempts to write to a read-only page
            return EFAULT;
        case VM_FAULT_READ:
            // Handle read faults (e.g., page not present)
            return EFAULT;
        case VM_FAULT_WRITE:
            // Handle write faults
            break;
        default:
            // Invalid fault type
            return EINVAL;
    }

    if (curproc == NULL) {
        // Fault occurred with no current process, likely a kernel fault
        return EFAULT;
    }

    // Address space handling and fault resolution logic
    // ...

    return 0;
}

/* 
 * alloc_kpages: Allocates contiguous physical memory pages.
 * @param npages: Number of pages to allocate
 * @return: Virtual address of the allocated memory
 */
vaddr_t alloc_kpages(unsigned npages) {
    paddr_t pa = getppages(npages);
    return (pa == 0) ? 0 : PADDR_TO_KVADDR(pa);
}

/* 
 * free_kpages: Frees memory allocated by alloc_kpages.
 * @param addr: Virtual address of the memory to free
 */
void free_kpages(vaddr_t addr) {
    // Memory deallocation logic
    // ...
    (void)addr; // Placeholder to avoid unused variable warning
}

/* 
 * vm_tlbshootdown_all: Handles global TLB shootdowns.
 * This function is used when memory mappings have changed significantly.
 */
void vm_tlbshootdown_all(void) {
    int spl = splhigh();
    for (int i = 0; i < NUM_TLB; i++) {
        tlb_write(TLBHI_INVALID(i), TLBLO_INVALID(), i);
    }
    splx(spl);
}

/* 
 * vm_tlbshootdown: Handles specific TLB shootdown requests.
 * @param ts: Information about the specific TLB entry to shootdown
 */
void vm_tlbshootdown(const struct tlbshootdown *ts) {
	(void)ts; // To avoid unused variable warnings
    panic("smartvm can't do specific tlb shootdown\n");
} 

/* 
 * getppages: Allocates physical pages from RAM.
 * @param npages: Number of pages to allocate
 * @return: Physical address of the allocated memory
 */
paddr_t getppages(unsigned long npages) {
    paddr_t addr;
    spinlock_acquire(&stealmem_lock);
    addr = ram_stealmem(npages);
    spinlock_release(&stealmem_lock);
    return addr;
}