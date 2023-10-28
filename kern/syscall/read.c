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
 * sys_read(int fd, void *buf, size_t buflen, ssize_t *retval)
 * 
 * Reads data from a file descriptor into the buffer.
 * 
 * Parameters:
 *  - fd: File descriptor from which data will be read
 *  - buf: Pointer to the buffer where data will be read into
 *  - buflen: Size of the buffer pointed to by buf
 *  - retval: Pointer to a ssize_t where the number of bytes read will be stored
 * 
 * Returns:
 *  - 0 on success, sets *retval to the number of bytes read
 *  - -1 on failure, sets *retval to -1 and returns the appropriate error code
 */
int sys_read(int fd, void *buf, size_t buflen, ssize_t *retval);

int sys_read(int fd, void *buf, size_t buflen, ssize_t *retval) {
    // Ensure current process exists
    KASSERT(curproc != NULL);

    // Set up iovec and uio structures for reading
    struct iovec i;
    struct uio uio_vec;

    i.iov_ubase = (userptr_t)buf;
    i.iov_len = buflen;
    uio_vec.uio_iov = &i;
    uio_vec.uio_iovcnt = 1;
    uio_vec.uio_resid = buflen;
    uio_vec.uio_offset = curproc->tb->entries[fd].offset;
    uio_vec.uio_segflg = UIO_USERSPACE;
    uio_vec.uio_rw = UIO_READ;
    uio_vec.uio_space = curproc->p_addrspace;

    // Call VOP_READ to read data from the file descriptor
    int result = VOP_READ(curproc->tb->entries[fd].vnodePtr, &uio_vec);

    // Check if VOP_READ was successful
    if (result) {
        *retval = -1; // Operation failed, set *retval to -1
        return result;
    }

    // Calculate the number of bytes read and update the file offset
    ssize_t new_len = buflen - uio_vec.uio_resid;
    curproc->tb->entries[fd].offset += new_len;

    // Set *retval to the number of bytes read
    *retval = new_len;

    return 0; // Operation successful
}
