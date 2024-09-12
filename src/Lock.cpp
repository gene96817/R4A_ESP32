/**********************************************************************
  Lock.cpp

  Robots-For-All (R4A)
  Locking support
**********************************************************************/

#include "R4A_Common.h"

//*********************************************************************
// Take out a lock
void r4aLockAcquire(volatile int * lock)
{
    uint32_t expected;

    // Single thread access to the lock
    expected = 0;
    while (!__atomic_compare_exchange_4(lock,
                                        &expected,
                                        xPortGetCoreID() + 1,
                                        false,
                                        __ATOMIC_RELAXED,
                                        __ATOMIC_RELAXED))
    {
        expected = 0;
    }
}

//*********************************************************************
// Release a lock
void r4aLockRelease(volatile int * lock)
{
    *lock = 0;
}
