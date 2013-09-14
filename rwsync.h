/*******************************************************************************
 * FILE:                rwsync.h
 * TITLE:               Header file for reader-writer synchronization
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

#ifndef _rwsync_h
#define _rwsync_h

/* Including Standard libraries */
#include <stdio.h>
#include <sched.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

/* Defining constants */
#define FALSE 0
#define TRUE 1
#define READER 0
#define WRITER 1
#define MAX_BUF_SIZE 1024
#define MAX_ITERATIONS 20
#define DECIMAL 10
#define WRITER_MAX_ITEMS 100

/* Defining global variable */
int g_reader_id;
int g_writer_id;  // ID generating counter for reader and writer
struct timeval start;               // Starting time
FILE *out_file;                     // pointer to writer output file


/* Structure of monitor variable which is shared by reader and writer */
struct monitor_struct
{
    int writing;        // flag to determing that the resource is being written
    int r_count;        // reader count
    int r_waiting;      // waiting reader count
    int w_waiting;      // waiting writer count
    int total_readers;  // total number of reader given from command line
    int total_writers;  // total number of writers given from command line
    
    int queue [5000];   // a large queue to store waiting order of max threads
    int queue_head;     // head of the queue
    int queue_tail;     // tail of the queue
    
    pthread_mutex_t *mutex_lock;    // mutex lock variable
    pthread_cond_t *can_read, *can_write;   // condition variables
};


/* Structure of reader thread */
struct reader_struct
{
    struct monitor_struct* monitor; // pointer to monitor variable
    int r_id;                       // ID of the reader
    int r_item;                     // no of item read
    long int r_in_seek;             // read position
};

/* Structure of write thread */
struct writer_struct
{
    struct monitor_struct* monitor; // pointer to monitor variable
    int w_id;                       // ID of the writer
    int w_item;                     // no of item written
    
};

/* Defining prototypes */
long timeElapsed(void);
char* myItoa(int val, int base);

struct monitor_struct* initMonitor(int tot_readers, int tot_writers);

void initReader(struct reader_struct* writer, struct monitor_struct* mon, int id);
void *readerHandler (void *args);
void startReading (struct monitor_struct *monitor, int r_id);
void readData (int r_id, int *r_item, long int *r_in_seek);
void finishReading (struct monitor_struct *monitor);

void initWriter(struct writer_struct* writer, struct monitor_struct* mon, int id);
void *writerHandler (void *args);
void startWriting (struct monitor_struct *monitor, int w_id);
void writeData (int w_id, int *w_item);
void finishWriting (struct monitor_struct *monitor);

#endif