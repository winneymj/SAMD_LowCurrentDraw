/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * This is a bare minimum user application template.
 *
 * For documentation of the board, go \ref group_common_boards "here" for a link
 * to the board-specific documentation.
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to system_init()
 * -# Basic usage of on-board LED and button
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#include <asf.h>

/** Function prototypes */
void prime_number(void);
uint32_t fib(uint32_t n);
void while_one(void);
void configure_gclk_pin(void);
void configure_power_manager(void);
void turn_off_bod33(void);
void fib_test(void);
void switch_gclkgen_to_peripherals(void);
void disable_cache(void);
void wind_click_handle(void);
void extint_configuration(void);

#define MAX_GCLK_ID 0x25

/**
 * \brief Prime number function
 *
 * This function is used to measure power spent to calculate prime numbers.
 */
void prime_number(void)
{
	#define PRIM_NUMS 64
	uint32_t i, d, n;
	uint32_t primes[PRIM_NUMS];


	/* Find prime numbers forever */
	while (1)
	{
		primes[0] = 1;
		for (i = 1; i < PRIM_NUMS;)
		{
			for (n = primes[i - 1] + 1; ;n++)
			{
				for (d = 2; d <= n; d++)
				{
					if (n == d)
					{
						primes[i] = n;
						goto nexti;
					}
					if (n%d == 0) break;
				}
			}
			nexti:
			i++;
		}
	}
}


/**
 * \brief Recursive Fibonacci function
 * 
 * Recursive function to test power consumption during calculation of
 * Fibonacci numbers.
 */
uint32_t fib(uint32_t n)
{
	if(n < 2){
		return 1;
	}
	return fib(n-1) + fib(n-2);
}

void fib_test(void)
{
	while(1) {
		/** 46 is the largest achievable Fibonacci number with 32-bits */
		fib(46);
	}
}

/**
 * \brief While 1 loop test function
 */
void while_one(void)
{
	while (1) {

	}
}

/**
 *  \brief Output the clock coming out of Generic clock generator 0 to pin
 *
 *  This function will output the clock being used as CPU clock (the main clock) to
 *  Pin PB14. This pin is on the EXT 2 header, pin 9 on the SAM D20 Explained Pro board.
 *  In order for the clock to be output on the CONF_CLOCK_GCLK_0_OUTPUT_ENABLE define in conf_clock.h
 *  must be set to true. When CONF_CLOCK_GCLK_0_OUTPUT_ENABLE is set to true the SAM D20 will
 *  draw more current independent of the pin setup.
 */
