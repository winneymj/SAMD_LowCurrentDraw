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
#include <RTCx.h>         // https://github.com/stevemarple/RTCx
#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>
#include "defs.h"
#include "watchface.h"

//extern void displayMenu();
//extern void initializeMenu();
//extern WatchMenu menu;
//extern void displayTime();
extern void displayCalendar();

Adafruit_SharpMem display(DISPLAY_SCK, DISPLAY_MOSI, DISPLAY_SS);
DS3232RTC MyDS3232;
RTCx MCP7941;
WatchFace *watchFace = nullptr;

volatile boolean rtcFired = false; //variables in ISR need to be volatile
volatile boolean pinValM = false;
volatile boolean pinValD = false;
volatile boolean pinValU = false;


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
	// are disabled. Other triggers (FALLING, RISING etc) need the clock running to be triggered.
	pinValM = true;
	enableInterrupts();
}

void buttonISR_U() //ISR for Up button presses
{
	disableInterrupts();  // This must exist, especially with the LOW trigger.  For some reason
	// when a LOW is interrupt trigger is received it continues to fire and locks the processor.
	// I am using a LOW trigger as I am planning to put the processor into deep sleep where the clocks
	// are disabled. Other triggers (FALLING, RISING etc) need the clock running to be triggered.
	pinValU = true;
	enableInterrupts();
}

void buttonISR_D() //ISR for Down button presses
{
	disableInterrupts();  // This must exist, especially with the LOW trigger.  For some reason
	// when a LOW is interrupt trigger is received it continues to fire and locks the processor.
	// I am using a LOW trigger as I am planning to put the processor into deep sleep where the clocks
	// are disabled. Other triggers (FALLING, RISING etc) need the clock running to be triggered.
	pinValD = true;
	enableInterrupts();
}

void RTC_int() //ISR for RTC interrupt every minute
{
	disableInterrupts();
	//rtcRead = !rtcRead;
	rtcFired = true;
	enableInterrupts();
}

#define GENERIC_CLOCK_GENERATOR_MAIN      (0u)

/*
 * \brief Function to configure Power Manager (PM)
 * 
 * This function configures the PM. The PM controls what synchronous
 * clocks clocks are running and not. This configuration of the PM is the one used in
 * the Power Consumption section in the data sheet. Some of the clocks in the clock masks
 * used here are disabled by default but have been added to give an easy overview of what
 * clocks are disabled and not.
 */

volatile uint32_t saved_APBCMASK;

void disableWire(void)
{
	saved_APBCMASK = PM->APBCMASK.reg;
	PM->APBCMASK.reg &= ~(PM_APBCMASK_SERCOM0 | PM_APBCMASK_SERCOM1);
}

void enableWire(void)
{
//	PM->APBCMASK.reg |= (PM_APBCMASK_SERCOM0);
	PM->APBCMASK.reg = saved_APBCMASK;
}

void configure8Mhz()
{
	/* Modify PRESCaler value of OSC8M to have 8MHz */
	SYSCTRL->OSC8M.bit.PRESC = SYSCTRL_OSC8M_PRESC_0_Val ;  // recent versions of CMSIS from Atmel changed the prescaler defines

	/* Put OSC8M as source for Generic Clock Generator 0 */
	GCLK->GENDIV.reg = GCLK_GENDIV_ID( GENERIC_CLOCK_GENERATOR_MAIN ) ; // Generic Clock Generator 0
	while ( GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY );

	GCLK->GENCTRL.reg = ( GCLK_GENCTRL_ID( GENERIC_CLOCK_GENERATOR_MAIN ) | GCLK_GENCTRL_SRC_OSC8M | GCLK_GENCTRL_GENEN );
	while ( GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY );

	SystemCoreClock=8000000ul;
	
	SysTick_Config(SystemCoreClock / 1000); // Configure the system ticks to 8Mhz
}

void initializePins()
{
	// Set pullups for all the interrupts
	pinMode(MBUT, INPUT_PULLUP);
	pinMode(UBUT, INPUT_PULLUP);
	pinMode(DBUT, INPUT_PULLUP);
	pinMode(RTC_INT, INPUT_PULLUP); // RTC Interrupt

	// Set LED pin to output.
	pinMode(LED, OUTPUT);

	// RTC square wave VCOM at 1Hz
	pinMode(DISPLAY_EXTMODE, OUTPUT); //VCOM Mode (h=ext l=sw)
	digitalWrite(DISPLAY_EXTMODE, HIGH); // switch VCOM to external
}

