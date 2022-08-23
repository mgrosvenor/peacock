#ifndef PEACOCK_LOCK
#define PEACOCK_LOCK

#include <peacock/peacock_util.h>
#include <stdbool.h>

int pck_lock_init(bool force);
int pck_lock();
int pck_unlock();
void pck_lock_close();
void pck_lock_shm_unlink();

#endif // PEACOCK_LOCK