void configure_gclk_pin(void)
{
	struct system_pinmux_config config_port;
	system_pinmux_get_config_defaults(&config_port);
//	config_port.mux_position = MUX_PB14H_GCLK_IO0;
	config_port.mux_position = MUX_PA14H_GCLK_IO0;
	config_port.direction = SYSTEM_PINMUX_PIN_DIR_OUTPUT;
	config_port.input_pull = SYSTEM_PINMUX_PIN_PULL_NONE;
//	system_pinmux_pin_set_config(PIN_PB14H_GCLK_IO0, &config_port);
	system_pinmux_pin_set_config(PIN_PA14H_GCLK_IO0, &config_port);
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

/*
 * \brief Function to configure Power Manager (PM)
 * 
 * This function configures the PM. The PM controls what synchronous
 * clocks clocks are running and not. This configuration of the PM is the one used in
 * the Power Consumption section in the data sheet. Some of the clocks in the clock masks
 * used here are disabled by default but have been added to give an easy overview of what
 * clocks are disabled and not.
 */
void configure_power_manager(void)
{
	system_apb_clock_clear_mask(
			SYSTEM_CLOCK_APB_APBA,
				(PM_APBAMASK_WDT \
				| PM_APBAMASK_PAC0 \
				| PM_APBAMASK_EIC \
				| PM_APBAMASK_GCLK \
				| PM_APBAMASK_RTC \
				/* These clocks should remain enabled on this bus
				| PM_APBAMASK_SYSCTRL \
				| PM_APBAMASK_PM \
				| PM_APBAMASK_RTC \
				*/
				));	

	system_apb_clock_clear_mask(
			SYSTEM_CLOCK_APB_APBB,
				(PM_APBBMASK_PAC1 \
				| PM_APBBMASK_PORT \
				| PM_APBBMASK_DSU \
				| PM_APBBMASK_DMAC \
				| PM_APBBMASK_USB  \
				| PM_APBBMASK_NVMCTRL \
				/* These clocks should remain enabled on this bus

				*/
				));

	system_apb_clock_clear_mask(
			SYSTEM_CLOCK_APB_APBC,
				(PM_APBCMASK_ADC \
				| PM_APBCMASK_PAC2 \
				| PM_APBCMASK_DAC \
				| PM_APBCMASK_AC \
				| PM_APBCMASK_TC7 \
				| PM_APBCMASK_TC6 \
				| PM_APBCMASK_TC5 \
				| PM_APBCMASK_TC4 \
				| PM_APBCMASK_TC3 \
				| PM_APBCMASK_TCC2 \
				| PM_APBCMASK_TCC1 \
				| PM_APBCMASK_TCC0 \
				| PM_APBCMASK_SERCOM5 \
				| PM_APBCMASK_SERCOM4 \
				| PM_APBCMASK_SERCOM3 \
				| PM_APBCMASK_SERCOM2 \
				| PM_APBCMASK_SERCOM1 \
				| PM_APBCMASK_SERCOM0 \
				| PM_APBCMASK_EVSYS \
				| PM_APBCMASK_I2S \
				));

	system_apb_clock_set_divider(
			SYSTEM_CLOCK_APB_APBA,
			SYSTEM_MAIN_CLOCK_DIV_4);

	system_ahb_clock_clear_mask(
				(PM_AHBMASK_USB \
				| PM_AHBMASK_DSU \
				| PM_AHBMASK_HPB1 \
				| PM_AHBMASK_HPB2 \
				| PM_AHBMASK_DMAC \
				/* These clocks should remain enabled on this bus
				| PM_AHBMASK_HPB1 \
				| PM_AHBMASK_HPB2 \
				| PM_AHBMASK_HPB0 \
				| PM_AHBMASK_NVMCTRL \ 
				*/
			));
}

void disable_cache(void)
{
	NVMCTRL->CTRLB.bit.CACHEDIS = 1;
	//Readmode settings : 0-3
	NVMCTRL->CTRLB.bit.READMODE = 0;
}


/**
 * This function switches all GCLK outputs to a generator that is 
 * configured with a source oscillator. The exception is GCLK_DFLL_REF which is kept running.
 * This function must be called after system_clock_init(). system_clock_init() will normaly
 * be claied in system_init() in asf. If it is not the change will not be aplied. 
 * In ASF versions later than 3.19 this will be done automaticlay when system_clock_init() is caled
 * and this function should not be called again as it will only be waisting time.
 */
void switch_gclkgen_to_peripherals(void)
{
	for (int gclk_id = 1; gclk_id < MAX_GCLK_ID; gclk_id++) {
		GCLK->CLKCTRL.reg = (gclk_id << GCLK_CLKCTRL_ID_Pos) | GCLK_CLKCTRL_GEN_GCLK7;
	}
}

#define GPIO_PIN_EXTINT				PIN_PA09
#define GPIO_PIN_EXTINT_ACTIVE		true
#define GPIO_PIN_EXTINT_INACTIVE    !GPIO_PIN_EXTINT_ACTIVE
#define GPIO_PIN_EXTINT_GPIO        GPIO_PIN_EXTINT

#define WIND_SPEED_INTERRUPT_PIN	PIN_PA28A_EIC_EXTINT8
#define WIND_SPEED_INTERRUPT_MUX	MUX_PA28A_EIC_EXTINT8
#define WIND_SPEED_INTERRUPT_LINE	8

void wind_click_handle(void)
{
	// g_wind_speed_callback.clicks++;
}

/* Configure external interrupt controller */
void extint_configuration(void)
{
	struct extint_chan_conf wind_speed_chan;
	
	extint_chan_get_config_defaults(&wind_speed_chan);
	
	wind_speed_chan.gpio_pin		= WIND_SPEED_INTERRUPT_PIN;
	wind_speed_chan.gpio_pin_mux	= WIND_SPEED_INTERRUPT_MUX;
	wind_speed_chan.gpio_pin_pull	= EXTINT_PULL_UP;
	wind_speed_chan.detection_criteria = EXTINT_DETECT_LOW;
	
	extint_chan_set_config(WIND_SPEED_INTERRUPT_LINE, &wind_speed_chan);
	extint_register_callback(wind_click_handle, WIND_SPEED_INTERRUPT_LINE, EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(WIND_SPEED_INTERRUPT_LINE, EXTINT_CALLBACK_TYPE_DETECT);
	
}

#define LED0_PIN                  PIN_PA09
#define LED0_ACTIVE               true
#define LED0_INACTIVE             !LED0_ACTIVE
#define LED0_GPIO                 LED0_PIN

int main (void)
{
	system_init();
	delay_init();

	struct port_config pin;
	port_get_config_defaults(&pin);
	pin.direction = PORT_PIN_DIR_OUTPUT;

	port_pin_set_config(LED0_PIN, &pin);
	port_pin_set_output_level(LED0_PIN, LED0_INACTIVE);

//	while (true) {
		for (int i = 0; i < 5; i++) {
			port_pin_toggle_output_level(LED0_PIN);
			delay_s(2);
		}

		//for (int i = 0; i < 50; i++) {
			//port_pin_toggle_output_level(LED0_PIN);
			//delay_ms(100);
		//}
//
		//for (int i = 0; i < 5000; i++) {
			//port_pin_toggle_output_level(LED0_PIN);
			//delay_cycles(100);
		//}
//	}	
	

	switch_gclkgen_to_peripherals();

	turn_off_bod33();
	
	configure_power_manager();
	
	
	//#if 0
	//system_set_sleepmode(SYSTEM_SLEEPMODE_IDLE_0);
	//system_sleep();
	//#endif
	//
	//#if 0
	//system_set_sleepmode(SYSTEM_SLEEPMODE_IDLE_1);
	//system_sleep();
	//#endif
	//
	//#if 1
	//system_set_sleepmode(SYSTEM_SLEEPMODE_IDLE_2);
	//system_sleep();
	//#endif
	
	#if 1
	system_set_sleepmode(SYSTEM_SLEEPMODE_STANDBY);
	system_sleep();
	#endif
	
	//#if 0
	//prime_number();
	//#endif
//
	//#if 0
	//fib_test();
	//#endif
	while_one();
}


/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to system_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
//#include <asf.h>
//
//int main (void)
//{
	//system_init();
//
	///* Insert application code here, after the board has been initialized. */
//}


