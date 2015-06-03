#ifndef PROCESS_H
#define PROCESS_H

#include "hal.h"

enum process_state_t
{
	st_terminated,
//	st_stop,
	st_starting,
	st_runing,
	st_stopping,
	st_off
};

struct process_t
{
	u16 start_time_ms;
	u16 on_time_ms;
	u16 stop_time_ms;
	u16 off_time_ms;
	u16 num_cycles;
	u16 passed_cycles;
	u8  low_pwm;
	u8  high_pwm;
	u8  forced_pwm;
	enum process_state_t state;
};

extern struct process_t process;


void process_init(void);
void process_start(void);
void process_stop(void);
void process_print_state(void);




#endif //PROCESS_H
