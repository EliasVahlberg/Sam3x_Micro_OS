void terminate(void)
{
    //Disable interrupts
    isr_off();
    //Remove running task from ReadyList
    remove_last(ReadyList); //Assuming the running task is in the tail of ReadyList
    //Set NextTask to equal TCB of the next task
    NextTask = ReadyList->pHead->pTask;
    //Switch to process stack of task to be loaded
    switch_to_stack_of_next_task();
    //Remove data structures of task being terminated
    //
    //Load context using:
    LoadContext_In_Terminate();
}

void run(void)
{
    //Initialize interrupt timer {Ticks = 0;}
    set_ticks(0);
    //Set KernelMode = RUNNING
    kernel_mode = RUNNING;
    //Set NextTask to equal TCB of the task to be loaded
    NextTask = ReadyList->pHead->pTask;
    //Load context using:
    //LoadContext_In_Run();
    LoadContext_In_Run();
}