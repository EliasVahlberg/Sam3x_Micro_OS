/**
* @file
*     handlers.h
* @authors 
*     Elias Vahlberg
*     Hamed Haghjo
* \brief 
*     ...
*/

#pragma region macros
#define MAILBOX_NON_EMPTY_THRESHOLD 100
#define NULLPOINTER_THRESHOLD 10
#pragma endregion macros

#pragma region FAULT_REGISTERS
#define SYS_FAULT_REG_AFSR	    0xE000ED3C 
#define SYS_FAULT_REG_AIRCR	    0xE000ED0C 
#define SYS_FAULT_REG_BFAR	    0xE000ED29 
#define SYS_FAULT_REG_CCR	    0xE000ED14 
#define SYS_FAULT_REG_CFSR	    0xE000ED28         
#define SYS_FAULT_REG_DFSR	    0xE000ED30 
#define SYS_FAULT_REG_HFSR	    0xE000ED2C 
#define SYS_FAULT_REG_ICSR	    0xE000ED04 
#define SYS_FAULT_REG_MMFAR	    0xE000ED28 
#define SYS_FAULT_REG_SCR	    0xE000ED10 
#define SYS_FAULT_REG_SHCSR	    0xE000ED24 
#define SYS_FAULT_REG_UFSR      0xE000ED2A         
#define SYS_FAULT_REG_BFSR      0xE000ED29         
#define SYS_FAULT_REG_MMFSR     0xE000ED28         
#pragma endregion FAULT_REGISTERS

#pragma region flags
volatile int task_exception_status          = 1;
volatile int num_memfaults                  = 0;
volatile int num_nullpointers               = 0;
volatile int num_escalated_nullpointers     = 0;
volatile int num_fails                      = 0;
volatile int num_allocfails                 = 0;
volatile int num_deadline_reached           = 0;
volatile int escalated_exception_set        = 0;
volatile int isr_cleared_during_exception   = 0;
volatile int num_failed_remove_mailbox      = 0;
volatile int FAULT_REGISTERS[14];
volatile void* memfault_adress2              = NULL;
volatile void* memfault_adress1              = NULL;
#pragma endregion flags


#pragma region Functions
exception   task_exception_manager(exception exc);
exception   task_NULLPOINTER_handler();
exception   task_ALLOCFAIL_handler();
exception   task_FAIL_handler();
exception   DEADLINE_REACHED_handler();
exception   MEMORY_LEAKAGE_handler();
exception   ESCALATED_NULLPOINTER_handler();
exception   NOT_EMPTY_handeler();
void        escalated_exception(exception exc);
void        get_system_fault_registers();
void        clear_task_exception();
#pragma endregion Functions

#include "handlers.c"