/*********************************************************/
/* Author	: Mohamed sayed								 */
/* Date 	: 15 ‎July 2020	    						 */		
/* Version  : V01										 */	
/*********************************************************/


#ifndef OS_INTERFACE_H
#define OS_INTERFACE_H


void update_next_task(void);
/*
*
*
*/
void update_g_tick_count(void);
/*
*
*
*/
void Schedule(void);
/*
*
*
*/
void task_delay(uint32_t tick_count);
/*
*
*
*/
void SysTick_Init(uint32_t Tick_Hz);
/*
*
*
*/


#endif /*OS_INTERFACE_H*/
