#ifndef TIMER_H_
#define TIMER_H_

void resetTimer();
void pauseTimer();
void unpauseTimer();
uint32_t getTimerStart();
uint32_t getTimerPause();
bool timerPaused();
bool timerEnded(uint32_t);

#endif
