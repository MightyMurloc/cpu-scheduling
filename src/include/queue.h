#ifndef PROCESS_QUEUE_H
#define PROCESS_QUEUE_H

#include "structs.h"

/* A node container for process in the queue */
struct process_node
{
	struct process 		*data; /* Process */

	struct process_node *next; /* Next process in queue */
};

/* Process queue: ready queue and in_queue (e.g. the list of
 * processes that will be loaded in the future) */
struct process_queue
{
	/* Head and tail of queue */
	struct process_node *head;
	struct process_node *tail;
	int quantum; /*!< Time a process can spend on queue before being pushed down */
};

void init_queue(struct process_queue *p_queue, int q_time)
{
	p_queue->head = p_queue->tail = NULL;
	p_queue->quantum = q_time;
}

int queue_empty(struct process_queue *p_queue)
{
	return (p_queue->head == NULL);
}

/* Get process from the queue
 * Return NULL if queue empty */
struct process * de_queue(struct process_queue *p_queue)
{
	/* If queue is empty, return NULL */
	if (queue_empty(p_queue))
	{
		return NULL;
	}

	/* Return value */
	struct process *process = NULL;

	/* Get process */
	process = p_queue->head->data;

	/* Move ahead */
	p_queue->head = p_queue->head->next;

	/* If it becomes NULL, update tail too */
	if (p_queue->head == NULL)
	{
		p_queue->tail = NULL;
	}

	return process;
}

/* Put process into the queue */
void en_queue(struct process_queue *p_queue, struct process *process)
{
	/* Initialize a process node from given process */
	struct process_node *p_node = (struct process_node *) malloc(sizeof(
													struct process_node));
	p_node->data = process;
	p_node->next = NULL;

	/* If queue is empty, update head and tail */
	if (queue_empty(p_queue))
	{
		p_queue->head = p_node;
		p_queue->tail = p_node;
		return;
	}

	/* Otherwise, update tail */
	p_queue->tail->next = p_node;
	p_queue->tail = p_node;
}

#endif