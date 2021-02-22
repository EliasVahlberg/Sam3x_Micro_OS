
/**
* @file
*     main.c
* @authors 
*     Elias Vahlberg
*     Hamed Haghjo
* \brief 
*     ...
*/

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
        isr_off();
        mailbox_dequeue(mBox);                                                  //Removing sending_msg Also returns the message, but it is not essential for this part
        isr_on();
        return DEADLINE_REACHED;
    }
    return OK;
}


exception receive_wait( mailbox* mBox, void* pData )
{
    if(mBox == NULL||pData ==NULL)
        return NULLPOINTER;
    isr_off();                                                                  //Disable interrupt
    exception exc1 = (mBox->pHead==NULL)?0:(mBox->pHead->Status == SENDER)?1:0;

    if(exc1)                                                                    //IF send Message is waiting
    {
        
        exception exc2 = mem_copy(mBox->pHead->pData,pData,mBox->nDataSize);    //Copy sender’s data to receiving task’s data area
        msg* mes = mailbox_dequeue(mBox);                                       //Remove sending task’s Message struct from the Mailbox
        if(mes->pBlock != NULL)                                                 //IF Message was of wait type
        {
            //Update PreviousTask
            PreviousTask = NextTask;
            if(move_listobj(WaitingList,ReadyList,mes->pBlock)<=FAIL)           //Move sending task to ReadyList
            {
                isr_on();
                return FAIL;
            }
            //Update NextTask
            NextTask = ReadyList->pHead->pTask;
        }
        else
            {
                mem_free(mes->pData);                                           //Free senders data area
                mem_free(mes);
            }
            
    }
    else
        {
        msg* newMes = (msg*) mem_alloc(sizeof(msg));            //Allocate a Message structure
        mailbox_enqueue(mBox, newMes);                          //Add Message to the Mailbox
        PreviousTask = NextTask;                                //Update PreviousTask
        move_listobj(ReadyList, WaitingList, ReadyList->pHead); //Move receiving task from ReadyList to WaitingList Update NextTask
        NextTask = ReadyList->pHead->pTask;
        }
    SwitchContext();                                            //Switch context
    if(deadline_reached(NextTask))
    {
        isr_off();
        mailbox_dequeue(mBox);                                  //Remove receive Message
        isr_on();
        return DEADLINE_REACHED;
    }
    else
        return OK;
}

exception send_no_wait( mailbox* mBox, void* pData )
{
    if(mBox == NULL || pData == NULL)
        return NULLPOINTER;
    exception status = OK;
    isr_off();
    int msg_status = 0;
    msg_status = (mBox->pHead==NULL)?0:(mBox->pHead->Status==RECEIVER)?1:0;
    if(msg_status)
    {
        status = mem_copy((char *) pData,mBox->pHead->pData,mBox->nDataSize);           //Copy data to receiving tasks’ data area.
        if(status<= FAIL)
        {
            isr_on();
            return status;
        }
        msg* mes = mailbox_dequeue(mBox);                                               //Remove receiving task’s Message struct from the Mailbox
        PreviousTask = ReadyList->pHead->pTask;                                         //Update PreviousTask
        move_listobj(ReadyList, WaitingList, mes->pBlock);                              //Move receiving task to Readylist
        NextTask = ReadyList->pHead->pTask;                                             //Update NextTask
        isr_on();
        SwitchContext();                                                                //Switch context
    
    }
    else
    {
        msg* newMes = (msg*) mem_alloc(sizeof(msg));                                    //Allocate a Message structure
        if(newMes ==NULL)
        {
            isr_on();
            return ALLOCFAIL;
        }
        newMes->pData = pData;                                                          //Copy Data to the Message
        newMes->Status = SENDER;
        if (mBox->nMaxMessages <= mBox->nMessages)
            mailbox_dequeue(mBox);                                                      //Remove the oldest Message struct
        mailbox_enqueue(mBox,newMes);//Add Message to the Mailbox
        isr_on();
    }
    return OK;
}

exception receive_no_wait( mailbox* mBox, void* pData)
{
    if(mBox == NULL || pData == NULL)
        return NULLPOINTER;
    exception status = OK;
    isr_off();
    int msg_status = 0;
    msg_status = (mBox->pHead==NULL)?0:(mBox->pHead->Status==SENDER)?1:0;
    if(msg_status)
    {
        status = mem_copy((char *) pData,mBox->pHead->pData,mBox->nDataSize);           //Copy data to receiving tasks’ data area.
        if(status<= FAIL)
        {
            isr_on();
            return status;
        }
        msg* mes = mailbox_dequeue(mBox);                                               //Remove receiving task’s Message struct from the Mailbox
        if(mes->pBlock!=NULL)
        {    
            mem_free(mes->pData);
            mem_free(mes);
            PreviousTask = ReadyList->pHead->pTask;                                         //Update PreviousTask
            move_listobj(ReadyList, WaitingList, mes->pBlock);                              //Move receiving task to Readylist
            NextTask = ReadyList->pHead->pTask;                                             //Update NextTask
            SwitchContext();          
        }
        else
        {
            mem_free(mes->pData);                                                       //Free senders data area
            mem_free(mes);
        }
        isr_on();
        return OK;
    }
    isr_on();
    return FAIL;
    // Return status on received Message
}

exception wait(uint nTicks)
{
    exception status;
    isr_off();
    PreviousTask = NextTask;                                                            //Update PreviousTask
    move_listobj(ReadyList, TimerList, ReadyList->pHead);                               //Place running task in the TimerList
    NextTask = ReadyList->pHead->pTask;                                                 //Update NextTask
    SwitchContext();                                                                    //Switch context
    if (deadline_reached(NextTask))                                                     //If deadline reched, then
        status = DEADLINE_REACHED;                                                      //Status is DEADLINE_REACHED
    else                            
        status = OK;                                                                    //Else Status is OK
    isr_on();
    return status;                                                                      //Return Status
}

exception no_messages( mailbox* mBox )
{
    return (mBox->nMessages == 0 && mBox->pHead==NULL)?1:0;
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
    mBox->nMessages++;
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
    mBox->nMessages--;
    return mes;
}