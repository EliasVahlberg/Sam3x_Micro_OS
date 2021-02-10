
mailbox* create_mailbox( uint nMessages, uint nDataSize )
{
    mailbox *mbox;
    msg *mes;
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
    exception status = OK;
    return status;
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
