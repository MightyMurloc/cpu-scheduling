#include <stdlib.h>
#include "include/structs.h"
#include "include/queue.h"
#include "include/functions.h"

/**
 * 	Number of priority queues
 */
#define MAX_PRIORITY	5

int time; /*!< Used to simulate system clock */

/**
 * 	Process interrupt
 */
typedef enum interrupt
{
	DEFAULT = 0,		/**< Initial state */
	NEW_PROCESS = 1,	/**< A new process has ben enqueued */
	PREEMPTED = 2,		/**< A process arrives in the queue above our current depth */
	Q_EXPIRED = 3 		/**< A process uses up the queue's quanta, or done execution */
} event_t;

struct process_queue p_queue[MAX_PRIORITY];


/**
 * @brief      Perform MFBQ scheduling
 *
 * @param      r_queue  Pointer to ready queue
 *
 * @return     0 if successful
 */
int sched_mfbq(struct process_queue *r_queue)
{
	int peek = -1; 					/**< Indicates the first non-empty queue */
	int prev; 						/**< Stores last iteration's first non-emprty queue */
	int current; 					/**< The last timeslot of a process */
	int elapsed_time = 0;			/**< Time a process spent on a queue */
	event_t event = DEFAULT;		/*!< Stores the interrupt of last process */
	struct process *proc = NULL; 	/*!< Pointer to the current process */
	struct process *pget = NULL; 	/*!< Pointer to the process gotten from ready queue */

	while (1)
	{
		prev = peek; // This will change if the prev. process is preempted

		/* For the last iteration */
		if (peek != -1 && queue_empty(r_queue))
		{
			/* Do nothing */
		}
		else if (time >= r_queue->head->data->arrival_time)
		{
			/* A process has arrived */
			pget = de_queue(r_queue);
			en_queue(&p_queue[0], pget);
			printf("Process %d enqueued\n", pget->pid);
			pget->flag = 0;
			event = NEW_PROCESS;
		}

		for (int i = 0; i < MAX_PRIORITY; i++)
		{
			if (!queue_empty(&p_queue[i]))
			{
				peek = i;
				break;
			}
			peek = -1;
		}

		/* If all queues are empty, we're done. */
		if (peek == -1 && queue_empty(r_queue))
		{
			break;
		}

		if (peek != prev && event != PREEMPTED) // A process gets prempted, or the first one arrives
		{
			proc = de_queue(&p_queue[peek]);
			/**
			 * 	Debug: uncomment this line
			 * 	printf("Got process %d in Q%d\n", proc->pid, peek);
			 */
			event = PREEMPTED;
			proc->timeslot_count++;
			int i = proc->timeslot_count - 1;
			proc->assigned_timeslot[i].start_time += time;
			proc->assigned_timeslot[i].end_time += time;
			current = i;
		}
		else if (peek == prev && event == Q_EXPIRED)
		{
			proc = de_queue(&p_queue[peek]);
			/**
			 * 	Debug: uncomment this line
			 * 	printf("Got process %d in Q%d\n", proc->pid, peek);
			 */
			event = DEFAULT;
			proc->timeslot_count++;
			int i = proc->timeslot_count - 1;
			proc->assigned_timeslot[i].start_time += time;
			proc->assigned_timeslot[i].end_time += time;
			current = i;
		}

		proc->remaining_time--;

		if (proc->remaining_time == 0)
		{
			proc->waiting_time = time + 1 - proc->burst_time - proc->arrival_time;
			if (proc->waiting_time < 0)
			{
				proc->waiting_time = 0;
			}

			proc->turnaround_time = proc->burst_time + proc->waiting_time;
			/**
			 * 	Debug: uncomment this line
			 * 	printf("Process %d completed\n", proc->pid);
			 */
			event = Q_EXPIRED;
			time++;
			proc->assigned_timeslot[current].end_time = time;
			proc = NULL;
			peek++;
			continue;
		}

		proc->assigned_timeslot[current].end_time++;
		elapsed_time = proc->assigned_timeslot[current].end_time -
				proc->assigned_timeslot[current].start_time;
		/**
		 * 	Debug: uncomment this line
		 * 	printf("Process %d has run for %d\n", proc->pid, elapsed_time);
		 */

		if (elapsed_time == p_queue[proc->flag].quantum)
		{
			/** 
			 * Debug: uncomment this line
			 * printf("Interrupt: process %d used up quanta\n", proc->pid);
			 */
			event = Q_EXPIRED;
			/* A process has used up the queue's quantum time */
			en_queue(&p_queue[proc->flag + 1], proc);
			peek = proc->flag + 1;
			proc->flag++;
		}
		time++; // Advances time
	}

	return 0;
}


/**
 * @brief      Main function
 *
 * @param[in]  argc  Number of arguments
 * @param      argv  Array of arguments
 *
 * @return     0 if successful
 */
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
	for (int i = 0; i < MAX_PRIORITY - 1; i++)
	{
		init_queue(&p_queue[i], 2*i + 2);
	}
	init_queue(&p_queue[4], INT_MAX);

	/* Implement scheduler */
	sched_mfbq(&ready_queue);
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
	fprintf(ofp, "%-8s %-10s %-12s %-15s\n", "Process", "Burst time",
					"Waiting time", "Turnaround time");

	/* Calculate scheduling criteria */
	for (int i = 0; i < list_size; i++)
	{
		total_waiting_time += p_list[i].waiting_time;
		total_turnaround_time += p_list[i].turnaround_time;

		fprintf(ofp, "%-8d %-10d %-12d %-15d\n", p_list[i].pid,
				p_list[i].burst_time, p_list[i].waiting_time, p_list[i].turnaround_time);
	}

	fprintf(ofp, "Average waiting time: %3.3f\n",
					(float) total_waiting_time / (float) list_size);
	fprintf(ofp, "Average turnaround time : %3.3f\n",
					(float) total_turnaround_time / (float) list_size);

	fclose(ofp);

	return 0;
}