/**
* @file
*     memory_manager.c
* @authors 
*     Elias Vahlberg
*     Hamed Haghjo
* \brief 
*     ...
*/


#pragma region Functions
void save_meminfo(struct mallinfo minfo);
void *mem_alloc(size_t size);
void mem_free(void *mem);
int dynamic_mem_adress(void *ptr);
#pragma endregion Functions

struct mallinfo meminfo;

/**
* mem_alloc
* \brief Allocates memory safely
* @param size the size that needs to be allocated 
* @return the adress to the allocated area
*/
void *mem_alloc(size_t size)
{
    void *mem;
    __ISR_OFF();
    mem = calloc(1, size);
    if (mem_counter == 0)
        first_heap = mem;
    if (mem != NULL)
        mem_counter++;
    DEBUG_memadress = mem;
    DEBUG_memsize = size;
    //BREAKPOINT HERE
    __ISR_ON();
    return mem;
}

/**
* mem_free
* \brief Frees allocated memory safely
* @param mem adress to the allocated area 
*/
void mem_free(void *mem)
{
    if (mem!=NULL)
    {
        if (!dynamic_mem_adress(mem))
        {
            task_exception_manager(MEMFAULT);
            return;
        }
        __ISR_OFF();
        free(mem);
        mem_counter--;
        DEBUG_memadress = mem;
        //BREAKPOINT HERE
        __ISR_ON();
        return;
    }
    else
    {
        DEBUG_memadress = (void*)0xfefefefe;
        return;
    }
}

exception mem_copy(char *src, char *dest, uint size)
{
    if (size <= 0)
        return task_exception_manager(FAIL);

    if (src == NULL || dest == NULL)
        return task_exception_manager(NULLPOINTER);

    for (int i = 0; i < size; i++)
        dest[i] = src[i];

    return OK;
}
void update_meminfo()
{
    meminfo = __iar_dlmallinfo();
    return;
}
void save_meminfo(struct mallinfo minfo)
{
    minfo = __iar_dlmallinfo();
    return;
}
int dynamic_mem_adress(void *ptr)
{
    if(kernel_mode == RUNNING)
    {
    if (first_heap != NULL)
    {
        if (ReadyList != NULL && WaitingList != NULL && TimerList != NULL)
        {
            //Check if the memory is in the heap area
            if (ptr < first_heap)
                return task_exception_manager(FAIL);
            //Check if memory is in a TCB Stack segment
            listobj *lobj = ReadyList->pHead;
            while (lobj != NULL)
            {
                if (ptr > lobj->pTask->StackSeg && ptr < (&(lobj->pTask->StackSeg[99])))
                    return task_exception_manager(FAIL);
                lobj = lobj->pNext;
            }
            lobj = WaitingList->pHead;
            while (lobj != NULL)
            {
                if (ptr > lobj->pTask->StackSeg && ptr < lobj->pTask->StackSeg + STACK_SIZE * 4)
                    return task_exception_manager(FAIL);
                lobj = lobj->pNext;
            }
            lobj = TimerList->pHead;
            while (lobj != NULL)
            {
                if (ptr > lobj->pTask->StackSeg && ptr < lobj->pTask->StackSeg + STACK_SIZE * 4)
                    return task_exception_manager(FAIL);
                lobj = lobj->pNext;
            }
            return OK;
        }
    }
    return task_exception_manager(FAIL);
    }
    return OK;
}