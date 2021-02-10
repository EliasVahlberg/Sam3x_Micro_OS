

#include "system_sam3x.h"
#include "at91sam3x8.h"
#include "kernel_functions.h"
#include "Tests\tests.h"


void main()
{
  SystemInit();
  SysTick_Config(100000);

  SCB->SHP[((uint32_t)(SysTick_IRQn)&0xF) - 4] = (0xE0);
  isr_off();
  lab1_test_main();

  while (1)
  { 
  }
}
