/*
   Copyright (c) 2015, Digi International Inc.

   Permission to use, copy, modify, and/or distribute this software for any
   purpose with or without fee is hereby granted, provided that the above
   copyright notice and this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
/********************************************************************
	thermistor.c

	This sample program is for the RCM3400 series controllers with
	prototyping boards.

	Description
	===========
	This program demonstrates using analog input AIN7 to calculate
	temperature for display to STDIO window.

	This example assumes that thermistor you are using is the one
	included in the toolkit.  Values for beta, series resistor and
	resistance at standard temperature are given per part
	specification.

	Given:
	Beta = 3965
	Series resistor on protoboard = 1k ohm
	Resistance at 25 degree C = 3k ohm


	Instructions
	============
	1. Install thermistor lead into AIN7 and the other leadside
		into AGND on the prototyping board.
	2. Compile and run this program.
	3. Apply heat and cold air to the thermistor to observe
		change in temperature in the stdio window.

********************************************************************/
#class auto
#define ADC_SCLKBAUD 115200ul

//---------------------------------------------------------
// second delay
//---------------------------------------------------------
nodebug
void sDelay(unsigned int delay)
{
	auto unsigned long done_time;

	done_time = SEC_TIMER + delay;
   while( (long) (SEC_TIMER - done_time) < 0 );
}

main()
{
	auto float 	Tc,		//temperature celcius
					Tk,		//calculated temperature kelvins
					Tkstd,	//standard temperature kelvins
					Tf,		//temperature farenheit
				 	Bt,		//given thermistor beta			// B=(-alpha*T^2)  ???
					Rs,		//given series resistor
					Rtstd,	//given thermistor resistance at standard temperature
					Draw,		//raw data value
					Gain,		//gain
					Dmax;		//max raw data value

	brdInit();

	//assign variables as float values for calculation
	Tc = 25.0;						//standard temp in Celcius
	Tkstd = Tc + 273.15;  		//convert to Kelvins
	Bt = 3965.0;					//thermistor beta
	Rs = 1000.0;					//series resistor
	Rtstd = 3000.0;				//standard temp resistance
	Dmax = 2047.0;					//max value on ADS7870
	Gain = 1.0;						//actual gain multiplier

	while (1)
	{
		//first-time call of this function will take 1 second to charge up cap
		//use single-ended and gain of 1
	 	Draw = anaIn(7, SINGLE, GAIN_1);

		//calculate temperature in kelvins
		Tk = (Bt*Tkstd) / ( Tkstd * ( log( fabs( (-Draw*Rs) / (Rtstd*(Draw-(Dmax*Gain))) ) ) ) + Bt );

		Tc = Tk - 273.15;				//convert to celcius
		Tf = 1.8*(Tk - 255.37);		//calculate fahrenheit

		printf("Temperature at %.2f C (%.2f F) from data %d\n", Tc, Tf, (int)Draw);

		sDelay(1);
	}
}