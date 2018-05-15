#include <stdlib.h>
#include "include/structs.h"
#include "include/queue.h"
#include "include/functions.h"

int time; /* Used to simulate system clock */

/* FIRST-COME-FIRST-SERVE SCHEDULER */
int sched_fcfs(struct process_queue *p_queue)
{
	/* Keep running until queue is empty */
	while (!queue_empty(p_queue))
	{
		struct process *proc = de_queue(p_queue); // Get the first process

		proc->timeslot_count++;

		proc->assigned_timeslot[0].start_time = time;
		time += proc->remaining_time;
		proc->assigned_timeslot[0].end_time = time;
		proc->remaining_time = 0;
		proc->waiting_time = time - proc->burst_time - proc->arrival_time;
		proc->turnaround_time = proc->burst_time + proc->waiting_time;
		/* Free a process once it's done */
		proc = NULL;
	}

	return 0;
}

/* MAIN PROGRAM */
int main(int argc, char *argv[])
{
	/* Parameters to parse console arguments */
	char 	*ivalue = NULL;
	char 	*ovalue = NULL;
	int 	param;
	int 	list_counter;

	/* File streams to read input file and write output file */
	FILE 	*ifp, *ofp;

	/* Process list */
	struct 	process p_list[MAX_PROCESS];
	int 	list_size = 0;

	/* Quantum time */
	int 	quantum_time = 1;
	char 	*endptr;

	/* Scheduling criteria */
	int 	total_waiting_time = 0;
	int 	total_turnaround_time = 0;

	struct process_queue ready_queue;
	ready_queue.head = ready_queue.tail = NULL;

	/* Argument parsing */
	while ((param = getopt(argc, argv, "i:o:")) != -1)
	{
		switch(param)
		{
			case 'i':
				ivalue = optarg;
				break;
			case 'o':
				ovalue = optarg;
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
										malloc(sizeof(struct timeslot));
		p_list[list_counter].timeslot_count = 0;
		p_list[list_counter].pid = list_counter;
		p_list[list_counter].remaining_time = p_list[list_counter].burst_time;

		/* Acknowledge a new process has been imported */
		list_size = ++list_counter;
	}

	fclose(ifp);

	sort_on_arrival_time(p_list, list_size);

	for (int i = 0; i < list_size; i++)
	{
		en_queue(&ready_queue, &p_list[i]);
	}

	/* Initialize system clock */
	time = 0;

	/* Implement scheduler */
	sched_fcfs(&ready_queue);

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