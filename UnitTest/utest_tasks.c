/**
* @file
*     utest_tasks.c
* @authors 
*     Elias Vahlberg
*     Hamed Haghjo
* \brief 
*     ...
*/

#pragma region functions    
test_info* utest_add_task_2_list    (test_info* t_info          );    //(list *l, TCB *task);
test_info* utest_remove_task        (test_info* t_info          );    //DEPRICATED //(list *l, TCB *task);
test_info* utest_remove_listobj     (test_info* t_info          );    //DEPRICATED //(list *l, listobj *o1);
test_info* utest_move_listobj       (test_info* t_info          );    //(list *src, list *dest, listobj* o1);
test_info* utest_compare_listobj    (test_info* t_info          );    //(listobj *o1, listobj *o2);
test_info* utest_create_listobj     (test_info* t_info          );    //(TCB *task,uint nTCnt);
test_info* utest_create_task        (test_info* t_info          );    //(void (*tbody)(), uint dl)
test_info* utest_remove_last        (test_info* t_info          );    //(list *list);
test_info* utest_push               (test_info* t_info          );    //(list *l, TCB *task,uint nTCnt);
test_info* utest_pop                (test_info* t_info          );    //(list* list);
test_info* utest_find_task          (test_info* t_info          );    //(list* l, TCB *task);
list*      before_test_build_list   (int n, int* dl,TCB** tasks );
listobj*   before_test_build_listobj(int nTCnt, TCB* task       );
TCB*       before_test_build_task   (int dl                     );
#pragma endregion functions
test_info* utest_task_main()
{
    test_info* t_info;
    
    if((t_info = pre_utest(t_info,1604,(void*)utest_add_task_2_list,TASK_ADMINISTRATION,17))==NULL)
        while(1){}
    utest_add_task_2_list(t_info);
    char res1[17];
    for (int i = 0; i < 17; i++)
        res1[i] = t_info->test_s[i];
    
    
    if((t_info = pre_utest(t_info,1605,(void*)utest_move_listobj,TASK_ADMINISTRATION,17))==NULL)
        while(1){}
    utest_move_listobj(t_info);
    utest_compare_listobj(t_info);
    utest_create_listobj(t_info);
    utest_create_task(t_info);
    utest_remove_last(t_info);
    utest_push(t_info);
    utest_pop(t_info);
    utest_find_task(t_info);
}    

list* before_test_build_list(int n, int* dl,TCB** tasks)
{
    list* l = mem_alloc(sizeof(list));
    if(!l)
        return NULL;
    if(n == 0)
        return l;
    l->pHead = mem_alloc(sizeof(listobj));
    if(!l->pHead)
        return NULL;
    if(n == 1)
    {
        if(tasks == NULL)
        {
            TCB* task = mem_alloc(sizeof(TCB));
            if(!task)
                return NULL;
            if(dl)
                task->Deadline = dl[0];
            l->pHead->pTask = l->pTail->pTask = task;
        }
        else 
            l->pHead->pTask = l->pTail->pTask = tasks[0];
        return l;
    }
    else
    {
        TCB* task;
        listobj* lobj;
        l->pTail = l->pHead;
        if(tasks)
            l->pTail->pTask = l->pHead->pTask = tasks[0];
        else
            l->pTail->pTask  = mem_alloc(sizeof(TCB));
        if(dl)
            l->pHead->pTask->Deadline = dl[0];
        else
            l->pHead->pTask->Deadline = UINT_MAX;
        for (int i = 1; i < n; i++)
        {
            lobj = mem_alloc(sizeof(listobj));
            if(!lobj)
                return NULL;
            l->pTail->pNext = lobj;
            lobj->pPrevious = l->pTail;
            l->pTail = lobj;
            if(tasks)
                l->pTail->pTask = tasks[i];
            else
                l->pTail->pTask = mem_alloc(sizeof(TCB));
            if(!l->pTail->pTask)
                return NULL;
            if(dl)
                l->pTail->pTask->Deadline = dl[i];
            else
                l->pTail->pTask->Deadline = UINT_MAX;
        }
        return l;
    }


}

