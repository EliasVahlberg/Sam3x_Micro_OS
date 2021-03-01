
#define A_EQ_S              1       //Assert equals returned OK
#define A_EQ_F              -1      //Assert equals returned FAIL
#define A_EQ_NP             -2      //Assert equals returned NULLPOINTER
#define A_NEQ_S             3       //Assert not equals returned OK
#define A_NEQ_F             -3      //Assert not equals returned FAIL
#define A_NEQ_NP            -4      //Assert not equals returned NULLPOINTER
#define A_O_S               5       //Assert ok returned OK
#define A_O_F               -5      //Assert ok returned FAIL
#define A_F_S               6       //Assert fail returned OK
#define A_F_F               -6      //Assert fail returned FAIL
#define A_I_S               7       //Assert interrupt returned OK
#define A_I_F               -7      //Assert interrupt returned FAIL

#define TASK_ADMINISTRATION 2
#define MAILBOX_SYSTEM 3
#define TASK_TIMING 4
typedef struct TestInfo
{
    int test_id;
    void *test_adress;
    int package;
    int num_assert;
} test_info;
typedef char *test_status;
exception assert_equals(void *pointer1, void *pointer2, int len, test_status status);
exception assert_not_equals(void *pointer1, void *pointer2, int len, test_status status);
exception assert_interrupt(test_status status);
exception assert_fail(exception exc, test_status status);
exception assert_ok(exception exc, test_status status);
void display_test_status(test_status status);

#include "utest_handler.c"
