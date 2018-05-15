#ifndef	_STRUCTS_H
#define _STRUCTS_H

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <limits.h>

#define MAX_PROCESS	50

/*
 ***************************** 
 *  User-defined structures  *
 *****************************
 */

struct timeslot
{
	int start_time;		/* TImestamp at the start of execution */

	int end_time;		/* Timestanp at the end of excution */
};

struct process
{
	/* Values initiated for each process */
	int pid;			/* Process ID */

	int arrival_time;	/* Timestamp at which processes arrive
						   and wishes to start */
	int burst_time;		/* Amount of time processes requires
						   to complete the job */
	int remaining_time;	/* Remaining time of process at
						   current timestamp */

	/* Values used to track process */
	struct timeslot *assigned_timeslot;

	int timeslot_count;

	int waiting_time;

	int turnaround_time;
};

#endif
