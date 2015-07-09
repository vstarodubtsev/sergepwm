#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "hal.h"

void timer1_init(void)
{
	//fast pwm top ICR1
	// Clock value: 8M/1
	// Mode: Fast PWM top=ICR1
	// OC1A output: Non-Inv.
	// OC1B output: None.
	// Noise Canceler: Off
	// Input Capture on Falling Edge
	// Timer1 Overflow Interrupt: Off
	// Input Capture Interrupt: Off
	// Compare A Match Interrupt: Off
	// Compare B Match Interrupt: Off
	TCCR1A = 0ul << WGM10 | 1ul << WGM11 | 0ul << COM1B0 | 0ul << COM1B1 | 0ul << COM1A0 | 1ul << COM1A1;
	TCCR1B = 1ul << CS10 | 0ul << CS11 | 0ul << CS12 | 1ul << WGM12 | 1ul << WGM13 | 0ul << ICES1 | 0ul << ICNC1;

	TCNT1H = 0x00;
	TCNT1L = 0x00;
	ICR1H=0x01; //400 * 20kHz = 8MHz
	ICR1L=0x90;
	//ICR1 = TOP_PWM;
	OCR1AH = 0x00;
	OCR1AL = 0x00;
	OCR1BH = 0x00;
	OCR1BL = 0x00;
	//TIMSK |= 1ul << TOIE1; //overflow interrupt
}
/*
void timer1_set_freq(u8 freq_khz)
{
	if (freq_khz = 0) { //pwm_off
		set_timet1_pwm(0);
		return;
	}
	//1000

	ICR 1000 F 1k
	ICR 100	 F = 10K

	u16 icr = 1000/

	ICR1H=0x03; //999
	ICR1L=0xE7;
}
*/

void timer2_init(void)
{
	//1ms ticks
	// Clock value: 8M/64
	// Timer2 Compate Interrupt: On
	// TOP: OCR2
	TCCR2 = 0ul << CS20 | 0ul << CS21 | 1ul << CS22 | 0ul <<  WGM20 | 1ul <<  WGM21 | 0ul <<  COM20 | 0ul <<  COM21; 
	TCNT2 = 0;
	OCR2 = 124;	//125 * 64 = 8000

	TIMSK |= 1ul << OCIE2; //output compare interrupt
}

void set_timet1_pwm (u8 pwm_persent)
{
	if (pwm_persent > 100)
		pwm_persent = 100;

	u16 val = ((u16)pwm_persent << 2);
	OCR1AH = val >> 8;
	OCR1AL = val & 0xFF;
}

void init_gpio(void)
{
	RGB_DDR  |=  (1<<RGB_RED_PIN)|(1<<RGB_BLUE_PIN)|(1<<RGB_GREEN_PIN);
	RGB_PORT &= ~((1<<RGB_RED_PIN)|(1<<RGB_BLUE_PIN)|(1<<RGB_GREEN_PIN));
	//RGB_PORT |= (1<<RGB_RED_PIN);
	
	DDRB |= 1ul << 1;
//	PORTB  |= 1ul << 1;
	
}

void led_off(void)
{
	RGB_PORT &= ~((1<<RGB_RED_PIN)|(1<<RGB_BLUE_PIN)|(1<<RGB_GREEN_PIN));
}
void led_red_on(void)
{
	led_off();
	RGB_PORT |= (1<<RGB_RED_PIN);
}

void led_green_on(void)
{
	led_off();
	RGB_PORT |= (1<<RGB_GREEN_PIN);
}

void led_blue_on(void)
{
	led_off();
	RGB_PORT |= (1<<RGB_BLUE_PIN);
}

void init_adc(void)
{
	// Clock: 16ÌÃö/32=500êÃö, time: 13*1/500êÃö=26ìêñ,
	// Vref=Avcc, MUX4:0=0 - channel0 8bit result
	ADMUX = (0 << REFS1) | (1 << REFS0) | (1 << ADLAR) | (0 << MUX3) | (0 << MUX2)  | (0 << MUX1) | (0 << MUX0);
	ADCSRA = (1 << ADEN)  | (1 << ADSC) | (1 << ADPS2) | (1 << ADPS0);
}

u8 get_adc(u8 ch)
{
	ADCSRA |= (1 << ADSC);      // Start ADC conversion

	while(ADCSRA & (1 << ADSC)); // Wait until conversion completed

	return ADCH;
}

u16 get_target_voltage(void)
{
	ADMUX = (0 << REFS1) | (1 << REFS0) | (1 << ADLAR) | (0 << MUX3) | (1 << MUX2)  | (0 << MUX1) | (1 << MUX0); //adc5

	u16 vol = 0;
	for (u8 i = 0; i < 8; i++)
		vol+= get_adc(ADC_CH_INPUT);

	vol>>=3;

	return vol*1.22;
}
//decivoltage
u16 get_bulp_voltage(void)
{
	ADMUX = (0 << REFS1) | (1 << REFS0) | (1 << ADLAR) | (0 << MUX3) | (1 << MUX2)  | (0 << MUX1) | (0 << MUX0); //adc5

	u16 vol = 0;
	for (u8 i = 0; i < 16; i++)
		vol+= get_adc(ADC_CH_BULB);

	vol>>=4;

	return vol*1.22;
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
	UCSRB |= (1 << RXCIE );
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

void print (const char * str)
{
	uart_puts (str);
}

void pgmspace_print (const char * pstr)
{
	char c;
	while (c = pgm_read_byte(pstr++) ) {
		uart_putc(c);
	}
}
