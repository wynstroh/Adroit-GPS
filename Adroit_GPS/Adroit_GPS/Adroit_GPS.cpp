/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>
#include <config.h>
#include <Functions.h>
#include <SoftwareSerial.h>
#include <keypad.h>

//Beginning of Auto generated function prototypes by Atmel Studio

//End of Auto generated function prototypes by Atmel Studio

#define		MODBUS_Station				1

//////////////// registers of your slave ///////////////////
enum
{
	// just add or remove registers and your good to go...
	// The first register starts at address 0
	STATUS_WORD,
	GPS_FIX_IND,
	LAT_VAL_LOW,
	LAT_VAL_HIGH,
	LON_VAL_LOW,
	LON_VAL_HIGH,
	ALT_VAL_LOW,
	ALT_VAL_HIGH,
	KEYCODE_VAL_LOW,
	KEYCODE_VAL_HIGH,
	TIME_ZONE,
	YEAR,
	MONTH,
	DAY,
	HOUR,
	MINUTE,
	SECOND,
	DAY_WEEK,
	HOLDING_REGS_SIZE // leave this one
	// total number of registers for function 3 and 16 share the same register array
	// i.e. the same address space
};

unsigned int holdingRegs[HOLDING_REGS_SIZE]; // function 3 and 16 register array
////////////////////////////////////////////////////////////

SoftwareSerial RS485_Serial(pin_RS485_RX, pin_RS485_TX); // RX, TX

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void processGPSString(String s_GPSData);

void setup() {

	//memset(&keyBuffer[0],0xff,5);

	//Digital IO
	pinMode(pin_keypadColumn1, OUTPUT);
	pinMode(pin_keypadColumn2, OUTPUT);
	pinMode(pin_keypadColumn3, OUTPUT);

	digitalWrite(pin_keypadColumn1, HIGH);
	digitalWrite(pin_keypadColumn2, HIGH);
	digitalWrite(pin_keypadColumn3, HIGH);

	pinMode(pin_keypadRow1, INPUT);
	pinMode(pin_keypadRow2, INPUT);
	pinMode(pin_keypadRow3, INPUT);
	pinMode(pin_keypadRow4, INPUT);

	pinMode(pin_keypadINT, INPUT);

	modbus_configure(&RS485_Serial, 9600, SERIAL_8N2, MODBUS_Station, pin_RS485_DE, HOLDING_REGS_SIZE, holdingRegs);

	//initialize interrupt 4 for Keypad press
	digitalWrite(pin_keypadINT, LOW);  // D19_RX1 RFID
	attachInterrupt(digitalPinToInterrupt(pin_keypadINT), ISR_keypadPress, RISING);

	// Open serial communications and wait for port to open:
	Serial.begin(9600);
	while (!Serial) {
		; // wait for serial port to connect. Needed for native USB port only
	}

	#ifdef DEBUG
	Serial.println("Serial Startup");
	RS485_Serial.println("Modbus Startup");
	#endif

}

void loop() // run over and over
{
	// modbus_update() is the only method used in loop(). It returns the total error
	// count since the slave started. You don't have to use it but it's useful
	// for fault finding by the modbus master.
	
	modbus_update();
	
	checkKeyPad(holdingRegs);

	if (stringComplete) {
		processGPSString(inputString);
		//RS485_Serial.println(inputString);
		// clear the string:
		inputString = "";
		stringComplete = false;
	}
}

/*
SerialEvent occurs whenever a new data comes in the
hardware serial RX.  This routine is run between each
time loop() runs, so using delay inside loop can delay
response.  Multiple bytes of data may be available.
*/
void serialEvent() {
	while (Serial.available()) {
		// get the new byte:
		char inChar = (char)Serial.read();
		// add it to the inputString:
		inputString += inChar;
		// if the incoming character is a newline, set a flag
		// so the main loop can do something about it:
		if (inChar == '\n') {
			stringComplete = true;
		}
	}
}

