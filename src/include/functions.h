#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H

#include "structs.h"

/*
 ***************************** 
 *        Functions          *
 *****************************
 */

/* Function to sort the process list based on their arrival time */
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

/* Function to find waiting time and turnaround time
 * For FCFS and SJF ONLY */
void sched_criteria(struct process *p_list, int list_size,
					int *w_time, int *ta_time)
{
	// Waiting time for first process is its arrival time
	w_time[0] = p_list[0].arrival_time;

	// Calculate waiting time
	for (int i = 1; i < list_size; i++)
	{
		w_time[i] = p_list[i - 1].burst_time + w_time[i - 1] -
											p_list[i].arrival_time;
	}

	// Calculate turnaround time
	for (int i = 0; i < list_size; i++)
	{
		ta_time[i] = p_list[i].burst_time + w_time[i];
	}
}

#endif