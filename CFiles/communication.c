
/**
* @file
*     communication.c
* @authors 
*     Elias Vahlberg
*     Hamed Haghjo
* \brief 
*     ...
*/

msg *mailbox_dequeue(mailbox *mBox);

exception mailbox_enqueue(mailbox *mBox, msg *mes);

mailbox *create_mailbox(uint nMessages, uint nDataSize)
{
    mailbox *mbox;
    //msg *mes;
    mbox = (mailbox *)mem_alloc(sizeof(mailbox));
    if (mbox == NULL)
    {
        task_exception_manager(NULLPOINTER);
        return NULL;
    }
    mbox->nDataSize = nDataSize;
    mbox->nMaxMessages = nMessages;
    mbox->nMessages = 0;
    return mbox;
}

exception append_msg(msg *mes, mailbox *mBox, void *pData, int status, int wait)
{
    if (mBox == NULL || pData == NULL)
        return task_exception_manager(NULLPOINTER);
    if (status == RECEIVER)
        mes = mem_alloc(sizeof(msg));
    else
        mes = mem_alloc(sizeof(msg) + mBox->nDataSize);
    if (mes == NULL)
        task_exception_manager(ALLOCFAIL);
    if (status == RECEIVER)
        mes->pData = pData;
    else
    {
        mes->pData = (void *)(mes + sizeof(msg));
        mem_copy(pData, mes->pData, mBox->nDataSize);
    }
    mes->Status = status;

    if (wait)
    {
        if (ReadyList != NULL)
        {
            if (ReadyList->pHead != NULL)
            {
                add_msg_to_listobject(ReadyList->pHead,mes);
            }
        }
    }
    else
        mes->pBlock = NULL;

    return task_exception_manager(mailbox_enqueue(mBox, mes));
}

exception force_remove_mailbox(mailbox *mBox)
{
    if (mBox == NULL)
        return task_exception_manager(FAIL);
    if (mBox->nMessages == 0)
    {
        mem_free(mBox);
        return OK;
    }
    msg *temp = mBox->pHead;
    while (temp != NULL)
    {
        if (temp->pNext == NULL)
        {
            mem_free(temp);
            mem_free(mBox);
            return task_exception_manager(FAIL);
        }
        temp = temp->pNext;
        mem_free(temp->pPrevious);
    }
    mem_free(mBox);
    return task_exception_manager(FAIL);
}

exception remove_mailbox(mailbox *mBox)
{
    if (mBox == NULL)
        return task_exception_manager(FAIL);
    if (mBox->nMessages == 0)
    {
        mem_free(mBox);
        return OK;
    }
    return task_exception_manager(NOT_EMPTY);
}

exception send_wait(mailbox *mBox, void *pData)
{
    if (mBox == NULL || pData == NULL)
        return task_exception_manager(NULLPOINTER);
    isr_off();
    int msg_status = 0;
    msg_status = (mBox->pHead == NULL) ? 0 : (mBox->pHead->Status == RECEIVER) ? 1
                                                                               : 0;
    if (msg_status)
    {
        exception exc1 = mem_copy(mBox->pHead->pData, (char *)pData, mBox->nDataSize);
        if (exc1 <= FAIL)
        {
            isr_on();
            return task_exception_manager(exc1);
        }
        msg *mes = mailbox_dequeue(mBox);

                                                                //Temporarily
        PreviousTask = ReadyList->pHead->pTask;                 //Inlagt nyss
        //PreviousTask = NextTask;                              //Update PreviousTask, the previous task is the task sending the message
        move_listobj(WaitingList, ReadyList, mes->pBlock);      //Insert recieving task in ReadyList
        NextTask = ReadyList->pHead->pTask;                     //Inlagt nyss
        //NextTask = mes->pBlock->pTask;                        //Update NextTask
    }
    else
    {
        msg *mes = NULL;
        exception status = append_msg(mes, mBox, pData, SENDER, 1);
        //Allocate a msg
        if (status != OK)
        {
            isr_on();
            return task_exception_manager(status);
        }
        //Update PreviousTask

        PreviousTask = ReadyList->pHead->pTask;
        //Remove sending_task from ReadyList and insert it to WaitingList
        move_listobj(ReadyList, WaitingList, ReadyList->pHead);
        //Update NextTask
        NextTask = ReadyList->pHead->pTask;
    }
    isr_on();
    SwitchContext();
    if (deadline_reached(NextTask))
    {
        isr_off();
        mailbox_dequeue(mBox); //Removing sending_msg Also returns the message, but it is not essential for this part
        isr_on();
        return task_exception_manager(DEADLINE_REACHED);
    }
    return OK;
}

