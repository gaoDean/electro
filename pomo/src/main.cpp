#include <Arduino.h>
#include "timer.h"

#define MS_IN_MINUTE 1000 * 60
#define DO_DURATION 2
#define STOP_DURATION 1
#define DOUBLE_CLICK_THRESHOLD 500
#define PAUSE_FRAME_DELAY 50
#define DISPLAY_MODE_TIME 2000
#define FLASH_DELAY 500

#define BUTTON_PIN 18
#define DATA_PIN 15
#define LATCH_PIN 16
#define CLOCK_PIN 17
/* #define DATA_PIN 2 */
/* #define LATCH_PIN 3 */
/* #define CLOCK_PIN 4 */


enum modes { Do = 25, Stop = 5, Nothing = 0};
enum modes mode = Nothing;

uint32_t lastPressed = 0;
uint32_t lastFlash = 0;
uint32_t pauseDuration = UINT32_MAX;
uint32_t lastPauseFrameUpdate = 0;
uint16_t animation = 0;
bool userPause = false;
bool lastFlashState = false;

enum modes nextMode() {
	return mode == Nothing ? Do : (mode == Do ? Stop : Nothing);
}

uint32_t convertToMillis(uint8_t minutes) {
	return minutes * 60 * 1000;
}

bool pressed() {
	static uint16_t state = 0;
	state = (state << 1) | digitalRead(BUTTON_PIN) | 1 << 15;
	return (state == 3 << 14);
}

void displayWrite(uint16_t data) { // registerWrite
	digitalWrite (CLOCK_PIN, LOW);
	digitalWrite(LATCH_PIN, LOW);

	shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, (data >> 8)); // left 8 bits
	shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, data); // right 8 bits

	digitalWrite(LATCH_PIN, HIGH);
}

void displayMode() {
	Serial.print("Mode is now ");
	Serial.println(mode);

	displayWrite(mode);

	pauseDuration = DISPLAY_MODE_TIME;
	pauseTimer();
}

void handlePress() {
	if (millis() - lastPressed < DOUBLE_CLICK_THRESHOLD) {
		mode = nextMode();
		resetTimer();
		unpauseTimer();
		displayMode();
	} else {
		if (timerPaused()) {
			unpauseTimer();
			displayMode();
		} else {
			pauseTimer();
		}
	}
	lastPressed = millis();
}

void animatePause() {
	if (userPause && millis() - lastPauseFrameUpdate > PAUSE_FRAME_DELAY) {
		animation = animation << 1;

		switch (animation) {
			case 0:
				animation = 0b1;
			break;
			case 0b10:
				animation = 0b11;
			break;
			case 0b110:
				animation = 0b111;
			break;
		}

		displayWrite(animation);

		lastPauseFrameUpdate = millis();
	}
}

void updateDisplay() {
	uint8_t numBitsToFill = 10 * (millis() - getTimerStart()) / convertToMillis(mode);
	uint16_t bitRepresentation = pow(2, numBitsToFill) - 1;

	if (millis() - lastFlash > FLASH_DELAY) {
		lastFlashState = !lastFlashState;

		if (lastFlashState) {
			bitRepresentation &= 1 << numBitsToFill;
		}

		lastFlash = millis();
	}

	displayWrite(bitRepresentation);
}

void setup() {
	Serial.begin(9600);

	pinMode(DATA_PIN, OUTPUT);
	pinMode(LATCH_PIN, OUTPUT);
	pinMode(CLOCK_PIN, OUTPUT);
	pinMode(BUTTON_PIN, INPUT_PULLUP);

	displayWrite(0);
}

void loop() {
	if (pressed()) {
		handlePress();
	}

	if (timerPaused()) {
		if (millis() - getTimerPause() > pauseDuration) {
			unpauseTimer();
			pauseDuration = UINT32_MAX;
		} else {
			animatePause();
		}
	} else if (mode != Nothing) {
		updateDisplay();
		if (timerEnded(convertToMillis(mode))) {
			resetTimer();
			pauseTimer();
			mode = (mode == Stop ? Do : Stop);
		}
	}
}
