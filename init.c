/*******************************************************************************
 * FILE:                init.c
 * TITLE:               initialization functions for reader-writer 
 *                      synchronization application with pthreads.
 * Date:                October 25, 2012.
 * Author:              Rohit Sinha
 * Email:               sinha049@umn.edu
 * Assignment:          CSCI 5103: Operating Systems - Assignment 4
 * Due Date:            October 29, 2012
 *
 * Problem Statement:   implement Reader-Writer synchronization through POSIX
 *                      threads. Complete problem statement: http://www-users.cselabs.umn.edu/classes/Fall-2012/csci5103/Assignments/Assignment4-POSIX.pdf
********************************************************************************/

/* Including header file*/
#include "rwsync.h"

/*******************************************************************************
 * Name of Function:    initMonitor
 * Purpose:             Initialized the monitor variable
 * Parameters:          total number of readers (int), total number of
 *                      writers(int)
 * Return Value:        pointer to the initialized monitor
 * Calls to:            none
 * Called from:         main
 * Method:              1. Safely allocate memory to all monitor variables
 *                      2. Initializes these variables
 ******************************************************************************/
struct monitor_struct* initMonitor(int tot_readers, int tot_writers)
{
    struct monitor_struct *monitor;
    
    /* Safely Allocate memory to different variables */
    if((monitor = (struct monitor_struct*) malloc (sizeof(struct monitor_struct))) == NULL)
    {
        perror("malloc");
        return (NULL);
    }
    
    if((monitor->mutex_lock = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t))) == NULL)
    {
        printf("RW SYNC: Error - Unable to allocate mutex lock\n");
        free(monitor);
        return(NULL);
    }
    if((monitor->can_read = (pthread_cond_t *) malloc (sizeof (pthread_cond_t))) == NULL)
    {
        printf("RW SYNC: Error - Unable to allocate condition variable\n");
        free(monitor->mutex_lock);
        free(monitor);
        return(NULL);
    }
    if((monitor->can_write = (pthread_cond_t *) malloc (sizeof (pthread_cond_t))) == NULL)
    {
        printf("RW SYNC: Error - Unable to allocate condition variable\n");
        free(monitor->can_read);
        free(monitor->mutex_lock);
        free(monitor);
        return(NULL);
    }
    if(pthread_mutex_init(monitor->mutex_lock, NULL))
    {
        perror("pthread_mutex_init");
        exit(TRUE);
    }
    if(pthread_cond_init(monitor->can_read, NULL))
    {
        perror("pthread_cond_int");
        exit(TRUE);
    }
    if(pthread_cond_init(monitor->can_write,NULL))
    {
        perror("pthread_cond_int");
        exit(TRUE);
    }
    
    /* Initialize variables */
    monitor->writing = FALSE;
    monitor->r_count = 0;
    monitor->r_waiting = 0;
    monitor->w_waiting = 0;
    monitor->total_readers = tot_readers;
    monitor->total_writers = tot_writers;
    
    monitor->queue_head =-1;
    monitor->queue_tail =-1;
    
    return (monitor);
}


/*******************************************************************************
 * Name of Function:    initWriter
 * Purpose:             Initializes writers
 * Parameters:          pointer to writer, pointer to monitor, id to be alloted 
 *                      to the writer
 * Return Value:        void
 * Calls to:            none
 * Called from:         main
 * Method:              1. Initializes the writer
 ******************************************************************************/
void initWriter(struct writer_struct* writer, struct monitor_struct* mon, int id)
{
    
    writer->monitor = mon;
    writer->w_id = id;
    writer->w_item = 1;     // Initialized to 1 to make loop initially
    //printf("RW SYNC: Intialized Writer W%d\n",id);
}


/*******************************************************************************
 * Name of Function:    initReader
 * Purpose:             Initializes readers
 * Parameters:          pointer to reader, pointer to monitor, id to be alloted
 *                      to the reader
 * Return Value:        void
 * Calls to:            none
 * Called from:         main
 * Method:              1. Initializes the reader
 ******************************************************************************/
void initReader(struct reader_struct* reader, struct monitor_struct* mon, int id)
{
    
    reader->monitor = mon;
    reader->r_id = id;
    reader->r_item = 1;   // Initialized to 1 to make loop initially
    reader->r_in_seek=0;
}