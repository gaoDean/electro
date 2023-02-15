#ifndef TIMER_H_
#define TIMER_H_

void resetTimer();
void pauseTimer();
void unpauseTimer();
uint32_t getTimer();
bool timerPaused();
bool timerEnded(uint32_t);

#endif
