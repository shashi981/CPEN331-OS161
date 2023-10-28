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
 * sys_write(int fd, void *buf, size_t nbytes, int *retval)
 * 
 * Writes data to a file descriptor.
 * 
 * Parameters:
 *  - fd: File descriptor where data will be written
 *  - buf: Pointer to the buffer containing data to be written
 *  - nbytes: Number of bytes to write
 *  - retval: Pointer to an integer where the number of bytes written will be stored
 * 
 * Returns:
 *  - 0 on success, sets *retval to the number of bytes written
 *  - -1 on failure, sets *retval to -1 and returns the appropriate error code
 */
int sys_write(int fd, void *buf, size_t nbytes, int *retval);

int sys_write(int fd, void *buf, size_t nbytes, int *retval) {
    // Ensure current process exists
    KASSERT(curproc != NULL);

    // Set up iovec and uio structures for writing
    struct iovec i;
    struct uio uio_vec;

    i.iov_ubase = (userptr_t)buf;
    i.iov_len = nbytes;
    uio_vec.uio_iov = &i;
    uio_vec.uio_iovcnt = 1;
    uio_vec.uio_resid = nbytes;
    uio_vec.uio_offset = curproc->tb->entries[fd].offset;
    uio_vec.uio_segflg = UIO_USERSPACE;
    uio_vec.uio_rw = UIO_WRITE;
    uio_vec.uio_space = curproc->p_addrspace;

    // Call VOP_WRITE to write data to the file descriptor
    int result = VOP_WRITE(curproc->tb->entries[fd].vnodePtr, &uio_vec);

    // Check if VOP_WRITE was successful
    if (result) {
        *retval = -1; // Operation failed, set *retval to -1
        return result;
    }

    // Calculate the number of bytes written and update the file offset
    ssize_t new_len = nbytes - uio_vec.uio_resid;
    curproc->tb->entries[fd].offset += new_len;

    // Set *retval to the number of bytes written
    *retval = new_len;

    return 0; // Operation successful
}
