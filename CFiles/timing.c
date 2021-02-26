void set_ticks( uint ticks )
{
    tick_counter = ticks;
}
int deadline_reached(TCB* task)
{
    return (task->Deadline <= tick_counter)? 1:0;
}