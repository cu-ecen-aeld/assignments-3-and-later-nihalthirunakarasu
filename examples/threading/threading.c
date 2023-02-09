#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    
    int status_int;

    struct thread_data* thread_param_data = (struct thread_data*)thread_param;

    // struct timespec time_sleep;

    // time_sleep.tv_sec = (int)((thread_param_data->wait_to_obtain_ms)/100);
    // time_sleep.tv_nsec = ((thread_param_data->wait_to_obtain_ms)/100)*1000000;
    // status_int = nanosleep(&time_sleep, &time_sleep);
    status_int = usleep(thread_param_data->wait_to_obtain_ms*1000);
    if(status_int != 0)
    {
        ERROR_LOG("Error: Failed to sleep through nanosleep(). Error code: %d", errno);
        thread_param_data->thread_complete_success = false;
        return thread_param_data;
    }

    status_int = pthread_mutex_lock(thread_param_data->mutex);
    if(status_int != 0)
    {
        ERROR_LOG("Error: Failed to lock mutex through pthread_mutex_lock(). Error code: %d", errno);
        thread_param_data->thread_complete_success = false;
        return thread_param_data;
    }

    // time_sleep.tv_sec = (int)((thread_param_data->wait_to_release_ms)/100);
    // time_sleep.tv_nsec = ((thread_param_data->wait_to_release_ms)/100)*1000000;
    // status_int = nanosleep(&time_sleep, &time_sleep);
    status_int = usleep(thread_param_data->wait_to_obtain_ms*1000);
    if(status_int != 0)
    {
        ERROR_LOG("Error: Failed to sleep through nanosleep(). Error code: %d", errno);
        thread_param_data->thread_complete_success = false;
        return thread_param_data;
    }

    status_int = pthread_mutex_unlock(thread_param_data->mutex);
    if(status_int != 0)
    {
        ERROR_LOG("Error: Failed to unlock mutex through pthread_mutex_unlock(). Error code: %d", errno);
        thread_param_data->thread_complete_success = false;
        return thread_param_data;
    }

    thread_param_data->thread_complete_success = true;
    return thread_param_data;
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
    struct thread_data* thread_param_data = (struct thread_data *)malloc(sizeof(struct thread_data));
    if(thread_param_data == NULL)
    {
        ERROR_LOG("Error: Failled to allocate through malloc(). Error code: %d", errno);
        return false;
    }

    int status_int;

    thread_param_data->thread = thread;
    thread_param_data->mutex = mutex;
    thread_param_data->wait_to_obtain_ms = wait_to_obtain_ms;
    thread_param_data->wait_to_release_ms = wait_to_release_ms;
    thread_param_data->thread_complete_success = false;

    status_int = pthread_create(thread, 
                                NULL,
                                threadfunc,
                                thread_param_data);
    if(status_int != 0)
    {
        ERROR_LOG("Error: Failled to create thread through pthread_create(). Error code: %d", errno);
        return false;
    }

    // free(thread_param_data);

    return true;
}

