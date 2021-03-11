
/**
* @file
*     handlers_task.c
* @authors 
*     Elias Vahlberg
*     Hamed Haghjo
* \brief 
*     ...
*/



exception MEMFAULT_handler()
{
    //Kinda sus
    num_memfaults++;
    //get_system_fault_registers();
    return MEMFAULT;
}
exception ESCALATED_NULLPOINTER_handler()
{
    if(ReadyList->pHead->pNext!=NULL&&ReadyList->pHead!=NULL)
        terminate();
    //get_system_fault_registers();
    return ESCALATED_NULLPOINTER;
}
exception   task_NULLPOINTER_handler()
{
    //Did you check malloc?
    num_nullpointers++;
    //get_system_fault_registers();
    return NULLPOINTER;
}
exception task_ALLOCFAIL_handler()
{
    //Out of memory my friend
    num_allocfails++;
    update_meminfo();
    //get_system_fault_registers();
    return ALLOCFAIL;
}
exception task_FAIL_handler()
{
    //Well this was descriptive...
    num_fails++;
    //get_system_fault_registers();
    return FAIL;
}
exception DEADLINE_REACHED_handler()
{
    //You should blame blackboard it usually works
    num_deadline_reached++;
    //get_system_fault_registers();
    return DEADLINE_REACHED;
}
exception task_exception_manager(exception exc)
{
    if(__get_BASEPRI() != 0x000000a0)
    {
        
        isr_cleared_during_exception = 1;
        __ISR_OFF();
    }
    if(exc == OK)
        return OK;
    if(task_exception_status == NULLPOINTER)
        exc = ESCALATED_NULLPOINTER;
    else if(task_exception_status !=0 )
        escalated_exception(exc);
    task_exception_status = exc;
    switch (exc)
    {
    case FAIL:
        exc = task_FAIL_handler();
        break;
    case NULLPOINTER:
        exc = task_NULLPOINTER_handler();
    break;
    case ALLOCFAIL:
        exc =  task_ALLOCFAIL_handler();
    break;
    case ESCALATED_NULLPOINTER:
        exc =  ESCALATED_NULLPOINTER_handler();
    break;
    case MEMFAULT:
        exc =  MEMFAULT_handler();
    break;
    default: escalated_exception(exc);
        break;
    }
    if(isr_cleared_during_exception)
        __ISR_ON();
    clear_task_exception();
    return exc;
}
void escalated_exception(exception exc)
{
    switch (exc)
    {
    case FAIL:
        while (1){/* Escalated Fail Exception*/}
        break;
    case NULLPOINTER:
        while (1){/* Escalated NULLPOINTER Exception*/}
        break;
    case ALLOCFAIL:
        while (1){/* Escalated ALLOCFAIL Exception*/}
        break;
    
    default:
        while (1){/*Unexpected exception type*/}
        break;
    }
    while (1)
    {/*This is bad, REAL bad*/}
    return; //Good luck getting here CPU
}
void clear_task_exception()
{
    task_exception_status = 0;
}
void get_system_fault_registers()
{
    FAULT_REGISTERS[0]  = *((volatile unsigned int*) SYS_FAULT_REG_AFSR);
    FAULT_REGISTERS[1]  = *((volatile unsigned int*) SYS_FAULT_REG_AIRCR);
    FAULT_REGISTERS[2]  = *((volatile unsigned int*) SYS_FAULT_REG_BFAR);
    FAULT_REGISTERS[3]  = *((volatile unsigned int*) SYS_FAULT_REG_CCR);
    FAULT_REGISTERS[4]  = *((volatile unsigned int*) SYS_FAULT_REG_CFSR);
    FAULT_REGISTERS[5]  = *((volatile unsigned int*) SYS_FAULT_REG_DFSR);
    FAULT_REGISTERS[6]  = *((volatile unsigned int*) SYS_FAULT_REG_HFSR);
    FAULT_REGISTERS[7]  = *((volatile unsigned int*) SYS_FAULT_REG_ICSR);
    FAULT_REGISTERS[8]  = *((volatile unsigned int*) SYS_FAULT_REG_MMFAR);
    FAULT_REGISTERS[9]  = *((volatile unsigned int*) SYS_FAULT_REG_SCR);
    FAULT_REGISTERS[10] = *((volatile unsigned int*) SYS_FAULT_REG_SHCSR);
    FAULT_REGISTERS[11] = *((volatile unsigned int*) SYS_FAULT_REG_UFSR);
    FAULT_REGISTERS[12] = *((volatile unsigned int*) SYS_FAULT_REG_BFSR);
    FAULT_REGISTERS[13] = *((volatile unsigned int*) SYS_FAULT_REG_MMFSR);
}