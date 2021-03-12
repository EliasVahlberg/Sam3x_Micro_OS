
/**
* @file
*     functional_tests_task.c
* @authors 
*     Elias Vahlberg
*     Hamed Haghjo
* \brief 
*     ...
*/

char *test_tasks_status;
int dl1 = 1;
int very_low_deadline = 50;
exception test2_status = OK;
int n_mess = 0;
int n_tasks_terminated = 0;

void test_task1();
void test_task2();
void test_task3();
void test_task4();
void test_task_sending();
void test_task_recieving();
void pre_functional_test();
void functional_test_communication_main();
void functional_test_multiple_tasks();


void pre_functional_test()
{
    if (init_kernel() != OK)
        while (1){}
    if (ReadyList->pHead != ReadyList->pTail)
        while (1){}
    if (WaitingList->pHead != WaitingList->pTail)
        while (1){}
    if (TimerList->pHead != TimerList->pTail)
        while (1){}
}
void functional_test_task_main()
{
    pre_functional_test();
    if (create_task(test_task1, dl1 * very_low_deadline) <= FAIL)
        while (1){}
    run();
}

void functional_test_multiple_tasks()
{
    pre_functional_test();
        update_meminfo();
    for (int i = 0; i < 10; i++)
    {
    if (create_task(test_task3, high_deadline) <= FAIL)
        while (1){}
    }
    run();
}

void functional_test_communication_main()
{
    pre_functional_test();
    if ((charMbox = create_mailbox(15, sizeof(char))) == NULL)
        while (1){}
    if ((intMbox = create_mailbox(15, sizeof(int))) == NULL)
        while (1){}
    if ((floatMbox = create_mailbox(15, sizeof(float))) == NULL)
        while (1){}
    if (create_task(test_task_sending, low_deadline*4) <= FAIL)
        while (1){}
    if (create_task(test_task_recieving, high_deadline) <= FAIL)
        while (1){}
    
    //100000    high_deadline
    //1000      low_deadline
    run();
}

void test_task1()
{
    dl1++;
    if ((create_task(test_task1, dl1 * very_low_deadline)) <= FAIL)
        terminate();
    wait(very_low_deadline);
    if (TimerList->pHead == NULL)
        create_task(test_task2, dl1 * very_low_deadline);
    terminate();
    return;
}
void test_task2()
{
    create_task(test_task2, dl1 * very_low_deadline);
    if (wait(very_low_deadline * dl1 * 2) != DEADLINE_REACHED)
        test2_status = FAIL;

    terminate();
    return;
}
void test_task3()
{
    update_meminfo();
    for (int i = 0; i < 100; i++)
    {
        if (create_task(test_task4, 10) <= FAIL)
            while (1){}
    }
    update_meminfo();
    terminate();
}
void test_task4()
{
    int i = 10;
    i++;
    update_meminfo();
    n_tasks_terminated++;
    terminate();
}
void test_task_sending()
{
    int val;
    n_mess++;
    val = n_mess;
    if(n_mess<intMbox->nMaxMessages)
    {   if((create_task(test_task_sending,  low_deadline*2-n_mess))<=FAIL)
        {
            __ISR_OFF();
                while (1){}
        }
    }
 
    if ((send_wait(intMbox, (void *)(&val))) <= FAIL) //Failing currently at n_mess = 15
    {
        __ISR_OFF();
        while (1)
        {
        }
    }
    terminate();
}

void test_task_recieving()
{
    int val;
    if ((receive_wait(intMbox, (void *)(&val))) <= FAIL)
    {
        __ISR_OFF();
        while (1)
        {
        }
    }
    n_mess--;
    if(n_mess>0)
        if((create_task(test_task_recieving,  high_deadline-n_mess))<=FAIL)
        {
            __ISR_OFF();
            while (1){}
        }
    if(ReadyList->pHead->pNext->pNext==NULL)
        while(1){/*is good*/}
    terminate();
}

