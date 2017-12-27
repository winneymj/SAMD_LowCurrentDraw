/*
 * Sketch1.ino
 *
 * Created: 12/23/2017 6:48:21 PM
 * Author: dell
 */ 
// Button pins

#define LEDX 6
#define MBUTX 4

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

void setup()
{
////	DSU->CTRL.reg = DSU_CTRL_SWRST;
////	while ( (DSU->CTRL.reg & DSU_CTRL_SWRST) && (DSU->STATUSA.reg & DSU_STATUSA_DONE) );	/* Wait for reset to complete */
	//
	//// Software reset the GCLK module to ensure it is re-initialized correctly
	//GCLK->CTRL.reg = GCLK_CTRL_SWRST;
	//while ( (GCLK->CTRL.reg & GCLK_CTRL_SWRST) && (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) );	/* Wait for reset to complete */
//
	//RTC->MODE0.CTRL.reg = RTC_MODE0_CTRL_SWRST;
////	while ( (RTC->MODE0.CTRL.reg & RTC_MODE0_CTRL_SWRST) && (RTC->MODE0.STATUS.reg & RTC_STATUS_SYNCBUSY) );	/* Wait for reset to complete */
//
	//RTC->MODE1.CTRL.reg = RTC_MODE1_CTRL_SWRST;
	////while ( (RTC->MODE1->CTRL.reg & RTC_MODE1_CTRL_SWRST) && (RTC->MODE1->STATUS.reg & RTC_STATUS_SYNCBUSY) );	/* Wait for reset to complete */
////
	//RTC->MODE2.CTRL.reg = RTC_MODE2_CTRL_SWRST;
	////while ( (RTC->MODE2->CTRL.reg & RTC_MODE2_CTRL_SWRST) && (RTC->MODE2->STATUS.reg & RTC_STATUS_SYNCBUSY) );	/* Wait for reset to complete */
//
	//EIC->CTRL.reg = EIC_CTRL_SWRST;
	//while ( (EIC->CTRL.reg & EIC_CTRL_SWRST) && (EIC->STATUS.reg & EIC_STATUS_SYNCBUSY) );	/* Wait for reset to complete */
	
	// Set LED pin to output.
	pinMode(LEDX, OUTPUT);
	
	// CALLS MUST BE IN THIS ORDER to get PMUX setup correct for the button interrupt
	pinMode(MBUTX, INPUT_PULLUP); 
	pinMode(MBUTX, PIO_EXTINT); // This sets up the PMUX.  Without this call the interrupt will not work with min setup.

	attachInterrupt(digitalPinToInterrupt(MBUTX), buttonISR_M, LOW); // when button B is pressed display

}

void loop()
{
	digitalWrite(LEDX, LOW);
	
	for (int j = 0; j < 3; j++)
	{
		digitalWrite(LEDX, HIGH);
		delay(500);
		digitalWrite(LEDX, LOW);
		delay(500);
	}
  
	PM->SLEEP.reg = PM_SLEEP_MASK;
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

	while (1)
	{
		// Disable SysTick timer (enables delay/time functions).. 
		// causes interrupt that wakes the processor...may be a diff way to disable via clocks...not sure
		SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk);

		__DSB();
		__WFI();

		// Enable SysTick IRQ and SysTick Timer
		SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
		
		uint32_t ints = EIC->INTFLAG.reg;
		
		for (int j = 0; j < 4; j++)
		{
			digitalWrite(LEDX, HIGH);
			delay(100);
			digitalWrite(LEDX, LOW);
			delay(100);
		}
	}
}
