#include <Arduino.h>
#include "timer.h"

uint32_t timerStartTime = 0;
uint32_t timerPauseTime = 0;

void resetTimer() {
	timerStartTime = millis();
}

void pauseTimer() {
	timerPauseTime = millis();
}

void unpauseTimer() {
	timerStartTime += millis() - timerPauseTime;
	timerPauseTime = 0;
}

uint32_t getTimerStart() {
	return timerStartTime;
}

uint32_t getTimerPause() {
	return timerPauseTime;
}

bool timerPaused() {
	return timerPauseTime == 0 ? false : true;
}

bool timerEnded(uint32_t timerDuration) {
	Serial.println("Timer ended");
	return millis() - timerStartTime > timerDuration;
}
