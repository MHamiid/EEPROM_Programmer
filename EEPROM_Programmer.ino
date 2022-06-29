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
#define EEPROM_D0_PIN 5
#define EEPROM_D7_PIN 12
#define WRITE_EN_PIN 13
/* EEPROM Specifications */
#define EEPROM_ADDRESSES_LENGTH 32768       // 32k (2^15 bit address line)
// Delay in microseconds
#define EEPROM_WRITE_LOW_DELAY 1
#define EEPROM_WRITE_CYCLE_DELAY 5000



/* 
*	Write the 15-bit for the EEPROM address and 1-bit for the EEPROM output enable status to the EEPROM indirectly through the two 8-bit shift registers .
*	The 16 bits are written serially to two (8-BIT SHIFT REGISTER WITH 8-BIT OUTPUT REGISTER),
*	with the 16-bit output from both register connected to the 15-bit address lines of the EEPROM [A0-A14] and the output enable pin [~OE] .
*/
void setEEPROMAddressAndOutputEnable(uint16_t address, bool EEPROMOutputEnable)
{

	/*
	*	Shift the 15-bit address 8 places to the right to get the top 7-bits (most siginificant bits) of the address,
	*	and set the output enable status as the top bit (8th bit) of the byte .
	*/
	byte topByte =  ( address >> 8 ) | ( EEPROMOutputEnable ? 0x00 /*Set bit to 0 to enable (Active Low)*/ : 0x80  /*Set bit to 1 to disable (Active Low)*/ );

	/*
	*	Assigning the full 15-bit address to a byte ( which is more than a byte ),
	*	results in using only the lower 8-bits of the address when implicitly casting from uint16_t to a byte .
	*/
	byte lowerByte = address;  


	// Write the top 7-bits of the address and the output enable status first
	shiftOut(SHIFT_REGISTER_DATA_PIN, SHIFT_REGISTER_CLK_PIN, MSBFIRST, topByte);
   
	/* 
	*  Write the remaining 8-bit of the address 
	*  Writing this second byte, results in the previous byte being shifted to the second 8-bit shift register,
	*  and this byte being in the first register
	*/
	shiftOut(SHIFT_REGISTER_DATA_PIN, SHIFT_REGISTER_CLK_PIN, MSBFIRST,  lowerByte);             



	/* 
	*   Output data from the shift registers to the output (storage) registers,
	*	by generating a single High clock pulse to the STORAGE_REGISTER_CLK_PIN
	*/
	digitalWrite(STORAGE_REGISTER_CLK_PIN, LOW);
	digitalWrite(STORAGE_REGISTER_CLK_PIN, HIGH);
	digitalWrite(STORAGE_REGISTER_CLK_PIN, LOW);

}



/*
*	Write a byte to the EEPROM at the specified address
*/
void writeEEPROMByte(uint16_t address, byte data)
{
   /*
   *	Set the address where the data will be written in .
   *	And disable the EEPROM output ( we are writing data not reading ),
   *	otherwise it would result in contention when trying to write to the data lines .
   *
   */
   setEEPROMAddressAndOutputEnable(address, false);

	/* Output the data on the data pins */
	for (int dataPin = EEPROM_D0_PIN; dataPin <= EEPROM_D7_PIN; dataPin++)
	{
		/*	Configure the data pins to be an output .
		*	(Note): The order where EEPROM output enable is disabled when setEEPROMAddressAndOutputEnable() is called first before configuring the pins mode to output,
		*	to avoid the EEPROM and the Arduino writting to the data lines at the same time resulting in contention .
		*/
		pinMode(dataPin, OUTPUT);
		
		// Get the least siginificant bit by masking the first bit
		byte leastSignificantBit = data & 1;

		digitalWrite(dataPin, leastSignificantBit);

		// Shift the data one bit to the right to position the least siginificant bit with corresponding pin
		data >>= 1;   
  } 
  
	/* Pulse the write enable pin LOW to write the data that is available at the data pins*/
	digitalWrite(WRITE_EN_PIN, LOW);
	// Delay to match the EERPOM chip min/max specifications time where the write pin has to be set low to write successfully
	delayMicroseconds(EEPROM_WRITE_LOW_DELAY);
	// Set the pin back to HIGH
	digitalWrite(WRITE_EN_PIN, HIGH);
	// Delay for some time to match the EERPOM chip min time specifications to let the EEPROM compelete the write cycle
	delayMicroseconds(EEPROM_WRITE_CYCLE_DELAY);   
}



