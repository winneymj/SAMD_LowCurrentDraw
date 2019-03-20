/*
 * Sketch1.ino
 *
 * Created: 12/23/2017 6:48:21 PM
 * Author: dell
 * Bootloader at C:\Users\dell\AppData\Local\Arduino15\packages\MattairTech_Arduino\hardware\samd\1.6.17-beta-b0\bootloaders\zero\binaries\sam_ba_MT_D21E_rev_B_SAMD21E18A.bin
 *
 */ 
// Button pins
#include <Time.h>         //http://www.arduino.cc/playground/Code/Time
#include <TimeLib.h>         //http://www.arduino.cc/playground/Code/Time
#include <Wire.h>
#include <DS3232RTC.h>    // http://github.com/JChristensen/DS3232RTC
#include <RTCx.h>         // https://github.com/stevemarple/RTCx
#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>
#include <Watch_Menu.h>
#include "defs.h"
#include "watchface.h"
#include "MainMenu.h"
#include "GlobalSettings.h"

#ifndef __AVR__
#include <SPI.h>
#endif
	
Adafruit_SharpMem display(DISPLAY_SCK, DISPLAY_MOSI, DISPLAY_SS, 128, 128);
DS3232RTC ds3232RTC;
RTCx MCP7941;
WatchFace watchFace(display, ds3232RTC);
MainMenu watchMenu;

volatile boolean rtcFired = false; //variables in ISR need to be volatile
volatile boolean pinValM = false;
volatile boolean pinValD = false;
volatile boolean pinValU = false;
volatile boolean battCharging = false;
volatile boolean flipflop = false;

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
	rtcFired = true;
	enableInterrupts();
}

void BattCharging_int_high() // Battery is charged
{
	// See description on BattCharging_int_low()
	disableInterrupts();
	attachInterrupt(digitalPinToInterrupt(BATTERY_STATUS), BattCharging_int_low, LOW); // Generate interrupt when charging
	battCharging = false;
//	digitalWrite(LED, LOW);
	enableInterrupts();
	//digitalWrite(RTC_INT, LOW); // Generate interrupt to wake up and update the battery icon
	//digitalWrite(RTC_INT, HIGH); // Generate interrupt to wake up and update the battery icon
}

