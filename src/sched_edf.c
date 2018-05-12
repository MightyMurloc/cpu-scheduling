#include "include/structs.h"
#include "include/functions.h"

int time;	/* Used to simulate system clock */

int sched_edf(struct process *p_list, int list_size)
{
	int completed = 0;
	int shortest = -1;
	int minimum = INT_MAX;
	int checked = 0;

	sort_on_arrival_time(p_list, list_size);

	while (completed < list_size)
	{
		int temp = shortest;
		int current_timeslot;

		for (int i = 0; i < list_size; i++)
		{
			if (p_list[i].arrival_time <= time &&
				p_list[i].deadline < minimum &&
				p_list[i].remaining_time > 0)
			{
				minimum = p_list[i].deadline;
				shortest = i;
				checked = 1;
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

		p_list[shortest].remaining_time--;
		if (p_list[shortest].remaining_time == 0)
		{
			minimum = INT_MAX;
			completed++;
			int finish_time = time + 1;
			p_list[shortest].waiting_time = finish_time - p_list[shortest].burst_time -
												p_list[shortest].arrival_time;
			if (p_list[shortest].waiting_time < 0)
			{
				p_list[shortest].waiting_time = 0;
			}
		}

		p_list[shortest].assigned_timeslot[current_timeslot].end_time++;

		time++;
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

void find_turnaround_time(struct process *p_list, int list_size)
{
	/* Calculate turnaround time by adding
	 * burst time and waiting time */
	for (int i = 0; i < list_size; i++)
	{
		p_list[i].turnaround_time = p_list[i].burst_time + p_list[i].waiting_time;
	}
}

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
	int 	w_time[MAX_PROCESS];
	int 	ta_time[MAX_PROCESS];
	int 	total_waiting_time = 0;
	int 	total_turnaround_time = 0;

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
	while (fscanf(ifp, "%d %d %d", &p_list[list_counter].arrival_time, 
			&p_list[list_counter].burst_time, &p_list[list_counter].deadline) != EOF)
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
	sched_edf(p_list, list_size);

	/* Remove dud */
	remove_dud(p_list, list_size);

	/* Find turnaround time */
	find_turnaround_time(p_list, list_size);

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