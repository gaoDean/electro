#include <Arduino.h>
#include "timer.h"

uint32_t timerStart = 0;
uint32_t timerPause = 0;

void resetTimer() {
	timerStart = millis();
}

void pauseTimer() {
	timerPause = millis();
}

void unpauseTimer() {
	timerStart += millis() - timerPause;
	timerPause = 0;
}

uint32_t getTimerStart() {
	return timerStart;
}

uint32_t getTimerPause() {
	return timerPause;
}

bool timerPaused() {
	return timerPause == 0 ? false : true;
}

bool timerEnded(uint32_t timerDuration) {
	Serial.println("Timer ended");
	return millis() - timerStart > timerDuration;
}
