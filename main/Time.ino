//*********************************************
// Group:            AE-DIY-Group?
// Module name:      Time
// Version:          V0.9Beta
// Editor:           Rededge
// Log:              Undertest
//*********************************************
#include <Arduino.h>
#include "main.h"
#include "Time.h"

int CounterTime = 0;
int startCounterTime = 0;

void Counter(int x)
{
    if (x == 1)
    {
        startCounterTime = millis();
    }

    while (x != 0)
    {
        CounterTime = startCounterTime - millis();
        delay(1);
    }

    return 0;
}

void startCounter() // restart counter with code 1
{
    Counter(1);
    return;
}

void ContCounter() // continue previous counter with code 2
{
    Counter(2);
    return;
}

void endCounter() // pause previous counter with code 0
{
    Counter(0);
    return;
}

int getCounter() // get countertime
{
    return CounterTime;
}

int endgetCounter() // stop and get countertime
{
    Counter(0);
    return CounterTime;
}