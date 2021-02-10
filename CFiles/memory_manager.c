
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
* \brief Allocates memory safely
* @param mem adress to the allocated adress 
*/
void mem_free(void *mem)
{
    if (mem)
    {
        isr_off();
        free(mem);
        mem_counter--;
        isr_on();
    }
}

void *alloc_task()
{

}
void update_meminfo()
{
    meminfo = __iar_dlmallinfo();
}
