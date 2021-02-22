msg* mailbox_dequeue(mailbox* mBox);

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
    isr_off();
    mem = calloc(1, size);
    if(mem_counter==0)
        first_heap = mem;
    if (mem != NULL)
        mem_counter++;
    isr_on();
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
        isr_off();
        free(mem);
        mem_counter--;
        isr_on();
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
           if(ptr<first_heap)
            return FAIL;
           listobj *lobj = ReadyList->pHead;
           while (lobj!=NULL)
           {
                if( ptr >lobj->pTask->StackSeg && ptr<lobj->pTask->StackSeg+STACK_SIZE*4)
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