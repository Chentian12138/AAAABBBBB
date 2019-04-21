#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdlib.h>

void read_adc(void);						//Read angle
void adc_init(void);						// Initialize ADC
volatile unsigned int angle;				// angle value


int main(void)
{
	DDRD = 0xFF;

	TCCR0B=3;
	TCCR0A=0x83;
  adc_init(); //initialize adc
  _delay_ms(100);

	while (1)
	{
    read_adc();
		OCR0A = angle; // set the angle
		_delay_ms(500);
		OCR0A = angle; // set the angle
		_delay_ms(500);
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
	angle = 0;						//initialize adc_temp variable
	while (i--) {
		ADCSRA |= (1<<ADSC);
		while(ADCSRA & (1<<ADSC));
		angle+= ADC;
		_delay_ms(50);
	}
	angle = angle / 4;			// Average a few samples

}
