#include "EERTOS.h"

/**
 * ������� �����.
 */
volatile static struct	{
	TPTR GoToTask; // ��������� ��������
	u16 Time;	// �������� � ��
} MainTimer[MainTimerQueueSize + 1];	// ������� ��������

/**
 * RTOS ����������. ������� ��������
 */
inline void InitRTOS(void)
{
	u8 index;

	for (index = 0; index != MainTimerQueueSize + 1; index++) { // �������� ��� �������.
		MainTimer[index].GoToTask = Idle;
		MainTimer[index].Time = 0;
	}
}

//������ ��������� - ������� ����.
void Idle(void)
{

}

/**
 * ������� ��������� ������ �� �������. ������������ ��������� - ��������� �� �������,
 * ����� �������� � ����� ���������� �������.
 */
void SetTimerTask(TPTR TS, u16 NewTime)
{
	u8 index = 0;

	for (index = 0; index != MainTimerQueueSize + 1; ++index) {	//����������� ������� ��������
		if (MainTimer[index].GoToTask == TS) {	// ���� ��� ���� ������ � ����� �������
			MainTimer[index].Time = NewTime;	// �������������� �� ��������
			return;
		}
	}

	for (index = 0; index != MainTimerQueueSize + 1; ++index) {	// ���� �� ������� ������� ������, �� ���� ����� ������
		if (MainTimer[index].GoToTask == Idle) {
			MainTimer[index].GoToTask = TS;	// ��������� ���� �������� ������
			MainTimer[index].Time = NewTime;	// � ���� �������� �������
			return;
		}
	}
}

void SetTask(TPTR TS)   // ��������� ������ � ������� ��� ������������ ����������
{
	SetTimerTask(TS, 0);
}

/**
 * ��������� ����� ��. �������� �� ������� ������ � ���������� �� ����������.
*/
inline void TaskManager(void)
{
	u8 index = 0;
	TPTR task;

	for (index = 0; index != MainTimerQueueSize + 1; ++index) { // ����������� ������� � ������� ������ ������
		if ((MainTimer[index].GoToTask != Idle) && (MainTimer[index].Time == 0)) { // ���������� ������ ������ � ��, ����� ������� ��� �� �������
			task = MainTimer[index].GoToTask; // �������� ������
			MainTimer[index].GoToTask = Idle; // ������ �������
			(task)();	// ��������� � ������
			return; // ����� �� ���������� �����
		}
	}

	Idle(); // ������ ������, ������ ��� - �������
}

/**
 * ������ �������� ����. ������ ���������� �� ���������� ��� � 1��.
 */
inline void TimerService(void)
{
	u8 index;

	for (index = 0; index != MainTimerQueueSize + 1; index++) {	// ����������� ������� ��������
		if ((MainTimer[index].GoToTask != Idle) && // ���� �� �������� �
		    (MainTimer[index].Time > 0)) {	// ��� ������� ������ �� ��������, ��
			MainTimer[index].Time--;	// ������� ��� ���.
		};
	}
}

