/*******************************************************************************
 * FILE:                reader.c
 * TITLE:               reader functions for reader-writer synchronization
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
 * Name of Function:    readerHandler
 * Purpose:             reads the writer output file till maximum item and copies
 *                      to its own output file <rid_output_file>
 * Parameters:          pointer to arguments as void
 * Return Value:        void
 * Calls to:            startReading, readData, finishReading
 * Called from:         main
 * Method:              1. Calculate total items to read
 *                      2. generates/flushes reader output files
 *                      3. call function to read data in synchronized way till
 *                      all items are read
 ******************************************************************************/
void *readerHandler (void *args)
{
    struct reader_struct *reader;
    int tot_items;
    char *id;
    char in_file_name[16];
    FILE *r_out_file;
    
    reader = (struct reader_struct *)args;
    
    /* total items is number of writer X WRITER_MAX_ITEMS */
    tot_items = reader->monitor->total_writers * WRITER_MAX_ITEMS;
    
    /* convert the id of the reader to string */
    id = myItoa(reader->r_id,DECIMAL);
    /* generate the filename */
    strcpy(in_file_name,"R");
    strcat(in_file_name,id);
    strcat(in_file_name,"_output_file");
    
    /* generate/flush all reader files needed */
    if((r_out_file = fopen(in_file_name,"w")) == NULL)
    {
        perror("fopen");
        exit(TRUE);
    }
    fclose(r_out_file);
    
    /* read items continously untill all items are read */
    while(reader->r_item <= tot_items)
    {
        printf("%ld R%d waiting-for-lock\n", timeElapsed(), reader->r_id);
        startReading(reader->monitor, reader->r_id);
        printf("%ld R%d lock-acquired\n", timeElapsed(), reader->r_id);
        
        readData(reader->r_id, &(reader->r_item),&(reader->r_in_seek));
        
        finishReading(reader->monitor);
        printf("%ld R%d lock-released\n",timeElapsed(), reader->r_id);
        
    }
    
    return;
}


/*******************************************************************************
 * Name of Function:    startReading
 * Purpose:             acquire lock for the reader
 * Parameters:          pointer to the monitor, reader id
 * Return Value:        void
 * Calls to:            none
 * Called from:         readerHandler
 * Method:              1. locks mutex
 *                      2. waits till shared resource is free to read by entering
 *                      in wait queue
 *                      3. if the next item in the waiting queue is reader then
 *                      signal it to read
 *                      4. release the mutex
 ******************************************************************************/
void startReading (struct monitor_struct *monitor, int r_id)
{
    int queue_pos;      // Variable to store position in the queue
    
    
    pthread_mutex_lock(monitor->mutex_lock);    // lock mutex
    monitor->r_waiting++;       // Increment waiting readers
    
    if(monitor->writing || monitor->w_waiting)
    {   //  if shared resource is being written or writer is waiting
        
        queue_pos = ++monitor->queue_tail;  // take position in queue
        
        monitor->queue[monitor->queue_tail] = READER;   // mark position in queue
        
        /* wait till shared resource is free and queue head reached to this
         thread queue position */
        do
        {
            pthread_cond_wait(monitor->can_read, monitor->mutex_lock);
            
        } while (monitor->writing || queue_pos != monitor->queue_head);
        
        /* If there are elements in queue */
        if (monitor->queue_head != monitor->queue_tail)
        {
            /* check if the next waiting thread is reader*/
            if(monitor->queue[(monitor->queue_head+1)]==READER)
            {
                /* advace the head to set turn to the next thread */
                monitor->queue_head++;
                
            }
        }
        // broadcast to readers so that they can start reading if its their turn
        pthread_cond_broadcast(monitor->can_read);
    }
    
    monitor->r_waiting--;   // decrement waiting readers
    monitor->r_count++;     // increment reading readers
    pthread_mutex_unlock(monitor->mutex_lock);  // unlock the mutex
    return;
}
/*******************************************************************************
 * Name of Function:    readData
 * Purpose:             read data from the writers file and copy to reader's
 *                      output file
 * Parameters:          reader id, item read, last read position
 * Return Value:        void
 * Calls to:            none
 * Called from:         readerHandler
 * Method:              1. Generate reader's output file name
 *                      2. Open reader's input and output file
 *                      3. read and copy data
 *                      4. close both the files
 ******************************************************************************/
void readData (int r_id, int *r_item, long int *r_in_seek)
{
    FILE *r_out_file;       // reader output file
    FILE *r_in_file;        // reader input file
    char in_file_name[16];  // file name buffer
    char read_line[MAX_BUF_SIZE];   // buffer to read from file
    char *id;                       // id of the reader
    
    /* convert id to string */
    id = myItoa(r_id,DECIMAL);
    /* generate filename */
    strcpy(in_file_name,"R");
    strcat(in_file_name,id);
    strcat(in_file_name,"_output_file");
    
    /* open the reader input file in read mode (writer's file) */
    if((r_in_file = fopen("writer_output_file","r")) == NULL)
    {
        perror("fopen");
        exit(TRUE);
    }
    /* seek to the last read posotion */
    fseek(r_in_file,(*r_in_seek),SEEK_SET);
    
    /* open the reader's output file in append mode */
    if((r_out_file = fopen(in_file_name,"a+")) == NULL)
    {
        perror("fopen");
        exit(TRUE);
    }
    /* start copying till end of file */
    while(fgets(read_line,MAX_BUF_SIZE,r_in_file)!=NULL)
    {
        fputs(read_line, r_out_file);
        //printf("RW SYNC: R%d wrote: %s\n",r_id, read_line);
        (*r_item)++;
        (*r_in_seek) += (strlen(read_line));
    }
    /* close both files */
    fclose(r_out_file);
    fclose(r_in_file);
    
    return;
}


/*******************************************************************************
 * Name of Function:    finishReading
 * Purpose:             releases lock and give chance to writers
 * Parameters:          pointer to the monitor
 * Return Value:        void
 * Calls to:            none
 * Called from:         readerHandler
 * Method:              1. locks mutex
 *                      2. decrement reader count
 *                      3. if last reader and writer waiting moves the queue head
 *                      4. broadcasts to writer and unlock mutex
 ******************************************************************************/
void finishReading (struct monitor_struct *monitor)
{
    /* get lock on mutex */
    pthread_mutex_lock(monitor->mutex_lock);
    monitor->r_count--;         // decrement reader count
    if(monitor->r_count == 0 && monitor->w_waiting)
    {   // if this is the last reader and there is writer waiting move the queue head
        monitor->queue_head++;
    }
    pthread_cond_broadcast(monitor->can_write);     // signal writer
    pthread_mutex_unlock(monitor->mutex_lock);      // unlock mutex
    return;
}