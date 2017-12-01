/*
 * CMTCS.cpp
 *
 * Cooperative MultiTask library based on Context Switch for AVR-8
 *
 * Created: 2017/9/18 14:28:02
 *  Author: dust
 */ 


#include "CMTCS.h"



static size_t totalTask = 1;
static uint8_t* stack_pointer[CMTCS::MAX_TASK_COUNT + 1] = {nullptr};
uint8_t ** volatile pActiveSP = nullptr;



void yield(void)
{
	//if pActiveSP == NULL, it means startMultiTask() have not been called.
	if (!pActiveSP) return;
	
	
	//save context
	__asm__ __volatile__
	(
		"push   r0                                      \n\t"   \
		"in     r0, __SREG__                            \n\t"   \
		"cli                                            \n\t"   \
		"push   r0                                      \n\t"   \
		"in     r0, 0x3b                                \n\t"   \
		"push   r0                                      \n\t"   \
		"in     r0, 0x3c                                \n\t"   \
		"push   r0                                      \n\t"   \
		"push   r1                                      \n\t"   \
		"clr    r1                                      \n\t"   \
		"push   r2                                      \n\t"   \
		"push   r3                                      \n\t"   \
		"push   r4                                      \n\t"   \
		"push   r5                                      \n\t"   \
		"push   r6                                      \n\t"   \
		"push   r7                                      \n\t"   \
		"push   r8                                      \n\t"   \
		"push   r9                                      \n\t"   \
		"push   r10                                     \n\t"   \
		"push   r11                                     \n\t"   \
		"push   r12                                     \n\t"   \
		"push   r13                                     \n\t"   \
		"push   r14                                     \n\t"   \
		"push   r15                                     \n\t"   \
		"push   r16                                     \n\t"   \
		"push   r17                                     \n\t"   \
		"push   r18                                     \n\t"   \
		"push   r19                                     \n\t"   \
		"push   r20                                     \n\t"   \
		"push   r21                                     \n\t"   \
		"push   r22                                     \n\t"   \
		"push   r23                                     \n\t"   \
		"push   r24                                     \n\t"   \
		"push   r25                                     \n\t"   \
		"push   r26                                     \n\t"   \
		"push   r27                                     \n\t"   \
		"push   r28                                     \n\t"   \
		"push   r29                                     \n\t"   \
		"push   r30                                     \n\t"   \
		"push   r31                                     \n\t"   \
		"lds    r26, pActiveSP                          \n\t"   \
		"lds    r27, pActiveSP + 1                      \n\t"   \
		"in     r0, __SP_L__                            \n\t"   \
		"st     x+, r0                                  \n\t"   \
		"in     r0, __SP_H__                            \n\t"   \
		"st     x+, r0                                  \n\t"   \
	);
	
	
	//switch context
	pActiveSP++;
	if (pActiveSP - stack_pointer >= totalTask)
	{
		pActiveSP = stack_pointer;
	}
	
	
	//restore context
	__asm__ __volatile__
	(
		"lds    r26, pActiveSP                          \n\t"   \
		"lds    r27, pActiveSP + 1                      \n\t"   \
		"ld     r28, x+                                 \n\t"   \
		"out    __SP_L__, r28                           \n\t"   \
		"ld     r29, x+                                 \n\t"   \
		"out    __SP_H__, r29                           \n\t"   \
		"pop    r31                                     \n\t"   \
		"pop    r30                                     \n\t"   \
		"pop    r29                                     \n\t"   \
		"pop    r28                                     \n\t"   \
		"pop    r27                                     \n\t"   \
		"pop    r26                                     \n\t"   \
		"pop    r25                                     \n\t"   \
		"pop    r24                                     \n\t"   \
		"pop    r23                                     \n\t"   \
		"pop    r22                                     \n\t"   \
		"pop    r21                                     \n\t"   \
		"pop    r20                                     \n\t"   \
		"pop    r19                                     \n\t"   \
		"pop    r18                                     \n\t"   \
		"pop    r17                                     \n\t"   \
		"pop    r16                                     \n\t"   \
		"pop    r15                                     \n\t"   \
		"pop    r14                                     \n\t"   \
		"pop    r13                                     \n\t"   \
		"pop    r12                                     \n\t"   \
		"pop    r11                                     \n\t"   \
		"pop    r10                                     \n\t"   \
		"pop    r9                                      \n\t"   \
		"pop    r8                                      \n\t"   \
		"pop    r7                                      \n\t"   \
		"pop    r6                                      \n\t"   \
		"pop    r5                                      \n\t"   \
		"pop    r4                                      \n\t"   \
		"pop    r3                                      \n\t"   \
		"pop    r2                                      \n\t"   \
		"pop    r1                                      \n\t"   \
		"pop    r0                                      \n\t"   \
		"out    0x3c, r0                                \n\t"   \
		"pop    r0                                      \n\t"   \
		"out    0x3b, r0                                \n\t"   \
		"pop    r0                                      \n\t"   \
		"out    __SREG__, r0                            \n\t"   \
		"pop    r0                                      \n\t"   \
	);
}


static void task_worker(void(*routine)(void))
{
	while (1)
	{
		routine();
	}
}


#if 0
#define cmtcs_debug_printF(str) Serial.print(F(str))
#define cmtcs_debug_println(...) Serial.println(__VA_ARGS__)
#else
#define cmtcs_debug_printF(str)
#define cmtcs_debug_println(...)
#endif

bool CMTCS::registTask(void(*routine)(void), uint8_t *stack_bottom)
{
	cmtcs_debug_printF("addr of routine = 0x");
	cmtcs_debug_println((uint16_t)routine, HEX);
	cmtcs_debug_printF("bottom of stack = 0x");
	cmtcs_debug_println((uint16_t)stack_bottom, HEX);
	
	//too many task
	if (totalTask >= MAX_TASK_COUNT + 1)
	{
		cmtcs_debug_printF("Too many task!\r\n");
		return false;
	}
	
	cmtcs_debug_printF("register as new task no.");
	cmtcs_debug_println(totalTask);
	
	stack_bottom--;
	
	//PC[0:7]
	*stack_bottom-- = (uint16_t)task_worker & 0xFF;
	
	//PC[8:15]
	*stack_bottom-- = (uint16_t)task_worker >> 8;
	
	//PC[16:21]
	*stack_bottom-- = 0;
	
	//R0
	*stack_bottom-- = 0;
	
	//SREG
	*stack_bottom-- = 0x80;
	
	//EIND
	*stack_bottom-- = 0;
	
	//RAMPZ
	*stack_bottom-- = 0;
	
	//R1 ~ R23
	for (int i = 1; i <= 23; i++)
	{
		*stack_bottom-- = 0;
	}
	
	//R24
	*stack_bottom-- = ((uint8_t *)&routine)[0];
	
	//R25
	*stack_bottom-- = ((uint8_t *)&routine)[1];
	
	//R26 ~ R31
	for (int i = 26; i <= 31; i++)
	{
		*stack_bottom-- = 0;
	}
	
	stack_pointer[totalTask++] = stack_bottom;
	
	return true;
}


void CMTCS::startMultiTask()
{
	pActiveSP = stack_pointer;
}


CMTCS cmtcs;
