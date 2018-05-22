#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H

#include "structs.h"

/**
 * 	FUNCTIONS
 */

/**
 * 	Function to sort process based on their arrival time
 * @param p_list    Process list
 * @param list_size Size of process list
 */
void sort_on_arrival_time(struct process *p_list, int list_size)
{
	int i = 1;
	while (i < list_size)
	{
		struct process temp = p_list[i];
		int j = i - 1;
		while (j >= 0 && p_list[j].arrival_time > temp.arrival_time)
		{
			p_list[j + 1] = p_list[j];
			j--;
		}
		p_list[j + 1] = temp;
		i++;
	}
}

void calc_sched_criteria(struct process *p_list, int list_size)
{
	/* Find waiting time, turnaround time and response time */
	for (int i = 0; i < list_size; i++)
	{
		p_list[i].waiting_time = p_list[i].assigned_timeslot[0].start_time - p_list[i].arrival_time;
		for (int j = 0; j < p_list[i].timeslot_count - 1; j++)
		{
			// Waiting time
			p_list[i].waiting_time +=
				p_list[i].assigned_timeslot[j + 1].start_time - p_list[i].assigned_timeslot[j].end_time;
		}

		// Turnaround time
		int last_timeslot = p_list[i].timeslot_count - 1;
		p_list[i].turnaround_time = p_list[i].assigned_timeslot[last_timeslot].end_time - p_list[i].arrival_time;

		// Response time
		p_list[i].response_time = p_list[i].assigned_timeslot[0].start_time - p_list[i].arrival_time;
	}
}	

#endif