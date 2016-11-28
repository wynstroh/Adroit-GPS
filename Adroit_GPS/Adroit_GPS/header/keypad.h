/*
 * keypad.h
 *
 * Created: 2016/10/24
 *  Author: Wynand S & David J
 */ 


#ifndef KEYPAD_H_
#define KEYPAD_H_

#include <Arduino.h>
#include <config.h>
#include <Functions.h>
#include <SoftwareSerial.h>

bool debounce = false;

void ISR_keypadPress() 
{  //disable interrupts ensure button press is only registered once
	//detachInterrupt(digitalPinToInterrupt(pin_RFID_RX1_D19));
	interruptReactivated = false;
	keyHasBeenPressed = true;
}//function


void keyClear()
{
	for (int i = 0; i < 6; i++)
		keyBuffer[i] = 0x00;
}

void checkKeyPressed(unsigned int* _regs) 
{
	boolean keyFound = true;											// Flag that shows if the key pressed has been determined
	keyHasBeenPressed = false;
	timerStart_keyPress = millis();

	boolean accept = false;
	boolean cancel = false;
	
	digitalWrite(pin_keypadColumn1, HIGH);
	digitalWrite(pin_keypadColumn2, LOW);										// Set one of the columns high
	digitalWrite(pin_keypadColumn3, LOW);
	delay(5);
	if (digitalRead(pin_keypadRow1))			
    { 
		keyBuffer[keycodeIndex] = 1;	// Check the rows to determine which key was pressed
		digitalWrite(pin_buzzer, HIGH);
		delay(50);
		digitalWrite(pin_buzzer, LOW);
		
	}
	else if (digitalRead(pin_keypadRow2))	
	{
		keyBuffer[keycodeIndex] = 4;
		digitalWrite(pin_buzzer, HIGH);
		delay(50);
		digitalWrite(pin_buzzer, LOW);		
	}
	else if (digitalRead(pin_keypadRow3))
	{
		keyBuffer[keycodeIndex] = 7;
		digitalWrite(pin_buzzer, HIGH);
		delay(50);
		digitalWrite(pin_buzzer, LOW);	
	}
	else if (digitalRead(pin_keypadRow4))
	{
		cancel = true;
	}
	else keyFound=false;														// If the key pressed was not 1, 4, or 7, set flag
	
	if (!keyFound)
	{															// If key already determined, skip the rest of the checking
		keyFound=true;
		digitalWrite(pin_keypadColumn1, LOW);									// Set one of the columns high
		digitalWrite(pin_keypadColumn2, LOW);
		digitalWrite(pin_keypadColumn3, HIGH);
		delay(5);
		if (digitalRead(pin_keypadRow1))
		{
			keyBuffer[keycodeIndex] = 3;	// Check the rows to determine which key was pressed
			digitalWrite(pin_buzzer, HIGH);
			delay(50);
			digitalWrite(pin_buzzer, LOW);			
		}
		else if (digitalRead(pin_keypadRow2))
		{
			keyBuffer[keycodeIndex] = 6;
			digitalWrite(pin_buzzer, HIGH);
			delay(50);
			digitalWrite(pin_buzzer, LOW);
		}
		else if (digitalRead(pin_keypadRow3))
		{
			keyBuffer[keycodeIndex] = 9;
			digitalWrite(pin_buzzer, HIGH);
			delay(50);
			digitalWrite(pin_buzzer, LOW);
		}
		else if (digitalRead(pin_keypadRow4))
		{
			accept = true;
		}
		else keyFound=false;													// If the key pressed was not 3, 6 or 9, set flag
	}
	
	if (!keyFound)
	{																			// If key already determined, skip the rest of the checking
		keyFound = true;
		digitalWrite(pin_keypadColumn1, LOW);
		digitalWrite(pin_keypadColumn2, HIGH);									// Keep one of the columns high
		digitalWrite(pin_keypadColumn3, LOW);
		delay(5);
		if (digitalRead(pin_keypadRow1))
		{
			keyBuffer[keycodeIndex] = 2;	// Check the rows to determine which key was pressed
			digitalWrite(pin_buzzer, HIGH);
			delay(50);
			digitalWrite(pin_buzzer, LOW);			
		}
		else if (digitalRead(pin_keypadRow2))
		{
			keyBuffer[keycodeIndex] = 5;
			digitalWrite(pin_buzzer, HIGH);
			delay(50);
			digitalWrite(pin_buzzer, LOW);
			delay(50);
			digitalWrite(pin_buzzer, HIGH);		
		}
		else if (digitalRead(pin_keypadRow3))	
		{
			keyBuffer[keycodeIndex] = 8;		
			digitalWrite(pin_buzzer, HIGH);
			delay(50);
			digitalWrite(pin_buzzer, LOW);	
		}
		else if (digitalRead(pin_keypadRow4))	
		{
			keyBuffer[keycodeIndex] = 0;
			digitalWrite(pin_buzzer, HIGH);
			delay(50);
			digitalWrite(pin_buzzer, LOW);				
		}
		else
		{
			keyFound=false;	
		}
											
	}
    
	if (keyFound)
	{		

		if (cancel)
		{
			keycodeIndex = 0;
			keycodeEntered = false;
			keyClear();

			#ifdef DEBUG_KEYPAD
			Serial.println("KeyPress : Canceled");
			#endif
		}

		//#ifdef DEBUG_KEYPAD
			//Serial.println("KeyPressed : " + String(keyBuffer[0]) + String(keyBuffer[1]) + String(keyBuffer[2]) + String(keyBuffer[3]) + String(keyBuffer[4]));
		//#endif

		if(accept || (keycodeIndex>=5))
		{
			previousKeycode = 0;
			int count = 0;

			for (int w = keycodeIndex - 1; w >= 0; w--)
			{
				#ifdef DEBUG_KEYPAD
				Serial.println("Key : " + String(w,DEC) + " : " + String(keyBuffer[count]) + " : " + String((ceil(keyBuffer[count]*pow(10,w)))));
				#endif
				previousKeycode = previousKeycode + ceil(keyBuffer[count]*pow(10,w));
				count++;
			}	

			#ifdef DEBUG_KEYPAD
			Serial.println("KeyPressed : " + String(previousKeycode));
			#endif

			keyClear();
			keycodeIndex = 0;
			keycodeEntered = true;

			unsigned long _dw;
			memcpy(&_dw,&previousKeycode,4);
			_regs[8] = _dw&0xffff;
			_regs[9] = (_dw >> 16)&0xffff;

		}
		else
		{
			if (!cancel)
			{
				keycodeIndex++;
				keycodeEntered = false;
			}
		}
	}
	
	digitalWrite(pin_keypadColumn1, HIGH);										// Reset all the columns to high to enable the interrupt
	digitalWrite(pin_keypadColumn2, HIGH);
	digitalWrite(pin_keypadColumn3, HIGH);
	keyHasBeenPressed = false;
}

