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

// PCB configuration

#define RGB_DDR				DDRD
#define RGB_PORT			PORTD
#define RGB_RED_PIN 		5		//PD5
#define RGB_GREEN_PIN		3		//PD3
#define RGB_BLUE_PIN		4		//PD4


#define DS1820_PORT			PORTD	//PD2
#define DS1820_DDR			DDRD
#define DS1820_PIN			PIND
#define DS1820_PIN_NUM		2




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
