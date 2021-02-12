
exception send_wait(mailbox* mBox, void* pData)
{
    //Disable interrupt
    isr_off();
    current = mBox->pHead;
    //Doesn't need to loop through the recieving task mailbox, it's a FIFO system
    //We just need to see if the first element is in recieving status
    
    //If recieving task is waiting
    if (current != NULL && current->status == RECIEVING)
    {
        //Copy sender's data to the data area of the recieving task msg
        current->pData -> (void*) pData;
        //Remove recieving task's message struct from its mailbox
        removeMsg(mBox, current);
        //Update PreviousTask
        PreviousTask = ReadyList->pHead->pTask;



    }






}