exception receive_wait(mailbox *mBox, void *pData)
{
    if (mBox == NULL || pData == NULL)
        return task_exception_manager(NULLPOINTER);
    isr_off(); //Disable interrupt
    exception exc1 = (mBox->pHead == NULL) ? 0 : (mBox->pHead->Status == SENDER) ? 1
                                                                                 : 0;

    if (exc1) //IF send Message is waiting
    {

        exception exc2 = mem_copy(mBox->pHead->pData, (char *)pData, mBox->nDataSize); //Copy sender’s data to receiving task’s data area
        msg *mes = mailbox_dequeue(mBox);                                              //Remove sending task’s Message struct from the Mailbox
        if (mes->pBlock != NULL)                                                       //IF Message was of wait type
        {
            //Update PreviousTask
            PreviousTask = NextTask;
            if (move_listobj(WaitingList, ReadyList, mes->pBlock) <= FAIL) //Move sending task to ReadyList
            {
                isr_on();
                return task_exception_manager(FAIL);
            }
            //Update NextTask
            NextTask = ReadyList->pHead->pTask;
        }
        else
        {
            mem_free(mes);
        }
    }
    else
    {
        msg *mes = NULL;
        exception status = append_msg(mes, mBox, pData, RECEIVER, 1);
        if (status != OK)
        {
            isr_on();
            return task_exception_manager(status);
        }
        PreviousTask = NextTask;                                //Update PreviousTask
        move_listobj(ReadyList, WaitingList, ReadyList->pHead); //Move receiving task from ReadyList to WaitingList Update NextTask
        NextTask = ReadyList->pHead->pTask;
    }
    SwitchContext(); //Switch context
    if (deadline_reached(NextTask))
    {
        isr_off();
        mailbox_dequeue(mBox); //Remove receive Message
        isr_on();
        return task_exception_manager(DEADLINE_REACHED);
    }
    else
        return OK;
}

exception send_no_wait(mailbox *mBox, void *pData)
{
    if (mBox == NULL || pData == NULL)
        return task_exception_manager(NULLPOINTER);
    exception status = OK;
    isr_off();
    int msg_status = 0;
    msg_status = (mBox->pHead == NULL) ? 0 : (mBox->pHead->Status == RECEIVER) ? 1
                                                                               : 0;
    if (msg_status)
    {
        status = mem_copy(mBox->pHead->pData, (char *)pData, mBox->nDataSize); //Copy data to receiving tasks’ data area.
        if (status <= FAIL)
        {
            isr_on();
            return task_exception_manager(status);
        }
        msg *mes = mailbox_dequeue(mBox);                  //Remove receiving task’s Message struct from the Mailbox
        PreviousTask = ReadyList->pHead->pTask;            //Update PreviousTask
        move_listobj(ReadyList, WaitingList, mes->pBlock); //Move receiving task to Readylist
        NextTask = ReadyList->pHead->pTask;                //Update NextTask
        isr_on();
        SwitchContext(); //Switch context
    }
    else
    {
        if (mBox->nMaxMessages <= mBox->nMessages)
            mailbox_dequeue(mBox); //Remove the oldest Message struct
        msg *newMes = NULL;
        append_msg(newMes, mBox, pData, SENDER, 0);
        isr_on();
    }
    return OK;
}

