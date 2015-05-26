/*
 * LDFBIN1_SLAVE_v.1.1.c
 *
 * Created: 19.05.2015 20:53:55
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

int main(void)
{
	uint8_t data_from_master = 0;
	uint8_t status_data_slave = 0x07;
	
	DDRC |= (1<<DDC0);					// Set PC0 as output for relay control signal
	DDRC |= (1<<DDC1);					// Set PC1 as output for test signal on GPIO1
	PORTC &= ~(1<<PC0);					// Ensure REALY is in off-state to begin with
	PORTC &= ~(1<<PC1);
	
	sei();
	
	TWI_init_slave(TWI_ADDR_MODUL);		// Initialize TWI (I2C) communication as SLAVE
	
	while(1)
	{
		TWI_slave_reciever_transmitter();		// Start dual-mode transmitter + receiver
		
		if ((TWSR & 0xF8) == 0x60)						// If SLA+W was received from master
		{
			data_from_master = TWI_slave_read_data();	// Get data from MASTER
			
			if (data_from_master == 0x05)
			{
				PORTC |= (1<<PC0);		// Switch on RELAY
				PORTC |= (1<<PC1);		// Set test signal on GPIO1
				status_data_slave = 0x05;
			}
			else if (data_from_master == 0x07)
			{
				PORTC &= ~(1<<PC0);		// Switch off RELAY
				PORTC &= ~(1<<PC1);		// Clear test signal on GPIO1
				status_data_slave = 0x07;
			}
			
			data_from_master = 0;
		}
		
		else if ((TWSR & 0xF8) == 0xA8)				// If SLA+R was received from master
		{
			TWI_slave_write_data(status_data_slave);
		}
		
	}
	
}
