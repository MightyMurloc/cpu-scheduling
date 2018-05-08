/*
 * FILE:		sched_helper.h
 * AUTHOR:		Long Kim
 * E-MAIL:		kimhoanglong.cs@gmail.com
 *
 * Created on May 08th, 2017
 */

#ifndef	SCHED_HELPER_H
#define SCHED_HELPER_H

#ifdef 	__cplusplus
extern	"C" {
#endif

/**
 * DESCRIPTION: The header file for structures and helper functions for CPU
   				scheduling algorithms simulator for OS 172 Assignment 2.
 
 * USAGE:		1. Include this file in the main scheduler code as follows:
 				#include "sched_helper.h"
 				2. Write appropriate Makefile instructions e.g.:
 				> gcc -o fcfs fcfs.c sched_helper.c
***/

/* Macro Definitions */

#define	MAX_PROCESSES	50	/* Max number of processes per CPU load file */
#define	MAX_TIMESLOTS	20	/* Max number of timeslots per process */

/**
 * Struct definitions
 */

typedef struct timeslot 		timeslot;
typedef struct process 			process;
typedef struct process_node		process_node;
typedef struct process_queue	process_queue;

/* Represents a timeslot of a process */

struct timeslot
{
	int start_time;			/* Timestamp at the start of execution */

	int end_time;			/* TImestamp at the end of execution */
};

/* Represents a process with all the neccessary information */

struct process
{
	/* Values initiated for each process */
	int pid;				/* Process ID */

	int arrival_time;		/* Timestamp at which processes arrive
						   	   and wishes to start */
	int burst_time;			/* Amount of time processes requires
						   	   to complete the job */
	int remaining_time;		/* Remaining time of process at
						   	   current timestamp */

	/* Values used to track process */
	struct timeslot *assigned_timeslot;

	int timeslot_count;

	int flag;
};

/* A node in a queue of processes */

struct process_node
{
	process 		*data;	/* Pointer to process */

	process_node 	*next;	/* Next process in queue */
};

/* FIFO queue implemented as a singly-linked list */

struct process_queue
{
	int 			length; /* Length of queue */

	process_node 	*head;	/* Pointer to the front of the queue */

	process_node 	*tail;	/* Pointer to the rear of the queue */
};

/** Queue management functions **/

process_node * create_process_node(process *);

void initialize_process_queue(process_queue *);

void enqueue_process(process_queue *, process *);

process * dequeue_process(process_queue *);

#ifdef 	__cplusplus
}
#endif

#endif // SCHED_HELPER_H