/*
*	Write a the given byte array to the EEPROM starting from the specified address
*/
void writeEEPROM(byte* dataArray, uint16_t dataArraySize, uint16_t address)
{ 
   for(uint16_t index = 0; index < dataArraySize; index++)
   {
	   writeEEPROMByte(address, dataArray[index]);
	   // Increment to the next address
	   address++;
   } 
}



/*
*  Override EEPROM content in the given address range [ startAddress:endAddress ] with the specified value
*/
void eraseEEPROM(uint16_t startAddress, uint16_t endAddress, byte value)
{
	for(uint16_t address = startAddress; address <= endAddress; address++)
	{
		// Override the current value at the address with the specified value
		writeEEPROMByte(address, value);
	}
}



/*
*	Read a byte of data from EEPROM at the specified address
*/
byte readEEPROMByte(uint16_t address)
{
	for(int dataPin = EEPROM_D0_PIN; dataPin <= EEPROM_D7_PIN; dataPin++)
	{
		/*	Configure the data pins to be an input .
		*	(Note): The order where configuring the pins mode to input before EEPROM output is enabled when setEEPROMAddressAndOutputEnable() is called,
		*	to avoid the EEPROM and the Arduino writting to the data lines at the same time resulting in contention .
		*/
		pinMode(dataPin, INPUT);
	}
   
	/*
	*	Set the address where the data will be read from .
	*	And enable the EEPROM output ( we are reading data from it ),
	*	This needs to be called after the Arduino data pins are configured to be an input, 
	*	otherwise it would result in contention on the data lines .
	*/
	setEEPROMAddressAndOutputEnable(address, true);
   
   // Initialize the data output
	byte data = 0;
   
	/* Read the data from the data pins */
	for(int dataPin = EEPROM_D7_PIN; dataPin >= EEPROM_D0_PIN; dataPin--)
	{
		// Shift the data 1-bit to the left and then read the pin data to the LSB
		data = ( data << 1 ) + digitalRead(dataPin); 
	}
   
	return data;
}



/*
*	Print the content of the EEPROM at the specified address range to the serial output .
*	The data is printed in 16-byte blocks .
*/
void serialPrintEEPROMContent(uint16_t startAddress, uint16_t endAddress)
{
	// Check if the start and end addresses don't exceed the maximum EEPROM address
	if(startAddress >= EEPROM_ADDRESSES_LENGTH || endAddress >= EEPROM_ADDRESSES_LENGTH)
	{
		Serial.print("[!] Address exceeded the maximum address, maximum address is: ");
		// The maximum EEPROM address
		Serial.println(EEPROM_ADDRESSES_LENGTH - 1);
		Serial.print("\t \t \t \t \tYour input:   Start Address  ");
		Serial.print(startAddress);
		Serial.print(", End Address  ");
		Serial.println(endAddress);

		// Exit the function
		return;
	}
	// Buffer for 16 bytes of data
	byte dataBuffer[16];
	// Keep track of the number of bytes read before being serially outputted
	uint8_t dataBufferIndex = 0;  // Range [0: 15]


	for(uint16_t address = startAddress; address <= endAddress; address++)
	{
		// Read a byte of data into the data buffer
		dataBuffer[dataBufferIndex] = readEEPROMByte(address);

		/*
		*	If we have 16 bytes of data in the data buffer or the last address reached,
		*	then write the data into the output buffer in a specific format . 
		*/
		if(dataBufferIndex == 15 || address == endAddress)
		{
			char outputBuffer[80];
			// Pointer to the first item of the output buffer
			char* outputBufferPtr = &outputBuffer[0];

			// Write the first address of the data into the output buffer
			outputBufferPtr += sprintf(outputBuffer, "%04x:    ", (address - dataBufferIndex));

			/* Write the data into the output buffer */
			for(uint8_t i = 0; i <= dataBufferIndex; i++)
			{
				// Byte format
				const char* format = "%02x ";

				// Add spacing after 8 bytes of data
				if(i == 7)
				{
					format =  "%02x   ";
				}
				
				// Write a byte into the output buffer
				outputBufferPtr += sprintf(outputBufferPtr, format, dataBuffer[i]);
				
			}
			
			// Serial output the output buffer
			Serial.println(outputBuffer);
			// Reset the index
			dataBufferIndex = 0;
			// Continue to avoid incrementing the dataBufferIndex
			continue;
		}

		dataBufferIndex++;
    }
      
}



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
	*	Initialize the write enable pin to HIGH (Disabled) .
   	*	(Note): Setting the pin to HIGH before setting the pin mode to OUTPUT,
	*	to safely make sure the pin is HIGH before outputting,
	*	and no data is written accidentally to the EEPROM .
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
