/*
 * Functions.cpp
 *
 * Created: 2016/10/23 11:36:56 AM
 *  Author: wynands
 */ 

 #include "Functions.h"

 String getValue(String data, char separator, int index)
 {
	 int found = 0;
	 int strIndex[] = {
	 0, -1  };
	 int maxIndex = data.length()-1;
	 for(int i=0; i<=maxIndex && found<=index; i++){
		 if(data.charAt(i)==separator || i==maxIndex){
			 found++;
			 strIndex[0] = strIndex[1]+1;
			 strIndex[1] = (i == maxIndex) ? i+1 : i;
		 }
	 }
	 return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
 }

 int dayofweek(int y, int m, int d)	/* 1 <= m <= 12,  y > 1752 (in the U.K.) */
 {
	 static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
	 y -= m < 3;
	 return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
 }


