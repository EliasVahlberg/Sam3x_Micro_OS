
/**
* @file
*     common_functions.c
* @authors 
*     Elias Vahlberg
*     Hamed Haghjo
* \brief 
*     ...
*/

/**
* min
* \brief Simple minimum function for the type integer type uint 
* @param a first value
* @param b second value
* @return returns a if a<b and b if b<a 
*/
uint min(uint a, uint b)
{return (a>b)?b:a;}

/**
* __ISR_ON
* \brief Uses the isr_on() function defined in context_switching_functions_march_2019.s to turn off all
* maskable interrupts IF the PRIMASK register is equal to 0 so that isr_on does not run when in the systick handler.
* @return value returned
*/
static int __ISR_ON(void)
{
    if(__get_PRIMASK()==0)
    {
        isr_on();
        return 1;
    }
    return 0;
}

/**
* __ISR_OFF
* \brief Uses the isr_off() function defined in context_switching_functions_march_2019.s to turn off all
* maskable interrupts IF the PRIMASK register is equal to 0  so that isr_off does not run when in the systick handler.
* @return value returned
*/
static int __ISR_OFF(void)
{
    if(__get_PRIMASK()==0)
    {
        isr_off();
        return 1;
    }
    return 0;
}