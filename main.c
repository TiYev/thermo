#include <avr/io.h>
#include <util/delay.h>
#include "temp.h"

#define TEMP_DDR	DDRD
#define TEMP_PIN	PIND
#define TEMP_BIT	0

unsigned int dec27seg(unsigned int dec_dig) {
	switch(dec_dig) {
		case 0:
			return 0x3F;
			break;
		case 1:
			return 0x06;
			break;
		case 2:
			return 0x5B;
			break;
		case 3:
			return 0x4F;
			break;
		case 4:
			return 0x66;
			break;
		case 5:
			return 0x6D;
			break;
		case 6:
			return 0x7D;
			break;
		case 7:
			return 0x07;
			break;
		case 8:
			return 0xFF;
			break;
		case 9:
			return 0xEF;
			break;
		default:
			return 0x00;
	}
}

void print_err() {
	PORTC = 0x79;
	PORTA = 0x50;
	PORTD &= 0xFD;
}

void print_num(int num) {
	if ((num <= - 100) || (num >= 100)) {
		print_err();
		return;
	}
	PORTC = dec27seg(abs(num) / 10);
	PORTA = dec27seg(abs(num) % 10);
	if (num < 0) {
		PORTD |= 0x02;
	}
	if (num >= 0) {
		PORTD &= 0xFD;
	}
}

/* Точка входа в программу */
int main (void) {
	/* Настройка портов */
	DDRC = 0xFF;
	DDRA = 0xFF;
	DDRD = 0x06;
	
	PORTC = 0x00;
	PORTA = 0x00;
	PORTD &= 0xF8;
	/********************/
	
	w1_config conf;
	conf.w1_ddr = (uint8_t*)&TEMP_DDR;
	conf.w1_pin = (uint8_t*)&TEMP_PIN;
	conf.w1_bit = TEMP_BIT;
	
	int temp = 0; /* Температура */
	
	while(1) {		
		
		if (temp_18b20(conf, &temp)) { /* Проверка связи с датчиком и попытка измерения температуры */
			if ((PORTD & 0x04) == 0x04) { /* Если светодиод "Обрыв" был включен? */
				PORTD &= 0xFB; /* Выключение светодиода "Обрыв" */
			}
			print_num(temp); /* Вывод измеренной температуры */
		}
		else { /* Связь с датчиком потеряна (обрыв) */
			
			/* Моргание светодиода "Обрыв" (одна итерация) */
			_delay_ms(100);
			PORTD |= 0x04; /* Включение светодиода "Обрыв" */
			_delay_ms(100);
			PORTD &= 0xFB; /* Выключение светодиода "Обрыв" */
			/***********************************************/
			
			print_err(); /* Вывод сообщения об ошибке */
		}
	}
}
