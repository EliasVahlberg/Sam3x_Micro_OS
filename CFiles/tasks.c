listobj* create_listobj(TCB* task);


void test_task1()
{
    
}
void test_task2()
{
    
}
void test_task3()
{
    
}


exception append_task_2_list(list* l, TCB* task)
{
    listobj* list_obj;
    list_obj = create_listobj(task);
    if(list_obj==NULL)
            return FAIL;
        
    if(l->pHead==NULL)
        l->pHead = l->pTail =  list_obj;
    else
    {
        l->pTail->pNext =list_obj;
        l->pTail = l->pTail->pNext; 
    }
    return OK;
}


listobj* create_listobj(TCB* task)
{
    listobj* list_obj;
    if((list_obj=mem_alloc(sizeof(listobj)))==NULL)
        return NULL;
    list_obj->nTCnt =  tick_counter;
    list_obj->pTask =  task;
    return list_obj;
}


exception create_task(void (*tbody)(), uint dl)
{
    TCB *task;
    task = (TCB *)mem_alloc(sizeof(TCB));
    if (task == NULL)
        return FAIL;
    task->PC = tbody;
    task->SPSR = DEF_SPSR;
    task->Deadline = dl + tick_counter;
    /*
    if (kernel_mode == INIT)
        return append_task_2_list(ReadyList, task);
    else
    {
        //Disable interrupts
        isr_off();
        //Update PreviousTask (????)
        SaveContext();
        //Insert new task in ReadyList
        append_task_2_list(ReadyList, task);
        RUNNING = ReadyList->pTail->pTask;
        //Update NextTask
        //Switch context
        ENDIF
        
        Return status

        SaveContext();
        if (firstExec)
        { // if first execution then
            firstExec = 0;

            append_task_2_list(ReadyList, task);
            Running = getFirstTask(readyList)->pTask;

            LoadContext();
        }
    }
    */

    task->StackSeg[STACK_SIZE - 2] = DEF_SPSR;
    task->StackSeg[STACK_SIZE - 3] = (unsigned int)tbody;
    task->SP = &(task->StackSeg[STACK_SIZE - 9]);
    // after the mandatory initialization you can implement the rest of the
    return  append_task_2_list(ReadyList, task);
}