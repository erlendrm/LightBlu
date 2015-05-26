/*
 * ATMEGA48V_switch_twi_testapp_v.c
 *
 * Created: 16.03.2015 14:57:31
 *  Author: Erlend
 */ 

#define F_CPU 1000000UL

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "config.h"
#include "TWI.h"

ISR(PCINT2_vect)
{
	if (!(PIND & (1<<PD2)))
	{
		TWI_start();				// Transmit TWI start condition
		TWI_sla_w(TWI_ADDR_MODUL);	// Transmit Slave address on bus
		TWI_write_data(0x05);		// Transmit value '5' to Slave
		TWI_stop();					// Transmit TWI stop condition
		_delay_ms(300);
	}
	else if (!(PIND & (1<<PD3)))
	{
		TWI_start();				// Transmit TWI start condition
		TWI_sla_w(TWI_ADDR_MODUL);	// Transmit Slave address on bus
		TWI_write_data(0x07);		// Transmit value '7' to Slave
		TWI_stop();					// Transmit TWI stop condition
		_delay_ms(300);
	}
	else;			// Do nothing
	
}

int main(void)
{
	
	TWI_init_master();		// Initialize TWI (I2C) communication
	
			
	DDRD &= ~(1<<DDD2);		// Set SWITCH1 on PD2 as input
	DDRD &= ~(1<<DDD3);		// Set SWITCH2 on PD3 as input
	
	
	PCICR |= (1<<PCIE2);					// Enable interrupt requests on pins PCINT23..16
	PCMSK2 |= (1<<PCINT18)|(1<<PCINT19);	// Enable interrupt on PB2 (PCINT18) and PB3 (PCINT19)
	
	sei();
	
	while(1)
	{
		// Do nothing
	}
	
}