///////////////////////////////////////////////////////////////////////
//
//boolean keyCodeCorrect() {														//check entered key code against door key code as well as the master key code
	//boolean keycodeEntered;
	//int keycodeRegistered = keyPressed[0]*10000 + keyPressed[1]*1000 + keyPressed[2]*100 + keyPressed[3]*10 + keyPressed[4];
	//
	//// doorKeycode - Lock / Unlock door
	//if (keycodeRegistered == doorKeycode) 
	//{
		//for(int h=0; h<5; h++)  keyPressed[h] = 255;
		//if(digitalRead(pin_maglock))
		//{
			//unlockDoor();
		//} 
		//else 
		//{
			//lockDoor();
		//}
		//keycodeEntered = true;	
	//}//if
	//else
	//{
		//keycodeEntered = false;												// if one incorrect key was entered, break from the loop and return wrong key code flag
		//alarm_validEntry = false;
		//lockDoor();
	//}//else
	//
	//// MASTER Code - Unlock door and Enable/Disable alarm system
	//if ((keycodeRegistered == masterKeycode) && (masterKeycode != 0)) 
	//{
		//if (alarm_SystemArmed)
		//{
			//alarm_SystemArmed = false;
			//Serial.println("-Key DISARM");
		//}
		//else
		//{
			//alarm_SystemArmed = true;
			//Serial.println("-Key ARM");
		//}		
		//keycodeEntered = true;
	//}//if
	//else 
	//{
		//keycodeEntered = false;												// if one incorrect key was entered, break from the loop and return wrong keycode flag
	//}//else
	//
	//// setAlarmKeycode - Enable / Disable Alarm system
	//if ((keycodeRegistered == setAlarmKeycode) && alarm_SystemArmed)
	//{
		//keycodeEntered = true;
		//keypad_systemArm = true;
		////return true;
	//}
	//else
	//{
		//keycodeEntered = false;												// if one incorrect key was entered, break from the loop and return wrong keycode flag
	//}
	//
	//if ((keycodeRegistered == setAlarmKeycode) && !alarm_SystemArmed)
	//{
		//keycodeEntered = true;
		//lockDoor();
		//keypad_systemArm = true;
		////return true;
	//}
	//else
	//{
		//keycodeEntered = false;
	//}
	//
	//for(int h=0; h<5; h++)  keyPressed[h] = 255;
	//return keycodeEntered;
