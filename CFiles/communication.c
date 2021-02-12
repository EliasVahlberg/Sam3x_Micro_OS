
msg*        mailbox_dequeue(mailbox* mBox);
exception   mailbox_enqueue(mailbox* mBox, msg* mes);

mailbox* create_mailbox( uint nMessages, uint nDataSize )
{
    mailbox *mbox;
    //msg *mes;
    mbox = (mailbox*) mem_alloc(sizeof(mailbox));
    if(mbox ==NULL)
        return NULL;
    mbox->nDataSize = nDataSize;
    mbox->nMaxMessages = nMessages;
    mbox->nMessages = 0;
    return mbox;
}

exception force_remove_mailbox(mailbox* mBox)
{
    if (mBox == NULL)
        return FAIL;
    if(mBox->nMessages == 0)
    {
        mem_free(mBox);
        return OK;
    }
    msg* temp = mBox->pHead;
    while (temp != NULL)
    {
        if(temp->pNext ==NULL)
        {
            mem_free(temp);
            mem_free(mBox);
            return MESSAGE_LOST;
        }
        temp = temp->pNext;
        mem_free(temp->pPrevious);
    }
    mem_free(mBox);
    return FAIL;
}

exception remove_mailbox(mailbox* mBox)
{
    if (mBox == NULL)
        return FAIL;
    if(mBox->nMessages == 0)
    {
        mem_free(mBox);
        return OK;
    }
    return NOT_EMPTY;
}

exception send_wait( mailbox* mBox, void* pData )
{
    if(mBox == NULL || pData == NULL)
        return NULLPOINTER;
    isr_off();
    int msg_status = 0;
    msg_status = (mBox->pHead==NULL)?0:(mBox->pHead->Status==RECEIVER)?1:0;
    if(msg_status)
    {
        exception exc1 = mem_copy((char *) pData,mBox->pHead->pData,mBox->nDataSize);
        if(exc1<= FAIL)
        {
            isr_on();
            return exc1;
        }
        msg* mes = mailbox_dequeue(mBox);
        
        //Is it in the head of the ReadyList? I don't think så but I will add it here
        //Temporarily
        PreviousTask = NextTask;                            //Update PreviousTask, the previous task is the task sending the message
        move_listobj(WaitingList,ReadyList,mes->pBlock);    //Insert recieving task in ReadyList
        NextTask = mes->pBlock->pTask;                      //Update NextTask

    }
    else
    {
        //Allocate a msg
        msg* mes = (msg*) mem_alloc(sizeof(msg));
        if(mes == NULL)
        {
            isr_on();
            return ALLOCFAIL;
        }
        //Set data pointer -> pData
        mes->pData  = pData;
        mes->pBlock = ReadyList->pHead;
        mes->Status = SENDER;
        //Add msg to mBox
        mailbox_enqueue(mBox,mes);
        //Update PreviousTask
        PreviousTask = ReadyList->pHead->pTask;
        //Remove sending_task from ReadyList and insert it to WaitingList
        move_listobj(ReadyList, WaitingList, mes->pBlock);
        //Update NextTask
        NextTask = ReadyList->pHead->pTask;
        
    }
    isr_on();
    SwitchContext();
    if (deadline_reached(NextTask))
    {
        //Disable interrupt
        isr_off();
        //Removing sending_msg Also returns the message, but it is not essential for this part
        mailbox_dequeue(mBox);
        //Enable interrupt
        isr_on();
        return DEADLINE_REACHED;
    }
    return OK;
}

//Empty declaration
exception receive_wait( mailbox* mBox, void* pData )
{
    if(mBox == NULL||pData ==NULL)
        return NULLPOINTER;
    isr_off();                              //Disable interrupt
    exception exc1 = (mBox->pHead==NULL)?0:(mBox->pHead->Status == SENDER)?1:0;

    if(exc1)                                 //IF send Message is waiting
    {
        
        exception exc2 = mem_copy(mBox->pHead->pData,pData,mBox->nDataSize);    //Copy sender’s data to receiving task’s data area
        msg* mes = mailbox_dequeue(mBox);   //Remove sending task’s Message struct from the Mailbox
        if(mes->pBlock != NULL)             //IF Message was of wait type
        {
            //Update PreviousTask
            PreviousTask = NextTask;
            if(move_listobj(WaitingList,ReadyList,mes->pBlock)<=FAIL)   //Move sending task to ReadyList
            {
                isr_on();
                return FAIL;
            }
            //Update NextTask
            NextTask = ReadyList->pHead->pTask;
        }
        else
            mem_free(mes->pData);           //Free senders data area
    }
    //ENDIFYOUWANT
    //ELSE
        //Allocate a Message structure
        msg* newMes = (msg*) mem_alloc(sizeof(msg));
        //Add Message to the Mailbox
        mailbox_enqueue(mBox, newMes); FUCK OFF
        //Update PreviousTask
        //Move receiving task from ReadyList to WaitingList Update NextTask
    //ENDIFYOUWANT
    //Switch context
    //IF deadline is reached THEN
        //Disable interrupt
        //Remove receive Message
        //Enable interrupt
    //Return DEADLINE_REACHED
    //ELSE
        //Return YESOK
    //ENDIFYOUWANT
    
    

}

//Empty declaration
exception send_no_wait( mailbox* mBox, void* pData )
{
    exception status = OK;
    return status;
}

//Empty declaration
exception receive_no_wait( mailbox* mBox, void* pData )
{
    exception status = OK;
    return status;
}

//Empty declaration
exception no_messages( mailbox* mBox )
{
    exception status = OK;
    return status;
}

exception mailbox_enqueue(mailbox* mBox, msg* mes)
{
    if(mBox==NULL || mes == NULL)
        return NULLPOINTER;
    if (mBox->pTail==NULL)
    {
           mBox->pTail = mes;
           mBox->pHead = mes;
    }
    else
    {
        mBox->pTail->pNext = mes;
        mes->pPrevious = mBox->pTail;
        mBox->pTail = mes;
    }
    return OK;
}

msg* mailbox_dequeue(mailbox* mBox)
{
    if(mBox==NULL)
        return NULL;
    if(mBox->pHead==NULL)
        return NULL;
    msg *mes = mBox->pHead;
    if(mBox->pHead == mBox->pTail)
    {
        mBox->pTail = NULL;
        mBox->pTail = NULL;
        return mes;
    }
    mBox->pHead = mBox->pHead->pNext;
    mBox->pHead->pPrevious = NULL;
    return mes;
}