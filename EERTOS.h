#ifndef EERTOS_H
#define EERTOS_H

#include "types.h"
//RTOS Config
#define	TaskQueueSize		20
#define MainTimerQueueSize	15

extern void InitRTOS(void);
extern void Idle(void);

typedef void (*TPTR)(void);

extern void SetTask(TPTR TS);
extern void SetTimerTask(TPTR TS, u16 NewTime);

extern void TaskManager(void);
extern void TimerService(void);

#endif
