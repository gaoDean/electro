#include <Arduino.h>

#define BUTTON_PIN 36

hw_timer_t *timer = NULL;

bool debounce() {
	static uint16_t state = 0;
	state = (state << 1) | digitalRead(BUTTON_PIN) | 1 << 15;
	return (state == 3 << 14);
}

void setup()
{
	Serial.begin(9600);
	pinMode(BUTTON_PIN, INPUT_PULLUP);

	timer = timerBegin(0, 80, true);
}

void loop() {
	if (debounce()) {
		Serial.println("pressed");
	}
	uint64_t count;
	gptimer_get_raw_count(gptimer, &count);
	Serial.println(count)
	delay(1);
}
