
/**
* @file
*     kernel_init.c
* @authors 
*     Elias Vahlberg
*     Hamed Haghjo
* \brief 
*     ...
*/
#pragma region Globalvars


extern TCB idle_task;

#pragma endregion Globalvars
#pragma region Function_PreInit
exception init_kernel(void);
list *create_task_list(void);
void idle();
#pragma endregion Function_PreInit

exception init_kernel(void)
{
    //Set tick counter to zero
    set_ticks(0);
    if (ticks() != 0)
        return task_exception_manager(FAIL);

    //Create necessary data structures
    if ((ReadyList = create_task_list()) == NULL)
        return task_exception_manager(FAIL);
    if ((WaitingList = create_task_list()) == NULL)
        return task_exception_manager(FAIL);
    if ((TimerList = create_task_list()) == NULL)
        return task_exception_manager(FAIL);

    //Create an Idle task
    if (!create_task(idle, UINT_MAX))
        return task_exception_manager(FAIL);
    
    //Set the kernel in INIT mode
    kernel_mode = INIT;
    //Return status
    return OK;
}

/*
 \brief creates a list of tasks
 @return pointer to the allocated list, if memory alloc failiure it returns NULL
 */
list *create_task_list()
{
    list *task_list = (list *)mem_alloc(sizeof(list));
    if (task_list == NULL)
    {
        task_exception_manager(ALLOCFAIL);
        return NULL;
    }

   // listobj *obj = mem_alloc(sizeof(listobj));
   // obj->pNext = obj;
   // obj->pPrevious = obj;
   // task_list->pHead = obj;
   // task_list->pTail = obj;
    return task_list;
}

/**
* idle_task
* \brief Does nothing but keep the OS idle
*/
void idle()
{
    while (1)
    {
    }
}
