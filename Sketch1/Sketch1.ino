/*
 * Sketch1.ino
 *
 * Created: 12/23/2017 6:48:21 PM
 * Author: dell
 */ 
// Button pins
#include <Time.h>         //http://www.arduino.cc/playground/Code/Time
#include <TimeLib.h>         //http://www.arduino.cc/playground/Code/Time
#include <Wire.h>
#include <DS3232RTC.h>    // http://github.com/JChristensen/DS3232RTC

#define LED 6
#define MBUT 4

DS3232RTC MyDS3232;

void enableInterrupts()
{
	// Enable EIC
	EIC->CTRL.bit.ENABLE = 1;
	while (EIC->STATUS.bit.SYNCBUSY == 1) { }
}

void disableInterrupts()
{
	// Disable EIC
	EIC->CTRL.bit.ENABLE = 0;
	while (EIC->STATUS.bit.SYNCBUSY == 1) { }
}

void buttonISR_M() //ISR for Middle button presses
{
	disableInterrupts();  // This must exist, especially with the LOW trigger.  For some reason
	// when a LOW is interrupt trigger is received it continues to fire and locks the processor.
	// I am using a LOW trigger as I am planning to put the processor into deep sleep where the clocks
	// are disabled. Other triggers (FALLING, RISING etc) need the clock running to be triggered.\
	
	enableInterrupts();
}

#define GENERIC_CLOCK_GENERATOR_MAIN      (0u)

void setup()
{
	// Set up communications
	Wire.begin();
	
	// Disable the RTC interrupts for the moment.
	//MyDS3232.alarmInterrupt(ALARM_1, false);
	//MyDS3232.alarmInterrupt(ALARM_2, false);

	// Will finally set to one minute.
	//MyDS3232.setAlarm(ALM2_EVERY_MINUTE, 0, 0, 0);
	//MyDS3232.alarmInterrupt(ALARM_2, true);

	// Enable 32Khz output on pin 1
//	MyDS3232.writeRTC(RTC_STATUS, MyDS3232.readRTC(RTC_STATUS) | _BV(EN32KHZ));

	// If using the DFLL then seems like processor cannot handle a 3V coin-cell as DFLL possibly needs more voltage..
	// or maybe a BOD is happening.
	// So to get this working the startup.c has been modified to add the default 8MHz reset startup setting which
	// can handle voltage as low as 3V.


	// Set LED pin to output.
	pinMode(LED, OUTPUT);
	pinMode(MBUT, INPUT_PULLUP); 

	attachInterrupt(digitalPinToInterrupt(MBUT), buttonISR_M, LOW); // when button B is pressed display
}

void loop()
{
	digitalWrite(LED, LOW);
	
	for (int j = 0; j < 3; j++)
	{
		digitalWrite(LED, HIGH);
		delay(500);
		digitalWrite(LED, LOW);
		delay(500);
	}
  
	PM->SLEEP.reg = PM_SLEEP_MASK;
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

	while (1)
	{
		// Disable SysTick timer (enables delay/time functions).. 
		// causes interrupt that wakes the processor...may be a diff way to disable via clocks...not sure
		SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk);
		
		// Clear the alarm interrupt in the RTC, else we will never wake up from sleep.
		// Very strange happening that only exhibits self when interrupt trigger is LOW.
		// and we want to deepsleep.
		uint8_t stat = MyDS3232.alarm(ALARM_2);

		__DSB();
		__WFI();

		// Enable SysTick IRQ and SysTick Timer
		SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
		
		uint32_t ints = EIC->INTFLAG.reg;
		
		for (int j = 0; j < 4; j++)
		{
			digitalWrite(LED, HIGH);
			delay(100);
			digitalWrite(LED, LOW);
			delay(100);
		}
		
		// Get the time from the RTC
		tmElements_t currTime; // TODO...make this global?
		MyDS3232.read(currTime);
	}
}
