#include <Arduino.h>

#define BUTTON_PIN 18
#define MS_IN_MINUTE 1000 * 60
#define DO_DURATION 2
#define STOP_DURATION 1
#define DOUBLE_CLICK_THRESHOLD 500
#define PAUSE_FLASH_DELAY 1000
#define DISPLAY_MODE_TIME 2000

#define DATA_PIN 15
#define LATCH_PIN 16
#define CLOCK_PIN 17
/* #define DATA_PIN 2 */
/* #define LATCH_PIN 3 */
/* #define CLOCK_PIN 4 */


enum modes { Do = 2, Stop = 1, Nothing = 0};
enum modes mode = Nothing;
uint32_t startTime;
uint32_t lastPressed = 0;
uint32_t lastPauseFlash = 0;
uint32_t durationAtPauseStart;
uint32_t pauseTime = false;
bool pauseFlashState = false;
bool pauseFlash = true;

enum modes nextMode() {
	return mode == Nothing ? Do : (mode == Do ? Stop : Nothing);
}

uint32_t convertToMillis(uint8_t minutes) {
	return minutes * 60 * 1000;
}

bool pressed() {
	static uint8_t state = 0;
	state = (state << 1) | digitalRead(BUTTON_PIN) | 1 << 7;
	return (state == 3 << 6);
}

void doPause(uint32_t duration, bool flash) {
	Serial.println("Paused");
	pauseTime = duration;
	durationAtPauseStart = millis() - startTime;
	pauseFlash = flash;
}

void displayWrite(uint16_t data) { // registerWrite
	digitalWrite (CLOCK_PIN, LOW);
	digitalWrite(LATCH_PIN, LOW);

	shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, (data >> 8)); // left 8 bits
	shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, data); // right 8 bits

	digitalWrite(LATCH_PIN, HIGH);
}

void displayMode() {
	Serial.println(mode);
	displayWrite(mode);
	doPause(DISPLAY_MODE_TIME, false);
}

void handlePress() {
	if (!pressed())
	Serial.println("Pressed");
	if (lastPressed == 0 || millis() - lastPressed < DOUBLE_CLICK_THRESHOLD) {
		mode = nextMode();
		startTime = millis();
		if (mode == Nothing) {
			doPause(0, false);
			displayWrite(0);
		} else {
			displayMode();
		}
	} else {
		doPause(pauseTime ? 0 : UINT32_MAX, true);
	}
	lastPressed = millis();
}

void handlePause() {
	if (millis() - startTime > pauseTime) {
		startTime = millis() - durationAtPauseStart;
		pauseTime = 0;
		Serial.println("Unpaused");
	}
	if (pauseFlash && millis() - lastPauseFlash > PAUSE_FLASH_DELAY) {
		pauseFlashState = !pauseFlashState;
		displayWrite(pauseFlashState ? 0xFFFFFFFF : 0);
		lastPauseFlash = millis();
	}
}

void updateDisplay() {
	uint8_t numBitsToFill = (10 * ((millis() - startTime)) / convertToMillis(mode));
	uint16_t bitRepresentation = pow(2, numBitsToFill) - 1;
	displayWrite(bitRepresentation);

	if (millis() - startTime > convertToMillis(mode)) {
		Serial.println("Timer Ended");
		mode = nextMode();
		if (mode == Nothing) {
			mode = Do;
		}
		startTime = millis();
		displayMode();
		doPause(UINT32_MAX, true);
	}
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

	if (pauseTime > 0) {
		handlePause();
	} else if (mode != Nothing) {
		updateDisplay();
	}
}
