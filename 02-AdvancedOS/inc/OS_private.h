/*********************************************************/
/* Author	: Mohamed sayed								 */
/* Date 	: 15 ‎July 2020	    						 */		
/* Version  : V01										 */	
/*********************************************************/


#ifndef OS_PRIVATE_H
#define OS_PRIVATE_H

#define SRAM_START           0x20000000U
#define SRAM_SIZE            ((1024)* (128))
#define SRAM_END             ((SRAM_START) + (SRAM_SIZE))



#define T1_STACK_START       (SRAM_END)
#define T2_STACK_START       ((SRAM_END) - (1*TASK_STACK_SIZE))
#define T3_STACK_START       ((SRAM_END) - (2*TASK_STACK_SIZE))
#define T4_STACK_START       ((SRAM_END) - (3*TASK_STACK_SIZE))
#define IDLE_STACK_START     ((SRAM_END) - (4*TASK_STACK_SIZE))

#define SCHED_STACK_START    ((SRAM_END) - (5*TASK_STACK_SIZE))

#define TASK_READY_STATE 	 0x00
#define TASK_BLOCK_STATE   0xFF


typedef struct 
{
	uint32_t psp_value;
	uint32_t task_state;
	uint32_t block_count;
	void(*task_handler)(void);
	
}TCB_t;

void Task_Stack_init(void);
uint32_t get_psp_value(void);
void save_psp_value(uint32_t PSP_CURRENT_VALUE);
void unblock_task(void);
__attribute__((naked)) void Scheduler_Stack_init(void);
__attribute__((naked))void MSP_To_PSP(void);



#endif /*OS_PRIVATE_H*/
