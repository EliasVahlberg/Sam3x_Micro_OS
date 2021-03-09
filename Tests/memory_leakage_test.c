
/**
* @file
*     memory_leakage_test.c
* @authors 
*     Elias Vahlberg
*     Hamed Haghjo
* \brief 
*     ...
*/
#define NUM_O_MBOXES                200
#define NUM_O_MESSAGES               50
#define NUM_O_LISTS                 200
#define MBOX_TEST_RECURSIONS        100
#define MESSAGE_TEST_RECURSIONS     100

void mem_leak_mailbox_test(int recursion_number);
void mem_leak_TCB_test();
void mem_leak_listobj(); //DEPRECATED
void mem_leak_message_test(int recursion_number);

void mem_leak_test_main()
{
    mem_leak_mailbox_test(0);
    mem_leak_TCB_test();
    mem_leak_message_test(0);
}

void mem_leak_mailbox_test(int recursion_number)
{
    mailbox** mb;
    if((mb = mem_alloc(4*NUM_O_MBOXES))==NULL)
        while(1){/*Bigfail.jpeg*/}
    
    for (int i = 0; i < NUM_O_MBOXES; i++)
        if((mb[i] = create_mailbox(10,4))==NULL)
            while(1){/*Failed to create mailbox*/}
    for (int i = 0; i < NUM_O_MBOXES; i++)
        if(remove_mailbox(mb[i]))
            while(1){/*Failed to remove mailbox*/}
    mem_free(mb);
    if(recursion_number<MBOX_TEST_RECURSIONS)
        mem_leak_mailbox_test(recursion_number+1);
    return;
    
}

void mem_leak_TCB_test()
{
    if(ReadyList)
        while(1){/*ohno.png*/}
    list* test = mem_alloc(sizeof(list));
    ReadyList = mem_alloc(sizeof(list));
    if (ReadyList != NULL &&test !=NULL)
        for (int i = 0; i < NUM_O_MBOXES; i++)
            if (add_task_2_list(test, (create_task(mem_leak_TCB_test, i)<=FAIL)?test->pHead->pTask:NULL) == FAIL)
                if (!pop(test) || pop(ReadyList))
                    while (1) {}
    else
        while(1){/*Allocation failure*/}
    
    if(ReadyList->pHead)
        while(1){/*all objects where not deleted*/}
}

void mem_leak_listobj()
{
    // for (int i = 0; i < NUM_O_MBOXES*200; i++)
    // {
    //     listobj * lobj = create_listobj(TCB * task, uint nTCnt)
    // }
}

void mem_leak_message_test(int recursion_number)
{
    mailbox* mbox1 = create_mailbox(NUM_O_MESSAGES+1,4);
    msg** messages = mem_alloc(4*NUM_O_MESSAGES);
    void** data    = mem_alloc(4*NUM_O_MESSAGES);
    for (size_t i = 0; i < NUM_O_MESSAGES; i++)
        if((data[i] = mem_alloc(4))==NULL)
            while(1){/*failed to allocate space for data*/}

    for (int i = 0; i < NUM_O_MESSAGES; i++)
        if(append_msg(messages[i],mbox1,(messages=&(messages[i])),1,100)<=FAIL)
            while(1){/*failed to create message*/}

    for (int i = 0; i < NUM_O_MESSAGES; i++)
    {
        msg* mes = mailbox_dequeue(mbox1);
        if(mes==NULL)
            while(1){/*failed to remove message*/}
        mem_free(mes->pData);
        mem_free(mes);
    }
    if(recursion_number<MESSAGE_TEST_RECURSIONS)
        mem_leak_message_test(recursion_number+1);
    return;
}

