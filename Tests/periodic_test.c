/**
* @file
*     periodic_test.c
* @authors 
*     Elias Vahlberg
*     Hamed Haghjo
* \brief 
*     ...
*/

#pragma region Functions
void periodic_task_main();
void periodic_func();
void periodic_task_test1();
void periodic_task_test2();
void periodic_task_test3();
void p_rec();
void p_send();
#pragma endregion Functions

int pCounter        = 0;

int pSendCount      = 0;
int pRecCount       = 0;

int pSendOKCount    = 0;
int pRecOKCount     = 0;
int pSendVal        = 2;
int pRetVal         = 0;

void periodic_task_test1()
{
    periodically_run(periodic_func,50,0);

    create_task(periodic_task_test2,high_deadline+10);
    create_task(periodic_task_test3,high_deadline);
    terminate();
    return;
}
void periodic_task_test2()
{
    wait(5);
    periodically_run(p_rec,50,100);
    terminate();
    return;
}
void periodic_task_test3()
{
    periodically_run(p_send,50,100);
    terminate();
    return;
}
void periodic_func()
{
    pCounter++;
    return;
}

void p_rec()
{
    pRecCount++;
    if(receive_no_wait(intMbox,&pRetVal)==OK)
        pRecOKCount++;
    else
    {
        __ISR_OFF();
        while (1){}
    }
    return;
}
void p_send()
{
    pSendCount++;
    if(send_no_wait(intMbox,&pSendVal)==OK)
        pSendOKCount++;
    else
    {
        __ISR_OFF();
        while (1){}
    }
    return;
}


void periodic_task_main()
{
    pre_functional_test();
    create_task(periodic_task_test1,low_deadline*5);
    if ((intMbox = create_mailbox(5, sizeof(int))) == NULL)
        while (1){}
    run();
    return;
}

