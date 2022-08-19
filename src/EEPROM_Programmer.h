#ifndef __EEPROM_PROGRAMMER_H__
#define __EEPROM_PROGRAMMER_H__

#include <Arduino.h>


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




/**
 * @brief Initialize and configure the pins to behave as an output
 *
 * @return void
 */
void initPins();




/**
 * @brief Set the EEPROM address and EEPROM output enable
 *
 * Write two bytes serially, the 15-bit for the EEPROM address and 1-bit for the EEPROM output enable status to the EEPROM indirectly through the two 8-bit shift registers .
 * The two bytes are written serially to two (8-BIT SHIFT REGISTER WITH 8-BIT OUTPUT REGISTER),
 * with the two bytes (16-bit) output from both register connected to the 15-bit address lines of the EEPROM [A0-A14] and the output enable pin [~OE] .
 *
 * @param address 15-bit address
 * @param EEPROMOutputEnable The 16th bit, set to true to enable EEPROM data output at the specified address, set to false to disable the EEPROM form outputting data   
 *
 * @return void
 */
void setEEPROMAddressAndOutputEnable(uint16_t address, bool EEPROMOutputEnable);




/**
 * @brief Write a byte to the EEPROM at the specified address
 *
 * @param address The address where the byte data would be written in
 * @param data The byte to be written at the specified address
 *
 * @return void
 */
void writeEEPROMByte(uint16_t address, byte data);




/**
 * @brief Write the given byte array to the EEPROM starting from the specified address
 *
 * @param dataArray Byte array
 * @param dataArraySize Size of dataArray
 * @param address The starting address where the dataArray is written
 *
 * @return void
 */
void writeEEPROM(byte* dataArray, uint16_t dataArraySize, uint16_t address);




/**
 * @brief Override EEPROM content in the given address range with the specified value
 *
 * @param startAddress Start of the address range
 * @param endAddress End of the address range
 * @param value The value to write at the address range
 *
 * @return void
 */
void eraseEEPROM(uint16_t startAddress, uint16_t endAddress, byte value);




/**
 * @brief Read a byte of data from EEPROM at the specified address
 *
 * @param address Address of the byte
 *
 * @return The byte at the address
 */
byte readEEPROMByte(uint16_t address);




/**
 * @brief Serial print EEPROM content at the specified address range
 *
 * Assuming that "Serial.begin()" is called before calling this function .
 * The data is printed in 16-byte blocks .
 *
 * @param startAddress Start of the address range
 * @param endAddress End of the address range
 *
 * @return void
 */
void serialPrintEEPROMContent(uint16_t startAddress, uint16_t endAddress);

#endif /* __EEPROM_PROGRAMMER_H__ */