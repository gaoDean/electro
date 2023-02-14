#include <Arduino.h>

#define DATA_PIN 15
#define LATCH_PIN 16
#define CLOCK_PIN 17
/* #define DATA_PIN 2 */
/* #define LATCH_PIN 3 */
/* #define CLOCK_PIN 4 */

uint16_t pattern = 0b1010101010;

void registerWrite(uint16_t data)
{
	digitalWrite (CLOCK_PIN, LOW);
	digitalWrite(LATCH_PIN, LOW);

	shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, (data >> 8)); // left 8 bits
	shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, data); // right 8 bits

	digitalWrite(LATCH_PIN, HIGH);
}

void setup()
{
  pinMode(DATA_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
}

void loop()
{
	registerWrite(pattern);
	delay(500);
	pattern = ~pattern;
}
