#include <Arduino.h>

#define BUTTON_PIN 18
#define MS_IN_MINUTE 1000 * 60
#define DO_DURATION 2
#define STOP_DURATION 1
#define DOUBLE_CLICK_THRESHOLD 500
#define PAUSE_FLASH_DELAY 1000
#define DISPLAY_MODE_TIME 1000

#define DATA_PIN 15
#define LATCH_PIN 16
#define CLOCK_PIN 17
/* #define DATA_PIN 2 */
/* #define LATCH_PIN 3 */
/* #define CLOCK_PIN 4 */


enum modes { Do = 2, Stop = 1, Nothing = 0};
enum modes mode = Nothing;
uint16_t sleeping = 0;
uint32_t startTime;
uint32_t lastPressed = 0;
uint32_t lastPauseFlash = 0;
uint32_t pauseTimeDifference;
bool paused = false;
bool pauseFlashState = false;

enum modes nextMode() {
	return mode == Nothing || mode == Stop ? Do : Stop;
}

uint32_t convertToMillis(uint8_t minutes) {
	return minutes * 60 * 1000;
}

void displayWrite(uint16_t data) { // registerWrite
	digitalWrite (CLOCK_PIN, LOW);
	digitalWrite(LATCH_PIN, LOW);

	shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, (data >> 8)); // left 8 bits
	shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, data); // right 8 bits

	digitalWrite(LATCH_PIN, HIGH);
}

void updateDisplay(uint32_t cur_time) {
	uint8_t numBitsToFill = 10 * (cur_time / convertToMillis(mode));
	uint16_t bitRepresentation = pow(2, numBitsToFill) - 1;
	displayWrite(bitRepresentation);
}

bool pressed() {
	static uint16_t state = 0;
	state = (state << 1) | digitalRead(BUTTON_PIN) | 1 << 15;
	return (state == 3 << 14);
}

void setup() {
	Serial.begin(9600);
	pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
	uint32_t cur_time = millis();
	if (pressed()) {
		if (millis() - lastPressed < DOUBLE_CLICK_THRESHOLD) {
			mode = nextMode();
			startTime = cur_time;
			if (mode == Nothing) {
				displayWrite(0);
			} else {
				displayWrite(mode);
				sleeping = DISPLAY_MODE_TIME;
			}
		} else {
			paused = !paused;
			pauseTimeDifference = cur_time - startTime;
		}
		lastPressed = cur_time;
	}

	if (sleeping > 0) {
		if (cur_time - startTime > DISPLAY_MODE_TIME) {
			sleeping = 0;
		}
	} else if (paused) {
		if (cur_time - lastPauseFlash > PAUSE_FLASH_DELAY) {
			pauseFlashState = !pauseFlashState;
			displayWrite(pauseFlashState ? 0xFFFFFFFF : 0);
			lastPauseFlash = cur_time;
			startTime = cur_time - pauseTimeDifference;
		}
	} else if (mode != Nothing) {
		updateDisplay(cur_time);

		if (cur_time - startTime > convertToMillis(Do)) {
			mode = nextMode();
			startTime = cur_time;
		}
	}

	delay(1);
}
