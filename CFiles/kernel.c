
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
    isr_off();                              //Disable interrupts
    PreviousTask = ReadyList->pHead->pTask;
    pop(ReadyList);                         //Remove running task from ReadyList
    NextTask = ReadyList->pHead->pTask;     //Set NextTask to equal TCB of the next task  
    if(NextTask == NULL|| NextTask->PC ==NULL || NextTask->StackSeg ==NULL)
        task_exception_manager(FATAL_EXCEPTION);
    int i = 0;
    i++;
    i++;
    //clear_task_exception();
    switch_to_stack_of_next_task();         //Switch to process stack of task to be loaded
    LoadContext_In_Terminate();             //Load context
}

void run(void)
{
    set_ticks(0);                           //Initialize interrupt timer {Ticks = 0;}
    kernel_mode = RUNNING;                  //Set KernelMode = RUNNING
    NextTask = ReadyList->pHead->pTask;     //Set NextTask to equal TCB of the task to be loaded
    if(NextTask == NULL|| NextTask->PC ==NULL || NextTask->StackSeg ==NULL)
        task_exception_manager(FATAL_EXCEPTION);
    LoadContext_In_Run();                   //Load context using: LoadContext_In_Run();
}