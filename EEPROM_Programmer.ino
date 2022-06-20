/* 
*
* Processor: ATmega328P
* Compiler:  Arduino AVR
*/

/* Shift Registers Pins */
#define SHIFT_REGISTER_DATA_PIN 2
#define SHIFT_REGISTER_CLK_PIN 3
#define STORAGE_REGISTER_CLK_PIN 4
/* EEPROM Pins */
#define WRITE_EN_PIN 13
/**/
#define EEPROM_ADDRESSES_LENGTH 32768   // 32k (2^15 bit address line)


/*
* 	Initialize and configure the pins to behave as an output 
*/

void initPins()
{

	/* Shift Registers Pins */
	pinMode(SHIFT_REGISTER_DATA_PIN, OUTPUT);
   	pinMode(SHIFT_REGISTER_CLK_PIN, OUTPUT);
  	pinMode(STORAGE_REGISTER_CLK_PIN, OUTPUT);

  	/* EEPROM Pins */
	/*
	* 	Initialize the write enable pin to HIGH (Disabled) .
   	* 	(Note): Setting the pin to HIGH before setting the pin mode to OUTPUT,
	*   to safely make sure the pin is HIGH before outputting,
	*   and no data is written accidentally to the EEPROM .
   	*/
   	digitalWrite(WRITE_EN_PIN, HIGH);
  	pinMode(WRITE_EN_PIN, OUTPUT);
}

void setup ()
{
	initPins();
}

void loop()
{


}
