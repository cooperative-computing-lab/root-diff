#ifndef __TIMER_H__
#define __TIMER_H__

#include <iostream>
#include <ctime>

/*
 * A timer for measuring the performance on millisecond 
 */

class Timer
{
public:
    Timer(); 
    double elapsed();

    void reset();

private:
    timespec begin, end;
};

#endif
