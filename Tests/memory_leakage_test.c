
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
#define NUM_O_TCB                   200
#define NUM_O_MESSAGES               50
#define NUM_O_LISTS                 200
#define MBOX_TEST_RECURSIONS        100
#define MESSAGE_TEST_RECURSIONS     100

void mem_leak_mailbox_test(int recursion_number);
void mem_leak_TCB_test();
void mem_leak_listobj(); //DEPRECATED
void mem_leak_message_test(int recursion_number);

    struct mallinfo meminfo_test1;
    struct mallinfo meminfo_test2;
    struct mallinfo meminfo_test3;


void mem_leak_test_main()
{
    mem_leak_mailbox_test(0);

    update_meminfo();
    meminfo = meminfo_test1;

    mem_leak_TCB_test();
    
    update_meminfo();
    meminfo = meminfo_test2;
    mem_leak_message_test(0);
    
    update_meminfo();
    meminfo = meminfo_test3;
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
        if(remove_mailbox(mb[i])==NOT_EMPTY)
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
        for (int i = 0; i < NUM_O_TCB; i++)
        {

            if (add_task_2_list(test, (create_task(mem_leak_TCB_test, NUM_O_TCB-i)>FAIL)?ReadyList->pHead->pTask:NULL) == FAIL)
                    while (1) {}    
            if (pop(test)==FAIL )
                    while (1) {}
            ReadyList->pHead->pTask=NULL; //To avoid error that can't 
            if(pop(ReadyList)==FAIL)
                while(1){/*Failed to remove*/}
        }
    else
        while(1){/*Allocation failure*/}
    
    if(ReadyList->pHead)
        while(1){/*All objects were not deleted*/}
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
    update_meminfo();
    mailbox* mbox1 = create_mailbox(NUM_O_MESSAGES+1,4);
    if(mbox1==NULL)
        while(1){}
    void* mem1 = (void*) mem_alloc(4*NUM_O_MESSAGES);
    msg** messages  = (msg**)mem1;
    if(messages==NULL)
        while(1){}
    void** data    = mem_alloc(4*NUM_O_MESSAGES);
    if(data==NULL)
        while(1){}
    for (size_t i = 0; i < NUM_O_MESSAGES; i++)
        if((data[i] = mem_alloc(4))==NULL)
            while(1){/*failed to allocate space for data*/}

    for (int i = 0; i < NUM_O_MESSAGES; i++)
        if((append_msg(messages[i],mbox1,(messages=&(messages[i])),1,100))<=FAIL)
            while(1){/*failed to create message*/}

    for (int i = 0; i < NUM_O_MESSAGES; i++)
    {
        msg* mes = mailbox_dequeue(mbox1);
        if(mes==NULL)
            while(1){/*failed to remove message*/}
        mem_free(mes->pData);
        mem_free(mes);
    }
    update_meminfo();
    mem_free((void*)messages);
    update_meminfo();
    mem_free((void*)data);
    update_meminfo();
    if((remove_mailbox(mbox1))==NOT_EMPTY)
        while(1){/*OHshit.gif*/}
    update_meminfo();
    if(recursion_number<MESSAGE_TEST_RECURSIONS)
        mem_leak_message_test(recursion_number+1);
    return;
}

