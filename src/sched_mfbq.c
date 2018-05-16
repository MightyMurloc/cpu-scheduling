#include <stdlib.h>
#include "include/structs.h"
#include "include/queue.h"
#include "include/functions.h"

#define MAX_PRIORITY	5

int time; /*!< Used to simulate system clock */

/*!
 *@function 	sched_mfbq
 *@abstract 	Performs multi-level feedback queue scheduling
 *@param 		**p_queue Pointer to array of process queues
 *@result 		0 if successful
 */
int sched_mfbq(struct process_queue *r_queue, struct process_queue **p_queue)
{
	int peek = -1; 	/*!< Indicates the first non-empty queue */
	int prev; 		/*!< Stores last iteration's first non-emprty queue */
	int current; 	/*!< The last timeslot of a process */
	int elapsed_time = 0;
	struct process *proc = NULL;

	while (1)
	{
		prev = peek; // This will change if the prev. process is preempted

		/* If all queues are empty, we're done. */
		if (peek == -1 && queue_empty(r_queue))
		{
			break;
		}

		/* For the last iteration */
		if (peek != -1 && queue_empty(r_queue))
		{
			continue;
		}

		if (time >= r_queue->head->data->arrival_time)
		{
			/* A process has arrived */
			proc = de_queue(r_queue);
			en_queue(p_queue[0], proc);
			printf("Process %d enqueued\n", proc->pid);
		}

		for (int i = 0; i < MAX_PRIORITY; i++)
		{
			if (!queue_empty(p_queue[i]))
			{
				peek = i;
				break;
			}
			peek = -1;
		}

		if (peek != prev) // A process gets prempted, or the first one arrives
		{
			printf("Interrupt: Process %d preempted\n", proc->pid);
			proc->timeslot_count++;
			int i = proc->timeslot_count - 1;
			proc->assigned_timeslot[i].start_time = time;
			proc->assigned_timeslot[i].end_time = time;
			current = i;
		}

		proc->remaining_time--;

		proc->timeslot_count++;
		int current = proc->timeslot_count - 1;

		printf("Process %d in queue %d used %d\n", proc->pid, peek, elapsed_time);
		if (proc->remaining_time == 0)
		{
			proc->waiting_time = time + 1 - proc->burst_time - proc->arrival_time;
			if (proc->waiting_time < 0)
			{
				proc->waiting_time = 0;
			}

			proc->turnaround_time = proc->burst_time + proc->waiting_time;

			proc = NULL;
			continue;
		}
		if (elapsed_time == p_queue[peek]->quantum)
		{
			printf("Interrupt: process %d used up quanta\n", proc->pid);
			/* A process has used up the queue's quantum time */
			en_queue(p_queue[peek + 1], proc);
		}

		proc->assigned_timeslot[current].end_time++;
		printf("Endtime is now %d\n", proc->assigned_timeslot[current].end_time);
		elapsed_time = proc->assigned_timeslot[current].end_time -
				proc->assigned_timeslot[current].start_time;
		//printf("Done an iteration\n");

		time++; // Advances time
	}

	return 0;
}

/* MAIN PROGRAM */
int main(int argc, char *argv[])
{
	/* Parameters to parse console arguments */
	char 	*ivalue = NULL;
	char 	*ovalue = NULL;
	char 	*qvalue = NULL;
	int 	param;
	int 	list_counter;

	/* File streams to read input file and write output file */
	FILE 	*ifp, *ofp;

	/* Process list */
	struct 	process p_list[MAX_PROCESS];
	int 	list_size = 0;

	/* Process queue */
	struct process_queue ready_queue;
	struct process_queue *p_queue;

	/* Scheduling criteria */
	int 	total_waiting_time = 0;
	int 	total_turnaround_time = 0;

	/* Argument parsing */
	while ((param = getopt(argc, argv, "i:o:q")) != -1)
	{
		switch(param)
		{
			case 'i':
				ivalue = optarg;
				break;
			case 'o':
				ovalue = optarg;
				break;
			case 'q':
				qvalue = optarg;
				break;
		}
	}

	/* Validate input arguments */

	// If no input file is specified, the default file "input.txt" is used
	if (ivalue == NULL)
	{
		perror("WARNING: Using default input file.\n");
		ivalue = "input.txt";
	}

	// If no output file is specified, the default file "input.txt" is used
	if (ovalue == NULL)
	{
		perror("WARNING: Using default output file.\n");
		ovalue = "output/output.txt";
	}

	/* Get data input */

	ifp = fopen(ivalue, "r");
	if (ifp == NULL) // Cannot open file
	{
		perror("ERROR: cannot open input file.\n");
		return -1;
	}

	list_counter = 0;
	while (fscanf(ifp, "%d %d", &p_list[list_counter].arrival_time, 
				  &p_list[list_counter].burst_time) != EOF)
	{
		/* Initialize the instance of struct process before using */
		p_list[list_counter].assigned_timeslot = 
										malloc(sizeof(struct timeslot) * 10);
		p_list[list_counter].timeslot_count = 0;
		p_list[list_counter].pid = list_counter;
		p_list[list_counter].remaining_time = p_list[list_counter].burst_time;

		/* Acknowledge a new process has been imported */
		list_size = ++list_counter;
	}

	fclose(ifp);

	sort_on_arrival_time(p_list, list_size);

	init_queue(&ready_queue, 0);
	for (int i = 0; i < list_size; i++)
	{
		en_queue(&ready_queue, &p_list[i]);
	}

	/* Initialize system clock */
	time = 0;

	/* Initialize process queues */
	p_queue = (struct process_queue *) malloc(sizeof(struct process_queue) * MAX_PRIORITY);
	for (int i = 0; i < MAX_PRIORITY - 1; i++)
	{
		init_queue(&p_queue[i], 2*i + 2);
	}
	init_queue(&p_queue[4], INT_MAX);

	/* Implement scheduler */
	sched_mfbq(&ready_queue, &p_queue);
	printf("Done.\n");

	/* Write data to output file */

	list_counter = 0;
	ofp = fopen(ovalue, "w");

	if (ofp == NULL) // Cannot open file
	{
		perror("ERROR: cannot open output file.\n");
		return -1;
	}

	/* Produce output data */
	while (list_counter < list_size)
	{
		int i;
		for (i = 0; i < p_list[list_counter].timeslot_count; i++)
		{
			fprintf(ofp, "Process %2d start %5d end %5d\n", 
					p_list[list_counter].pid,
					p_list[list_counter].assigned_timeslot[i].start_time,
					p_list[list_counter].assigned_timeslot[i].end_time);
		}
		
		list_counter++;
	}

	fprintf(ofp, "\nScheduling criteria: \n\n");
	fprintf(ofp, "Process\tBurst time\tWaiting time\tTurnaround time\n");

	/* Calculate scheduling criteria */
	for (int i = 0; i < list_size; i++)
	{
		total_waiting_time += p_list[i].waiting_time;
		total_turnaround_time += p_list[i].turnaround_time;

		fprintf(ofp, "%d\t%d\t\t%d\t\t%d\n", p_list[i].pid,
				p_list[i].burst_time, p_list[i].waiting_time, p_list[i].turnaround_time);
	}

	fprintf(ofp, "Average waiting time: %3.3f\n",
					(float) total_waiting_time / (float) list_size);
	fprintf(ofp, "Average turnaround time : %3.3f\n",
					(float) total_turnaround_time / (float) list_size);

	fclose(ofp);

	return 0;
}