void BattCharging_int_low() // Battery is charging
{
	disableInterrupts();
	// This may look strange but this is to overcome the battery status pin pulling the interrupt
	// pin LOW.  This type of interrupt will continue to fire and locks the processor.  To handle
	// this I have removed the interrupt and changed it to a HIGH level.  While the level is LOW
	// no interrupt occurs.
	attachInterrupt(digitalPinToInterrupt(BATTERY_STATUS), BattCharging_int_high, HIGH); // Generate interrupt when charging
	battCharging = true;
//	digitalWrite(LED, HIGH);
	enableInterrupts();
	//digitalWrite(RTC_INT, LOW); // Generate interrupt to wake up and update the battery icon
	//digitalWrite(RTC_INT, HIGH); // Generate interrupt to wake up and update the battery icon
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

/**
 * \brief Function to turn of the BOD33 detector.
 *
 * The Configuration described in the Power Consumption section in the data sheet
 * has the BOD33 off this function turns the BOD33 off.
 */

void turn_off_bod33(void)
{
	SYSCTRL->BOD33.reg = 0;
}

boolean isBatteryCharging()
{
	return battCharging;
}

void configureInternalDFLL()
{
  /* Do not allow to run in standby */ 
  SYSCTRL->DFLLCTRL.bit.RUNSTDBY = 0;

  /* Enable the DFLL */
  SYSCTRL->DFLLCTRL.reg |= SYSCTRL_DFLLCTRL_ENABLE ;
  while ( (SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY) == 0 );
  
  /** change the NVM controllers flash wait states according to the table in the SAMD21 datasheet */
  NVMCTRL->CTRLB.bit.RWS = 3;
	
}

//void configure8Mhz()
//{
  ///* configure internal 8MHz oscillator to run without prescaler */
  //SYSCTRL->OSC8M.bit.PRESC = 0;
  //SYSCTRL->OSC8M.bit.ONDEMAND = 0;
  //SYSCTRL->OSC8M.bit.RUNSTDBY = 0;
  //SYSCTRL->OSC8M.bit.ENABLE = 1;
  //while (!(SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_OSC8MRDY)) {}
//
  //#if CLOCK_USE_PLL
  ///* reset the GCLK module so it is in a known state */
  //GCLK->CTRL.reg = GCLK_CTRL_SWRST;
  //while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {}
//
  ///* setup generic clock 1 to feed DPLL with 1MHz */
  //GCLK->GENDIV.reg = (GCLK_GENDIV_DIV(8) | GCLK_GENDIV_ID(1));
  //GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSC8M | GCLK_GENCTRL_ID(1));
  //GCLK->CLKCTRL.reg = (GCLK_CLKCTRL_GEN(1) | GCLK_CLKCTRL_ID(1) |	GCLK_CLKCTRL_CLKEN);
  //while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {}
//
  ///* Calculate LDRFRAC and LDR */
  //uint32_t reference_divider = 1;
  //uint32_t reference_frequency = 1000000;
  //uint32_t output_frequency = 48000000;
  //
  //uint32_t refclk = reference_frequency;
  //refclk = refclk / reference_divider;
  //uint32_t tmpldr = (output_frequency << 4) / refclk;
  //uint8_t tmpldrfrac = tmpldr & 0x0f;
  //tmpldr = (tmpldr >> 4) - 1;
//
  ///* enable PLL */
  //SYSCTRL->DPLLRATIO.reg = (SYSCTRL_DPLLRATIO_LDR(tmpldr));
  //SYSCTRL->DPLLCTRLB.reg = (SYSCTRL_DPLLCTRLB_REFCLK_GCLK);
  //SYSCTRL->DPLLCTRLA.reg = (SYSCTRL_DPLLCTRLA_ENABLE);
  //SYSCTRL->DPLLCTRLA.bit.ONDEMAND = 0;
  //SYSCTRL->DPLLCTRLA.bit.RUNSTDBY = 0;
  //while(!(SYSCTRL->DPLLSTATUS.reg & (SYSCTRL_DPLLSTATUS_CLKRDY | SYSCTRL_DPLLSTATUS_LOCK))) {}
//
  ///* select the PLL as source for clock generator 0 (CPU core clock) */
  //GCLK->GENDIV.reg =  (GCLK_GENDIV_DIV(CLOCK_PLL_DIV) | GCLK_GENDIV_ID(0));
  //GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_DPLL96M | GCLK_GENCTRL_ID(0));
  //
  ///** change the NVM controllers flash wait states according to the table in the SAMD21 datasheet */
  //NVMCTRL->CTRLB.bit.RWS = 3;
//
  //#else /* do not use PLL, use internal 8MHz oscillator directly Generic Clock Generator 0 */
  //GCLK->GENDIV.reg =  (GCLK_GENDIV_DIV(GENERIC_CLOCK_GENERATOR_MAIN) | GCLK_GENDIV_ID(0));
  //GCLK->GENCTRL.reg = (GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSC8M | GCLK_GENCTRL_ID(0));
  //#endif
//
  ///* make sure we synchronize clock generator 0 before we go on */
  //while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {}
//}

void initializePins()
{
	// Set pullups for all the interrupts
	pinMode(MBUT, INPUT_PULLUP);
	pinMode(UBUT, INPUT_PULLUP);
	pinMode(DBUT, INPUT_PULLUP);
	pinMode(RTC_INT, INPUT_PULLUP); // RTC Interrupt
	pinMode(BATTERY_STATUS, INPUT_PULLUP); // Battery status
	pinMode(POWER_SENSE, INPUT); // Sense power
	pinMode(BATTERY_LEVEL, INPUT); // Battery Level

	// Set LED pin to output.
	pinMode(LED, OUTPUT);

	// RTC square wave VCOM at 1Hz
	pinMode(DISPLAY_EXTMODE, OUTPUT); //VCOM Mode (h=ext l=sw)
	digitalWrite(DISPLAY_EXTMODE, HIGH); // switch VCOM to external
}

void enableRTC(const bool enable)
{
#ifdef EVERY_SECOND
	ds3232RTC.setAlarm(ALM1_EVERY_SECOND, 0, 0, 0);
	ds3232RTC.alarmInterrupt(ALARM_1, enable);
#endif
#ifdef EVERY_MINUTE
	ds3232RTC.setAlarm(ALM2_EVERY_MINUTE, 0, 0, 0);
	ds3232RTC.alarmInterrupt(ALARM_2, enable);
#endif
}

void initializeRTC()
{
	// Disable the RTC interrupts for the moment.
	ds3232RTC.alarmInterrupt(ALARM_1, false);
	ds3232RTC.alarmInterrupt(ALARM_2, false);

	// Attach button and RTC interrupt routine to the pins.
	attachInterrupt(digitalPinToInterrupt(MBUT), buttonISR_M, LOW); // when button B is pressed display
	attachInterrupt(digitalPinToInterrupt(UBUT), buttonISR_U, LOW); // when button C is pressed display
	attachInterrupt(digitalPinToInterrupt(DBUT), buttonISR_D, LOW); // when button C is pressed display battery status
	attachInterrupt(digitalPinToInterrupt(RTC_INT), RTC_int, LOW); // RTC Interrupt
	attachInterrupt(digitalPinToInterrupt(BATTERY_STATUS), BattCharging_int_low, LOW); // Generate interrupt when charging

	// Set RTC to interrupt every second for now just to make sure it works.
	// Will finally set to one minute.
	enableRTC(true);

	// Enable 32Khz output on pin 1
	ds3232RTC.writeRTC(RTC_STATUS, ds3232RTC.readRTC(RTC_STATUS) | _BV(EN32KHZ));

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

void sleepProcessor()
{
	// Clear the alarm interrupt in the RTC, else we will never wake up from sleep.
	// Very strange happening that only exhibits self when interrupt trigger is LOW.
	// and we want to deepsleep.
#ifdef EVERY_SECOND
	ds3232RTC.alarm(ALARM_1);
#endif
#ifdef EVERY_MINUTE
	ds3232RTC.alarm(ALARM_2);
#endif
//	uint8_t stat = ds3232RTC.alarm(ALARM_2);
	// Power down the I2C (SERCOM0) to reduce power while sleeping
	disableWire();

	PM->SLEEP.reg = PM_SLEEP_MASK;
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

	__DSB();
	__WFI();

	// Power up the I2C (SERCOM0) comms
	enableWire();
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

		// See if inverted and set display
		watchFace.invert(GlobalSettings::_inverted);
		
		watchFace.displayTime();

		watchFace.displayCalendar();
		
		// Clear the alarm interrupt
#ifdef EVERY_SECOND
		ds3232RTC.alarm(ALARM_1);
#endif
#ifdef EVERY_MINUTE
		ds3232RTC.alarm(ALARM_2);
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
		
		// Disable the RTC while we are in the menu
		enableRTC(false);

		// Display the menu on button press
		watchMenu.draw();

		// Point to top level menu
		MainMenu::_currentMenu = MainMenu::_menu;
		MainMenu::_setting = {0, 0};

		// Enable the RTC now we are back to the watch face
		enableRTC(true);
		
		// Back from the menu so redisplay the watch face
		rtcFired = true;
		updateDisplay();
	}
}

void setup()
{
	Serial.begin(115200);
//	delay(10000);
	
	turn_off_bod33();
	configureInternalDFLL();
	
	// Set up communications
	Wire.begin();

	// Set up display
	display.begin();

	disableInterrupts();

	// Setup the pins
	initializePins();

	// Initialize the RTC's
	initializeRTC();
	
	// Set display Rotation
	display.setRotation(2);
	
	// Clear display
	display.clearDisplay();
	display.refresh();

	display.setTextColor(BLACK);
	display.setTextSize(1);

	// Initialise the display menu
	watchMenu.initialize();
	
	enableInterrupts();

	// Display something the first time
	rtcFired = true;
	updateDisplay();
}

void loop()
{
	while (1)
	{
//		digitalWrite(LED, HIGH);
Serial.println("BS");
		// Before we sleep set the VCOM to external, the 1Hz VCOM signal
		digitalWrite(DISPLAY_EXTMODE, HIGH); // switch VCOM to external

#ifdef SLEEP_PROCESSOR
		// Disable SysTick timer (enables delay/time functions)..
		// causes interrupt that wakes the processor...may be a diff way to disable via clocks...not sure
		SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk);

		// Sleep and wait for interrupt from buttons or RTC
		sleepProcessor();

		// Enable SysTick IRQ and SysTick Timer
		SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
#else
		delay(1000);
#endif		
		// If we got here we were woken up by interrupt
		digitalWrite(DISPLAY_EXTMODE, LOW); // switch VCOM to software.

		//for (int j = 0; j < 4; j++)
		//{
			//digitalWrite(LED, HIGH);
			//delay(100);
			//digitalWrite(LED, LOW);
			//delay(100);
		//}
		
//		digitalWrite(LED, LOW);

		updateDisplay();
	}
}
