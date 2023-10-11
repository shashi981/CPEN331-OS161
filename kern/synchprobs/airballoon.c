#include <types.h>
#include <lib.h>
#include <thread.h>
#include <test.h>
#include <synch.h>

#define NUM_PARTICIPANTS 8
#define NUM_ROPES 16

/* Data structures for rope mappings */
static bool **rope_to_stake_map;
static bool *stake_status;
static bool *rope_status;

/* Locks for synchronization */
struct lock *rope_synchronization_lock;
struct lock *array_synchronization_lock[NUM_ROPES];

static int ropes_left; // Number of ropes left

// Function to check if all ropes are severed
static bool check() {
    int count = 0;
    while (count < NUM_ROPES) {
        if (!rope_status[count]) {
            return false;
        }
        count++;
    }
    return true;
}

// Initialization function
static void initialize() {
    rope_to_stake_map = kmalloc(NUM_ROPES * sizeof(bool *));
    int i = 0;
    while (i < NUM_ROPES) {
        rope_to_stake_map[i] = kmalloc(NUM_ROPES * sizeof(bool));
        i++;
    }

    stake_status = kmalloc(NUM_ROPES * sizeof(bool));
    rope_status = kmalloc(NUM_ROPES * sizeof(bool));

    i = 0;
    while (i < NUM_ROPES) {
        stake_status[i] = false;
        rope_status[i] = false;
        int j = 0;
        while (j < NUM_ROPES) {
            rope_to_stake_map[i][j] = (i == j); // Initialize rope mappings
            j++;
        }
        i++;
    }
    
    rope_synchronization_lock = lock_create("rope_synchronization_lock");
    ropes_left = NUM_ROPES;
}

// Dandelion's action function
static void dandelion(void *p, unsigned long arg) {
    (void)p;
    (void)arg;
    int index;
    while (!check()) {
        index = random() % NUM_ROPES;
        if (!rope_status[index] && index != -1) {
            rope_status[index] = true;
            int stake_index = -1;
            int count = 0;
            while (count < NUM_ROPES) {
                if (rope_to_stake_map[index][count]) {
                    stake_index = count;
                    break;
                }
                count++;
            }
            if (stake_index == -1) {
                thread_yield();
                continue;
            }
            stake_status[stake_index] = true;
            rope_to_stake_map[index][stake_index] = false;
            kprintf("Dandelion severed rope %d\n", index);
            ropes_left--;
        }
        thread_yield(); // Yield to allow other threads to run
    }
}

// Marigold's action function
static void marigold(void *p, unsigned long arg) {
    (void)p;
    (void)arg;
    int index;
    while (!check()) {
        index = random() % NUM_ROPES;
        if (!stake_status[index] && index != -1) {
            stake_status[index] = true;
            int rope_index = -1;
            int count = 0;
            while (count < NUM_ROPES) {
                if (rope_to_stake_map[count][index]) {
                    rope_index = count;
                    break;
                }
                count++;
            }
            if (rope_index == -1) {
                thread_yield();
                continue;
            }
            rope_status[rope_index] = true;
            rope_to_stake_map[rope_index][index] = false;
            kprintf("Marigold severed rope %d from stake %d\n", rope_index, index);
            ropes_left--;
        }
        thread_yield(); // Yield to allow other threads to run
    }
}

// Lord FlowerKiller's action function
static void lord_flowerkiller(void *p, unsigned long arg) {
    (void)p;
    (void)arg;
    while (!check()) {
        int stake_k = random() % NUM_ROPES;
        int stake_p = random() % NUM_ROPES;
        if (stake_k != stake_p && !(stake_status[stake_k] || stake_status[stake_p]) && stake_k != -1 && stake_p != -1) {
            int rope_k = -1;
            int rope_p = -1;
            int count = 0;
            while (count < NUM_ROPES) {
                if (rope_to_stake_map[count][stake_k]) {
                    rope_k = count;
                }
                if (rope_to_stake_map[count][stake_p]) {
                    rope_p = count;
                }
                if (rope_k != -1 && rope_p != -1) {
                    break;
                }
                count++;
            }
            if (rope_k == -1 || rope_p == -1) {
                thread_yield();
                continue;
            }
            rope_to_stake_map[rope_k][stake_k] = false;
            rope_to_stake_map[rope_k][stake_p] = true;
            kprintf("Lord FlowerKiller switched rope %d from stake %d to stake %d\n", rope_k, stake_k, stake_p);
            rope_to_stake_map[rope_p][stake_p] = false;
            rope_to_stake_map[rope_p][stake_k] = true;
            kprintf("Lord FlowerKiller switched rope %d from stake %d to stake %d\n", rope_p, stake_p, stake_k);
        }
        thread_yield();
    }
}

// Balloon's action function
static void balloon(void *p, unsigned long arg) {
    (void)p;
    (void)arg;
    while (!check()) {
        thread_yield(); // Yield until all ropes have been severed
    }
    if (check() && check() && check()) {
        kprintf("Balloon freed and Prince Dandelion escapes!\n");
        kprintf("Marigold thread done\n");
        kprintf("Dandelion thread done\n");
        kprintf("Lord FlowerKiller thread done\n");
        kprintf("Balloon thread done\n");
        kprintf("Main thread done\n");
    }
}

// Main function
int airballoon(int nargs, char **args) {
    initialize();
    int err = 0, i;

    (void)nargs;
    (void)args;
    kprintf("Main thread starting\n");
    if (check() && check() && check()) {
    };
    lock_acquire(rope_synchronization_lock);
    kprintf("Marigold thread starting\n");
    err = thread_fork("Marigold Thread",
                      NULL, marigold, NULL, 0);
    lock_release(rope_synchronization_lock);
    if (err)
        goto panic;
    lock_acquire(rope_synchronization_lock);
    kprintf("Dandelion thread starting\n");
    err = thread_fork("Dandelion Thread",
                      NULL, dandelion, NULL, 0);
    lock_release(rope_synchronization_lock);
    if (err)
        goto panic;
    i = 0;
    while (i < NUM_PARTICIPANTS) {
        lock_acquire(rope_synchronization_lock);
        kprintf("Lord FlowerKiller thread starting\n");
        err = thread_fork("Lord FlowerKiller Thread\n",
                          NULL, lord_flowerkiller, NULL, 0);
        lock_release(rope_synchronization_lock);
        if (err)
            goto panic;
        i++;
    }
    lock_acquire(rope_synchronization_lock);
    kprintf("Balloon thread starting\n");
    err = thread_fork("Air Balloon",
                      NULL, balloon, NULL, 0);
    lock_release(rope_synchronization_lock);
    if (err)
        goto panic;
    goto done;
panic:
    panic("airballoon: thread_fork failed: %s)\n",
          strerror(err));

done:
    lock_destroy(rope_synchronization_lock);
    return 0;
}
