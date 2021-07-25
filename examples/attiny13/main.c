/**
 * Copyright (c) 2018, Łukasz Marcin Podkalicki <lpodkalicki@gmail.com>
 *
 * This is ATtiny13 "The Best Room Temperature" using DS18B20.
 */

#include <avr/io.h>
#include <util/delay.h>
#include "onewire.h"

#define	DS18B20_PIN	PB0
#define	LED_PIN		PB1

#define	TEMP_MIN	(19)
#define	TEMP_MAX	(22)

static uint8_t
read_temperature(uint8_t pin)
{
	uint16_t t;

	onewire_reset(pin); // 1-Wire reset
	onewire_write(pin, ONEWIRE_SKIP_ROM); // to all devices on the bus
	onewire_write(pin, 0x44); // send DS18B20 command, "CONVERT T"

	onewire_reset(pin); // 1-Wire reset
	onewire_write(pin, ONEWIRE_SKIP_ROM); // to all devices on the bus
	onewire_write(pin, 0xBE); // send DS18B20 command, "READ SCRATCHPAD"

	t = onewire_read(pin); // read temperature low byte
	t |= (uint16_t)onewire_read(pin) << 8; // and high byte
	t = ((t >> 4) * 100 + ((t << 12) / 6553) * 10) / 100; // decode temp

	return (uint8_t)t;
}

int
main(void)
{
	uint8_t t;

	/* setup */
	DDRB |= _BV(LED_PIN); // set LED pin as OUTPUT

	/* loop */
	while (1) {
		t = read_temperature(DS18B20_PIN);
		if (t < TEMP_MIN || t > TEMP_MAX) {
			PORTB |= _BV(LED_PIN); // light up the alarm LED
		} else {
			PORTB &= ~_BV(LED_PIN); // turn the alarm LED off
		}
		_delay_ms(1000);
	}
}