void initializeRTC()
{
	// Disable the RTC interrupts for the moment.
	MyDS3232.alarmInterrupt(ALARM_1, false);
	MyDS3232.alarmInterrupt(ALARM_2, false);

	// Attach button and RTC interrupt routine to the pins.
	attachInterrupt(digitalPinToInterrupt(MBUT), buttonISR_M, LOW); // when button B is pressed display
	attachInterrupt(digitalPinToInterrupt(UBUT), buttonISR_U, LOW); // when button C is pressed display
	attachInterrupt(digitalPinToInterrupt(DBUT), buttonISR_D, LOW); // when button C is pressed display battery status
	attachInterrupt(digitalPinToInterrupt(RTC_INT), RTC_int, LOW); // RTC Interrupt

	// Set RTC to interrupt every second for now just to make sure it works.
	// Will finally set to one minute.
	#ifdef EVERY_SECOND
	MyDS3232.setAlarm(ALM1_EVERY_SECOND, 0, 0, 0);
	MyDS3232.alarmInterrupt(ALARM_1, true);
	#endif
	#ifdef EVERY_MINUTE
	MyDS3232.setAlarm(ALM2_EVERY_MINUTE, 0, 0, 0);
	MyDS3232.alarmInterrupt(ALARM_2, true);
	#endif

	// Enable 32Khz output on pin 1
	MyDS3232.writeRTC(RTC_STATUS, MyDS3232.readRTC(RTC_STATUS) | _BV(EN32KHZ));

	// The address used by the DS1307 is also used by other devices (eg
	// MCP3424 ADC). Test for a MCP7941x device first.
	uint8_t addressList[] = {RTCx::MCP7941xAddress};

	// Autoprobe to find a real-time clock.
	MCP7941.autoprobe(addressList, sizeof(addressList));

	// Setup square wave to 1Hz (1 per sec)
	MCP7941.setSQW(RTCx::freq1Hz);
	
	// Set EXTOSC to use the external 32Khz oscillator being passed in.
	MCP7941.setEXTOSC();

	// Ensure the oscillator is running.
	MCP7941.startClock();
	//	MCP7941.stopClock();
}

bool updateDisplay()
{
	// If this is a RTC interrupt then need to
	// get the time and display it.
	if (rtcFired)
	{
		rtcFired = false;

		// Clear the display buffer before writing to the display.
		// Don't need to clear the display as the refresh will
		// write it all.
		display.clearDisplayBuffer();

		watchFace->displayTime();

		displayCalendar();
		
		// Clear the alarm interrupt
#ifdef EVERY_SECOND
		MyDS3232.alarm(ALARM_1);
#endif
#ifdef EVERY_MINUTE
		MyDS3232.alarm(ALARM_2);
#endif

		display.refresh();
	}
	
	// See if middle button fired and woke up the processor.
	if (pinValM)
	{
		// Clear which button was pressed.
		pinValM = false;
		pinValU = false;
		pinValD = false;

		// Display the menu on button press
//		displayMenu();

		// Back from the menu so redisplay the watch face
		rtcFired = true;
		updateDisplay();
	}
}

void setup()
{
	// Reset to processors main clock to be 8Mhz clock.  
	// Allows for very low power consumption during sleep
	configure8Mhz();
	
	// Set up communications
	Wire.begin();
	
	// Set up display
	display.begin();

	disableInterrupts();

	// Setup the pins
	initializePins();

	// Initialize the RTC's
	initializeRTC();
	
	display.clearDisplay();
	display.refresh();

	display.setTextColor(BLACK);
	display.setTextSize(1);
	
	enableInterrupts();

	// Create instance of the watches face
	watchFace = new WatchFace(display, MyDS3232);

	// Display something the first time
	rtcFired = true;
	updateDisplay();
}

void loop()
{
	while (1)
	{
		// Before we sleep set the VCOM to external, the 1Hz VCOM signal
		digitalWrite(DISPLAY_EXTMODE, HIGH); // switch VCOM to external
		
		// Disable SysTick timer (enables delay/time functions).. 
		// causes interrupt that wakes the processor...may be a diff way to disable via clocks...not sure
		SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk);
		
		// Clear the alarm interrupt in the RTC, else we will never wake up from sleep.
		// Very strange happening that only exhibits self when interrupt trigger is LOW.
		// and we want to deepsleep.
		uint8_t stat = MyDS3232.alarm(ALARM_2);

		// Power down the I2C (SERCOM0) to reduce power while sleeping
		disableWire();

		PM->SLEEP.reg = PM_SLEEP_MASK;
		SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

		__DSB();
		__WFI();

		// Power up the I2C (SERCOM0) comms
		enableWire();

		// Enable SysTick IRQ and SysTick Timer
		SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;

		// If we got here we were woken up by interrupt
		digitalWrite(DISPLAY_EXTMODE, LOW); // switch VCOM to software.

		for (int j = 0; j < 4; j++)
		{
			digitalWrite(LED, HIGH);
			delay(100);
			digitalWrite(LED, LOW);
			delay(100);
		}
		
		updateDisplay();
	}
}
