/**
 * DESCRIPTION: Some helper functions for the CPU scheduling alogrithms
 				simulator for OS 172 Assignment 2

 * USAGE:		1. Include this file in the main scheduler code as follows:
 				#include "sched_helper.h"
 				2. Write appropriate Makefile instructions e.g.:
 				> gcc -o fcfs fcfs.c sched_helper.c
***/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "sched_helper.h"

/** Queue management functions **/

/* Creates a process node to hold a process 'proc' */
process_node * create_process_node(process *proc)
{
	process_node *proc_node = (process_node *) malloc(sizeof(process_node));

	proc_node->data = proc;
	proc_node->next = NULL;

	return proc_node;
}

/* Peforms basic initialization on the process queue 'proc_queue' */
void initalize_process_queue(process_queue *proc_queue)
{
	proc_queue->head = proc_queue->tail = NULL;
	proc_queue->length = 0;
}

/* Enqueues a process 'proc' at the back of the process queue pointed to
 * by 'proc_queue'.
 *
 * WARNING: process pointer 'proc' must not point to a temporary memory
 *  		location. It must be accessible as long as the queue is. */
void enqueue_process(process_queue *proc_queue, process *proc)
{
	process_node *proc_node = create_process_node(proc);

	if (proc_queue->head == NULL) 			// Queue is empty
	{
		proc_queue->head = NULL;
		proc_queue->tail = NULL;
	}
	else 									// Queue not empty
	{
		proc_queue->tail->next = proc_node;
		proc_queue->tail = proc_node;
	}

	proc_queue->length++;
}

/* Dequeues a process from the front of the process queue
 * pointed to by 'proc_queue' */
process * dequeue_process(process_queue *proc_queue)
{
	if (proc_queue->head == NULL)			// Queue is empty
	{
		return NULL;
	}

	process *proc = proc_queue->head->data; // Return value

	proc_queue->head = proc_queue->head->next;

	if (proc_queue->head == NULL)			// Update tail as well
	{
		proc_queue->tail = NULL;
	}

	return proc;
}