exception receive_no_wait(mailbox *mBox, void *pData)
{
    if (mBox == NULL || pData == NULL)
        return task_exception_manager(NULLPOINTER);
    exception status = OK;
    isr_off();
    int msg_status = 0;
    msg_status = (mBox->pHead == NULL) ? 0 : (mBox->pHead->Status == SENDER) ? 1
                                                                             : 0;
    if (msg_status)
    {
        status = mem_copy(mBox->pHead->pData, (char *)pData, mBox->nDataSize); //Copy data to receiving tasks’ data area.
        if (status <= FAIL)
        {
            isr_on();
            return task_exception_manager(status);
        }
        msg *mes = mailbox_dequeue(mBox); //Remove receiving task’s Message struct from the Mailbox
        if (mes->pBlock != NULL)
        {
            mem_free(mes);
            PreviousTask = ReadyList->pHead->pTask;            //Update PreviousTask
            move_listobj(ReadyList, WaitingList, mes->pBlock); //Move receiving task to Readylist
            NextTask = ReadyList->pHead->pTask;                //Update NextTask
            SwitchContext();
        }
        else
        {
            mem_free(mes);
        }
        isr_on();
        return OK;
    }
    isr_on();
    return task_exception_manager(FAIL);
    // Return status on received Message
}

exception no_messages(mailbox *mBox)
{
    return (mBox != NULL && (mBox->nMessages == 0 && mBox->pHead == NULL)) ? 1 : 0;
}

uint get_mailbox_size(mailbox *mBox)
{
    return (mBox != NULL && mBox->nMessages >= 0) ? mBox->nMessages : 0;
}

exception is_mailbox_full(mailbox *mBox)
{
    return (mBox != NULL && mBox->nMessages == mBox->nMaxMessages) ? 1 : 0;
}

msg *get_oldest_mes(mailbox *mBox)
{
    return (mBox != NULL && mBox->pTail != NULL) ? mBox->pTail : NULL;
}

msg *get_latest_mes(mailbox *mBox)
{
    return (mBox != NULL && mBox->pHead != NULL) ? mBox->pHead : NULL;
}

exception mailbox_enqueue(mailbox *mBox, msg *mes)
{
    if (mBox == NULL || mes == NULL)
        return task_exception_manager(NULLPOINTER);
    if (mBox->pTail == NULL)
    {
        mBox->pHead = mes;
        mBox->pTail = mes;
    }
    else
    {
        mBox->pTail->pNext = mes;
        mes->pPrevious = mBox->pTail;
        mBox->pTail = mes;
    }
    mBox->nMessages++;
    return OK;
}

msg *mailbox_dequeue(mailbox *mBox)
{
    if (mBox == NULL)
    {
        task_exception_manager(NULLPOINTER);
        return NULL;
    }
    if (mBox->pHead == NULL)
    {
        task_exception_manager(NULLPOINTER);
        return NULL;
    }
    //if(mBox->pHead->pBlock!=NULL)
      //  remove_msg_from_listobject(mBox->pHead->pBlock,mBox->pHead);
    msg *mes = mBox->pHead;
    if (mBox->pHead == mBox->pTail)
    {
        mBox->pHead = NULL;
        mBox->pTail = NULL;
        mBox->nMessages--;
        return mes;
    }
    mBox->pHead = mBox->pHead->pNext;
    mBox->pHead->pPrevious = NULL;
    mBox->nMessages--;
    return mes;
}

exception add_msg_to_listobject(listobj* l_obj,msg* mes)
{
    if(l_obj == NULL||mes==NULL)
        return task_exception_manager(NULLPOINTER);
    if(l_obj->pMessage==NULL)
        l_obj->pMessage = mes;
    else
    {
        msg* t_mes = l_obj->pMessage;
        while (t_mes->pNext!=NULL)
            t_mes = t_mes->pNext;
        t_mes->pNext = mes;
        mes->pPrevious = t_mes;
    }
    mes->pBlock = l_obj;
    return OK;
}

exception remove_msg_from_listobject(listobj* l_obj, msg* mes)
{
    if(l_obj == NULL||mes==NULL)
        return task_exception_manager(NULLPOINTER);
    if(l_obj->pMessage==NULL)
        return task_exception_manager(FAIL);
    if(l_obj->pMessage==mes)
        l_obj->pMessage = l_obj->pMessage->pNext;
    else
    {
        msg* t_mes = l_obj->pMessage;
        while (t_mes->pNext!=mes)
            t_mes = t_mes->pNext;
        t_mes->pNext = mes->pNext;
        if(mes->pNext!=NULL)
        mes->pNext->pPrevious=t_mes;
    }
    return OK;   
}