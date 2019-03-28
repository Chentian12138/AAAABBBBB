/*
 * DA3.c
 *
 * Created: 3/27/2019 9:21:51 PM
 * Author : chent
 */ 

#define F_CPU 16000000UL
#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/setbaud.h>


void USART_init( unsigned int ubrr );	// initializes analog to digital
void USART_tx_string(char*data);		// prints a string

int ranNumber = 4;						// declare a random number
volatile float ranFloat = 5.5648965;	// declare random float
char outs[20];							// allocate memory space for float

int main(void)
{
    USART_init(BAUD_PRESCALLER);  // Initialize the USART
	
	TCCR1B |= (1<<CS12) | (1<<CS10);	//	set prescaler = 1024
	TIMSK1 = (1<<TOIE1);				// enable overflow flag
	TCNT1 = 75001;						// reset timer (65535 - 15624)
	sei();								// enable interrupts
	
    while (1) {}						// wait for timer interrupt
}


ISR (TIMER1_OVF_vect)
{
	USART_tx_string("\n");					// start
	USART_tx_string("This is a string");	// print string
	USART_tx_string("\n");					// got to next one
	snprintf(outs, sizeof(outs), "%3d\r\n", ranNumber);					// print int
	USART_tx_string("\n");					// go to next one
	sprintf(outs, "%g", ranFloat);
	USART_tx_string(outs);					// print float
	USART_tx_string("\n");					// start over
	
	TCNT1 = 75001;						// reset timer
}

/* INIT USART (RS-232)  */
void USART_init( unsigned int ubrr )
{
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	UCSR0B = (1 << TXEN0);     // Enable RX, TX & RX interrupt
	UCSR0C = (3 << UCSZ00);    //asynchronous 8 N 1
}

/* SEND A STRING THROUGH RS-232  BY FTDI BOARD*/
void USART_tx_string( char *data )
{
	while ((*data != '\0'))
	{
		while (!(UCSR0A & (1 <<UDRE0)));
		UDR0 = *data;
		data++;
	}
}


