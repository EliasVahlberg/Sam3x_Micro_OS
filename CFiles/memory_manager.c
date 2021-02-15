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
    if (mem && dynamic_mem_adress(mem))
    {
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
// int dynamic_mem_adress(void *ptr)
// {
//     char dynMem [__region_RAM_end__ - __region_RAM_start__];
//     for (int i = 0; i < __region_RAM_end__ - __region_RAM_start__; i++)
//     {
//         if (&ptr == dynMem[i])
//             return OK;
//     }
//     return FAIL;
// }