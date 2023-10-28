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
int sys_open(const char *filename, int flags, int *retval);

// #include <filemanager.h>

/*
 * sys_open(const char *filename, int flags, int *retval)
 * 
 * Opens a file given its filename and flags.
 * 
 * Parameters:
 *  - filename: Pointer to the filename string in user space
 *  - flags: Flags specifying the mode of opening the file
 *  - retval: Pointer to an integer where the file descriptor or error code will be stored
 * 
 * Returns:
 *  - 0 on success, sets *retval to the file descriptor
 *  - -1 on failure, sets *retval to the appropriate error code
 */
int sys_open(const char *filename, int flags, int *retval) {
    // Check if the filename is correct
    // Check if filename is not null
    if (filename == NULL) {
        *retval = -1; // Set error code in retval
        return -1;
    }

    // Call copyinstr to copy the filename from user space to kernel space
    char *dest = kmalloc(PATH_MAX);
    size_t *acc = kmalloc(sizeof(size_t));
    int err = copyinstr((const_userptr_t) filename, dest, PATH_MAX, acc);

    // Check status and return -1, store err in retval
    if (err) {
        *retval = err;
        kfree(acc);
        kfree(dest);
        return -1;
    }

    // VFS open
    struct vnode *vn_entry;
    int vnode_err = vfs_open(dest, flags, 0, &vn_entry);

    if (vnode_err) {
        *retval = vnode_err;
        kfree(acc);
        kfree(dest);
        return -1;
    }

    // Acquire file manager lock to add file entry
    lock_acquire(curproc->tb->file_manager_lk);
    
    // Add file entry to the current process's file table
    int fd = add_entry(curproc->tb, 0, flags, vn_entry);
    
    // Release file manager lock
    lock_release(curproc->tb->file_manager_lk);

    // Check if file entry was successfully added
    if (fd == -1) {
        *retval = -1;
        kfree(acc);
        kfree(dest);
        return -1;
    }

    // Set the file descriptor in retval and clean up allocated memory
    *retval = fd;
    kfree(acc);
    kfree(dest);
    return 0;
}
