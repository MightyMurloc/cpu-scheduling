#include "include/structs.h"
#include "include/functions.h"

int time; 	/* Used to simulate system clock */

/* SHORTEST JOB REMAINING FIRST SCHEDULER */
int sched_srtf(struct process *p_list, int list_size)
{
	int completed = 0; /* Completed processes */
	int shortest = -1; /* Process with shortest remaining time */
	int minimum = INT_MAX;
	int checked = 0;

	/* Sort processes on arrival time */
	sort_on_arrival_time(p_list, list_size);

	/* Process until all prcesses completes */
	while (completed != list_size)
	{
		int temp = shortest; /* Keeps track whether shortest is changed */
		int current_timeslot;

		/* Find process with minimum remaining time
		 * among the processes that has arrived
		 * until current time */
		for (int i = 0; i < list_size; i++)
		{
			if (p_list[i].arrival_time <= time &&
				p_list[i].remaining_time < minimum &&
				p_list[i].remaining_time > 0)
			{
				minimum = p_list[i].remaining_time;
				shortest = i;
				checked = 1;
				// If process chenged, add timeslot
				if (shortest != temp)
				{
					p_list[i].timeslot_count++;

					int j = p_list[i].timeslot_count - 1;
					p_list[i].assigned_timeslot[j].start_time = time;
					p_list[i].assigned_timeslot[j].end_time = time;

					current_timeslot = j;
				}
			}
		}

		if (checked == 0)
		{
			time++;
			continue;
		}

		// Reduce remaining time by 1
		p_list[shortest].remaining_time--;

		// Update minimum
		minimum = p_list[shortest].remaining_time;
		if (minimum == 0)
			minimum = INT_MAX;

		// If a process is complete
		if (p_list[shortest].remaining_time == 0)
		{
			// Increment completed
			completed++;
		}

		p_list[shortest].assigned_timeslot[current_timeslot].end_time++;

		time++; // Advances time.
	}

	return 0;
}

void remove_dud(struct process *p_list, int list_size)
{
	for (int i = 0; i < list_size; i++)
	{
		for (int j = 0; j < p_list[i].timeslot_count; j++)
		{
			if (p_list[i].assigned_timeslot[j].start_time ==
				p_list[i].assigned_timeslot[j].end_time) // Remove dud
			{
				for (int k = j; k < p_list[i].timeslot_count; k++)
				{
					p_list[i].assigned_timeslot[k] = 
						p_list[i].assigned_timeslot[k + 1];
				}

				j--;
				p_list[i].timeslot_count--;
			}
		}
	}
}

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

	/* Scheduling criteria */
	int 	total_waiting_time = 0;
	int 	total_turnaround_time = 0;
	int 	total_response_time = 0;

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

	/* Initialize system clock */
	time = 0;

	/* Implement scheduler */
	sched_srtf(p_list, list_size);

	/* Remove dud */
	remove_dud(p_list, list_size);

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
	fprintf(ofp, "%-8s %-10s %-12s %-15s %-13s\n", "Process", "Burst time",
					"Waiting time", "Turnaround time", "Response time");

	calc_sched_criteria(p_list, list_size);

	/* Calculate scheduling criteria */
	float total_time = 0;
	for (int i = 0; i < list_size; i++)
	{
		total_waiting_time += p_list[i].waiting_time;
		total_turnaround_time += p_list[i].turnaround_time;
		total_response_time += p_list[i].response_time;

		fprintf(ofp, "%-8d %-10d %-12d %-15d %-13d\n", p_list[i].pid,
				p_list[i].burst_time, p_list[i].waiting_time, p_list[i].turnaround_time, p_list[i].response_time);

		int last_timeslot = p_list[i].timeslot_count - 1;
		if (total_time < p_list[i].assigned_timeslot[last_timeslot].end_time)
		{
			total_time = p_list[i].assigned_timeslot[last_timeslot].end_time;
		}
	}

	fprintf(ofp, "Average waiting time: %3.3f\n",
					(float) total_waiting_time / (float) list_size);
	fprintf(ofp, "Average turnaround time : %3.3f\n",
					(float) total_turnaround_time / (float) list_size);
	fprintf(ofp, "Average response time: %3.3f\n",
					(float) total_response_time / (float) list_size);
	fprintf(ofp, "Throughput: %1.3f\n",
					(float) list_size / (float) total_time);

	fclose(ofp);

	return 0;
}