listobj* before_test_build_listobj(int nTCnt, TCB* task)
{
    listobj* lobj = mem_alloc(sizeof(listobj));
    if(!lobj)
        return NULL;
    lobj->nTCnt = nTCnt;
    lobj->pTask = (task)? task : mem_alloc(sizeof(TCB));
}

TCB* before_test_build_task(int dl)
{
    TCB* task = (TCB*) mem_alloc(sizeof(TCB));
    if(task)
        task->Deadline = dl;
    return task;
}
/*
*utest_add_task_2_list
*ID-1604
*\brief
*/
test_info* utest_add_task_2_list(test_info* t_info)
{
    int k = 0;
 
    // #BEFORE TEST Create 3 Lists with 0,1,3 elements  
    int dl[] = {30,20,10,0};
    TCB** tasks = mem_alloc(16);
    for (int i = 0; i < 4; i++)
        if((tasks[i]    =before_test_build_task(dl[i]))==NULL)
            {
                for (int j = 0; j < i; j++)
                    mem_free(tasks[j]);

                t_info->test_s[k++] = UNEXPECTED_ERR;
                return t_info;
            }
    list* l0 = NULL;
    list* l1;
    list* l2;
    list* l3;
    if((l1  = before_test_build_list(0,NULL,NULL))  ==  NULL)
    {
        t_info->test_s[k++]= UNEXPECTED_ERR;
        return t_info;
    }
    if((l2  = before_test_build_list(1,dl,NULL))    ==  NULL)
    {
        t_info->test_s[k++]= UNEXPECTED_ERR;
        return t_info;
    }
    if((l3  = before_test_build_list(3,NULL,NULL))    ==  NULL)
    {
        t_info->test_s[k++]= UNEXPECTED_ERR;
        return t_info;
    }
        //ADD to NULL list
    assert_fail(add_task_2_list(l0,tasks[0]),&t_info->test_s[k++]);                                      

        //Add to list with 0 elements
    assert_ok(add_task_2_list(l1,tasks[0]),&t_info->test_s[k++]);                                        //Add to empty list
    assert_equals((void*)l1->pHead->pTask,(void*)tasks[0],(int)sizeof(TCB),&t_info->test_s[k++]);        //check if there
    
        //Add to list with 1 element  
    assert_ok(add_task_2_list(l2,tasks[0]),&t_info->test_s[k++]);                                        
    assert_ok(add_task_2_list(l2,tasks[1]),&t_info->test_s[k++]);                                        
    assert_ok((exception)(l2->pHead->pTask!=NULL && l2->pHead->pNext->pTask!=NULL),&t_info->test_s[k++]); //Check if both elements are there
    assert_equals((void*)l2->pHead->pTask,(void*)tasks[1],(int)sizeof(TCB),&t_info->test_s[k++]);        //check if the order is correct
    assert_equals((void*)l2->pHead->pNext->pTask,(void*)tasks[0],(int)sizeof(TCB),&t_info->test_s[k++]); 
    
            //Add to list with 1 element  
    assert_ok(add_task_2_list(l3,tasks[0]),&t_info->test_s[k++]);                                        
    assert_ok(add_task_2_list(l3,tasks[2]),&t_info->test_s[k++]);                                        
    assert_ok(add_task_2_list(l3,tasks[1]),&t_info->test_s[k++]);                                        
    assert_ok(add_task_2_list(l3,tasks[3]),&t_info->test_s[k++]);                                        
    assert_ok((exception)(l3->pHead->pTask!=NULL && l3->pHead->pNext->pTask!=NULL &&l3->pHead->pNext->pNext!=NULL&&l3->pHead->pNext->pNext->pNext!=NULL)
    ,&t_info->test_s[k++]); //Check if all elements are there
    assert_equals((void*)l3->pHead->pTask,(void*)tasks[3],(int)sizeof(TCB),&t_info->test_s[k++]);        //check if the order is correct
    assert_equals((void*)l3->pHead->pNext->pTask,(void*)tasks[2],(int)sizeof(TCB),&t_info->test_s[k++]); 
    assert_equals((void*)l3->pHead->pNext->pNext->pTask,(void*)tasks[1],(int)sizeof(TCB),&t_info->test_s[k++]); 
    assert_equals((void*)l3->pHead->pNext->pNext->pNext->pTask,(void*)tasks[0],(int)sizeof(TCB),&t_info->test_s[k++]); 
    //Add to list with multiple elements
    //    check if there
    //    check position
    return t_info;
}
/*
*
*\brief
*/
test_info* utest_remove_task(test_info* t_info) //DEPRICATED
{
    return NULL;
}
/*
*
*\brief
*/
test_info* utest_remove_listobj(test_info* t_info)//DEPRICATED
{
    return NULL;
}
/*
*ID-1605
*\brief
*/
test_info* utest_move_listobj(test_info* t_info)
{
    int k = 0;

    //#BEFORE TEST Create 6 Lists with {0,0},{0,1},{1,0},{1,1},{3,1},{1,3},{3,3} elements  
    list* l0 = NULL;
    list* l1;
    list* l2;
    list* l3;
    list* l4;
    list* l5;
    list* l6;
    TCB** tasks = mem_alloc(16);
    for (int i = 0; i < 4; i++)
        if((tasks[i] = before_test_build_task(50-10*i))==NULL)
        {
            for (int j = 0; j < i; j++)
                mem_free(tasks[i]);
            t_info->test_s[k++] = UNEXPECTED_ERR;
        }
    
    l1 = before_test_build_list(0,NULL,NULL);
    l1 = before_test_build_list(1,NULL,NULL);
    l2 = before_test_build_list(3,NULL,NULL);
    l3 = before_test_build_list(3,NULL,tasks);
    l4 = before_test_build_list(4,NULL,tasks);
    assert_fail(move_listobj(l0,l1,l6->pHead),t_info->test_s);
    assert_fail(move_listobj(l0,l2,l6->pHead),t_info->test_s);
    assert_fail(move_listobj(l1,l0,l1->pHead),t_info->test_s);
    assert_fail(move_listobj(l2,l0,l2->pHead),t_info->test_s);

    assert_fail(move_listobj(l1,l2,l6->pHead),t_info->test_s);
    assert_ok(move_listobj(l1,l2,l1->pHead),t_info->test_s);
    //      Move using all the lists above 
    //          check for NULLPOINTER,
    //          check for FAIL
    //          check that the element is in dest
    //          check that the element is not in src
    //      Try to move object not in src list 
    //          check for FAIL
    //      Try to move object in dest
    //          check for FAIL
    //      Move 100 time back and forth 
    //      check for FAIL
    //      check memory

    return NULL;
}
/*
*
*\brief
*/
test_info* utest_compare_listobj(test_info* t_info)
{
    //#BEFORE TEST create 4 list objects: 1: {NULL},2: { DL = 1, nTCnt = 0},3: { DL = 2, nTCnt = 0},4: { DL = 3 nTCnt = 1},5: { DL = 3 nTCnt = 2} 
    //        compare: (1,2)
    //            Assert return = -1
    //        compare: (1,3)
    //            Assert return = -1
    //        compare: (1,4)
    //            Assert return = -1
    //        compare: (1,5)
    //            Assert return = -1
    //        compare: (2,3)
    //            Assert return = 1
    //        compare: (2,4)
    //            Assert return = 1
    //        compare: (2,5)
    //            Assert return = 1
    //        compare: (3,4)
    //            Assert return = 0
    //        compare: (3,5)
    //            Assert return = 1
    //        compare: (4,5)
    //            Assert return = 1
    return NULL;
}
/*
*
*\brief
*/
test_info* utest_create_listobj(test_info* t_info)
{
    return NULL;
}
/*
*
*\brief
*/
test_info* utest_create_task(test_info* t_info)
{
    return NULL;
}
/*
*
*\brief
*/
test_info* utest_remove_last(test_info* t_info)
{
    return NULL;
}
/*
*
*\brief
*/
test_info* utest_push(test_info* t_info)
{
    return NULL;
}
/*
*
*\brief
*/
test_info* utest_pop(test_info* t_info)
{
    return NULL;
}
/*
*
*\brief
*/
test_info* utest_find_task(test_info* t_info)
{
    return NULL;
}