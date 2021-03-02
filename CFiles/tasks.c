listobj *create_listobj(TCB *task,uint nTCnt);
static int compare_listobj(listobj *o1, listobj *o2);


exception add_task_2_list(list *l, TCB *task)
{
    listobj *list_obj;
    listobj *temp;
    list_obj = create_listobj(task,0);
    if (list_obj == NULL)
        return FAIL;

    if (l->pHead == NULL)
        l->pHead = l->pTail = list_obj;
    else
    {
        temp = l->pHead;
        while (1)
        {
            int le = compare_listobj(list_obj,temp);
            if (temp == l->pTail && !le)
            {

                temp->pNext = list_obj;
                l->pTail = list_obj;
                list_obj->pPrevious = temp;
                return OK;
            }
            else if (le)
            {
                //if list_obj has a tighter deadline

                if (temp != l->pHead)
                {
                    list_obj->pPrevious = temp->pPrevious;
                    list_obj->pNext = temp;
                    temp->pPrevious = list_obj;
                    list_obj->pPrevious->pNext = list_obj;
                }
                else
                {
                    temp->pPrevious = list_obj;
                    list_obj->pNext = temp;
                    l->pHead = list_obj;
                }
                return OK;
            }
            temp = temp->pNext;
        }
        l->pTail->pNext = list_obj;
        l->pTail = l->pTail->pNext;
    }
    return OK;
}

exception remove_task(list *l, TCB *task)
{
    if(task==NULL)
        return FAIL;
    if(task == l->pHead->pTask && task == l->pTail->pTask)
    {
        mem_free(l->pHead->pTask);
        mem_free(l->pHead);
        l->pHead = l->pTail = NULL;
        return OK;
    }
    else if(task == l->pHead->pTask)
    {
        l->pHead = l->pHead->pNext;
        mem_free(l->pHead->pPrevious->pTask);
        mem_free(l->pHead->pPrevious);
        l->pHead->pPrevious = NULL;
        return OK;
    }
    listobj* temp = l->pHead;
    while (temp!=NULL)
    {
        if (temp->pTask == task)
            break;
        if(temp->pNext == NULL)
            return FAIL;
        temp = temp->pNext;
        
    }
    if(temp!=NULL)
    {
        if(temp == l->pTail)
        {
            l->pTail = l->pTail->pPrevious;
            l->pTail->pNext = NULL;
        }
        else
        {
            temp->pPrevious->pNext = temp->pNext;
            temp->pNext->pPrevious = temp->pPrevious;
        }
        mem_free(temp->pTask);
        mem_free(temp);
        return OK;
    }
    return FAIL;
}

exception remove_listobj(list *l, listobj *o1)
{
    if(l == NULL || o1 == NULL)
        return FAIL;
    if(o1->pPrevious == NULL && o1 != l->pHead)   
        return FAIL;
    if(o1->pNext == NULL && o1 != l->pTail)
        return FAIL;
    if(o1 == l->pHead)
    {
        l->pHead = l->pHead->pNext;
        l->pHead->pPrevious = NULL;
    }
    if(o1 == l->pTail)
    {
        l->pTail = l->pTail->pPrevious;
        l->pTail->pNext = NULL;
    }
    else
    {
        o1->pNext->pPrevious = o1->pPrevious;
        o1->pPrevious->pNext = o1->pNext;
    }
    free(o1->pTask);
    free(o1);
    
}

exception move_listobj(list *src, list *dest, listobj* o1)
{
    if(src==NULL || dest==NULL || o1==NULL) {return NULLPOINTER;}
    if(find_task(src, o1->pTask)==FAIL){return FAIL;}
    if(find_task(dest,o1->pTask)==OK){return OK;}
    
    if(o1 == src->pHead)
    {
        src->pHead = src->pHead->pNext;
        src->pHead->pPrevious = NULL;
    }
    if(o1 == src->pTail)
    {
        src->pTail = src->pTail->pPrevious;
        src->pTail->pNext = NULL;
    }
    else
    {
        o1->pNext->pPrevious = o1->pPrevious;
        o1->pPrevious->pNext = o1->pNext;
    }
    if(src == TimerList)
        push(dest,o1->pTask,0);
    else
        push(dest, o1->pTask,o1->nTCnt);
    mem_free(o1);

}

