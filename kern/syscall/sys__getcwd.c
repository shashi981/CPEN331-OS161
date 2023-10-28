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
 * sys__getcwd(char *buf, size_t buflen, int *retval)
 * 
 * Gets the current working directory of the process.
 * 
 * Parameters:
 *  - buf: Pointer to a buffer where the current working directory path will be stored
 *  - buflen: Size of the buffer pointed to by buf
 *  - retval: Pointer to an integer where the result of the operation will be stored
 * 
 * Returns:
 *  - 0 on success, sets *retval to the length of the path copied to buf
 *  - -1 on failure, sets *retval to -1 and returns the appropriate error code
 */
int sys__getcwd(char *buf, size_t buflen, int *retval);

int sys__getcwd(char *buf, size_t buflen, int *retval) {
    struct iovec i;
    struct uio uio_vec;

    // Set up the iovec and uio structures
    i.iov_ubase = (userptr_t)buf;
    i.iov_len = buflen;
    uio_vec.uio_iov = &i;
    uio_vec.uio_iovcnt = 1;
    uio_vec.uio_resid = buflen;
    uio_vec.uio_offset = 0;
    uio_vec.uio_segflg = UIO_USERSPACE;
    uio_vec.uio_rw = UIO_READ;
    uio_vec.uio_space = curproc->p_addrspace;

    // Call vfs_getcwd to obtain the current working directory
    int result = vfs_getcwd(&uio_vec);

    // Check if vfs_getcwd was successful
    if (result) {
        *retval = -1; // Operation failed, set *retval to -1
        return result;
    }

    // Calculate the length of the path copied to buf and set *retval
    *retval = (int)buflen - uio_vec.uio_resid;

    return 0; // Operation successful
}
