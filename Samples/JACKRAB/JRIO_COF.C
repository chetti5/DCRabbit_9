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
/************************************************************************

	  Samples\Jackrab\jrio_cof.c

     Program to demonstrate JackRabbit analog input
     cofunction

     This program demonstrates the use of the Jackrabbit analog input 
	  driver as a cofunction.  Connect DA1 to AD0 to provide an input
	  voltage.  When the program runs, it will read the input voltage
     ten times while another costate is executed concurrently.  The
     values will be printed out at the end of the program.

*************************************************************************/
//  Set a default of declaring all local variables "auto" (on stack)
#class auto


void main()
{
	int	values[10], value, i;
	int	count1, count2;

	count1 = count2 = 0;

	jrioInit();								// initialize I/O drivers
	anaOut(1, 512);						// provide input voltage by
												//   connecting DA1 to AD0)

	while (count1<10) {					// read voltage 10 times

		costate {
			waitfordone {
				cof_anaIn(0, &value);		// execute one step
			}
			if (value == -1023) {			// this typically means that AD0 is disconnected
				printf("value = -1023, is AD0 disconnected?\n");
				exit(1);
			}

			if (value > 0) {					// is value "good"?
				values[count1] = value;		//   yes - store it
				count1++;						//   otherwise repeat
			}
		}
		
		costate {
			count2++;							// 2nd costate (does
													//		nothing here)
		}

	}

	anaOut(1, 0);								// turn off output

	for (i=0; i<10; i++)						// print values
		printf("values[%4d] = %6d\n", i, values[i]);
	
	printf("1st costate (cos_anaIn) executed %d times\n", count1);
	printf("2nd costate (nothing)   executed %d times\n", count2);
}

////////////////////////////////////////////////////////////////////////////////
