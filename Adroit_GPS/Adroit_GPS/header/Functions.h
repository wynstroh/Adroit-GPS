/*
 * Functions.h
 *
 * Created: 2016/10/23 11:30:48 AM
 *  Author: Wynand Stroh
 */ 


#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include <Arduino.h>
#include "Functions.h"
#include <SoftwareSerial.h>
#include "SimpleModbusSlave.h"

String getValue(String data, char separator, int index);

int dayofweek(int y, int m, int d);

#endif /* FUNCTIONS_H_ */