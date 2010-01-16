#include <macros.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <./hd44780/hd44780.h>

#define TMIN 	25 // minimum temperature in Celsius
#define TMAX 	50 // maximum temperature in Celsius
#define TOL 	1  // temperature tolerance

void init_controller() {
	// DEFINE I/O PORTS
	/*
		Port D as output for the LCD
	*/
	DDRD = 0xFF; 

	/*	
		Port B as input buttons, LCD output flags, PWM output
		PB7 - LCD RS (output)
		PB6 - LCD E (output)
		PB5 - + button (input)
		PB4 - - button (input)
		PB3 - *
		PB2 - *
		PB1 - PWM (output)
		PB0 - *
	*/
	DDRB = 0b11000010;

	/*
		Port C as sensor input, led output
		PC6 - used as RESET
		PC5 - Lifeline (output)
		PC4 - Red_LED (output)
		PC3 - Yellow_LED (output)
		PC2 - Green_LED (output)
		PC1 - *
		PC0 - LM35 sensor (input)
	*/
	DDRC = 0b00111100;

	/*
		Turn on the LEDs
	*/
	C_SETBIT(Lifeline);
	Delay_ms(500);
	C_SETBIT(Red_LED);
	Delay_ms(500);
	C_SETBIT(Yellow_LED);
	Delay_ms(500);
	C_SETBIT(Green_LED);
}

void init_ADC() {
	ADMUX=(1<<REFS0);// For Aref=AVcc;
	ADCSRA=(1<<ADEN)|(7<<ADPS0);
}

void init_PWM() {
	ICR1 = 320; //TOP value for 25 KHz
	TCCR1A = _BV(WGM11);
	TCCR1B = _BV(WGM13);
	TCCR1A |= _BV(COM1A1) | _BV(COM1A0);
	TCCR1B |= _BV(CS10);
}

void clearLEDS() {
	C_CLEARBIT(Red_LED);
	C_CLEARBIT(Yellow_LED);
	C_CLEARBIT(Green_LED);
}

void setDutyCycle1() {
	clearLEDS();
	C_SETBIT(Green_LED);
	OCR1A = 255;
}
	
void setDutyCycle2() {
	clearLEDS();
	C_SETBIT(Yellow_LED);
	OCR1A = 127;
}

void setDutyCycle3() {	
	clearLEDS();
	C_SETBIT(Red_LED);
	OCR1A = 0;
}

uint16_t ReadADC(uint8_t ch) {
   //Select ADC Channel ch must be 0-7
   ch=ch&0b00000111;
   ADMUX|=ch;

   //Start Single conversion
   ADCSRA|=(1<<ADSC);

   //Wait for conversion to complete
   while(!(ADCSRA & (1<<ADIF)));

   //Clear ADIF by writing one to it
   ADCSRA|=(1<<ADIF);

   return(ADC);
}

int main() {
	uint8_t refTemp = 30; // reference
	uint8_t check, prev_check = 0;
	short up = 1, down = 1;
	uint16_t adc_in; // ADC value
	uint8_t temp = 1; // real temperature
	init_controller();
	init_PWM();
	lcd_init();
	sei(); // turn on interrupts
	lcd_clrscr();
	lcd_puts("Spinning");
	Delay_s(2); // 2 seconds delay
	init_ADC();
	refTemp = eeprom_read_byte((uint8_t*)0);
	if (!(refTemp >= TMIN && refTemp <= TMAX))
		refTemp = 30;
	while(1) {
		if (C_CHECKBIT(Minus_REF)) down = 0;
		if (C_CHECKBIT(Minus_REF) == 0 && down ==0)	{
			down = 1;
			if (refTemp > TMIN) refTemp--;
		}
		if (C_CHECKBIT(Plus_REF)) up = 0;
		if (C_CHECKBIT(Plus_REF) == 0 && up ==0) {
			up = 1;
			if (refTemp < TMAX) refTemp++;
		}
		eeprom_write_byte ((uint8_t *)0, refTemp); 
		adc_in = ReadADC(0);
		temp = adc_in/2;
		lcd_puts2("T%d-R%dC", temp, refTemp);
		if ((temp - refTemp) > TOL)
			check = 3;
		else if ((refTemp - temp) > TOL)
			check = 1;
		else
			check = 2;
		switch(check) {
			case 1:
				if (prev_check == 1) break;
				setDutyCycle1();
				prev_check = 1;
				break;
			case 2:
				if (prev_check == 2) break;
				setDutyCycle2();
				prev_check = 2;
				break;
			case 3:
				if (prev_check == 3) break;
				setDutyCycle3();
				prev_check = 3;
				break;
		}
	}

	return 1;
}
