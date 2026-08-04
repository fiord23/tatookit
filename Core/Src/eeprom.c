#include "eeprom.h"
#include "stdio.h"
#include "motor.h"


extern I2C_HandleTypeDef hi2c3;
extern uint8_t speed;
extern uint8_t fw_version;
extern uint8_t motor_direction;
extern uint8_t display_orientation;
uint8_t data_buff[AT24CXX_PAGE_BYTE];

/*------------------------------------Проверка наличия микросхемы памяти EEPROM на шине i2c-------------------------------------------------*/
bool AT24xx_Connect_test(void) {
	///Функция проверки наличия микросхемы памяти EEPROM на шине i2c
	///Вернет true, если по указанному адресу i2c, микросхема даст ответ. false при неудаче.

	WP_PORT->BSRR = WP_Pin; //Защита от записи включена
	if (HAL_I2C_IsDeviceReady(&AT24CXX_I2C, AT24CXX_I2C_ADDR, 5, 100) == HAL_OK)
		return true; else
		return false;
}
/*------------------------------------Проверка наличия микросхемы памяти EEPROM на шине i2c-------------------------------------------------*/

/*------------------------------------------------Форматирование EEPROM---------------------------------------------------------------------*/
void AT24Cxx_erase_chip(void) {
	///Функция форматирования микросхемы памяти EEPROM

	WP_PORT->BSRR = (uint32_t) WP_Pin << 16u; //Защита от записи отключена
	memset(data_buff, 0xFF, AT24CXX_PAGE_BYTE);
	for (uint16_t i = 0; i < AT24CXX_MAX_MEM_ADDRESS; i = i + AT24CXX_PAGE_BYTE) {

#if defined(AT24C01) || defined(AT24C02)
		HAL_I2C_Mem_Write(&AT24CXX_I2C, AT24CXX_I2C_ADDR, i, I2C_MEMADD_SIZE_8BIT, (uint8_t*)data_buff, AT24CXX_PAGE_BYTE, HAL_MAX_DELAY);
#else
		HAL_I2C_Mem_Write(&AT24CXX_I2C, AT24CXX_I2C_ADDR, i, I2C_MEMADD_SIZE_16BIT, (uint8_t*) data_buff, AT24CXX_PAGE_BYTE, HAL_MAX_DELAY);
#endif

		HAL_Delay(10);
	}
	WP_PORT->BSRR = WP_Pin; //Защита от записи включена
}
/*------------------------------------------------Форматирование EEPROM---------------------------------------------------------------------*/

