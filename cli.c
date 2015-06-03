#include <string.h>
#include <stdlib.h>
#include <avr/io.h>
#include "cli.h"
#include "types.h"
#include "hal.h"
#include "process.h"

// definition commands word
#define _CMD_HELP   "help"
#define _CMD_CLEAR  "clear"

#define _CMD_SET          "set"
// arguments for get
	#define _SG_PWM       "pwm"				//0-100
	#define _SG_FREQ      "freq"			//5 10 20 30 40 50 in kHz
	#define _S_TIME_START "start_time"      //1000ms
	#define _S_TIME_STOP  "stop_time"       //1000ms
	#define _S_TIME_ON    "on_time"    	    //5000ms
	#define _S_TIME_OFF   "off_time"        //7000ms
	#define _S_CYCLES     "cycles"
	#define _S_LOW_PWM    "lpwm"			//10
	#define _S_HIGH_PWM   "hpwm"			//95


#define _CMD_GET          "get"
// arguments for get
//	#define _SG_PWM       "pwm"
	#define _G_TEMP       "temp"
	#define _G_STATE      "state"	//timings, pwm frequency, voltage, current, rest cycles, past cycles


#define _CMD_PROCESS  "process"
// arguments for process
	#define _P_START       "start"
	#define _P_STOP        "stop"

#ifdef _USE_COMPLETE

#define _NUM_OF_CMD 4	//5
#define _NUM_OF_SETGET_SCMD 2

#define _NUM_OF_SET_SCMD 8
#define _NUM_OF_GET_SCMD 3
#define _NUM_OF_PROCESS_SCMD 2

//available  commands
char * keyworld [] = {_CMD_HELP, _CMD_CLEAR, _CMD_SET, _CMD_GET/*, CMD_PROCESS*/};

// 'set' command argements
char * set_key [] = {_SG_PWM, _SG_FREQ, _S_TIME_START, _S_TIME_STOP, _S_TIME_ON, _S_TIME_OFF, _S_CYCLES, _S_CYCLES, _S_LOW_PWM, _S_HIGH_PWM};

// 'get' command argement
char * get_key [] = {_SG_PWM, _G_TEMP, _G_STATE};

// 'process' command argements
char * process_key [] = {_P_START, _P_STOP};

// array for comletion
char * compl_world [_NUM_OF_CMD + 1];

#endif //_USE_COMPLETE


void print_help (void)
{
#ifdef _USE_COMPLETE
	pprint ("Use TAB key for completion\n\rCommand:\n\r");
#endif
	pprint ("\tclear  - clear screen\n\r");
	pprint ("\tset {pwm, start_time, stop_time, off_time, cycles, lpwm, hpwm} VALUE\n\r");
	pprint ("\tget {state, pwm, temp}\n\r");
	pprint ("\tprocess {start, stop}\n\r");
}

