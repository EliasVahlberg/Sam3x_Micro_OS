/*********************************************************/
/** Global variables and definitions                     */
/** Modified  on 27th Feb 2019                           */
/*********************************************************/

#include <stdlib.h>  /* for using the functions calloc, free */
#include <string.h>  /* for using the function memcpy        */
#include <limits.h>  /* for using the constant UINT_MAX      */
#include <stdint.h>

#define CONTEXT_SIZE            8   /*  for the 8 registers: r4 to r11   */ 
#define STACK_SIZE              100 /*  about enough space for the stack */

#define TRUE                    1
#define FALSE                   !TRUE

#define RUNNING                 1
#define INIT                    !RUNNING

#define FAIL                    0
#define SUCCESS                 1
#define OK                      1

#define DEADLINE_REACHED        -4      
#define NOT_EMPTY               0

#define SENDER                  +1
#define RECEIVER                -1


#define DEF_SPSR                0x21000000 

#define MESSAGE_LOST            -1 //If a message was deleted in force_remove_mailbox()
#define NULLPOINTER             -2
#define ALLOCFAIL               -3


typedef int                     exception;
typedef int                     bool;
typedef unsigned int            uint;
typedef int 			action;

struct  l_obj;         // Forward declaration

// Task Control Block, TCB.  Modified on 24/02/2019

extern void     TimerInt(void);

typedef struct
{
        //uint    Context[CONTEXT_SIZE];        
        uint    *SP;
        uint    R4toR11[CONTEXT_SIZE];
        //void    (*LR)();
        void    (*PC)();
        uint    SPSR;     
        uint    StackSeg[STACK_SIZE];
        uint    Deadline;
} TCB;


// Message items
typedef struct msgobj {
        char            *pData;
        exception       Status;
        struct l_obj    *pBlock;
        struct msgobj   *pPrevious;
        struct msgobj   *pNext;
} msg;

// Mailbox structure
typedef struct {
        msg             *pHead;
        msg             *pTail;
        int             nDataSize;
        int             nMaxMessages;
        int             nMessages;
        int             nBlockedMsg;
} mailbox;


// Generic list item
typedef struct l_obj {
         TCB            *pTask;
         uint           nTCnt;
         msg            *pMessage;
         struct l_obj   *pPrevious;
         struct l_obj   *pNext;
} listobj;


// Generic list
typedef struct _list {
         listobj        *pHead;
         listobj        *pTail;
} list;


// Function prototypes


// Task administration
exception       init_kernel(void);
exception	create_task( void (* body)(), uint d );
void            terminate( void );
void            run( void );

// Communication
mailbox*	create_mailbox( uint nMessages, uint nDataSize );
exception       send_wait( mailbox* mBox, void* pData );
exception       receive_wait( mailbox* mBox, void* pData );
exception	send_no_wait( mailbox* mBox, void* pData );
exception       no_messages( mailbox* mBox );
exception       receive_no_wait( mailbox* mBox, void* pData );
exception       force_remove_mailbox(mailbox* mBox);
exception       remove_mailbox(mailbox* mBox);


// Timing
exception	wait( uint nTicks );
void            set_ticks( uint no_of_ticks );
uint            ticks( void );
uint		deadline( void );
void            set_deadline( uint nNew );

//Interrupt and context switch
extern void     isr_on(void);
extern void     isr_off(void);

extern void     SwitchContext( void );
                                
extern void     LoadContext_In_Run( void );
                   /* To be used on the last line of the C function run() */

extern void     switch_to_stack_of_next_task( void );
                   /* To be used inside the C function terminate() */
                     
extern void     LoadContext_In_Terminate( void );
                   /* To be used on the last line of the C function terminate() */



TCB *PreviousTask, *NextTask;
extern int32_t kernel_mode      = 0;
extern int32_t mem_counter      = 0;
extern int32_t tick_counter     = 0; 

uint min(uint a, uint b);
static int __ISR_ON(void);
static int __ISR_OFF(void);

exception add_task_2_list(list* l, TCB* task);
exception create_task(void (*body)(), uint d);
void *mem_alloc(size_t size);
void mem_free(void *mem);


list *ReadyList;
list *WaitingList;
list *TimerList;
void* first_heap = NULL;

#pragma region debugg
void*   DEBUG_memadress = NULL;
uint    DEBUG_memsize = 0;
#pragma endregion debugg


#include "CFiles\mallocstats.h"
#include "CFiles\memory_manager.c"
#include "CFiles\common_functions.c"
#include "CFiles\tasks.c"
#include "CFiles\kernel_init.c"
#include "CFiles\timing.c"
#include "CFiles\kernel.c"
#include "CFiles\communication.c"
