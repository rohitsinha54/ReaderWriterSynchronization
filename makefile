rwsync:	rwsync.c init.c utility.c reader.c writer.c
	gcc -o	rwsync rwsync.c init.c utility.c reader.c writer.c -lpthread -I.