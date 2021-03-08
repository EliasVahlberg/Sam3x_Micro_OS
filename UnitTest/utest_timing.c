/**
* @file
*     utest_timing.c
* @authors 
*     Elias Vahlberg
*     Hamed Haghjo
* \brief 
*     ...
*/

#pragma region functions
test_info*  utest_set_ticks         (test_info* t_info);    //(uint ticks);
test_info*  utest_ticks             (test_info* t_info);    //(void);
test_info*  utest_deadline          (test_info* t_info);    //(void);
test_info*  utest_deadline_reached  (test_info* t_info);    //(TCB* task);
test_info*  utest_set_deadline      (test_info* t_info);    //(uint deadline);
test_info*  utest_wait              (test_info* t_info);    //(uint nTicks);
test_info*  utest_TimerInt          (test_info* t_info);    //(void);
#pragma endregion functions
