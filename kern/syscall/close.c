#include <types.h>
#include <vnode.h>
#include <kern/unistd.h>
#include <kern/fcntl.h>
#include <vfs.h>
#include <current.h>
#include <proc.h>
#include <kern/errno.h>
#include <copyinout.h>

//function declaration 
int sys_close(int fd, int *retval);

/*
 * sys_close(int fd, int *retval)
 * 
 * Closes the file associated with the given file descriptor.
 * 
 * Parameters:
 *  - fd: File descriptor to be closed
 *  - retval: Pointer to an integer where the result of the operation will be stored
 * 
 * Returns:
 *  - 0 on success, sets *retval to 0
 *  - -1 on failure, sets *retval to the appropriate error code (EBADF if the file descriptor is invalid)
 */
int sys_close(int fd, int *retval) {
    // Ensure current process exists
    KASSERT(curproc != NULL);

    // Acquire file manager lock and check if the file descriptor is valid
    lock_acquire(curproc->tb->file_manager_lk);
    if (get_entry(curproc->tb, fd) == NULL) {
        *retval = EBADF; // Invalid file descriptor
        lock_release(curproc->tb->file_manager_lk);
        return -1;
    }

    // Delete the file entry corresponding to the file descriptor
    delete_entry(curproc->tb, fd);
    lock_release(curproc->tb->file_manager_lk);

    // Operation successful, set *retval to 0
    *retval = 0;
    return 0;
}
