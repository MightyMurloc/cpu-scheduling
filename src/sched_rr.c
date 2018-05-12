#include "include/structs.h"
#include "include/functions.h"

int time; /* Used to simulate system clock */

/* ROUND ROBIN SCHEDULER */
int sched_rr(struct process *p_list, int list_size,
				int quantum_time, int *w_time)
{
	/* Sort on arrival time */
	sort_on_arrival_time(p_list, list_size);

	time += p_list[0].arrival_time;

	/* Keep running untill all processes are complete */
	while (1)
	{
		int done = 1;

		// Traverse processes one by one repeatedly
		for (int i = 0; i < list_size; ++i)
		{
			/* If process is not done executing
			 * we process further */
			if (p_list[i].remaining_time > 0)
			{
				done = 0; // There is a pending process

				p_list[i].timeslot_count++; // Add a timeslot

				int current = p_list[i].timeslot_count - 1;

				if (p_list[i].remaining_time > quantum_time)
				{
					/* If remaining burst time is greater than quantum
					 * time, increase time by quantum_time */
					p_list[i].assigned_timeslot[current].start_time = time;
					time += quantum_time;
					p_list[i].assigned_timeslot[current].end_time = time;

					/* Update remaining time */
					p_list[i].remaining_time -= quantum_time;
				}
				else /* Last cycle of a process */
				{
					p_list[i].assigned_timeslot[current].start_time = time;
					time += p_list[i].remaining_time;
					p_list[i].remaining_time = 0;
					p_list[i].assigned_timeslot[current].end_time = time;

					// Calulate waiting time
					w_time[i] = time - p_list[i].burst_time -
											p_list[i].arrival_time;
				}
			}
		}

		/* If all processes are done, break */
		if (done == 1)
		{
			break;
		}
	}

	return 0;
}

/* Function to find turnaround time */
void find_turnaround_time(struct process *p_list, int list_size,
							int *w_time, int *ta_time)
{
	/* Calculate turnaround time by adding
	 * burst time and waiting time */
	for (int i = 0; i < list_size; i++)
	{
		ta_time[i] = p_list[i].burst_time + w_time[i];
	}
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

	/* Quantum time */
	int 	quantum_time = 1;
	char 	*endptr;

	/* Scheduling criteria */
	int 	w_time[MAX_PROCESS];
	int 	ta_time[MAX_PROCESS];
	int 	total_waiting_time = 0;
	int 	total_turnaround_time = 0;

	/* Argument parsing */
	while ((param = getopt(argc, argv, "i:o:q:")) != -1)
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

	// Parse quantum time
	if (qvalue != NULL)
	{
		quantum_time = (int) strtol(qvalue, &endptr, 10); // Base decimal
		if (quantum_time < 1 || *endptr) // Check if quantum time is valid
		{
			perror("ERROR: invalid quantum time.\n");
			return -1;
		}
		fprintf(stderr, "INFO: use passing argument quantum_time %d\n",
				quantum_time);
	}
	else
	{
		fprintf(stderr, "INFO: use default quantum_time 1\n");
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

	/* Initialize system clock */
	time = 0;

	/* Implement scheduler */
	sched_rr(p_list, list_size, quantum_time, w_time);

	/* Find turnaround time */
	find_turnaround_time(p_list, list_size, w_time, ta_time);

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
		total_waiting_time += w_time[i];
		total_turnaround_time += ta_time[i];

		fprintf(ofp, "%d\t%d\t\t%d\t\t%d\n", p_list[i].pid,
				p_list[i].burst_time, w_time[i], ta_time[i]);
	}

	fprintf(ofp, "Average waiting time: %3.3f\n",
					(float) total_waiting_time / (float) list_size);
	fprintf(ofp, "Average turnaround time : %3.3f\n",
					(float) total_turnaround_time / (float) list_size);

	fclose(ofp);
	
	return 0;
}