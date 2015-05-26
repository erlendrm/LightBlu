/*
 * TWI.h
 *
 * Created: 19.05.2015 21:00:03
 *  Author: Erlend
 */ 


#ifndef TWI_H_
#define TWI_H_

void TWI_error(void);

///////////////////////////////////////////////////////////////////////////////////////

void TWI_init_master(void)
{
	TWBR=0x01;						// Bit rate
	TWSR=(0<<TWPS1)|(0<<TWPS0);		// Setting prescaler bits
	// SCL freq = F_CPU/(16+2(TWBR)*prescaler)
	// SCL freq = 55555,55 Hz = 55,5 kHz
}

///////////////////////////////////////////////////////////////////////////////////////

void TWI_init_slave(uint8_t slave_addr)
{
	TWAR =	(slave_addr<<1);					// Set own TWI slave address. Accept TWI general calls.
	//TWCR =	(1<<TWINT)|(1<<TWEN)|(1<<TWEA);		// Enable TWI, clear TWI interrupt flag
}

///////////////////////////////////////////////////////////////////////////////////////

void TWI_start(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);		// Transmit START condition
	
	while (!(TWCR &(1<<TWINT)));				// Wait for TWINT flag set
	while ((TWSR & 0xF8) != 0x08);				// Wait for ACK
}

///////////////////////////////////////////////////////////////////////////////////////

void TWI_repeated_start(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);		// Transmit START condition
	
	while (!(TWCR &(1<<TWINT)));				// Wait for TWINT flag set
	while ((TWSR & 0xF8) != 0x10);				// Wait for ACK
}

///////////////////////////////////////////////////////////////////////////////////////

void TWI_sla_w(uint8_t SLA)
{
	TWDR = (SLA << 1);							// Copy Slave Address (SLA) into data register
	TWDR &= ~(1<<0);							// Enable WRITE Mode
	TWCR = (1<<TWINT)|(1<<TWEN);				// Clear TWINT and set TWEN to start transmission
	
	while (!(TWCR &(1<<TWINT)));				// Wait for TWINT flag set
	while ((TWSR & 0xF8) != 0x18);				// Wait for ACK
}

///////////////////////////////////////////////////////////////////////////////////////

void TWI_sla_r(uint8_t SLA)
{
	TWDR = (SLA << 1);							// Copy Slave Address (SLA) into data register
	TWDR |= (1<<0);								// Enable READ Mode
	TWCR = (1<<TWINT)|(1<<TWEN);				// Clear TWINT and set TWEN to start transmission
	
	while (!(TWCR &(1<<TWINT)));				// Wait for TWINT flag set
	while ((TWSR & 0xF8) != 0x40);				// Wait for ACK
}

///////////////////////////////////////////////////////////////////////////////////////

void TWI_slave_reciever(void)
{
	while ((TWSR & 0xF8) != 0x60)				// Loop until correct acknowledgment has been received
	{
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);	// Enable TWI, clear TWI interrupt flag
		while (!(TWCR &(1<<TWINT)));			// Wait for TWINT flag receiver
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void TWI_slave_transmitter(void)
{
	while ((TWSR & 0xF8) != 0xA8)				// Loop until correct acknowledgment has been received
	{
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);	// Enable TWI, clear TWI interrupt flag
		while (!(TWCR &(1<<TWINT)));			// Wait for TWINT flag
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void TWI_slave_reciever_transmitter(void)
{
	while ( ((TWSR & 0xF8) != 0xA8) && ((TWSR & 0xF8) != 0x60))				// Loop until correct acknowledgment has been received
	{
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);	// Enable TWI, clear TWI interrupt flag
		while (!(TWCR &(1<<TWINT)));			// Wait for TWINT flag
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void TWI_write_data(uint8_t data)
{
	TWDR = data;								// Copy Data into data register
	TWCR = (1<<TWINT)|(1<<TWEN);				// Clear TWINT and set TWEN to start transmission
	
	while (!(TWCR &(1<<TWINT)));				// Wait for TWINT flag set
	while ((TWSR & 0xF8) != 0x28);				// Wait for ACK
}

///////////////////////////////////////////////////////////////////////////////////////

void TWI_slave_write_data(uint8_t data)
{
	TWDR = data;								// Copy Data into data register
	TWCR = (1<<TWINT)|(1<<TWEN);				// Clear TWINT and set TWEN to start transmission
	
	while ((TWSR & 0xF8) != 0xC0);				// Wait for ACK
}

///////////////////////////////////////////////////////////////////////////////////////

uint8_t TWI_read_data(void)
{
	uint8_t data = 0;
	
	TWCR = (1<<TWINT)|(1<<TWEN);				// Clear TWINT and set TWEN to start receiving data
	
	while (!(TWCR &(1<<TWINT)));				// Wait for TWINT flag set
	while ((TWSR & 0xF8) != 0x58);				// Wait for NOT ACK
	
	data = TWDR;								// Load First byte of data from data register into the variable 'data'
	return data;
}

///////////////////////////////////////////////////////////////////////////////////////

uint8_t TWI_slave_read_data(void)
{
	uint8_t data = 0;
	
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);		// Clear TWINT and set TWEN and TWEA to start receiving data
	
	while (!(TWCR &(1<<TWINT)));				// Wait for TWINT flag set
	while ((TWSR & 0xF8) != 0x80);				// Wait for ACK
	
	data = TWDR;								// Load First byte of data from data register into the variable 'data'
	return data;
}

///////////////////////////////////////////////////////////////////////////////////////

void TWI_stop(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);		// Transmit STOP condition
	while (!(TWCR & (1<<TWSTO)));				// Wait till STOP condition is transmitted
}

///////////////////////////////////////////////////////////////////////////////////////

void TWI_error(void){
	// Do nothing
}


#endif /* TWI_H_ */
