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
 * sys_chdir(const char *pathname)
 * 
 * Changes the current working directory of the process.
 * 
 * Parameters:
 *  - pathname: Pointer to the null-terminated string containing the path of the new working directory
 * 
 * Returns:
 *  - 0 on success
 *  - Error code on failure (such as ENAMETOOLONG if the path name is too long)
 */
int sys_chdir(const char *pathname);

int sys_chdir(const char *pathname) {
    // Allocate a kernel buffer for the pathname
    char *file = kmalloc(PATH_MAX);
    
    // Copy the pathname from user space to kernel space
    int err = copyinstr((const_userptr_t)pathname, file, PATH_MAX, NULL);
    
    // Free the allocated kernel buffer
    kfree(file);
    
    // Check for copyinstr errors
    if (err) {
        return err; // Return error code
    }

    // Call vfs_chdir to change the current working directory
    int result = vfs_chdir(file);

    // Return the result of vfs_chdir (0 on success, error code on failure)
    return result;
}
