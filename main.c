
/**
* @file
*     main.c
* @authors 
*     Elias Vahlberg
*     Hamed Haghjo
* \brief 
*     ...
*/

#pragma region TestVars

unsigned int g0 = 0, g1 = 0, g2 = 0, g3 = 1; /* gate flags for various stages of unit test */

unsigned int low_deadline = 1000;
unsigned int high_deadline = 100000;


#define create_count_from_main_MAX 10
#define create_count_from_task2_MAX 10
#pragma endregion TestVars

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
  lab2_test_main();
  lab1_test_main();

  while (1)
  { 
  }
}
