/**
* @file
*     utest_communication.c
* @authors 
*     Elias Vahlberg
*     Hamed Haghjo
* \brief 
*     ...
*/
#pragma region functions
void         utest_communication_before      (test_info* t_info);
test_info *  utest_mailbox_dequeue           (test_info* t_info);
test_info *  utest_mailbox_enqueue           (test_info* t_info);
test_info *  utest_create_mailbox            (test_info* t_info);
test_info *  utest_append_msg                (test_info* t_info);
test_info *  utest_remove_mailbox            (test_info* t_info);
test_info *  utest_send_wait                 (test_info* t_info);
test_info *  utest_receive_wait              (test_info* t_info);
test_info *  utest_send_no_wait              (test_info* t_info);
test_info *  utest_receive_no_wait           (test_info* t_info);
test_info *  utest_no_messages               (test_info* t_info);
#pragma endregion functions


mailbox * utest_mailbox;
msg * utest_mes;
int testvar = 0;

/*
*ID-1200
*\brief
*/
void utest_communication_before(test_info* t_info)
{
    //PRE-TEST
    t_info->test_id = 1200;
    t_info->test_adress = utest_communication_before;
    t_info->package = MAILBOX_SYSTEM;
    t_info->num_assert = 0;
    //END PRE-TEST

    utest_mailbox = create_mailbox(3, sizeof(int));
    if (utest_mailbox == NULL)
        while(1)
        {}
    
}

/*
*ID-1201
*\brief
*/
test_info* utest_mailbox_dequeue(test_info* t_info)
{
    //Test specifications
    t_info->test_id = 1201;
    t_info->test_adress = utest_mailbox_dequeue;

    exception dequeue_status = FAIL;

    dequeue_status = (mailbox_dequeue(utest_mailbox) != NULL) ? OK : FAIL;
    if (assert_fail(dequeue_status, t_info->test_s) == OK)
    {
        if (!assert_ok(append_msg(utest_mes, utest_mailbox, testvar, 1, 100), t_info->test_s))
            while (1)
            {}
    } else
    while(1)
    {}
    
    return t_info->test_s;
}

/*
*ID-1202
*\brief
*/
test_info* utest_mailbox_enqueue(test_info* t_info)
{
    t_info->test_id = 1202;
    t_info->test_adress = utest_mailbox_enqueue;

    return NULL;
}

/*
*ID-1203
*\brief
*/
test_info* utest_create_mailbox(test_info* t_info)
{
    t_info->test_id = 1203;
    t_info->test_adress = utest_create_mailbox;
    
    return NULL;
}

/*
*ID-1204
*\brief
*/
test_info* utest_append_msg(test_info* t_info)
{
    t_info->test_id = 1204;
    t_info->test_adress = utest_append_msg;

    return NULL;
}

/*
*ID-1205
*\brief
*/
test_info* utest_remove_mailbox(test_info* t_info)
{
    t_info->test_id = 1205;
    t_info->test_adress = utest_remove_mailbox;

    return NULL;
}

/*
*ID-1206
*\brief
*/
test_info* utest_send_wait(test_info* t_info)
{
    t_info->test_id = 1206;
    t_info->test_adress = utest_send_wait;

    return NULL;
}

/*
*ID-1207
*\brief
*/
test_info* utest_receive_wait(test_info* t_info)
{
    t_info->test_id = 1207;
    t_info->test_adress = utest_receive_wait;

    return NULL;
}

/*
*ID-1208
*\brief
*/
test_info* utest_send_no_wait(test_info* t_info)
{
    t_info->test_id = 1208;
    t_info->test_adress = utest_send_no_wait;

    return NULL;
}

/*
*ID-1209
*\brief
*/
test_info* utest_receive_no_wait(test_info* t_info)
{
    t_info->test_id = 1209;
    t_info->test_adress = utest_receive_no_wait;

    return NULL;
}

/*
*ID-1210
*\brief
*/
test_info* utest_no_messages(test_info* t_info)
{
    t_info->test_id = 1210;
    t_info->test_adress = utest_no_messages;

    return NULL;
}