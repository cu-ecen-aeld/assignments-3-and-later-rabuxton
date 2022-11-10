#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;

    struct thread_data *data = (struct thread_data *)thread_param;

    DEBUG_LOG("Sleeping for %d ms", data->wait_to_obtain_ms);
    int ret = usleep(1000*data->wait_to_obtain_ms);
    if(ret != 0)
    {
        ERROR_LOG("usleep error %d", ret);
        data->thread_complete_success = 0;
        return thread_param;
    }

    DEBUG_LOG("Locking mutex");
    ret = pthread_mutex_lock(data->mutex);
    if(ret != 0)
    {
        ERROR_LOG("Mutex lock error %d", ret);
        data->thread_complete_success = 0;
        return thread_param;
    }

    DEBUG_LOG("Sleeping for %d ms", data->wait_to_release_ms);
    ret = usleep(1000*data->wait_to_release_ms);
    if(ret != 0)
    {
        ERROR_LOG("usleep error %d", ret);
        data->thread_complete_success = 0;
        return thread_param;
    }

    DEBUG_LOG("Unlocking mutex");
    ret = pthread_mutex_unlock(data->mutex);
    if(ret != 0)
    {
        ERROR_LOG("Mutex unlock error %d", ret);
        data->thread_complete_success = 0;
        return thread_param;
    }

    data->thread_complete_success = 1;

    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */

    /* Allocate thread_data */
    DEBUG_LOG("Allocating thread_data");
    struct thread_data *params = malloc(sizeof(struct thread_data));

    /* Setup mutex and wait args */
    params->wait_to_obtain_ms = wait_to_obtain_ms;
    params->wait_to_release_ms = wait_to_release_ms;
    params->mutex = mutex;

    /* Create the pthread */
    DEBUG_LOG("Creating thread");
    int ret = pthread_create(thread, 
                            NULL, 
                            threadfunc, 
                            (void *)(params));
    if(ret != 0)
    {
        ERROR_LOG("Failed to create thread");
        return false;
    }

    return true;
}

