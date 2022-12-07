/*
 * scheduler.c
 *
 *  Created on: Nov 24, 2022
 *      Author: Acer
 */

#include "scheduler.h"
sTasks SCH_tasks_G[SCH_MAX_TASKS];

void SCH_Init(void) {
	unsigned char i ;
	for ( i = 0; i < SCH_MAX_TASKS; i++) {
		SCH_Delete_Task(i);
	}
}

void SCH_Add_Task ( void (*pFunction)() , uint32_t DELAY, uint32_t PERIOD){
	unsigned char Index = 0;
	// First find a gap in the array ( if there is one)
	while (( SCH_tasks_G[Index].pTask != 0) && ( Index < SCH_MAX_TASKS) ) {
		Index++;
	}
	// Have we reached the end of the list ?
	if ( Index == SCH_MAX_TASKS) {
		// Task list is full
		return;
	}
	//If we're here , there is a space in the task array
	SCH_tasks_G[Index].pTask = pFunction ;
	SCH_tasks_G[Index].Delay = DELAY;
	SCH_tasks_G[Index].Period = PERIOD;
	SCH_tasks_G[Index].RunMe = 0;
	//Return position of task (to allow later deletion)
}

void SCH_Update(void){
	unsigned char Index ;
	// NOTE: calculations are in *TICKS* ( not milliseconds )
	for ( Index = 0; Index < SCH_MAX_TASKS; Index++) {
		// Check if there is a task at this location
		if ( SCH_tasks_G[Index].pTask) {
			if ( SCH_tasks_G[Index].Delay == 0) {
				// The task i s due to run
				// Inc. the 'RunMe' flag
				SCH_tasks_G[Index].RunMe += 1;
				if ( SCH_tasks_G[Index].Period ) {
					// Schedule periodic tasks to run again
					SCH_tasks_G[Index].Delay = SCH_tasks_G[Index].Period ;
				}
			}
			else {
				// Not yet ready to run : just decrement the delay
				SCH_tasks_G[Index].Delay--;
			}
		}
	}
}

void SCH_Dispatch_Tasks(void){
	for(int i = 0; i < SCH_MAX_TASKS; i++){
		if(SCH_tasks_G[i].RunMe > 0){
			SCH_tasks_G[i].RunMe--;
			(*SCH_tasks_G[i].pTask)();
			// if this is a ’one shot ’ task , remove it from the array
			if ( SCH_tasks_G[i].Period == 0) {
				SCH_Delete_Task(i);
			}
		}
	}
}

void SCH_Delete_Task(uint32_t taskID) {
	SCH_tasks_G[taskID].pTask = 0x0000;
	SCH_tasks_G[taskID].Delay = 0;
	SCH_tasks_G[taskID].Period = 0;
	SCH_tasks_G[taskID].RunMe = 0;
}
