#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "hal.h"

void timer1_init(void)
{
	//fast pwm top ICR1
	// Clock value: 8M/8
	// Mode: Fast PWM top=ICR1
	// OC1A output: Non-Inv.
	// OC1B output: None.
	// Noise Canceler: Off
	// Input Capture on Falling Edge
	// Timer1 Overflow Interrupt: On
	// Input Capture Interrupt: Off
	// Compare A Match Interrupt: Off
	// Compare B Match Interrupt: Off
	TCCR1A = 0ul << WGM10 | 1ul << WGM11 | 0ul << COM1B0 | 1ul << COM1B1 | 0ul << COM1A0 | 1ul << COM1A1;
	TCCR1B = 0ul << CS10 | 1ul << CS11 | 0ul << CS12 | 1ul << WGM12 | 1ul << WGM13 | 0ul << ICES1 | 0ul << ICNC1;

	TCNT1H = 0x00;
	TCNT1L = 0x00;
	ICR1H=0x03; //999
	ICR1L=0xE7;
	//ICR1 = TOP_PWM;
	OCR1AH = 0x00;
	OCR1AL = 0x00;
	OCR1BH = 0x00;
	OCR1BL = 0x00;
	TIMSK = 1ul << TOIE1; //overflow interrupt
}

void set_timet1_pwm (u8 pwm_persent)
{
	u16 val = (u16)pwm_persent * 10 - 1;
	OCR1AH = val >> 8;
	OCR1AL = val & 0xFF;
}

void init_gpio(void)
{
	RGB_DDR  |=  (1<<RGB_RED_PIN)|(1<<RGB_BLUE_PIN)|(1<<RGB_GREEN_PIN);
	RGB_PORT &= ~((1<<RGB_RED_PIN)|(1<<RGB_BLUE_PIN)|(1<<RGB_GREEN_PIN));
	//RGB_PORT |= (1<<RGB_RED_PIN);
	
	
}

void init_adc(void)
{
	// Clock: 16ÌÃö/32=500êÃö, time: 13*1/500êÃö=26ìêñ,
	// Vref=Avcc, MUX4:0=0 - channel0 8bit result
	ADMUX = (0 << REFS1) | (1 << REFS0) | (1 << ADLAR) | (0 << MUX3) | (0 << MUX2)  | (0 << MUX1) | (0 << MUX0);
	ADCSRA = (1 << ADEN)  | (1 << ADSC) | (1 << ADPS2) | (1 << ADPS0);
}

u8 get_adc(void)
{
	ADCSRA |= (1 << ADSC);      // Start ADC conversion

	while(ADCSRA & (1 << ADSC)); // Wait until conversion completed

	return ADCH;
}

u16 get_voltage(void)
{
	return 0;
}

void uart_init(void)
{
	//DDRD |= 1 << 0 | 1 << 1;
	//PORTD |= 1 << 0 | 1 << 1;

	// Turn on USART hardware (RX, TX)
	UCSRB |= (1 << RXEN) | (1 << TXEN);
	// 8 bit char sizes
	UCSRC |= (1 << UCSZ0) | (1 << UCSZ1);
	// Set baud rate
	UBRRH = (BAUD_PRESCALE >> 8);
	UBRRL = BAUD_PRESCALE;
	// Enable the USART Receive interrupt
	//UCSRB |= (1 << RXCIE );
}

void uart_putc (char send)
{
	// Do nothing for a bit if there is already
	// data waiting in the hardware to be sent
	while ((UCSRA & (1 << UDRE)) == 0) {};
	UDR = send;
}

void uart_puts (const char *send)
{
	// Cycle through each character individually
	while (*send) {
		uart_putc(*send++);
	}
}

/*
ISR (USART_RXC_vect)
{
	uart_putc(UDR); //echo
}
*/
