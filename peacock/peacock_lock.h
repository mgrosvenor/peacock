#ifndef PEACOCK_LOCK
#define PEACOCK_LOCK
#include <peacock/peacock_util.h>

int pck_lock_init();
int pck_lock();
int pck_unlock();
void pck_lock_close();

#endif // PEACOCK_LOCK
