
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
    listobj* lobj = ReadyList->pHead;
    ReadyList->pHead = ReadyList->pHead->pNext;
    msg* pmess = lobj->pMessage;
    mem_free(lobj);
    push(ReadyList, PreviousTask,0, pmess);
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
exception periodically_run(void (*body)(),int ticks_period,int num_periods)
{
    int k=0;
    while((wait(ticks_period))!=DEADLINE_REACHED)
    {
        (*body)();
        if((++k)==num_periods&&num_periods!=0)
            break;
    }
    return OK;
}
extern void TimerInt(void)
{
    tick_counter++;
    if(ticks() >= min(TimerList->pHead->nTCnt,TimerList->pHead->pTask->Deadline))
    {
        move_listobj(TimerList,ReadyList,TimerList->pHead);
        NextTask = ReadyList->pHead->pTask;
    }
    else if(ticks() >= WaitingList->pHead->pTask->Deadline)
    {
        move_listobj(WaitingList,ReadyList,WaitingList->pHead);
        NextTask = ReadyList->pHead->pTask;
    }
}
