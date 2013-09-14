/*******************************************************************************
 * FILE:                writer.c
 * TITLE:               writer functions for reader-writer synchronization
 *                      application with pthreads.
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
 * Name of Function:    writerHandler
 * Purpose:             writes the writer output file till maximum item
 * Parameters:          pointer to arguments as void
 * Return Value:        void
 * Calls to:            startWriting, writeData, finishWriting
 * Called from:         main
 * Method:              1. get mutually exclusive access to writer output file
 *                      2. writes 5 items
 *                      3. release the shared writer file
 *                      4. repeats this process till max iteration(20) for each
 *                      writer
 ******************************************************************************/
void *writerHandler (void *args)
{
    struct writer_struct *writer;
    int i;
    
    writer = (struct writer_struct *)args;
    
    /* write till max iterations for each writer */
    for(i=1;i<=MAX_ITERATIONS;i++)
    {
        printf("%ld W%d waiting-for-lock\n", timeElapsed(), writer->w_id);
        startWriting(writer->monitor, writer->w_id);
        printf("%ld W%d lock-acquired\n", timeElapsed(), writer->w_id);
        
        writeData(writer->w_id, &(writer->w_item));
        
        finishWriting(writer->monitor);
        printf("%ld W%d lock-released\n",timeElapsed(), writer->w_id);
    }
    return;
}

/*******************************************************************************
 * Name of Function:    startWriting
 * Purpose:             acquire lock for the writer
 * Parameters:          pointer to the monitor, writer id
 * Return Value:        void
 * Calls to:            none
 * Called from:         writerHandler
 * Method:              1. locks mutex
 *                      2. waits till shared resource is free to write by
 *                      entering in wait queue
 *                      3. set writing variable to true
 *                      4. release the mutex
 ******************************************************************************/
void startWriting (struct monitor_struct *monitor, int w_id)
{
    int queue_pos;
    
    /* lock the mutex */
    pthread_mutex_lock(monitor->mutex_lock);
    
    if (monitor->r_count || monitor->writing || monitor->w_waiting )
    { /* if there are reader or writer writing or waiting */
        
        monitor->w_waiting++;
        queue_pos = ++monitor->queue_tail;
        monitor->queue[monitor->queue_tail] = WRITER;
        do
        {   /* block writer */
            pthread_cond_wait(monitor->can_write, monitor->mutex_lock);
            
        }while(monitor->r_count || monitor->writing || queue_pos != monitor->queue_head);
        
        monitor->w_waiting--;
    }
    /* set writing to true */
    monitor->writing = TRUE;
    /* unlock the mutex */
    pthread_mutex_unlock(monitor->mutex_lock);
    return;
}


/*******************************************************************************
 * Name of Function:    writeData
 * Purpose:             writes data to the writer file
 * Parameters:          writer id, items written
 * Return Value:        void
 * Calls to:            none
 * Called from:         writerHandler
 * Method:              1. opens writer output file in append mode
 *                      2. write five items to the writer output file
 *                      3. close the file
 ******************************************************************************/
void writeData (int w_id, int *w_item)
{
    int i;
    
    /* open writer output file */
    if((out_file = fopen("writer_output_file","a+")) == NULL)
    {
        perror("fopen");
        exit(TRUE);
    }
    
    /* write items */
    for(i=(*w_item); i<((*w_item)+5); i++)
    {
        fprintf(out_file, "<W%d,%d>\n",w_id,i);
    }
    *w_item = *w_item + 5;
    
    /* close the file */
    fclose(out_file);
    
    return;
}


/*******************************************************************************
 * Name of Function:    finishWriting
 * Purpose:             releases lock and give chance to reader or queue
 * Parameters:          pointer to the monitor
 * Return Value:        void
 * Calls to:            none
 * Called from:         readerHandler
 * Method:              1. locks mutex
 *                      2. clear the writing flag
 *                      3. if waiting thread give signal queue head
 *                      4. if queue is empty signal readers
 *                      5. unlock the mutex
 ******************************************************************************/
void finishWriting (struct monitor_struct *monitor)
{
    /* lock the mutex */
    pthread_mutex_lock(monitor->mutex_lock);
    
    
    monitor->writing = FALSE;   // clear writing flag
    
    if(monitor->queue_head < monitor->queue_tail)
    {   // if there are waiting threads in the queue
        
        monitor->queue_head++;  // increment the queue head
        if (monitor->queue[monitor->queue_head]==READER)
        {   // if queue front is reader
            pthread_cond_broadcast(monitor->can_read);
        }
        else if (monitor->queue[monitor->queue_head]==WRITER)
        {   //if queue front is writer
            pthread_cond_broadcast(monitor->can_write);
        }
    }
    else
    {
        pthread_cond_broadcast(monitor->can_read);
    }
    
    /* unlock the mutex */
    pthread_mutex_unlock(monitor->mutex_lock);
    
    return;
}
