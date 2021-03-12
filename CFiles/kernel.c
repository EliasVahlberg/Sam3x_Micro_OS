
/**
* @file
*     kernel.c
* @authors 
*     Elias Vahlberg
*     Hamed Haghjo
* \brief 
*     ...
*/
void terminate(void)
{
    //Disable interrupts
    isr_off();
    //Remove running task from ReadyList
    
    pop(ReadyList); //Assuming the running task is in the head of ReadyList
    //Set NextTask to equal TCB of the next task
    NextTask = ReadyList->pHead->pTask;     
    if(NextTask == NULL|| NextTask->PC ==NULL || NextTask->StackSeg ==NULL)
        task_exception_manager(FATAL_EXCEPTION);
    switch_to_stack_of_next_task();         //Switch to process stack of task to be loaded
    //Remove data structures of task being terminated
    LoadContext_In_Terminate();             //Load context
}

void run(void)
{
    set_ticks(0);                           //Initialize interrupt timer {Ticks = 0;}
    kernel_mode = RUNNING;                  //Set KernelMode = RUNNING
    NextTask = ReadyList->pHead->pTask;     //Set NextTask to equal TCB of the task to be loaded
    if(NextTask == NULL|| NextTask->PC ==NULL || NextTask->StackSeg ==NULL)
        task_exception_manager(FATAL_EXCEPTION);
    LoadContext_In_Run();    //Load context using: LoadContext_In_Run();
}