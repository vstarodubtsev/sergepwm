#include "process.h"

struct process_t process;

void process_init(void)
{
	process.state  = st_terminated;

	process.start_time_ms = 2000;
	process.on_time_ms	= 5000;
	process.stop_time_ms = 1000;
	process.off_time_ms = 7000;

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

void process_print_state(void)
{
	pprint ("process state:\n\r");
	pprint ("start time:");
	
	pprint ("\n\r");
	
	
	pprint ("stop time:\n\r");
		
	pprint ("\n\r");
}

void process_exec(void)	//EERTOS TASK
{
	
}

