/*******************************************************************************
 * FILE:                utility.c
 * TITLE:               utility functions for reader-writer synchronization
 *                      application with pthreads.
 * Date:                October 25, 2012.
 * Author:              Rohit Sinha
 * Email:				sinha049@umn.edu
 * Assignment:          CSCI 5103: Operating Systems - Assignment 4
 * Due Date:            October 29, 2012
 *
 * Problem Statement:   implement Reader-Writer synchronization through POSIX
 *                      threads. Complete problem statement: http://www-users.cselabs.umn.edu/classes/Fall-2012/csci5103/Assignments/Assignment4-POSIX.pdf
********************************************************************************/

/* Including header file*/
#include "rwsync.h"

/*******************************************************************************
 * Name of Function:    timeElapsed
 * Purpose:             counts the time elapsed in milliseconds from starting
 *                      of program
 * Parameters:          none
 * Return Value:        long: time elapsed
 * Calls to:            None
 * Called from:         readerHandler, writerHandler
 * Method:              1. get time of the day
 *                      2. calculate the difference in millisecond from starting
 *                      time stored in global variable "start"
 ******************************************************************************/
long timeElapsed(void)
{
    struct timeval end;
    
    long mtime, seconds, useconds;
    
    gettimeofday(&end, NULL);
    
    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    
    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    
    return (mtime);
    
}

/*******************************************************************************
 * Name of Function:    myItoa
 * Purpose:             converts the integer id to string
 * Parameters:          val: value to be converted
 *                      base: baese of the number (Decimal = 10)
 * Return Value:        pointer to the converted string
 * Calls to:            None
 * Called from:         readerHandler, writerHandler
 * Method:              1. Converts the integer id and stores in buffer as string
 ******************************************************************************/
char* myItoa(int val, int base)
{
	
	static char buf[32] = {0};
	
	int i = 30;
	
	for(; val && i ; --i, val /= base)
        
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i+1];
	
}