#include "include/structs.h"
#include "include/functions.h"

#define MAX_PROCESS	50

int time; /* Used to simulate system clock */

/* FIRST COME FIRST SERVED SCHEDULER */
int sched_fcfs(struct process *p_list, int list_size)
{
	int list_counter = 0;

	/* Sort on arrival time */
	sort_on_arrival_time(p_list, list_size);

	/* Do loop simulation engine */
	while (list_size > 0)
	{
		/* Perform an event */
		if (time >= p_list[list_counter].arrival_time)
		{
			/* Execute that process */
			p_list[list_counter].assigned_timeslot[0].start_time = time;
			time += p_list[list_counter].burst_time;
			p_list[list_counter].assigned_timeslot[0].end_time = time;

			/* Update the process list */
			list_size--;
			list_counter++;
		}
		else
		{
			/* Process not yet arrived. Advance time */
			time++;
		}
	}

	return 0; // Executed successful
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
	while (fscanf(ifp, "%d %d", &p_list[list_counter].arrival_time,
					&p_list[list_counter].burst_time) != EOF)
	{
		/* Initialize the instance of struct process before using */
		p_list[list_counter].timeslot_count = 1;
		p_list[list_counter].assigned_timeslot = malloc(sizeof
													(struct timeslot));
		p_list[list_counter].pid = list_counter;

		/* Acknowledge a new process has been imported */
		list_size = ++list_counter;
	}

	fclose(ifp);

	/* Initialize system clock */
	time = 0;

	/* Implement scheduler */
	sched_fcfs(p_list, list_size);

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