/*------------------------------------Функция записи в память массива данных(uint8_t)-------------------------------------------------------*/
uint16_t AT24Cxx_write(uint16_t addMem_write, uint8_t *data_write, uint16_t size_write) {
	///Функция записи в память массива данных(uint8_t)
	///вернет 0, если запись не произошла ( передали адрес и размер массива больше чем память чипа )
	///если запись удалась, то вернет адрес, на котором зокончили запись. Это удобно для последуещей записи с конца.
	///param addMem_write - адрес памяти, с которого начнем писать данные. Значение от 0 до AT24CXX_MAX_MEM_ADDRESS
	///param data_write - данные, которые хотим записать
	///param size_write - размер массива данных, которые хотим записать

	if ((addMem_write + size_write) < AT24CXX_MAX_MEM_ADDRESS) {
		WP_PORT->BSRR = (uint32_t) WP_Pin << 16u; //Защита от записи отключена
		uint16_t page_count_write = addMem_write / AT24CXX_PAGE_BYTE; //узнаем на какой странице мы находимся
		uint16_t byte_count_write = AT24CXX_PAGE_BYTE - (addMem_write - (page_count_write * AT24CXX_PAGE_BYTE)); // узнаем сколько байт нужно отправить до следуещей страницы
		if (byte_count_write >= size_write) {
			//если размер данных помещается в остаток до конца страницы
			memcpy(data_buff, data_write, size_write);

#if defined(AT24C01) || defined(AT24C02)
			HAL_I2C_Mem_Write(&AT24CXX_I2C, AT24CXX_I2C_ADDR, addMem_write, I2C_MEMADD_SIZE_8BIT, (uint8_t*)data_buff, size_write, HAL_MAX_DELAY);
#else
			HAL_I2C_Mem_Write(&AT24CXX_I2C, AT24CXX_I2C_ADDR, addMem_write, I2C_MEMADD_SIZE_16BIT, (uint8_t*) data_buff, size_write, HAL_MAX_DELAY);
#endif

			HAL_Delay(10);
		} else {
			//если размер данных не помещается в остаток до конца страницы
			memcpy(data_buff, data_write, byte_count_write);

#if defined(AT24C01) || defined(AT24C02)
			HAL_I2C_Mem_Write(&AT24CXX_I2C, AT24CXX_I2C_ADDR, addMem_write, I2C_MEMADD_SIZE_8BIT, (uint8_t*)data_buff, byte_count_write, HAL_MAX_DELAY);
#else
			HAL_I2C_Mem_Write(&AT24CXX_I2C,
				AT24CXX_I2C_ADDR,
				addMem_write,
				I2C_MEMADD_SIZE_16BIT,
				(uint8_t*) data_buff,
				byte_count_write,
				HAL_MAX_DELAY);
#endif

			HAL_Delay(10);
			size_write = size_write - byte_count_write;
			addMem_write = addMem_write + byte_count_write;
			uint16_t data_offset_write = byte_count_write;
			while (size_write >= AT24CXX_PAGE_BYTE) {
				//если остаток не помещается до конца страницы ( размер на больше чем страница
				memcpy(data_buff, data_write + data_offset_write, AT24CXX_PAGE_BYTE);

#if defined(AT24C01) || defined(AT24C02)
				HAL_I2C_Mem_Write(&AT24CXX_I2C, AT24CXX_I2C_ADDR, addMem_write, I2C_MEMADD_SIZE_8BIT, (uint8_t*)data_buff, AT24CXX_PAGE_BYTE, HAL_MAX_DELAY);
#else
				HAL_I2C_Mem_Write(&AT24CXX_I2C,
					AT24CXX_I2C_ADDR,
					addMem_write,
					I2C_MEMADD_SIZE_16BIT,
					(uint8_t*) data_buff,
					AT24CXX_PAGE_BYTE,
					HAL_MAX_DELAY);
#endif

				HAL_Delay(10);

				size_write = size_write - AT24CXX_PAGE_BYTE;
				addMem_write = addMem_write + AT24CXX_PAGE_BYTE;
				data_offset_write = data_offset_write + AT24CXX_PAGE_BYTE;

			}
			if (size_write) {

				memcpy(data_buff, data_write + data_offset_write, size_write);

#if defined(AT24C01) || defined(AT24C02)
				HAL_I2C_Mem_Write(&AT24CXX_I2C, AT24CXX_I2C_ADDR, addMem_write, I2C_MEMADD_SIZE_8BIT, (uint8_t*)data_buff, size_write, HAL_MAX_DELAY);
#else
				HAL_I2C_Mem_Write(&AT24CXX_I2C,
					AT24CXX_I2C_ADDR,
					addMem_write,
					I2C_MEMADD_SIZE_16BIT,
					(uint8_t*) data_buff,
					size_write,
					HAL_MAX_DELAY);
#endif
				HAL_Delay(10);
			}
		}
		WP_PORT->BSRR = WP_Pin; //Защита от записи включена
		return addMem_write + size_write;
	} else {
		return 0;
	}
}
/*------------------------------------Функция записи в память массива данных(uint8_t)-------------------------------------------------------*/