//*****************************************************************************
// execute callback for microrl library
// do what you want here, but don't write to argv!!! read only!!
int execute (int argc, const char * const * argv)
{
	int i = 0;
	// just iterate through argv word and compare it with your commands
	while (i < argc) {
		if (strcmp (argv[i], _CMD_HELP) == 0) {
			pprint ("PWM stand\n\r");
			print_help ();        // print help
		} else if (strcmp (argv[i], _CMD_CLEAR) == 0) {
			pprint ("\033[2J");    // ESC seq for clear entire screen
			pprint ("\033[H");     // ESC seq for move cursor at left-top corner
		} else if (strcmp (argv[i], _CMD_SET) == 0) {
			if (++i < argc) {
				u16 setvalue;
				if ((i+1) < argc) {
					setvalue = atoi (argv[i+1]);
				} else {
					pprint ("Specify value for set command\n\r");
					return 1;
				}

				if (strcmp (argv[i], _SG_PWM)  == 0) {
					//TODO check val
					set_timet1_pwm(setvalue);
					process.forced_pwm = setvalue;
					pprint ("set pwm ok\n\r");
					return 0;
				} else if (strcmp (argv[i], _S_TIME_START)  == 0) {
					process.start_time_ms = setvalue;
					return 0;
				} else if (strcmp (argv[i], _S_TIME_STOP)  == 0) {
					process.stop_time_ms = setvalue;
					return 0;
				} else if (strcmp (argv[i], _S_TIME_ON)  == 0) {
					process.on_time_ms = setvalue;
					return 0;
				} else if (strcmp (argv[i], _S_TIME_OFF)  == 0) {
					process.off_time_ms = setvalue;
					return 0;
				} else if (strcmp (argv[i], _S_CYCLES)  == 0) {
					process.num_cycles = setvalue;
					return 0;
				} else if (strcmp (argv[i], _S_LOW_PWM)  == 0) {
					process.low_pwm = setvalue;
					return 0;
				} else if (strcmp (argv[i], _S_HIGH_PWM)  == 0) {
					process.high_pwm = setvalue;
					return 0;
				} else {
					pprint ("No such set command:");
					print ((char*)argv[i]);
					pprint ("\n\r");
					return 1;
				}
			} else {
				pprint ("specify set command, use Tab\n\r");
				return 1;
			}
		} else if (strcmp (argv[i], _CMD_GET) == 0) {
			if (++i < argc) {
				if (strcmp (argv[i], _SG_PWM)  == 0) {
					pprint ("forced pwm ");
					print  (ultoa32(process.forced_pwm));	
					pprint ("\%\n\r");
					return 0;
				} else if (strcmp (argv[i], _G_TEMP) == 0) {
					print_temp();
					return 0;
				} else if (strcmp (argv[i], _G_STATE) == 0) {
					process_print_state();
					return 0;
				} else {
					pprint ("No such get command:");
					print ((char*)argv[i]);
					pprint ("\n\r");
					return 1;
				}
			} else {
					pprint ("specify getcommand, use Tab\n\r");
				return 1;
			}

		} else if (strcmp (argv[i], _CMD_PROCESS) == 0) {
			if (++i < argc) {
				if (strcmp (argv[i], _P_START)  == 0) {
					process_start();
					return 0;
				} else if (strcmp (argv[i], _P_STOP) == 0) {
					process_stop();
					return 0;
				} else {
					pprint ("No such process command:");
					print ((char*)argv[i]);
					print ("\n\r");
					return 1;
				}
			} else {
				pprint ("specify process, use Tab\n\r");
				return 1;
			}
		} else {
			pprint ("command: '");
			print ((char*)argv[i]);
			pprint ("' Not found.\n\r");
		}
		i++;
	}
	return 0;
}

#ifdef _USE_COMPLETE
//*****************************************************************************
// completion callback for microrl library
char ** complet (int argc, const char * const * argv)
{
	int j = 0;

	compl_world [0] = NULL;

	// if there is token in cmdline
	if (argc == 1) {
		// get last entered token
		char * bit = (char*)argv [argc-1];
		// iterate through our available token and match it
		for (int i = 0; i < _NUM_OF_CMD; i++) {
			// if token is matched (text is part of our token starting from 0 char)
			if (strstr(keyworld [i], bit) == keyworld [i]) {
				// add it to completion set
				compl_world [j++] = keyworld [i];
			}
		}
	}	else if ((argc > 1) && ((strcmp (argv[0], _CMD_SET)==0) || 
							    (strcmp (argv[0], _CMD_GET)==0))) { // if command needs subcommands
		// iterate through subcommand
		for (int i = 0; i < _NUM_OF_SETGET_SCMD; i++) {
			if (strstr (set_get_key [i], argv [argc-1]) == set_get_key [i]) {
				compl_world [j++] = set_get_key [i];
			}
		}
	} else { // if there is no token in cmdline, just print all available token
		for (; j < _NUM_OF_CMD; j++) {
			compl_world[j] = keyworld [j];
		}
	}

	// note! last ptr in array always must be NULL!!!
	compl_world [j] = NULL;
	// return set of variants
	return compl_world;
}
#endif


#ifdef _USE_CTLR_C
void sigint (void)
{
	pprint ("^C catched!\n\r");
}
#endif
