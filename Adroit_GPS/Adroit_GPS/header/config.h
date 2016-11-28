/*
 * config.h
 *
 * Created: 2016/10/23 8:42:00 AM
 *  Author: Wynand Stroh
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_

#include <Arduino.h>
#include <Functions.h>
#include <SoftwareSerial.h>
#include <SimpleModbusSlave.h>
//#include <>

#define		DEBUG
#define		DEBUG_GPS
#define		DEBUG_KEYPAD

//Pin definition
// Digital Outputs:
#define		pin_keypadColumn3			9
#define		pin_keypadColumn2			14
#define		pin_keypadColumn1			15

// Digital Inputs:
#define		pin_keypadRow1				5
#define		pin_keypadRow2				6
#define		pin_keypadRow3				7
#define		pin_keypadRow4				8

#define		pin_keypadINT				2
#define		pin_buzzer					10			// On-board buzzer output

// SPI:
#define		pin_SPI_SS					10
#define		pin_SPI_MOSI				11
#define		pin_SPI_MISO				12
#define		pin_SPI_SCK					13

// Analogue Inputs:
#define		pin_batteryVoltage		16
#define		pin_pvVoltage				17
#define		pin_AISpare_01				18

// UART:
#define		pin_GPS_RX					0			// GPS RX
#define		pin_DEBUG_TX				1			// Debug TX
// RS485:
#define		pin_RS485_DE				19			// RS485 Driver Enable
#define		pin_RS485_RX				3			// RS485 RX
#define		pin_RS485_TX				4			// RS485 TX

#define		MODBUS_Station				1

float fLat = 0.0;
float fLon = 0.0;
float fAlt = 0.0;
boolean GPSDataChanged = false;

byte		keyBuffer[5];//  = {255, 255, 255, 255, 255};
int		keycodeIndex						= 0;
unsigned long	previousKeycode					= 0;
byte		incorrectKeyCounter				= 0;

boolean		newKeycodeGenerated			= false;

volatile boolean interruptReactivated	= false;
volatile boolean keyHasBeenPressed		= false;
boolean		keycodeEntered					= false;
boolean		incorrectKeycodeEntered		= false;

unsigned long	doorKeycode							= 12345;
unsigned long	masterKeycode						= 11223;
unsigned long	setAlarmKeycode					= 54321;	// the max value that rand() can return is 32767 (0x7FFF), so this will never be the same as the normal keycode

//System Arming/disarming
boolean		keypad_systemArm				= false;
boolean		systemNotYetArmed				= false;
boolean		systemArmed						= false;

uint32_t	timerStart_keyPress				= 0;
uint32_t	timerCurrent_keyPress			= 0;

uint32_t	timerInterval_quickestKeypress = 200; 
uint32_t	timerInterval_keyPress			= 15000;

volatile boolean gpsFix	= false;


#endif /* CONFIG_H_ */