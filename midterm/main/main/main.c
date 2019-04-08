/*
 * main.c
 *
 * Created: 4/6/2019 9:49:11 PM
 * Author : chent
 */ 



#define F_CPU 16000000UL							// MCU operating Frequency
#define BAUD 9600									// Baudrate setting
#define BAUD_PRESCALLER F_CPU/16/BAUD-1				// Baudrate prescaller

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdlib.h>


void read_adc(void);						//Read temp with the LM35 using ADC
void adc_init(void);						// Initialize ADC
void USART_init( unsigned int ubrr );		// Initialize USART communication
void USART_tx_string(char *data);			//sending string through usart
volatile unsigned int adc_temp;				// raw temp value from adc
volatile unsigned int tempF;				// convert to fahrenheit
volatile unsigned int tempC;				// convert to celsius
char outs[256];								// out for sending USART commands
volatile char received_data;				// String for receiving USART communication


int main(void) {

	adc_init();								// Initialize Analog to Digital Converter on PC4
	USART_init(BAUD_PRESCALLER);			// Initialize the USART
	_delay_ms(500);							// Delay to sync all the commuication
	while(1)
	{
		
		//AT connect check
		unsigned char AT[] = "AT\r\n";															//Set device mode 
		unsigned char AT_CWMODE[] = "AT+CWMODE=1\r\n";											//set the ssid and password
		unsigned char AT_CWJAP[] = "AT+CWJAP=\"4CD534UITRCD\",\"password\"\r\n";				//Set device for single IP Address Mode
		unsigned char AT_CIPMUX[] = "AT+CIPMUX=0\r\n";											//start commuication and the port will be 80
		unsigned char AT_CIPSTART[] = "AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n";		//set the string length
		unsigned char AT_CIPSEND[] = "AT+CIPSEND=100\r\n";
		
		// set up the commuication from the esp 8266 to the router
		_delay_ms(200);
		USART_tx_string(AT); 
		_delay_ms(5000);
		USART_tx_string(AT_CWMODE);
		_delay_ms(5000);
		USART_tx_string(AT_CWJAP); 
		_delay_ms(15000);
		USART_tx_string(AT_CIPMUX);
		_delay_ms(10000);
		USART_tx_string(AT_CIPSTART);
		_delay_ms(10000);
		USART_tx_string(AT_CIPSEND);
		_delay_ms(5000);
		
		PORTC^=(1<<5);						// port pc2 open
		read_adc();							//read from adc
		adc_temp = (adc_temp*500)/1023;		//calc the temp variable from the raw data
		tempC = adc_temp;					// C value
		tempF = (tempC*1.8)+32;				// F value
		snprintf(outs,sizeof(outs),"GET https://api.thingspeak.com/update?api_key=S7S1IV6H0B04TYPP&field1=%3d\r\n", tempF); // this is the api key
		
		USART_tx_string(outs);				//send data
		_delay_ms(10000);
	}
}


void adc_init(void)					//initialize ADC
{
	
	ADMUX = (0<<REFS1)|				// Reference Selection Bits

	(1<<REFS0)|						// AVcc - external cap at AREF
	(0<<ADLAR)|						// ADC Left Adjust Result
	(1<<MUX2)|						// Analog Channel Selection Bits
	(0<<MUX1)|						// ADC2 (PC2 PIN25)
	(0<<MUX0);						// 010

	ADCSRA = (1<<ADEN)|				// ADC ENable

	(0<<ADSC)|						// ADC Start Conversion
	(0<<ADATE)|						// ADC Auto Trigger Enable
	(0<<ADIF)|						// ADC Interrupt Flag
	(0<<ADIE)|						// ADC Interrupt Enable
	(1<<ADPS2)|						// ADC Prescaler Select Bits
	(0<<ADPS1)|
	(1<<ADPS0);

}


void read_adc(void) {
	unsigned char i =4;
	adc_temp = 0;						//initialize adc_temp variable
	while (i--) {
		ADCSRA |= (1<<ADSC);
		while(ADCSRA & (1<<ADSC));
		adc_temp+= ADC;
		_delay_ms(50);
	}
	adc_temp = adc_temp / 4;			// Average a few samples

}


void USART_init( unsigned int ubrr ) {
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	UCSR0B |= (1 << TXEN0) | (1 << RXEN0)| ( 1 << RXCIE0); // Enable receiver, transmitter & RX interrupt
	UCSR0C |= (1<<UCSZ01) | (1 << UCSZ00);
}


void USART_tx_string( char *data ) {
	while ((*data != '\0')) {
		while (!(UCSR0A & (1 <<UDRE0)));
		UDR0 = *data;
		data++;
	}
}