static int compare_listobj(listobj *o1, listobj *o2)
{
    if(o1->nTCnt==0 || o2->nTCnt==0)
        return (o1->pTask->Deadline < o2->pTask->Deadline) ? 1 : 0;
    else
        return (min(o1->nTCnt,o1->pTask->Deadline) < min(o2->nTCnt,o2->pTask->Deadline))? 0 : 1 ;

}

listobj *create_listobj(TCB *task,uint nTCnt)
{
    listobj *list_obj;
    if ((list_obj = mem_alloc(sizeof(listobj))) == NULL)
        return NULL;
    list_obj->nTCnt = nTCnt;
    list_obj->pTask = task;
    return list_obj;
}

exception create_task(void (*tbody)(), uint dl)
{
    int status = 0;
    //Allocate memory for TCB
    TCB *task;
    task = (TCB *)mem_alloc(sizeof(TCB));
    if (task == NULL)
        return FAIL;
    //Set deadline in TCB
    task->Deadline = dl + tick_counter;
    //Set the TCB’s PC to point to the task body
    task->PC = tbody;
    //Set up stack frame and set TCB’s SP to point to the correct cell in stack segment
    task->SPSR = DEF_SPSR;

    task->StackSeg[STACK_SIZE - 2] = DEF_SPSR;
    task->StackSeg[STACK_SIZE - 3] = (unsigned int)tbody;
    task->SP = &(task->StackSeg[STACK_SIZE - 9]);

    if (kernel_mode == INIT)
        return add_task_2_list(ReadyList, task);
    else
    {
        //Disable interrupts
        isr_off();
        //Update PreviousTask (????)
        PreviousTask = ReadyList->pHead->pTask;
        //Insert new task in ReadyList
        status = add_task_2_list(ReadyList, task);
        //Update NextTask
        NextTask = ReadyList->pHead->pTask;
        //Switch context
        SwitchContext();
    }
    return status;
}

exception remove_last(list *list)
{
    //The assumption below is probably false, due to the list acting as a PriorityQueue and the first element is the one
    //with highest priority
    //But the function might come in handy if the task with least priority is to be removed
    //Assuming the running task is in the tail of the ReadyList, just input the ReadyList
    listobj *toDelete, *prevTask;
    listobj *firstTask = list->pHead;
    if (!firstTask) //same as FirstTask == null
        return FAIL;
    else
    {
        toDelete = firstTask;
        prevTask = firstTask;
        while (toDelete->pNext != NULL)
        {
            prevTask = toDelete;
            toDelete = toDelete->pNext;
        }
        if (toDelete == firstTask)
            firstTask = NULL;
        else
            prevTask->pNext = NULL;
        free(toDelete);
        return OK;
    }
}

exception push(list *l, TCB *task,uint nTCnt)
{
    listobj *list_obj;
    list_obj =  create_listobj(task,nTCnt);
    if (!list_obj)
        return FAIL;
    listobj *current = l->pHead;
    if (!l->pHead)
    {
        l->pHead = l->pTail = list_obj;
        return OK;
    }

    //Here we have the special case that the first task of the list has LESSER priority than
    //the task to be added
    //Meaning deadline for current > list_obj
    if (compare_listobj(list_obj, current))
    {
        // Insert New Node before head
        list_obj->pNext = current;
        l->pHead = list_obj;
        return OK;
    }
    else
    {
        //Traversing the list and finding a position to insert the new node
        while (current != NULL && compare_listobj(list_obj, current))
            current = current->pNext;
        // Either at the ends of the list
        // or at required position
        if(current == NULL)
            {
                list_obj->pPrevious = l->pTail;
                list_obj->pNext = NULL;
                l->pTail->pNext = list_obj;
                l->pTail = list_obj;
                return OK;
            }
        list_obj->pNext= current;
        list_obj->pPrevious = current->pPrevious;
        current->pPrevious = list_obj;
        return OK;
    }
    return FAIL;
}

exception pop(list* list)
{
    if (!list)
        return FAIL;
    if (list->pHead)
    {
        listobj* toRemove = list->pHead;
        list->pHead = list->pHead->pNext;
        free(toRemove->pTask);
        free(toRemove);
        return OK;
    }
    return FAIL;
}

exception find_task(list* l, TCB *task)
{
    listobj* temp = l->pHead;
    while(temp!=NULL)
    {
        if(temp->pTask == task)
            return OK;
        if(temp->pNext==NULL)
            break;
        temp = temp->pNext;
    }
    return FAIL;
}
