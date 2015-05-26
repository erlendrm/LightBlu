/*
 * LDFDIM1_SLAVE_v.c
 *
 * Created: 19.05.2015 20:58:46
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

uint8_t light_lvl;

ISR(PCINT1_vect)
{
	if ((PINC & (1<<PC1)))
	{
		
		// Timer frequencies calculated by using the formula:
		// f = F_CPU / ( N * (1 + OCR0A) )
		// t = 1 / f
		
		// Hence the value of OCR0A can be found for desired values of f:
		// OCR0A = (F_CPU / ( N * f )) - 1  
		
		if (light_lvl == 0x14)
		{
			OCR0A	 = 186;						// Set compare value so that f = 666Hz -> t = 1,5ms
			TCCR0B	|= (1<<CS01);				// Start timer with prescaler N = 8
		}
		
		else if (light_lvl == 0x12)
		{
			OCR0A	 = 61;						// Set compare value so that f = 250Hz -> t = 4ms
			TCCR0B	|= ((1<<CS01)|(1<<CS00));	// start timer with prescaler N = 64
		}
		
		else if (light_lvl == 0x10)
		{
			OCR0A	 = 85;						// Set compare value so that f = 180Hz -> t = 5,5ms
			TCCR0B	|= ((1<<CS01)|(1<<CS00));	// start timer with prescaler N = 64
		}
		
		else if (light_lvl == 0x0E)
		{
			OCR0A	 = 101;						// Set compare value so that f = 153Hz -> t = 6,5ms
			TCCR0B	|= ((1<<CS01)|(1<<CS00));	// start timer with prescaler N = 64
		}
		
		else if (light_lvl == 0x0C)
		{
			OCR0A	 = 117;						// Set compare value so that f = 133Hz -> t = 7,5ms
			TCCR0B	|= ((1<<CS01)|(1<<CS00));	// start timer with prescaler N = 64
		}
		
		else
		{
			TCCR0B	= 0;			// Stop timer
			OCR0A	= 0;			// Clear compare value
			TCNT0	= 0;			// Clear counter value
			TIFR0	|= (1<<OCF0A);	// Clear interrupt flag
		}
	}
	
}

ISR(TIMER0_COMPA_vect)
{
	TCCR0B	= 0;			// Stop timer
	OCR0A	= 0;			// Clear compare value
	TCNT0	= 0;			// Clear counter value
	TIFR0	|= (1<<OCF0A);	// Clear interrupt flag
	
	
	PORTC |= (1<<PC0);		// Set output to triacdriver high
	PORTC |= (1<<PC2);		// For test
	
	_delay_us(200);
				
	PORTC &= ~(1<<PC0);		// Set output to triacdriver low
	PORTC &= ~(1<<PC2);		// for test
	
}

int main(void)
{
	uint8_t data_from_master = 0;
	uint8_t status_data_slave = 0x0A;	// Start value for light output, 0x0A = 0% Light.
	
	light_lvl = 0;
	
	DDRC |= (1<<DDC0);					// Set PC0 as output
	DDRC |= (1<<DDC2);					// Set PC2 as output
	DDRC &= ~(1<<DDC1);					// Set PC1 as input, circuit already contains pull-up resistor
	
	PORTC &= ~(1<<PC0);					// Ensure TRIAC is in off-state to begin with
	PORTC &= ~(1<<PC2);					// For test
	
	// Initiate interrupts
	PCICR |= (1<<PCIE1);				// Enable interrupt requests on pins PCINT14..8
	PCMSK1 |= (1<<PCINT9);				// Enable interrupt request on PCINT9 (PC1)
	
	// Initiate timer
	TCCR0A &= ~((1<<COM0A1)|(1<<COM0A0));	// Set normal port operation, OC0A disconnected
	TCCR0A |= (1<<WGM01);					// Enable CTC-mode
	TIMSK0 |= (1<<OCIE0A);					// Enable output compare match interrupt
	
	sei();
	
	TWI_init_slave(TWI_ADDR_DIM_MODUL);		// Initialize TWI (I2C) communication as SLAVE
	
	while(1)
	{
		TWI_slave_reciever_transmitter();		// Start dual-mode transmitter + receiver
		
		if ((TWSR & 0xF8) == 0x60)						// If SLA+W was received from master
		{
			data_from_master = TWI_slave_read_data();	// Get data from MASTER
						
			// Values in actual use
			if (data_from_master == 0x0A)
			{
				light_lvl = data_from_master;
				status_data_slave = 0x0A;
			}
			else if (data_from_master == 0x0C)
			{
				light_lvl = data_from_master;
				status_data_slave = 0x0C;
			}
			else if (data_from_master == 0x0E)
			{
				light_lvl = data_from_master;
				status_data_slave = 0x0E;
			}
			else if (data_from_master == 0x10)
			{
				light_lvl = data_from_master;
				status_data_slave = 0x10;
			}
			else if (data_from_master == 0x12)
			{
				light_lvl = data_from_master;
				status_data_slave = 0x12;
			}
			else if (data_from_master == 0x14)
			{
				light_lvl = data_from_master;
				status_data_slave = 0x14;
			}
			
			data_from_master = 0;
		}
		
		else if ((TWSR & 0xF8) == 0xA8)					// If SLA+R was received from master
		{
			TWI_slave_write_data(status_data_slave);
		}
		
	}
	
}
