#include "include/structs.h"
#include "include/functions.h"

int time; /* Used to simulate system clock */
int sum_burst_time;

int sched_hrrn(struct process *p_list, int list_size)
{
	sort_on_arrival_time(p_list, list_size);

	for (int t = p_list[0].arrival_time; t < sum_burst_time;)
	{
		float highest_rr = -1;

		float response_ratio;

		int next_proc;

		for (int i = 0; i < list_size; i++)
		{
			if (p_list[i].arrival_time <= t && p_list[i].remaining_time > 0)
			{
				response_ratio = (p_list[i].burst_time + t -
							p_list[i].arrival_time) / p_list[i].burst_time;
				if (highest_rr < response_ratio)
				{
					highest_rr = response_ratio;
					next_proc = i;
				}
			}
		}
		p_list[next_proc].assigned_timeslot[0].start_time = t;
		t += p_list[next_proc].burst_time;
		p_list[next_proc].assigned_timeslot[0].end_time = t;
		p_list[next_proc].remaining_time = 0;
		//printf("Time: %d\n", t);
	}

	return 0;
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
	sum_burst_time = 0;
	while (fscanf(ifp, "%d %d", &p_list[list_counter].arrival_time,
					&p_list[list_counter].burst_time) != EOF)
	{
		/* Initialize the instance of struct process before using */
		p_list[list_counter].timeslot_count = 1;
		p_list[list_counter].assigned_timeslot = malloc(sizeof
													(struct timeslot));
		p_list[list_counter].pid = list_counter;
		p_list[list_counter].remaining_time = p_list[list_counter].burst_time;

		sum_burst_time += p_list[list_counter].burst_time;
		printf("sum_burst_time: %d\n", sum_burst_time);

		/* Acknowledge a new process has been imported */
		list_size = ++list_counter;
	}
	printf("Total burst time: %d\n", sum_burst_time);

	fclose(ifp);

	/* Initialize system clock */
	time = 0;

	/* Implement scheduler */
	sched_hrrn(p_list, list_size);

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
	sched_criteria(p_list, list_size, w_time, ta_time);

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