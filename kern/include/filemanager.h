#include <types.h>
#include <vnode.h>
#include <limits.h>
#include <synch.h>



struct tableentry {
    off_t offset;
    //flags 
    int flag;
    struct vnode* vnodePtr;
};

struct table {
    struct tableentry entries[OPEN_MAX];
    int count; // Number of entries currently in the table
    //semaphore 
    struct semaphore *file_manager_lock ;
    struct lock *file_manager_lk;

};

struct table* create_table(void);
int add_entry(struct table* tbl, off_t offset, int flag, struct vnode* vnodePtr);
void delete_entry(struct table* tbl, int index);
void destroy_table(struct table* tbl);
struct tableentry* get_entry(struct table* tbl, int index);

