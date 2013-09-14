/*******************************************************************************
 * FILE:                rwsync.c
 * TITLE:               Reader-Writer Synchronization in FIFO order with pthreads
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
 * Name of Function:    main
 * Purpose:             creates reader writer thread and manages them
 * Parameters:          command line arguments
 *                      argc = 3 argv[1] = reader argv[2] = writer
 * Return Value:        int
 * Calls to:            none directly
 * Called from:         --
 * Method:              1. Calculate total items to read
 *                      2. generates/flushes reader output files
 *                      3. call function to read data in synchronized way till
 *                      all items are read
 ******************************************************************************/
int main(int argc, char **argv)
{
    pthread_t *r_thread;        // pointer to reader thread
    pthread_t *w_thread;        // pointer to writer thread
    long mtime, seconds, useconds;  // clock time variables
    
    struct writer_struct* writer;   // pointer to writer structure
    
    struct reader_struct* reader;   // pointer to reader structutre
    
    struct monitor_struct *monitor; // pointer to monitor variable
    
    int tot_writers, tot_readers, i;
    
    /* get the starting time from where elapsed tiem will be measures */
    gettimeofday(&start, NULL);
    
    g_reader_id=1;
    g_writer_id=1;
    
    /* if number of arguments are not equal to 3 */
    if (argc != 3)
    {
        printf("RW SYNC: Error - Invalid number of command line arguments\n");
        exit(TRUE);
    }
    
    tot_readers = atoi(argv[1]);
    tot_writers = atoi(argv[2]);
    
    /* initialize the monitor */
    monitor = initMonitor(tot_readers, tot_writers);
    
    /* allocate memory */
    if((writer = (struct writer_struct*) malloc(tot_writers * sizeof(struct writer_struct))) == NULL)
    {
        perror("malloc");
        return(TRUE);
    }
    
    if((reader = (struct reader_struct*) malloc(tot_readers * sizeof(struct reader_struct))) == NULL)
    {
        perror("malloc");
        return(TRUE);
    }
    
    if((w_thread = (pthread_t*) malloc(tot_writers * sizeof(pthread_t))) == NULL)
    {
        perror("malloc");
        return(TRUE);
    }
    
    if((r_thread = (pthread_t*) malloc(tot_readers * sizeof(pthread_t))) == NULL)
    {
        perror("malloc");
        return(TRUE);
    }
    
    /*create the writer output file with global file pointer pointing to it */
    if((out_file = fopen("writer_output_file","w")) == NULL)
    {
        perror("fopen");
        exit(TRUE);
    }
    fclose(out_file);
    
    /* create reader and writer threads */
    for(i=1;i<=tot_writers;i++)
    {
        
        initWriter((writer+i), monitor, g_writer_id);
        pthread_create((w_thread+i),NULL,writerHandler,(writer+i));
        g_writer_id++;
    }
    
    for(i=1;i<=tot_readers;i++)
    {
        
        initReader((reader+i), monitor, g_reader_id);
        pthread_create((r_thread+i),NULL,readerHandler,(reader+i));
        g_reader_id++;
        
    }
    
    /* wait for reader and writer threads to terminate */
    for(i=1;i<=tot_writers;i++)
    {
        pthread_join(w_thread[i], NULL);
        
    }
    
    for(i=1;i<=tot_readers;i++)
    {
        pthread_join(r_thread[i], NULL);
        
    }
    
    return (0);
}














