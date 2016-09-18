/*
- The circuit
LCD
pin		LCD		Arduinio Pin
1		Gnd		Gnd
2		5V		5V

4		RS		7
5		RW		Gnd		Read/Write Gnd = write
6		EN		8
14		DB7		12
13		DB6		11
12		DB5		10
11		DB4		9

POT		center to LCD pin3 (10K POT)
		left to gnd | - one side to 5V one to Gnd
		right to 5V |

Clock: (I2C)
SDA		A4		data
SCL		A5		clock

IR Sensor
A0		Analog output that varies from 3.1V at 4cm to 0.3V at 30cm with a supply voltage between 4.5 and 5.5VDC
		http://bildr.org/2011/03/various-proximity-sensors-arduino/
		https://www.sparkfun.com/products/12728

*/

// include the library code:
#include <LiquidCrystal.h>	// LCD
#include "Wire.h"			// I2C = clock
#include "Clock.h"			// eric's clock methods

// IR sensor pin
uint8_t irPin = A0;
int irValue = 0;

// initialize the LCD library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

char *hiBruce = "Hi Bruce!       ";
char *needO3 = "Need a O3 sensor";
bool firstMessage = true;

// which row for clock
enum ClockRow
{
	top,
	bottom,
};

// clock returns an int for the weekday 1 - 7
char *weekdayTable[] = { "Sun", "Mon", "Tue", "Wed", "Thr", "Fri", "Sat" };

// store the last time values, set to -1 so all time values are displayed on load
//uint8_t lastSec = -1;
uint8_t lastMin = -1;
uint8_t lastHr = -1;
uint8_t lastWeekDay = -1;
uint8_t lastMonthDay = -1;
uint8_t lastMonth = -1;
uint8_t lastYear = -1;

Clock *clock = new Clock();

long lastMills = 0;
const int pollingInterval = 500;

// set the clock row position - TODO: config this
ClockRow cr = bottom;

void setup()
{
	pinMode(irPin, INPUT);

	Serial.begin(9600);
	Wire.begin();

	//clock->SetDateTime(0, 34, 21, 7, 17, 9, 16);

	// set up the LCD's number of (columns, rows)
	lcd.begin(16, 2);

	// config the display
	setupDisplay();

	// Print a message to the LCD.
	lcd.setCursor(0, 0);
	lcd.print(hiBruce);
}

void loop() 
{
	// set the cursor to column 0, line 1
	// (note: line 1 is the second row, since counting begins with 0):

	// print the number of seconds since reset:
	//lcd.setCursor(0, cr);
	//lcd.print(millis() / 1000);

	delay(1000);

	// get current tick
	if (millis() - lastMills > pollingInterval)
	{
		lcd.setCursor(0, 0);
		if (firstMessage)
		{
			lcd.print(needO3);
		}
		else
		{
			lcd.print(hiBruce);
		}
		firstMessage = !firstMessage;
	}

	// read the IR sensor
	irValue = analogRead(irPin);
	Serial.println(irValue);

	if (irValue > 400)
	{
		Serial.println("Closed");
	}
	else
	{
		Serial.println("Open");
	}

	clock->GetDateTime();
	displayTime();
}

void displayTime()
{
	// display only what changed, LCD reference is: (column, row)
	lcd.setCursor(0, cr);

	// weekday
	if (lastWeekDay != clock->weekDay)
	{
		lcd.print(weekdayTable[clock->weekDay - 1]); // week day is 1 - 7 -> sunday(1) - saturday(7)
		lastWeekDay = clock->weekDay;
	}

	if (lastMonth != clock->month)
	{
		if (clock->month < 10)
		{
			// single digit
			lcd.setCursor(4, cr);
			lcd.print(" ");
			lcd.setCursor(5, cr);
			lcd.print(clock->month);
		}
		else
		{
			// double digit
			lcd.setCursor(4, cr);
			lcd.print(clock->month);
		}

		lastMonth = clock->month;
	}

	if (lastMonthDay != clock->monthDay)
	{
		if (clock->monthDay < 10)
		{
			// single digit
			lcd.setCursor(7, cr);
			lcd.print(clock->monthDay);
			lcd.setCursor(8, cr);
			lcd.print(" ");
		}
		else
		{
			// double digit
			lcd.setCursor(7, cr);
			lcd.print(clock->monthDay);
		}

		lastMonthDay = clock->monthDay;
	}

	if (lastHr != clock->hour)
	{
		if (clock->hour < 10)
		{
			// single digit
			lcd.setCursor(10, cr);
			lcd.print(" ");
			lcd.setCursor(11, cr);
			lcd.print(clock->hour);
		}
		else
		{
			// double digit
			lcd.setCursor(10, cr);
			lcd.print(clock->hour);
		}

		lastHr = clock->hour;
	}

	if (lastMin != clock->minute)
	{
		if (clock->minute < 10)
		{
			// single digit
			lcd.setCursor(13, cr);
			lcd.print("0");
			lcd.setCursor(14, cr);
			lcd.print(clock->minute);
		}
		else
		{
			// single digit
			lcd.setCursor(13, cr);
			lcd.print(clock->minute);
		}

		lastMin = clock->minute;
	}
}

void setupDisplay()
{
	lcd.setCursor(6, cr);
	lcd.print("/");
	lcd.setCursor(12, cr);
	lcd.print(":");
}
