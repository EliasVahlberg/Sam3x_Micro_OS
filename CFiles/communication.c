
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
    if(mBox->nMessages == 0)
    {
        mem_free(mBox);
        return OK;
    }
    return NOT_EMPTY;
}

//Empty declaration
exception send_wait( mailbox* mBox, void* pData )
{
    isr_off();
    int msg_status = 0;
    msg* temp = mBox->pHead;
    while(temp!=NULL)
    {
        if(temp->Status == RECEIVER)
            {
                msg_status = 1;
                break;
            }
        temp = temp->pNext;
    }
    if(msg_status)
    {
        if(mem_copy((char *) pData,temp->pData,mBox->nDataSize)==FAIL) {return FAIL;}
        temp->pBlock->pMessage = NULL;
        
    }
    else
    {
        
    }
    isr_on();
    return OK;
}


//Empty declaration
exception receive_wait( mailbox* mBox, void* pData )
{
    
    exception status = OK;
    return status;
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

}

exception mailbox_dequeue(mailbox* mBox,msg* mes, void* pData)
{
    if(mes->Status==RECEIVER)
    {
        if(mem_copy((char *) pData,mes->pData,mBox->nDataSize)==FAIL) {return FAIL;}
        if(find_task(WaitingList, mes->pBlock->pTask))
            move_listobj(WaitingList,ReadyList,mes->pBlock);
        

    }
}