/*------------------------------------Функция чтения из памяти массива данных(uint8_t)------------------------------------------------------*/
uint16_t AT24Cxx_read(uint16_t addMem_read, uint8_t *data_read, uint16_t size_read) {
	///Функция чтения из памяти массива данных(uint8_t)
	///вернет 0, если чтение не произошло ( передали адрес и размер массива больше чем память чипа )
	///если чтение прошло успешно, то вернет адрес, на котором зокончилось чтение. Это удобно для последующего чтения с конца.
	///param addMem_read - адрес памяти, с которого начнем читать данные. Значение от 0 до AT24CXX_MAX_MEM_ADDRESS
	///param data_write - данные, куда запишем считанные данные
	///param size_write - размер массива данных, которые хотим считать

	WP_PORT->BSRR = WP_Pin; //Защита от записи включена
	if ((addMem_read + size_read) < AT24CXX_MAX_MEM_ADDRESS) {

#if defined(AT24C01) || defined(AT24C02)
		HAL_I2C_Mem_Read(&AT24CXX_I2C, AT24CXX_I2C_ADDR, addMem_read, I2C_MEMADD_SIZE_8BIT, (uint8_t*)data_read, size_read, HAL_MAX_DELAY);
#else
		HAL_I2C_Mem_Read(&AT24CXX_I2C, AT24CXX_I2C_ADDR, addMem_read, I2C_MEMADD_SIZE_16BIT, (uint8_t*) data_read, size_read, HAL_MAX_DELAY);
#endif

		return addMem_read + size_read;
	} else {
		return 0;
	}
}
/*------------------------------------Функция чтения из памяти массива данных(uint8_t)------------------------------------------------------*/
/*SPEED SAVE LOAD*/

void eeprom_speed_init(void)
{
	uint8_t speed_reg[1] = {0};
    AT24Cxx_read(EEPROM_PAGE_SPEED, speed_reg, 1);

    if (speed_reg[0] == 0xFF)
    {
        speed = MOTOR_SPEED_DEFAULT;
		speed_reg[0] = MOTOR_SPEED_DEFAULT;
		AT24Cxx_write(EEPROM_PAGE_SPEED, speed_reg, 1);
    }
    else
    {
        speed = speed_reg[0];
    }

}

void eeprom_speed_save(uint8_t value)
{	
	AT24Cxx_write(EEPROM_PAGE_SPEED, &value, 1);
}


/*FIRMWARE VERSION SAVE LOAD*/
void eeprom_fw_init(void)
{
	uint8_t fw_reg[1] = {0};
	 AT24Cxx_read(EEPROM_PAGE_FW, fw_reg, 1);

    if (fw_reg[0] == 0xFF)
    {
        fw_version = 0;
        AT24Cxx_write(EEPROM_PAGE_FW, &fw_version, 1);
    }
    else
    {
        fw_version = fw_reg[0];
    }
}

void eeprom_fw_save(uint8_t value)
{
	AT24Cxx_write(EEPROM_PAGE_FW, &value, 1);
}



/* MOTOR DIRECTION SAVE LOAD*/
void motor_direction_init(void)
{
	uint8_t fw_reg[1] = {0};
	 AT24Cxx_read(MOTOR_DIRECTION_PAGE, fw_reg, 1);

    if (fw_reg[0] == 0xFF)
    {
        motor_direction = 0;
        AT24Cxx_write(MOTOR_DIRECTION_PAGE, &motor_direction, 1);
    }
    else
    {
        motor_direction = fw_reg[0];
    }
}

void motor_direction_save(uint8_t value)
{
	AT24Cxx_write(MOTOR_DIRECTION_PAGE, &value, 1);
}


/* Display orientation SAVE LOAD*/
void display_orientation_init(void)
{
	uint8_t fw_reg[1] = {0};
	
	 AT24Cxx_read(DISPLAY_ORIENTATION_PAGE, fw_reg, 1);

    if (fw_reg[0] == 0xFF)
    {
        display_orientation = 0;
        AT24Cxx_write(DISPLAY_ORIENTATION_PAGE, &display_orientation, 1);
    }
    else
    {
        display_orientation = fw_reg[0];
    }
}

void display_orientation_save(uint8_t value)
{
	AT24Cxx_write(DISPLAY_ORIENTATION_PAGE, &value, 1);
}

