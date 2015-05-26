#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define SKIP_ROM 		0xCC	// Пропустить индентификацию
#define CONVERT_T 		0x44	// Измерить температуру
#define READ_SCRATCHPAD 0xBE	// Прочитать измеренное

//определяем порт и бит к которому подключено устройство
#define OWI_PORT PORTD
#define OWI_DDR DDRD
#define OWI_PIN PIND
#define OWI_BIT 2

#define OWI_LOW OWI_DDR |= 1<<OWI_BIT		//притягиваем шину к 0
#define OWI_HIGH OWI_DDR &= ~(1<<OWI_BIT)	//отпускаем шину

//функция определяет есть ли устройство на шине
static char OWI_find(void)
{
	char device;
	OWI_LOW; 			//притягиваем шину к 0
	_delay_us(485);		//ждем минимум 480мкс
	OWI_HIGH;			//отпускаем шину
	_delay_us(65);		//ждем минимум 60мкс и смотрим что на шине

	if((OWI_PIN & (1<<OWI_BIT)) ==0x00)//смотрим есть ли ответ
		device = 1; 	//на шине есть устройство
	else
		device = 0; 	//на шине нет устройства

	_delay_us(420);		//ждем оставшееся время до 480мкс
	return device;
}

// функция отправляет бит на устройство
static void OWI_write_bit (char bit)
{
	OWI_LOW;			//логический "0"
	_delay_us(2);
	if(bit) 
		OWI_HIGH;	//отпускаем шину

	_delay_us(65);
	OWI_HIGH; 			//отпускаем шину
}

//функция посылает байт на устройство
static void OWI_write_byte(unsigned char c)
{
	char i;
	for(i = 0; i < 8; i++)//в цикле посылаем побитно
	{
		if(c & (1<<i))//если бит=1 посылаем 1
			OWI_write_bit(1);
		else  //иначе посылаем 0
			OWI_write_bit(0);
	}
}

//функция чтения одного бита
static char OWI_read_bit (void)
{
	char OWI_rbit; 		//переменная хранения бита
	OWI_LOW;			//логический "0"
	_delay_us(2);
	OWI_HIGH; 			//отпускаем шину
	_delay_us(13);
	OWI_rbit=(OWI_PIN & (1<<OWI_BIT))>>OWI_BIT;
	_delay_us(45);		//задержка до окончания тайм-слота

	return OWI_rbit;
}

//функция читает один байт с устройства 1-wire
static unsigned char OWI_read_byte(void)
{
	char data=0,i;
	for(i = 0; i < 8; i++) //в цикле смотрим что на шине и сохраняем значение
		data|= OWI_read_bit()<<i;//читаем очередной бит

	return data;
}


//функция преобразует полученные с датчика 18b20 данные в температуру
int  temp_18b20(void)
{
	unsigned char B;
	unsigned int ds18_temp=0;
	if(OWI_find()==1)//если есть устройство на шине
	{
		OWI_write_byte(SKIP_ROM);		//пропустить ROM код, мы знаем, что у нас одно устройство или передаем всем
		OWI_write_byte(CONVERT_T);		//преобразовать температуру
//		_delay_ms(94);					//преобразование в  9 битном режиме занимает  94ms
//		_delay_ms(188);					//преобразование в 10 битном режиме занимает 188ms
//		_delay_ms(375);					//преобразование в 11 битном режиме занимает 375ms
		_delay_ms(750);					//преобразование в 12 битном режиме занимает 750ms
		OWI_find();						//снова посылаем Presence и Reset
		OWI_write_byte(SKIP_ROM);
		OWI_write_byte(READ_SCRATCHPAD);//передать байты ведущему(у 18b20 в первых двух содержится температура)

		B = OWI_read_byte(); 			//читаем бит LS
		ds18_temp = OWI_read_byte(); 	//читаем бит MS
		ds18_temp = (ds18_temp<<8)|B;	//уладываем биты в последовательности MS потом LS
	}
	// else return можно сделать возврат числа что датчика нет
	return ds18_temp;//возвращаем int (MS,LS)
}


void convert (unsigned int td, char* int_part, unsigned int* frac_part)
{
	*frac_part = (td & 0x0F) * 625; // * 625;

	char data = td>>4;
	*int_part = data & 0xFF;
}
