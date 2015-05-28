#include "EERTOS.h"
#include "types.h"
#include "hal.h"
#include "ds18x20.h"
#include "microrl/microrl.h"
#include "cli.h"

// global variables
u16 num_cycles;

//microrl object and pointer on it
microrl_t rl;
microrl_t * prl = &rl;

// Task's poll intervals
const u16 uart_periodic_timer_ms = 2000;
const u16 led_short_ms = 250;
const u16 pwm_cycle_ms = 3000;

// Function prototypes ==========================================================
void uart_periodic(void);
void led_blink(void);
//

// Interrupts ===================================================================

ISR(TIMER2_OVF_vect) //1ms periodic
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


void uart_periodic(void)
{
	uart_puts("now ");
	u32 frac = 0;
	u8 temp = 0; 
	convert(temp_18b20(), &temp, &frac);
	char *res;
	res = ultoa(temp);
	uart_puts(res);
	uart_puts(".");
	res = ultoa(frac);
	uart_puts(res);
	uart_puts(" deg\n\r");

	SetTimerTask(uart_periodic, uart_periodic_timer_ms);
}

void led_blink(void)
{
	//RGB_PORT ^= (1<<RGB_RED_PIN)|(1<<RGB_BLUE_PIN)|(1<<RGB_GREEN_PIN);	//blue blink
	SetTimerTask(led_blink, led_short_ms);
}

u8 dc_now;
void pwm_cycle(void)
{
	if (dc_now > 100)
		dc_now = 0;
	else
		dc_now++;

	set_timet1_pwm(dc_now);

	num_cycles++;
	SetTimerTask(pwm_cycle, 100);
}

void main(void) __attribute__((noreturn));
void main(void)
{
	init_gpio();
	//init_adc();
	uart_init();
	timer1_init();
	timer2_init();
	dc_now = 0;
	set_timet1_pwm(dc_now);
	num_cycles = 0;

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

	sei();//enable interrupts

	// backgroud tast start
	SetTask(led_blink);
	//SetTimerTask(uart_periodic, uart_periodic_timer_ms);

	SetTask(pwm_cycle);

	for(;;) {
		TaskManager();
	}

}
//#error test
