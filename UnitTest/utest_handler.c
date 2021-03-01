
/**
* assert_equals 
* \brief Checks if two values are equal regardless of data type, provided a pointer to each of the values and the sie of the type
* @param pointer1   Pointer to the first value
* @param pointer2   Pointer to the seconds value
* @param len        The length of the data typ (or array)
* @param status     Pointer to the current status element of the test, value is set according to the assert type and the return value
* @return OK if the two pointers contain the same value, FAIL if the values are not equal. NULLPOINTER if the one or more of the pointers are equal to NULL
*/
exception assert_equals(void *pointer1, void *pointer2, int len, test_status status)
{
    if (pointer1 == NULL || pointer2 == NULL)
    {
        if (status != NULL)
            status[0] = A_EQ_NP;
        return NULLPOINTER;
    }
    for (int i = 0; i < len; i++)
        if (((char *)pointer1)[i] != ((char *)pointer2)[i])
        {
            if (status != NULL)
                status[0] = A_EQ_F;
            return FAIL;
        }
    if (status != NULL)
        status[0] = A_EQ_S;
    return OK;
}

/**
* assert_not_equals
* \brief Same as assert_equals but reverse 
* @param pointer1   Pointer to the first value
* @param pointer2   Pointer to the seconds value
* @param len        The length of the data typ (or array)
* @param status     Pointer to the current status element of the test, value is set according to the assert type and the return value
* @return FAIL if the two pointers contain the same value, OK if the values are not equal. NULLPOINTER if the one or more of the pointers are equal to NULL
*/
exception assert_not_equals(void *pointer1, void *pointer2, int len, test_status status)
{
    exception exc = assert_equals(pointer1, pointer2, len, NULL);
    exc = (exc == NULLPOINTER) ? NULLPOINTER : assert_fail(exc, NULL);
    if (status != NULL)
        status[0] = (exc == NULLPOINTER) ? A_NEQ_NP : (exc) ? A_NEQ_S
                                                            : A_NEQ_F;
    return exc;
}

/**
* assert_interrupt
* \brief NOT IMPLEMENTED 
* @param status     Pointer to the current status element of the test, value is set according to the assert type and the return value     
* @return OK
*/
exception assert_interrupt(test_status status)
{
    if (status != NULL)
        status[0] = A_I_S;
    return OK;
}

/**
* assert_fail
* \brief Checks if an exception is equal to FAIL (used to reduce an exception to only be one of two values OK or FAIL) 
* @param exc an exception 
* @param status     Pointer to the current status element of the test, value is set according to the assert type and the return value
* @return if the exception is equal to FAIL return OK otherwise return FAIL
*/
exception assert_fail(exception exc, test_status status)
{
    if (status != NULL)
        status[0] = (exc == FAIL) ? A_F_S : A_F_F;
    return (exc == FAIL) ? OK : FAIL;
}

/**
* assert_ok
* \brief Checks if an exception is equal to OK (used to reduce an exception to only be one of two values OK or FAIL) 
* @param exc an exception 
* @param status     Pointer to the current status element of the test, value is set according to the assert type and the return value
* @return if the exception is equal to OK return OK otherwise return FAIL
*/
exception assert_ok(exception exc, test_status status)
{

    if (status != NULL)
        status[0] = (exc == OK) ? A_O_S : A_O_F;
    return (exc == OK) ? OK : FAIL;
}

exception full_test_status(test_status status, int len)
{
    for (int i = 0; i < len; i++)
        if (status[i] < 0)
            return FAIL;
    return OK;
}
void display_test_status(test_status status)
{
}