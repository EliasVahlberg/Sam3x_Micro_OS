
#include "at91sam3x8.h"
#include "system_sam3x.h"

#define CLKSPEED 84000000
#define MAXTICKS 10;
volatile uint32_t ticks = 0;
uint8_t task = 0;
uint8_t set_task_nr = 0;

uint32_t stack_f1[40];
uint32_t stack_f2[40];
uint32_t *sp_f1 = &stack_f1[40];
uint32_t *sp_f2 = &stack_f2[40];
uint32_t f1 = 0;
uint32_t f2 = 0;

#pragma region Functions
void Func1();
void Func2();
void init_tasks();
#pragma endregion Functions

int main()
{
  init_tasks();
  while (1)
  {
    set_task_nr = 1;
  }
  return 2;
}

void SysTick_Handler(void)
{
  ticks++;
  if (ticks == MAXTICKS)
  {
    ticks = 0;
    if (set_task_nr == 1)
    {
      set_task_nr = 2;
      __set_SP((unsigned int)sp_f2);
    }
    else if (set_task_nr == 2)
    {
      set_task_nr = 1;
      __set_SP((unsigned int)sp_f1);
    }
  }
}
void init_tasks()
{
  *(--sp_f1) = 0x21000000;       //XPRS
  *(--sp_f1) = (uint32_t)&Func1; //PC
  *(--sp_f1) = 0xEU;             //LR
  *(--sp_f1) = 0xCU;             //R12
  *(--sp_f1) = 0x3U;             //R3
  *(--sp_f1) = 0x2U;             //R2
  *(--sp_f1) = 0x1U;             //R1
  *(--sp_f1) = 0x0U;             //R0

  *(--sp_f2) = 0x21000000;       //XPRS
  *(--sp_f2) = (uint32_t)&Func2; //PC
  *(--sp_f2) = 0xEU;             //LR
  *(--sp_f2) = 0xCU;             //R12
  *(--sp_f2) = 0x3U;             //R3
  *(--sp_f2) = 0x2U;             //R2
  *(--sp_f2) = 0x1U;             //R1
  *(--sp_f2) = 0x0U;             //R0
}
void set_task(uint8_t t_nr)
{
}
void Func1()
{
  while (1)
  {
    f1++;
  }
}

void Func2()
{
  while (1)
  {
    f2++;
  }
}