void processGPSString(String s_GPSData)
{
	if (s_GPSData.startsWith("$GPGGA,",0))
	{
		String s_Time = getValue(s_GPSData,',',1);
		String s_Lat = getValue(s_GPSData,',',2);
		String s_Lat_NS = getValue(s_GPSData,',',3);
		String s_Lon = getValue(s_GPSData,',',4);
		String s_Lon_EW = getValue(s_GPSData,',',5);
		String s_GPSFix = getValue(s_GPSData,',',6);

		String s_Alt = getValue(s_GPSData,',',9);

		float _fLat = 0.0;
		float _fLon = 0.0;
		float _fAlt = 0.0;

		int _iGPSFix = s_GPSFix.toInt();

		if (_iGPSFix > 0)
			gpsFix = true;

		holdingRegs[GPS_FIX_IND] = _iGPSFix;

		if (gpsFix)
		{
			String s_Lat_deg = s_Lat.substring(0,2);
			String s_Lat_min = s_Lat.substring(2);

			_fLat = (s_Lat_deg.toFloat());
			_fLat = _fLat + (s_Lat_min.toFloat() / 60.0);

			String s_Lon_deg = s_Lon.substring(0,3);
			String s_Lon_min = s_Lon.substring(3);

			_fLon = (s_Lon_deg.toFloat());
			_fLon = _fLon + (s_Lon_min.toFloat() / 60.0);

			_fAlt = (s_Alt.toFloat());

			unsigned long _dw;

			if (s_Lat_NS == "S")
			{
				_fLat = _fLat * -1;
			}

			if (s_Lat_NS == "W")
			{
				_fLon = _fLon * -1;
			}
		
			if (fLat != _fLat)
			{
				memcpy(&_dw,&_fLat,4);
				holdingRegs[LAT_VAL_LOW] = _dw&0xffff;
				holdingRegs[LAT_VAL_HIGH] = (_dw >> 16)&0xffff;
				fLat = _fLat;
			}

			if (fLon != _fLon)
			{
				memcpy(&_dw,&_fLon,4);
				holdingRegs[LON_VAL_LOW] = _dw&0xffff;
				holdingRegs[LON_VAL_HIGH] = (_dw >> 16)&0xffff;
				fLon = _fLon;
			}

			if (fAlt != _fAlt)
			{
				memcpy(&_dw,&_fAlt,4);
				holdingRegs[ALT_VAL_LOW] = _dw&0xffff;
				holdingRegs[ALT_VAL_HIGH] = (_dw >> 16)&0xffff;
				fAlt = _fAlt;
			}

			#ifdef DEBUG_GPS
			Serial.print(s_Time + ",");
			Serial.print(String(fLat) + ",");
			Serial.print(String(fLon) + ",");
			Serial.println(String(fAlt));
			#endif
		}
		
		#ifdef DEBUG_GPS
		Serial.print(s_Time + ",");
		Serial.print(s_Lat + " " + s_Lat_NS + ",");
		Serial.print(s_Lon + " " + s_Lon_EW + ",");
		Serial.println(String(fAlt));
		#endif
	}
	else if (s_GPSData.startsWith("$GPRMC,",0) || s_GPSData.startsWith("$GNRMC,",0))
	{
		String s_Time = getValue(s_GPSData,',',1);
		String s_Date = getValue(s_GPSData,',',9);

		int ihour = s_Time.substring(0,2).toInt();
		int iminute = s_Time.substring(2,4).toInt();
		int isecond = s_Time.substring(4,6).toInt();

		int iday = s_Date.substring(0,2).toInt();
		int imonth = s_Date.substring(2,4).toInt();
		int iyear = s_Date.substring(4,6).toInt();
		int idayofweek = dayofweek((iyear + 2000), imonth, iday);

		int itimezone = holdingRegs[TIME_ZONE];

		if (gpsFix)
		{	
			holdingRegs[YEAR] = iyear + 2000;
			holdingRegs[MONTH] = imonth;
			holdingRegs[DAY] = iday;
			holdingRegs[DAY_WEEK] = idayofweek;

			holdingRegs[HOUR] = ihour + itimezone;
			holdingRegs[MINUTE] = iminute;
			holdingRegs[SECOND] = isecond;
		}
		
		#ifdef DEBUG_GPS
		Serial.print(String(iyear+2000) + "/" + String(imonth) + "/" + String(iday) + " " + String(idayofweek) + " ");
		Serial.println(String(ihour + itimezone) + ":" + String(iminute) + ":" + String(isecond));
		#endif
	}

}