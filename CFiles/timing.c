
/**
* @file
*     timing.c
* @authors 
*     Elias Vahlberg
*     Hamed Haghjo
* \brief 
*     ...
*/

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
    push(ReadyList, PreviousTask,0);
    //Update NextTask
    //Switch context
    if (ReadyList->pHead->pTask != PreviousTask)
    {
        NextTask = ReadyList->pHead->pTask;
        isr_on();
        SwitchContext();
    }
}


exception wait(uint nTicks)
{
    exception status;
    isr_off();
    ReadyList->pHead->nTCnt = ticks() + nTicks;
    PreviousTask = NextTask;                              //Update PreviousTask
    move_listobj(ReadyList, TimerList, ReadyList->pHead); //Place running task in the TimerList
    NextTask = ReadyList->pHead->pTask;                   //Update NextTask
    SwitchContext();                                      //Switch context
    if (deadline_reached(NextTask))                       //If deadline reched, then
        status = DEADLINE_REACHED;                        //Status is DEADLINE_REACHED
    else
        status = OK; //Else Status is OK
    isr_on();
    return status; //Return Status
}

extern void TimerInt(void)
{
    tick_counter++;
    if(ticks() >= min(TimerList->pHead->nTCnt,TimerList->pHead->pTask->Deadline))
    {
        NextTask = TimerList->pHead->pTask;
        move_listobj(TimerList,ReadyList,TimerList->pHead);
    }
    else if(ticks() >= WaitingList->pHead->pTask->Deadline)
    {
        NextTask = WaitingList->pHead->pTask;
        move_listobj(WaitingList,ReadyList,WaitingList->pHead);
    }
}
