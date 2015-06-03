#include "EERTOS.h"
#include "types.h"
#include "hal.h"
#include "ds18x20.h"
#include "microrl/microrl.h"
#include "cli.h"
#include "process.h"

// global variables
u8 pwm_global;

//microrl object and pointer on it
microrl_t rl;
microrl_t * prl = &rl;

// Task's poll intervals
const u16 led_short_ms = 10;
const u16 pwm_cycle_ms = 3000;

// Function prototypes ==========================================================
void led_blink(void);
//

// Interrupts ===================================================================

ISR(TIMER2_COMP_vect) //1ms periodic
{
	TimerService();
}

ISR(USART_RXC_vect) //UART recieve 
{
	microrl_insert_char (prl, UDR);
}

/*unsigned long to ansi*/
char *ultoa(u32 num)
{
	static char buf[11];
	char *ptr;
	ptr=&buf[10];
	*ptr='\0';
	do {
			*--ptr="0123456789"[num%10];
			num/=10;
	} while(num!=0);

	return(ptr);
}

void print_temp(void)
{
	//FIXME negative temp
	u32 frac = 0;
	u8 temp = 0; 
	convert(temp_18b20(), &temp, &frac);
	char *res;
	res = ultoa(temp);
	uart_puts("temp =");
	uart_puts(res);
	uart_puts(".");
	res = ultoa(frac);
	uart_puts(res);
	uart_puts(" deg\n\r");
}

void led_blink(void)
{
	//RGB_PORT ^= (1<<RGB_RED_PIN)|(1<<RGB_BLUE_PIN)|(1<<RGB_GREEN_PIN);	//blue blink
	SetTimerTask(led_blink, led_short_ms);
}

void main(void) __attribute__((noreturn));
void main(void)
{
	init_gpio();
	//init_adc();
	uart_init();
	timer1_init();
	timer2_init();
	set_timet1_pwm(0);

	InitRTOS();

	microrl_init (prl, print);
	// set callback for execute
	microrl_set_execute_callback (prl, execute);

#ifdef _USE_COMPLETE
	// set callback for completion
	microrl_set_complete_callback (prl, complet);
#endif

#ifdef _USE_CTLR_C
	// set callback for Ctrl+C
	microrl_set_sigint_callback (prl, sigint);
#endif

	process_init();

	sei();//enable interrupts

	// backgroud tast start
	SetTimerTask(led_blink, led_short_ms);
	SetTimerTask(process_exec, 10);

	//SetTask(pwm_cycle);

	pprint("digital started\n\r");
	for(;;) {
		TaskManager();
	}

}
//#error test
