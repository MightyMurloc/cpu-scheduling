#ifndef	_STRUCTS_H
#define _STRUCTS_H

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <limits.h>

#define MAX_PROCESS	50

/**
 * 	USER_DEFINED STRUCTURES
 */

/** Represents a timeslot */
struct timeslot
{
	int start_time;		/*!< Timestamp at the start of execution */

	int end_time;		/*!< Timestanp at the end of excution */
};

/** A structure to store a process's information. Think of it as a PCB. */
struct process
{
	/* Values initiated for each process */
	int pid;								/*!< Process ID */

	int arrival_time;						/*!< Timestamp at which processes arrive
						   						and wishes to start */
	int burst_time;							/*!< Amount of time processes requires
						  						 to complete the job */
	int remaining_time;						/*!< Remaining time of process at
						   						current timestamp */
	int deadline;							/**< Process deadline */

	/* Values used to track process */
	struct timeslot *assigned_timeslot;		/**< An array to store process's timeslots */

	int timeslot_count;						/**< Number of timeslots */

	int waiting_time;						/**< Time spent waiting */

	int turnaround_time;					/**< Process turnaround time */

	int response_time;						/**< Process response time */

	int flag;								/*!< The current queue the process is in */

	int elapsed_time;
};

#endif
