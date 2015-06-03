#include "process.h"
#include "EERTOS.h"

struct process_t process;
static u16 runtime_timeout;

const u16 time_step = 10; //exec poll timeout in ms

static void process_exec(void);

void process_init(void)
{
	process.state  = st_terminated;

	process.start_time_ms = 2000;
	process.on_time_ms	= 5000;
	process.stop_time_ms = 1000;
	process.off_time_ms = 5000;

	process.high_pwm = 100;
	process.low_pwm = 0;
	
	process.forced_pwm = 0;

	process.num_cycles = 100;
	process.passed_cycles = 0;
}

void process_start(void)
{
	if (process.state == st_terminated) {
		process.state = st_starting;
		runtime_timeout = process.start_time_ms;
		led_red_on();
		process_exec();
		pprint ("process started\n\r");
	} else {
		pprint ("process already running\n\r");
	}
}

void process_stop(void)
{
	if (process.state == st_terminated) {
		pprint ("process already stopped\n\r");
	} else {
		process.state = st_terminated;
		pprint ("process stopped\n\r");
	}
}

void print_process_state(void)
{
	pprint ("process state:");
	switch (process.state)
	{
	case st_starting:
	pprint ("starting");
	break;

	case st_runing:
	pprint ("running");
	break;

	case st_stopping:
	pprint ("stopping");
	break;

	case st_off:
	pprint ("off");
	break;

	case st_terminated:
	pprint ("terminated");
	break;
	}

	pprint ("\n\r");
}

void process_print_state(void)
{
	print_process_state();

	pprint ("start time:");
	print  (ultoa32(process.start_time_ms));
	pprint ("ms\n\r");

	pprint ("stop time:");
	print  (ultoa32(process.stop_time_ms));
	pprint ("ms\n\r");

	pprint ("on time:");
	print  (ultoa32(process.on_time_ms));
	pprint ("ms\n\r");

	pprint ("off time:");
	print  (ultoa32(process.off_time_ms));
	pprint ("ms\n\r");

	pprint ("cycles:");
	print  (ultoa32(process.num_cycles));
	pprint ("\n\r");

	pprint ("passed cycles:");
	print  (ultoa32(process.passed_cycles));
	pprint ("\n\r");

	pprint ("low pwm:");
	print  (ultoa32(process.low_pwm));
	pprint ("\%\n\r");

	pprint ("high pwm:");
	print  (ultoa32(process.high_pwm));
	pprint ("\%\n\r");

}



static void process_exec(void)	//EERTOS TASK
{
	switch (process.state)
	{
	case st_starting:
		if (runtime_timeout > time_step) {
//			u8 pwm = process.high_pwm - (runtime_timeout * process.high_pwm) / process.start_time_ms;
			u8 pwm = 100 - runtime_timeout / (process.start_time_ms/100);

			set_timet1_pwm(pwm);

			runtime_timeout-=time_step;
		} else {
			led_green_on();
			runtime_timeout = process.on_time_ms;
			process.state = st_runing;
		}
	break;

	case st_runing:
		if (runtime_timeout > time_step) {

			set_timet1_pwm(100);
//			set_timet1_pwm(process.high_pwm);

			runtime_timeout-=time_step;
		} else {
			led_blue_on();
			runtime_timeout = process.stop_time_ms;
			process.state = st_stopping;
		}
	break;

	case st_stopping:
		if (runtime_timeout > time_step) {
//			u8 pwm_delt = process.high_pwm - process.low_pwm;
//			u8 pwm = process.high_pwm - (runtime_timeout * process.high_pwm) / process.start_time_ms;
			u8 pwm = runtime_timeout / (process.stop_time_ms/100);

			set_timet1_pwm(pwm);

			runtime_timeout-=time_step;
		} else {
			led_off();
			set_timet1_pwm(0);
			runtime_timeout = process.off_time_ms;
			process.state = st_off;
		}
	break;

	case st_off:
		if (runtime_timeout > time_step) {

			runtime_timeout-=time_step;
		} else {
			process.passed_cycles++;

			if (process.num_cycles) {
				process.num_cycles--;
				runtime_timeout = process.start_time_ms;
				led_red_on();
				process.state = st_starting;
			} else {
				process.state = st_terminated;
			}
		}
	break;

	case st_terminated:
		runtime_timeout = 0;
		set_timet1_pwm(0);
		led_off();
		return; //finish process
	break;
	}
	
	SetTimerTask(process_exec, time_step);
}
