#include "EERTOS.h"

/**
 * Очередь задач.
 */
volatile static struct	{
	TPTR GoToTask; // Указатель перехода
	u16 Time;	// Выдержка в мс
} MainTimer[MainTimerQueueSize + 1];	// Очередь таймеров

/**
 * RTOS Подготовка. Очистка очередей
 */
inline void InitRTOS(void)
{
	u8 index;

	for (index = 0; index != MainTimerQueueSize + 1; index++) { // Обнуляем все таймеры.
		MainTimer[index].GoToTask = Idle;
		MainTimer[index].Time = 0;
	}
}

//Пустая процедура - простой ядра.
void Idle(void)
{

}

/**
 * Функция установки задачи по таймеру. Передаваемые параметры - указатель на функцию,
 * Время выдержки в тиках системного таймера.
 */
void SetTimerTask(TPTR TS, u16 NewTime)
{
	u8 index = 0;

	for (index = 0; index != MainTimerQueueSize + 1; ++index) {	//Прочесываем очередь таймеров
		if (MainTimer[index].GoToTask == TS) {	// Если уже есть запись с таким адресом
			MainTimer[index].Time = NewTime;	// Перезаписываем ей выдержку
			return;
		}
	}

	for (index = 0; index != MainTimerQueueSize + 1; ++index) {	// Если не находим похожий таймер, то ищем любой пустой
		if (MainTimer[index].GoToTask == Idle) {
			MainTimer[index].GoToTask = TS;	// Заполняем поле перехода задачи
			MainTimer[index].Time = NewTime;	// И поле выдержки времени
			return;
		}
	}
}

void SetTask(TPTR TS)   // Поставить задачу в очередь для немедленного выполнения
{
	SetTimerTask(TS, 0);
}

/**
 * Диспетчер задач ОС. Выбирает из очереди задачи и отправляет на выполнение.
*/
inline void TaskManager(void)
{
	u8 index = 0;
	TPTR task;

	for (index = 0; index != MainTimerQueueSize + 1; ++index) { // Прочесываем очередь в поисках нужной задачи
		if ((MainTimer[index].GoToTask != Idle) && (MainTimer[index].Time == 0)) { // пропускаем пустые задачи и те, время которых еще не подошло
			task = MainTimer[index].GoToTask; // запомним задачу
			MainTimer[index].GoToTask = Idle; // ставим затычку
			(task)();	// Переходим к задаче
			return; // выход до следующего цикла
		}
	}

	Idle(); // обошли задачи, нужных нет - простой
}

/**
 * Служба таймеров ядра. Должна вызываться из прерывания раз в 1мс.
 */
inline void TimerService(void)
{
	u8 index;

	for (index = 0; index != MainTimerQueueSize + 1; index++) {	// Прочесываем очередь таймеров
		if ((MainTimer[index].GoToTask != Idle) && // Если не пустышка и
		    (MainTimer[index].Time > 0)) {	// для которой таймер не выщелкал, то
			MainTimer[index].Time--;	// щелкаем еще раз.
		};
	}
}

