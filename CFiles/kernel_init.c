
#pragma region Globalvars


extern TCB idle_task;

#pragma endregion Globalvars
#pragma region Function_PreInit
exception init_kernel(void);
list *create_task_list(void);
void mem_free(void *mem);
void *mem_alloc(size_t size);
void idle();
#pragma endregion Function_PreInit

exception init_kernel(void)
{
    //Set tick counter to zero
    set_ticks(0);

    //Create necessary data structures
    if ((ReadyList = create_task_list()) == NULL)
        return FAIL;
    if ((WaitingList = create_task_list()) == NULL)
        return FAIL;
    if ((TimerList = create_task_list()) == NULL)
        return FAIL;

    //Create an Idle task
    if (!create_task(idle, UINT_MAX))
        return FAIL;
    
    //Set the kernel in INIT mode
    kernel_mode = INIT;
    //Return status
    return OK;
}

/*
 \brief creates a list of tasks
 @return pointer to the allocated list, if memory alloc faliure it returns NULL
 */
list *create_task_list()
{
    list *task_list = (list *)mem_alloc(sizeof(list));
    if (task_list == NULL)
        return NULL;

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

//Needed to be added since they are referenced in the asm file
extern void TimerInt(void)
{
    tick_counter++;
}
