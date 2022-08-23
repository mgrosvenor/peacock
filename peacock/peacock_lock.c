#include "peacock_lock.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

//Container for state that might be shared
//across threads or processes.
#define SHM_PATH  "/peacock"
#define SHM_MAGIC 0xC0C0C0C0FULL 
#define SMH_VERSION 0x01ULL 
#define SHM_WAIT_TIMEOUT_US (1000 * 1000) //1sec in us
#define SHM_RETRY_DELAY_US (10 * 1000) //10ms

#define LOCK_WAIT_TIMEOUT_US (10 * 1000 * 1000) //10 seconds in us
#define LOCK_RETRY_DELAY_US (5) //5us

typedef struct
{
    uint64_t init_magic;
    uint64_t version;
    pthread_mutex_t  mutex;
    uint64_t lock_count;
} shm_state_t;

static long long shm_mem_size = -1;
static shm_state_t* shm_state = NULL;
static volatile void* shm_map = NULL;
static int shm_fd = -1;

//Open, truncate and mmap a shared memory region
//Optionally do so in either exclusive or non-exclusive mode
//If exclusive, and opening fails, return 1. 
static int _pck_lock_open_map(bool exclusive)
{
    int oflags = O_RDWR;    
    if(exclusive) 
    {
        oflags |= (O_EXCL | O_CREAT); 
    }
    
    shm_fd = shm_open(SHM_PATH, oflags, S_IREAD | S_IWRITE);
    if (shm_fd == -1 && exclusive && (errno == EEXIST))
    {
        return 1;
    }

    if (shm_fd < 0)
    {
        fprintf(stderr, "Could not open shared memory path '%s'. Error: %s\n", SHM_PATH, strerror(errno));
        return -1;
    }

    // we'll make the shared memory region at least 2x page sizes so there'll be an aligned region somewhere in it
    shm_mem_size = ((sizeof(shm_state_t) + 2 * getpagesize() - 1) / getpagesize()) * getpagesize();

    if (exclusive)
    {
        // As the owner of the file, grow the shared memory to a size big enough to hold our data aligned
        //This function zero's the file, so other users will read init_magic == 0 until we've changed it.
        int res = ftruncate(shm_fd, shm_mem_size);
        if (res == -1)
        {
            fprintf(stderr, "Could not resize shared memory region '%s' to '. Error: %s\n", SHM_PATH, strerror(errno));
            return -1;
        }
    }
    else
    {

        // We don't own the file, so let's wait around until someone else grows it for us. Then we can map it
        const int max_retries = SHM_WAIT_TIMEOUT_US / SHM_RETRY_DELAY_US;
        struct stat fstats = { 0 };
        for (int retries = 0; fstats.st_size != shm_mem_size; fstat(shm_fd, &fstats), retries++)
        {
            usleep(SHM_RETRY_DELAY_US); //Just wait a little bit for our best chance at success
            if (retries >= max_retries)
            {
                fprintf(stderr, "Timed out waiting for shared memory to resize!\n");
                return -1; //Failed! 
            }
        }
    }

#if __APPLE__
    shm_map = mmap(NULL, shm_mem_size, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_HASSEMAPHORE, shm_fd, 0);
#else
    shm_map = mmap(NULL, shm_mem_size, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
#endif

    if (shm_map == MAP_FAILED)
    {
        fprintf(stderr, "Could not map shared memory region '%s'. Error: %s\n", SHM_PATH, strerror(errno));
        return -1;
    }

    //Get a page aligned offset in the shared memory region
    size_t aligned_mem = ((((size_t)(shm_map)) + getpagesize() - 1) / getpagesize()) * getpagesize();    
    shm_state = (void*)(aligned_mem);

    return 0;
}


//We're the first one to open the shared segment
//Initialise it into a good state
static int _pck_lock_shm_init()
{
    shm_state->version = SMH_VERSION;

    pthread_mutexattr_t mutexattr;
    int res = pthread_mutexattr_init(&mutexattr);
    if (res != 0)
    {
        fprintf(stderr, "Could not initialize mutex attributes");
        return -1;
    }

    res = pthread_mutexattr_setpshared(&mutexattr, PTHREAD_PROCESS_SHARED);
    if (res != 0)
    {
        fprintf(stderr, "Could not initialize mutex attributes");
        return -1;
    }

    res = pthread_mutex_init(&shm_state->mutex, &mutexattr);
    if (res != 0)
    {
        fprintf(stderr, "Could not initialize mutex ");
        return -1;
    }    

    //let everyone else know that we're done
    __sync_synchronize(); //Make sure memory is commited before we do
    shm_state->init_magic = SHM_MAGIC;
    return 0;
}

//Someone else has opened the shared region first
//Wait until it is ready.
static int _pck_lock_shm_init_wait()
{
    __sync_synchronize(); //Make sure memory is clean before we try to read
    const int max_retries = SHM_WAIT_TIMEOUT_US / SHM_RETRY_DELAY_US;
    for (int retries = 0; shm_state->init_magic != SHM_MAGIC; retries++)
    {
        usleep(SHM_RETRY_DELAY_US); //Just wait a little bit for our best chance at success
        if (retries >= max_retries)
        {
            fprintf(stderr, "Timed out waiting for shared memory to initialize!");
            return -1; //Failed! 
        }
    }

    if (shm_state->version != SMH_VERSION)
    {
        fprintf(stderr, "Different versions of this shared memory region are coexisting!");
        return -1;
    }
    
    return 0;
}


// Initialize a shared memory segment for shared state
// Force init is used to recover from situations where the shared region
// still exists, but is in a bad state (e.g. lock is held)
static int _pck_lock_open_shm(bool force)
{
    //Did someone come here before us...
    if (shm_state != NULL)
    {
        //This is the safest thing we can do, it should just fall through
        const int res = _pck_lock_shm_init_wait();
        if (res < 0)
        {
            pck_lock_close();
            return -1;
        }
    }

    //Try to get exclusive access first!
    int res = _pck_lock_open_map(true);
    if (res == 0)
    {
        //We're the owner of this memory, so we need to initialize it and 
        res = _pck_lock_shm_init();
        if (res != 0)
        {
            pck_lock_close();
            return -1;
        }

    }
    else if (res == 1) {
        //That failed, so wait around until someone else has initialized
        res = _pck_lock_open_map(false);
        if (res < 0)
        {
            return -1;
        }

        // The shm region exists, but we're going to force initialization
        // Note: there's a race condition here if this is done with
        // more than one process. Should be done with care, really only as
        // a last resort!
        if (force)
        {
            res = _pck_lock_shm_init();
        }
        else
        {
            res = _pck_lock_shm_init_wait();
        }

        if (res < 0)
        {
            pck_lock_close();
            return -1;
        }
    }

    return 0;
}


// This function is *not* thread safe. It should be called by the 
// main thread before other threads are started. All threads
// can then share the internal state variables.
int pck_lock_init(bool force)
{
    int res = _pck_lock_open_shm(force);
    if (res != 0)
    {
        fprintf(stderr, "Error opening shared memory region\n");
        return -1;
    }
    return 0;
}


int pck_lock()
{
    __sync_synchronize(); //Make sure everyone else sees this!
    const int max_retries = LOCK_WAIT_TIMEOUT_US / LOCK_RETRY_DELAY_US;    
    for (int retries = 0; retries < max_retries; retries++)
    {        
        int err = pthread_mutex_trylock(&shm_state->mutex);        
        if (err == 0)
        {            
            shm_state->lock_count++;
            return 0;
        }
        
        if (err != EBUSY)
        {
            fprintf(stderr, "Unexpected mutex error %i!\n", err);
            return -1;
        }
        usleep(LOCK_RETRY_DELAY_US); //Slow down so this is not a spin lock!
    }
    fprintf(stderr, "Timed out waiting to acquire lock!\n");
    return -1; //Failed!         
}

int pck_unlock()
{
    const int max_retries = LOCK_WAIT_TIMEOUT_US / LOCK_RETRY_DELAY_US;
    for (int retries = 0; retries < max_retries; retries++)
    {
        int err = pthread_mutex_unlock((pthread_mutex_t*)&shm_state->mutex);
        if (err == 0)
        {
            sched_yield();//Help give others a chance!
            return 0;
        }        
        usleep(LOCK_RETRY_DELAY_US); //Slow down so this is not a spin lock!
    }
    fprintf(stderr, "Timed out waiting to release lock!\n");
    return -1; //Failed!     
}


void pck_lock_close()
{    
    if (shm_map)
    {
        munmap((void*)shm_map, shm_mem_size);
    }
    if (shm_fd)
    {
        close(shm_fd);
    }
}

//In case the shm memory region is really unrecoverable
void pck_lock_shm_unlink()
{
    shm_unlink(SHM_PATH);
}
