ReaderWriterSynchronization
===========================

rwsync:
---------
Reader-Writer Synchronization by FIFO waiting queue with pthreads.

Date: October 25, 2012

Problem Statement:
------------------
Implement Reader-Writer synchronization through POSIX threads. 

Complete problem statement: 
http://www-users.cselabs.umn.edu/classes/Fall-2012/csci5103/Assignments/Assignment4-POSIX.pdfFiles

Files:
-----
Filename | Description 
----------|------------
/rwsync.h| the header file
/rwsync.c| the main file
/utility.c| file containing utility functions definition
/init.c| file containing initializing functions definition
/reader.c| file containing function definition for reader functions
/writer.c| file containing function definition for writer functions 
/makefile| the make file
/Readme.txt| the README file

How to compile and run
-----------------------
1. execute "make" on terminal when in directory containing these files
2. ./rwsync [no. of readers] [no. of writers]

Tested Platforms:
------------------
1. GCC compiler running on MAC OS X Mountain Lion 
2. Unix Machine @ kh2170-10.cselabs.umn.edu (by remote login through ssh)

Special Notes:
-------------
1. The program is tested extensively to work for 120 reader and 120 writers. But it is suggested to keep the maximum count of number of readers and writers below 100 for guaranteed performance.

2. Though the program overwrite the content of existing reader file when executed again after first execution. But it is suggested to manually delete all files created by readers before executing second time to ensure consistent performance.

Contact Information
--------------------
Rohit Sinha

Email id: rsinha@cs.umn.edu | sinha049@umn.edu


