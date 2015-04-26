/*******************************************
*hardware abstact file
*
******************************************/

#ifndef HAL_H
#define HAL_H

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "types.h"

#ifndef cli
#define cli()           __asm__ __volatile__ ("cli" ::)
#endif
#ifndef sei
#define sei()           __asm__ __volatile__ ("sei" ::)
#endif

#define BAUD 38400
#define BAUD_PRESCALE (((F_CPU / (BAUD * 16UL))) - 1)

// define PCB configuration here
#define GREEN_LED_PIN		0	  	//PB0
#define LED_PORT    		PORTB
#define LED_DDR				DDRB

//functions declarations
void init_gpio(void);
u8 get_current_from_switch(void);

void timer1_init(void);
void set_timet1_pwm (u8 pwm_persent);

void init_adc(void);
u8  get_adc(void);

void uart_init(void);
void uart_putc (char send);
void uart_puts (const char *send);

#endif //HAL_H
