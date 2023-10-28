// #include <../include/filemanager.h>
// #include <lib.h>

// struct table* create_table() {
//     struct table* newTable = (struct table*)kmalloc(sizeof(struct table));
//     //lock initilize 
//     if (newTable != NULL) {
//         newTable->count = 0; // Initialize the count to 0
//     }
//         for (int i = 0; i < OPEN_MAX; ++i) {
//             newTable->entries[i] = NULL ;
//         }
    
//     newTable->file_manager_lock = sem_create("file manager",1);
//     return newTable;
// }

// int add_entry(struct table* tbl, off_t offset, int flag, struct vnode* vnodePtr) {
//     // Find an empty spot in the array and add the table entry there
//     for (int i = 0; i < OPEN_MAX; ++i) {
//         if (tbl->entries[i] == NULL) {
//             tbl.entries[i]->offset = offset;
//             tbl->entries[i]->flag = flag;
//             tbl->entries[i]->vnodePtr = vnodePtr;
//             tbl->count++;
//             return i; // Return the index where the entry is stored
//         }
//     }
//     // Return -1 if the table is full and entry cannot be added
//     return -1;
// }

// void delete_entry(struct table* tbl, int index) {
//     if (index >= 0 && index < OPEN_MAX && tbl->entries[index] != NULL  && tbl->entries[index]->vnodePtr != NULL  ) {
//         tbl->entries[index]->vnodePtr = NULL;
//         tbl->count--;
//     } 
// }

// void destroy_table(struct table* tbl) {
//     kfree(tbl); // Free the memory allocated for the table
// }

#include <../include/filemanager.h>
#include <lib.h>

struct table* create_table() {
    struct table* newTable = (struct table*)kmalloc(sizeof(struct table));
    // Lock initialization
    if (newTable != NULL) {
        newTable->count = 0; // Initialize the count to 0

        for (int i = 0; i < OPEN_MAX; ++i) {
            newTable->entries[i].vnodePtr = NULL;
        }

        newTable->file_manager_lock = sem_create("file manager", 2);
        newTable->file_manager_lk = lock_create("new file lock");
    }
    return newTable;
}

int add_entry(struct table* tbl, off_t offset, int flag, struct vnode* vnodePtr) {
    // Find an empty spot in the array and add the table entry there
    
    for (int i = 0; i < OPEN_MAX; ++i) {
        if (tbl->entries[i].vnodePtr == NULL) {
            tbl->entries[i] = *(struct tableentry*)kmalloc(sizeof(struct tableentry));
                tbl->entries[i].offset = offset;
                tbl->entries[i].flag = flag;
                tbl->entries[i].vnodePtr = vnodePtr;
                tbl->count++;
                return i; // Return the index where the entry is stored
        }
    }
    // Return -1 if the table is full and entry cannot be added
    return -1;
}

void delete_entry(struct table* tbl, int index) {
    if (index >= 0 && index < OPEN_MAX && tbl->entries[index].vnodePtr != NULL) {
        tbl->entries[index].vnodePtr = NULL;
        //kfree(&(tbl->entries[index])); // Free the memory allocated for the table entry
        tbl->count--;
    } 
}

void destroy_table(struct table* tbl) {
    // Free the memory allocated for each table entry
    for (int i = 0; i < OPEN_MAX; ++i) {
        if (tbl->entries[i].vnodePtr != NULL) {
            kfree(&(tbl->entries[i]));
        }
    }
    sem_destroy(tbl->file_manager_lock); // Destroy the semaphore
    kfree(tbl); // Free the memory allocated for the table
}

struct tableentry* get_entry(struct table* tbl, int index) {
    // Check if the index is within valid bounds
    if (index >= 0 && index < OPEN_MAX && tbl->entries[index].vnodePtr != NULL) {
        return &(tbl->entries[index]); // Return a pointer to the entry at the given index
    } else {
        return NULL; // Return NULL if the index is out of bounds or the entry is not valid
    }
}

