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
/*******************************************************************

	toggleswitch.c

	This program is used with RCM3000 series controllers
	with prototyping boards.
	
	The sample library, rcm3000.lib, contains a brdInit()
	function which initializes port pins. Modify this
	function to fit your application.

	Description
	===========
	This program uses costatements to detect switches with
	press and release method debouncing. Corresponding LED's,
	DS1 and DS2, will turn on or off.
	
	Instructions
	============
	1. Compile and run this program.
	2. Press and release S2 switch to toggle DS1 LED on/off.
	3. Press and release S3 switch to toggle DS2 LED on/off.

*******************************************************************/
#class auto


#use rcm3000.lib 	//sample library used for this demo

#define DS1 6		//led, port G bit 6
#define DS2 7		//led, port G bit 7
#define S2  1		//switch, port G bit 1
#define S3  0		//switch, port G bit 0

void main()
{
	auto int sw1, sw2, led1, led2;
	
	brdInit();				//initialize board for this demo
	
	led1=led2=1;			//initialize leds to off value 
	sw1=sw2=0;				//initialize switches to false value 

	while (1)
	{
		costate
		{	
			if (BitRdPortI(PGDR, S2))		//wait for switch S2 press
				abort;
			waitfor(DelayMs(50));			//switch press detected if got to here
			if (BitRdPortI(PGDR, S2))		//wait for switch release
			{
				sw1=!sw1;						//set valid switch
				abort;
			}
		}
		
		costate
		{	
			if (BitRdPortI(PGDR, S3))		//wait for switch S3 press
				abort;
			waitfor(DelayMs(50));			//switch press detected if got to here
			if (BitRdPortI(PGDR, S3))		//wait for switch release
			{
				sw2=!sw2;						//set valid switch
				abort;
			}
		}

		costate
		{	// toggle DS1 led upon valid S2 press/release and clear switch
			if (sw1)
			{
				BitWrPortI(PGDR, &PGDRShadow, led1=led1?0:1, DS1);
				sw1=!sw1;
			}
		}
		
		costate
		{	// toggle DS2 upon valid S3 press/release and clear switch
			if (sw2)
			{
				BitWrPortI(PGDR, &PGDRShadow, led2=led2?0:1, DS2);
				sw2=!sw2;
			}
		}

	}
}
