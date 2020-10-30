/*********************************************************/
/* Author	: Mohamed sayed								 */
/* Date 	: 15 ‎July 2020	    						 */		
/* Version  : V01										 */	
/*********************************************************/


#include "Std_types.h"
#include "BIT_MATH.h"

#include "stm32f446xx.h"

#include "OS_interface.h"
#include "OS_Private.h"
#include "OS_config.h"


static uint32_t PSP_Of_Tasks[MAX_TASKS] = {T1_STACK_START , T2_STACK_START , T3_STACK_START , T4_STACK_START ,IDLE_STACK_START};
static TCB_t user_tasks[MAX_TASKS];

static uint8_t static Current_Task = 1; 
static volatile uint32_t g_tick_count = 0 ;

void SysTick_Init(uint32_t Tick_Hz)
{
	uint32_t Count_Value = (SYSTICK_TIM_TICK / Tick_Hz) - 1;
	
	SysTick->LOAD = (SysTick->LOAD & 0xFF000000) | Count_Value ;
	
	SysTick->CTRL |= (7 << 0);
	
	
}

void Task_Stack_init(void)
{
    uint32_t *pPSP;
	
	for(uint32_t i = 0 ; i <MAX_TASKS ; i++)
	{
		user_tasks[i].task_state = TASK_READY_STATE;
		user_tasks[i].psp_value = PSP_Of_Tasks[i];
	}	
	
	user_tasks[0].task_handler = Idle_Task;
	user_tasks[1].task_handler = Task1_Hnadler;
	user_tasks[2].task_handler = Task2_Hnadler;
	user_tasks[3].task_handler = Task3_Hnadler;
	user_tasks[4].task_handler = Task4_Hnadler;
	
	for(uint32_t i = 0 ; i < MAX_TASKS ; i++)
	{
	  
		pPSP = (uint32_t *)user_tasks[i].psp_value;
		
		pPSP--;		
		*pPSP = 0x01000000;//XPSR
		
		pPSP--;
		*pPSP = (uint32_t)user_tasks[i].task_handler;//PC
	
		pPSP--;	
		*pPSP = 0xFFFFFFFD;
		
		for(uint32_t j = 0 ; j < 13 ; j++)
		{
		   pPSP--;	
			*pPSP = 0 ;
		
		}
		user_tasks[i].psp_value = (uint32_t)pPSP;
	
	}

}

uint32_t get_psp_value(void)
{
	return user_tasks[Current_Task].psp_value;
}

void save_psp_value(uint32_t PSP_CURRENT_VALUE)
{
	user_tasks[Current_Task].psp_value = PSP_CURRENT_VALUE;
}

void update_next_task(void)
{
	uint8_t Status = TASK_BLOCK_STATE ;
	for(volatile uint32_t i = 0 ; i < MAX_TASKS ; i++)
	{
		 Current_Task++;
	   Current_Task %= MAX_TASKS;	
		Status = (uint8_t)user_tasks[Current_Task].task_state;
		if((Status == TASK_READY_STATE) && (Current_Task !=0))
		{
		  break;
		}
	}
	if(Status != TASK_READY_STATE)
		Current_Task = 0;
	
}

__attribute__((naked))void Scheduler_Stack_init(void)
{
	__asm volatile("MSR MSP,%0": : "r"(SCHED_STACK_START) : );
	__asm volatile("BX LR");
	
}

__attribute__((naked))void MSP_To_PSP(void)
{
	__asm volatile("PUSH {LR}");
	__asm volatile("BL get_psp_value");
	__asm volatile("MSR PSP,R0");
	__asm volatile("POP {LR}");
	
	__asm volatile("MOV R0,#0x02");
	__asm volatile("MSR CONTROL,R0");
	
	__asm volatile("BX LR");

}

void Schedule(void)
{
	uint32_t *pICSR = (uint32_t *)0xE000ED04;

	*pICSR |= (1 << 28 );

}
	
void task_delay(uint32_t tick_count)
{
	if(Current_Task)
	{
	   user_tasks[Current_Task].block_count = g_tick_count + tick_count;
		user_tasks[Current_Task].task_state = TASK_BLOCK_STATE;
	   Schedule();
	}
	
}
void update_g_tick_count(void)
{
  g_tick_count++;
}

void unblock_task(void)
{
  for(volatile uint32_t i = 0 ; i < MAX_TASKS ; i++)
	{
	  if(user_tasks[i].task_state != TASK_READY_STATE)
		{
				if(user_tasks[i].block_count == g_tick_count)
				{
				  user_tasks[i].task_state = TASK_READY_STATE;
				}
		
		}
		
	}

}

void SysTick_Handler(void)
{
	
	uint32_t *pICSR = (uint32_t *)0xE000ED04;
	
	g_tick_count++;
	
    unblock_task();
	*pICSR |= (1 << 28 );

}

__attribute__((naked)) void PendSV_Handler(void)
{
    __asm volatile("MRS R0,PSP");
	__asm volatile("STMDB R0!,{R4-R11}");
	__asm volatile("PUSH {LR}");
	__asm volatile("BL save_psp_value");
	
    __asm volatile("BL update_next_task");
    __asm volatile("BL get_psp_value");
	__asm volatile("LDMIA R0!,{R4-R11}");
	__asm volatile("MSR PSP,R0");
	__asm volatile("POP {LR}");
	__asm volatile("BX LR");

}
