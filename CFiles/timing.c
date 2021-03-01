void set_ticks( uint ticks )
{
    tick_counter = ticks;
}

uint ticks(void)
{
    return tick_counter;
}

uint deadline(void)
{
    return ReadyList->pHead->pTask->Deadline;
}

int deadline_reached(TCB* task)
{
    return (task->Deadline <= tick_counter)? 1:0;
}

void set_deadline(uint deadline)
{
    //Disable interrupt
    isr_off();
    //Set the deadline field in the calling TCB
    ReadyList->pHead->pTask->Deadline = deadline;
    //Update PreviousTask
    PreviousTask = ReadyList->pHead->pTask;
    //Reschedule ReadyList
    pop(ReadyList);
    push(ReadyList, PreviousTask);
    //Update NextTask
    isr_on();
    //Switch context
    if (ReadyList->pHead->pTask != PreviousTask)
        SwitchContext();
}