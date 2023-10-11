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
struct lock *array_synchronization_lock[NUM_ROPES];

struct semaphore *thread_sem;


static int ropes_left; // Number of ropes left

static bool check() {
    for (int count = 0; count < NUM_ROPES; count++) {
        if (!rope_status[count]) {
            return false;
        }
    }
    return true;
}

/* Initialize data structures and locks */
static void init() {
    thread_sem = sem_create("thread_sem", 0);
if (thread_sem == NULL) {
    panic("Could not create thread semaphore");
}
    rope_to_stake_map = kmalloc(NUM_ROPES * sizeof(bool *));
    for (int i = 0; i < NUM_ROPES; i++) {
        rope_to_stake_map[i] = kmalloc(NUM_ROPES * sizeof(bool));
        array_synchronization_lock[i] = lock_create("array_synchronization_lock");
    }

    stake_status = kmalloc(NUM_ROPES * sizeof(bool));
    rope_status = kmalloc(NUM_ROPES * sizeof(bool));

    for (int i = 0; i < NUM_ROPES; i++) {
        stake_status[i] = false;
        rope_status[i] = false;
        for (int j = 0; j < NUM_ROPES; j++) {
            rope_to_stake_map[i][j] = (i == j); // Initialize rope mappings
        }
    }
    
    ropes_left = NUM_ROPES;
}

/* Dandelion's action */
static void dandelion(void *p, unsigned long arg) {
    (void)p;
    (void)arg;
    int index;
    while (!check()) {
        index = random() % NUM_ROPES;
        lock_acquire(array_synchronization_lock[index]);
        if (!rope_status[index] && index != -1) {
            rope_status[index] = true;
            int stake_index = -1;
            for (int count = 0; count < NUM_ROPES; count++) {
                if (rope_to_stake_map[index][count]) {
                    stake_index = count;
                    break;
                }
            }
            if (stake_index == -1) {
                lock_release(array_synchronization_lock[index]);
                thread_yield();
                continue;
            }
            stake_status[stake_index] = true;
            rope_to_stake_map[index][stake_index] = false;
            kprintf("Dandelion severed rope %d\n", index);
            ropes_left--;
        }
        lock_release(array_synchronization_lock[index]);
        thread_yield(); // Yield to allow other threads to run
    }
    kprintf("Dandelion thread done\n");
    V(thread_sem);
}

/* Marigold's action */
static void marigold(void *p, unsigned long arg) {
    (void)p;
    (void)arg;
    int index;
    while (!check()) {
        index = random() % NUM_ROPES;
        lock_acquire(array_synchronization_lock[index]);
        if (!stake_status[index] && index != -1) {
            stake_status[index] = true;
            int rope_index = -1;
            for (int count = 0; count < NUM_ROPES; count++) {
                if (rope_to_stake_map[count][index]) {
                    rope_index = count;
                    break;
                }
            }
            if (rope_index == -1) {
                lock_release(array_synchronization_lock[index]);
                thread_yield();
                continue;
            }
            rope_status[rope_index] = true;
            rope_to_stake_map[rope_index][index] = false;
            kprintf("Marigold severed rope %d from stake %d\n", rope_index, index);
            ropes_left--;
        }
        lock_release(array_synchronization_lock[index]);
        thread_yield(); // Yield to allow other threads to run
    }
    kprintf("Marigold thread done\n");
    V(thread_sem);
}

/* Lord FlowerKiller's action */
/* Lord FlowerKiller's action */
static void lord_flowerkiller(void *p, unsigned long arg) {
    (void)p;
    (void)arg;

    while (!check()) {
        int stake_k = random() % NUM_ROPES;
        int stake_p;

        do {
            stake_p = random() % NUM_ROPES;
        } while (stake_k == stake_p); // Ensure the two stakes are different

        if (stake_k > stake_p) { // Ensure consistent lock ordering
            int temp = stake_k;
            stake_k = stake_p;
            stake_p = temp;
        }

        lock_acquire(array_synchronization_lock[stake_k]);
        lock_acquire(array_synchronization_lock[stake_p]);

        int rope_k = -1;
        int rope_p = -1;
        for (int count = 0; count < NUM_ROPES; count++) {
            if (rope_to_stake_map[count][stake_k]) {
                rope_k = count;
            }
            if (rope_to_stake_map[count][stake_p]) {
                rope_p = count;
            }
            if (rope_k != -1 && rope_p != -1) {
                break;
            }
        }

        if (rope_k != -1 && rope_p != -1) {
            rope_to_stake_map[rope_k][stake_k] = false;
            rope_to_stake_map[rope_k][stake_p] = true;
            kprintf("Lord FlowerKiller switched rope %d from stake %d to stake %d\n", rope_k, stake_k, stake_p);

            rope_to_stake_map[rope_p][stake_p] = false;
            rope_to_stake_map[rope_p][stake_k] = true;
            kprintf("Lord FlowerKiller switched rope %d from stake %d to stake %d\n", rope_p, stake_p, stake_k);
        }

        lock_release(array_synchronization_lock[stake_p]);
        lock_release(array_synchronization_lock[stake_k]);
        thread_yield();
    }
    kprintf("Lord FlowerKiller thread done\n");
    V(thread_sem);
}
/* Balloon's action */
static void balloon(void *p, unsigned long arg) {
    (void)p;
    (void)arg;
    while (!check()) {
        thread_yield(); // Yield until all ropes have been severed
    }
    kprintf("Balloon freed and Prince Dandelion escapes!\n");
    kprintf("Balloon thread done\n");
    V(thread_sem);
}

/* Main function */
int airballoon(int nargs, char **args) {
    init();
    int err = 0, i;

    (void)nargs;
    (void)args;
    kprintf("Main thread starting\n");
    if (check() && check() && check()) {
    };

    kprintf("Marigold thread starting\n");
    err = thread_fork("Marigold Thread",
                      NULL, marigold, NULL, 0);
    if (err)
        goto panic;

    kprintf("Dandelion thread starting\n");
    err = thread_fork("Dandelion Thread",
                      NULL, dandelion, NULL, 0);
    if (err)
        goto panic;

    for (i = 0; i < (2 + NUM_PARTICIPANTS + 1); i++) {
        P(thread_sem);
        kprintf("Lord FlowerKiller thread starting\n");
        err = thread_fork("Lord FlowerKiller Thread\n",
                          NULL, lord_flowerkiller, NULL, 0);
        if (err)
            goto panic;
    }

    kprintf("Balloon thread starting\n");
    err = thread_fork("Air Balloon",
                      NULL, balloon, NULL, 0);
    if (err)
        goto panic;

    goto done;

panic:
    panic("airballoon: thread_fork failed: %s)\n",
          strerror(err));

done:
    for (int i = 0; i < NUM_ROPES; i++) {
        lock_destroy(array_synchronization_lock[i]);
    }
    sem_destroy(thread_sem);
    return 0;
}
