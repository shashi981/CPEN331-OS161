#include <types.h>
#include <vnode.h>
#include <kern/unistd.h>
#include <kern/fcntl.h>
#include <vfs.h>
#include <current.h>
#include <proc.h>
#include <kern/errno.h>
#include <copyinout.h>
#include <uio.h>

/*
 * sys_dup2(int oldfd, int newfd, int *output)
 * 
 * Duplicates an open file descriptor to a specified file descriptor.
 * 
 * Parameters:
 *  - oldfd: The file descriptor to be duplicated
 *  - newfd: The new file descriptor where the duplication will occur
 *  - output: Pointer to an integer where the new file descriptor number will be stored
 * 
 * Returns:
 *  - 0 on success, sets *output to the new file descriptor number
 */
int sys_dup2(int oldfd, int newfd, int *output);

int sys_dup2(int oldfd, int newfd, int *output) {
    // Check if newfd is the same as oldfd
    if (newfd == oldfd) {
        *output = newfd; // No duplication needed, set *output to newfd
        return 0; // Operation successful
    }

    // Duplicate the file descriptor
    curproc->tb->entries[newfd] = curproc->tb->entries[oldfd];
    *output = newfd; // Set *output to the new file descriptor number

    return 0; // Operation successful
}
