
/**
* @file
*     memory_manager.c
* @authors 
*     Elias Vahlberg
*     Hamed Haghjo
* \brief 
*     ...
*/


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
    if(mem_counter==0)
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
    if (mem ) 
    {
        if(!dynamic_mem_adress(mem))
            return;
        __ISR_OFF();
        free(mem);
        mem_counter--;
        DEBUG_memadress = mem;
        //BREAKPOINT HERE
        __ISR_ON();
    }
}

exception mem_copy(char* src, char* dest, uint size)
{
    if(size <= 0)
        return FAIL;
    
    if(src == NULL || dest == NULL)
        return NULLPOINTER;

    for (int i = 0; i < size; i++)
        dest[i] = src[i];
    
    return OK;
}

void update_meminfo()
{
    meminfo = __iar_dlmallinfo();
}
int dynamic_mem_adress(void *ptr)
{
   if(first_heap != NULL)
   {
       if(ReadyList != NULL && WaitingList != NULL && TimerList != NULL)
       {
           //Check if the memory is in the heap area
           if(ptr<first_heap)
            return FAIL;
            //Check if memory is in a TCB Stack segment
           listobj *lobj = ReadyList->pHead;
           while (lobj!=NULL)
           {
                if( ptr >lobj->pTask->StackSeg && ptr<(&(lobj->pTask->StackSeg[99])))
                    return FAIL;
                lobj = lobj->pNext;
           }
           lobj = WaitingList->pHead;
           while (lobj!=NULL)
           {
                if( ptr >lobj->pTask->StackSeg && ptr<lobj->pTask->StackSeg+STACK_SIZE*4)
                    return FAIL;
                lobj = lobj->pNext;
           }
           lobj = TimerList->pHead;
           while (lobj!=NULL)
           {
                if( ptr >lobj->pTask->StackSeg && ptr<lobj->pTask->StackSeg+STACK_SIZE*4)
                    return FAIL;
                lobj = lobj->pNext;
           }
           return OK;
       }
   }
    return FAIL;
}