//}

/////////////////////////////////////////////////////////////////////

void generateKeycode() {
	doorKeycode = rand();									// assigns a number between 0 and 0x7FFF to newKeycode
	if (doorKeycode < 10000) doorKeycode+=10000;			// if the number starts with a 0, add 10000 so that the code starts with a 1 (to avoid confusion)
	setAlarmKeycode = rand();
	if (setAlarmKeycode < 10000) setAlarmKeycode+=10000;
}


/////////////////////////////////////////////////////////////////////

void checkKeyPad(unsigned int* _regs)
{
	
	//Check for key pressed
	if ( (keyHasBeenPressed) && (!debounce) ) 
	{
		checkKeyPressed(_regs);
		debounce = true;
	}
	
	//if (keycodeEntered) 
	//{
		//if (keyCodeCorrect()) 
		//{
			//alarm_validEntry = true;
			//incorrectKeyCounter = 0;      // restart the counter if a correct keycode was entered
			////stopAlarm();
			////unlockDoor();
		//} else incorrectKeycodeEntered = true;
		//keycodeEntered = false;         // reset flag
	//}//if
//
	timerCurrent_keyPress = millis();
	//Reactivate interrupt after [timerInterval_quickestKeypress] ms to debounce
	if (!interruptReactivated)
	{
		if ((timerCurrent_keyPress - timerStart_keyPress) >= timerInterval_quickestKeypress) 
		{
			//attachInterrupt(digitalPinToInterrupt(pin_RFID_RX1_D19), ISR_keypadPress, RISING);
			interruptReactivated = true;
			debounce = false;
		}//if
	}//if

	////Reset keycode index if it has been 15 seconds since a key has been pressed
	//if  ((timerCurrent_keyPress - timerStart_keyPress) >= timerInterval_keyPress) 
	//{
		//for(int q=0; q<5; q++)  keyBuffer[q] = 255;
		////attachInterrupt(digitalPinToInterrupt(pin_RFID_RX1_D19), ISR_keypadPress, RISING);
		//keycodeIndex = 0;
		//timerStart_keyPress = 0xFFFFFFFF;
	//}//if
//
	////Lock door if the door was closed
	//if  ((timerCurrent_keyPress - timerStart_doorUnlocked) >= timerInterval_doorUnlocked) 
	//{
		//if (!alarm_doorOpen && alarm_doorUnlocked) lockDoor();
		//timerStart_doorUnlocked = 0xFFFFFFFF;
	//}//if   KM1
	//
	////Generate new keycode at midnight
	//if (rtcHour==0 && !newKeycodeGenerated) 
	//{
		////generateKeycode(); //remove until future date (dj 06/09/2016)
		//newKeycodeGenerated=true;       //Set flag to ensure keycode is only generated once at midnight
	//}
	//if (rtcHour==1 && newKeycodeGenerated)
	//newKeycodeGenerated=false;        //Reset keycode flag once at 1am

}

#endif /* KEYPAD_H_ */