/*******************************************
*hardware abstact file
*
******************************************/

#ifndef HAL_H
#define HAL_H

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

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

#define ADC_CH_INPUT		5
#define ADC_CH_BULB			4


//functions declarations
void init_gpio(void);

void led_off(void);
void led_red_on(void);
void led_green_on(void);
void led_blue_on(void);

void timer1_init(void);
void timer2_init(void);
void set_timet1_pwm (u8 pwm_persent);
void timer1_set_freq(u8 freq_khz);

void init_adc(void);

void uart_init(void);
void uart_putc (char send);
void uart_puts (const char *send);
void print (const char * str);
void pgmspace_print (const char * pstr);
#define pprint(a) pgmspace_print(PSTR(a))

const char *ultoa32(u32 num);

u16 get_target_voltage(void);
u16 get_bulp_voltage(void);

#endif //HAL_H
