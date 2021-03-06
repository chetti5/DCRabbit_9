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
/*******************************************************************************

        telnet.c

        This program takes anything that comes in on PORT and sends it
        out serial port B (can be configured, see  #define SERIAL_PORT
        below).  It uses digital input IN00 as a signal that the
        TCP/IP connection should be closed and a high current output
        OUT00 as an indication that there is an open connection. You could
        change the digital input and output to something that you need in
        your application design.

        It is important to note, once you established a TCP/IP connection
        that it is difficult to detect a broken connection when there is no
        data moving across the link.  It is recommended that you occasionally
        send data across the link or implement timeouts to handle this situation.

        This program is based on the "samples/RCM2100/ethcore2.c" program.

*******************************************************************************/
#class auto 			// Change default storage class for local variables: on the stack

/***********************************
 * Configuration                   *
 * -------------                   *
 * All fields in this section must *
 * be altered to match your local  *
 * network settings.               *
 ***********************************/
/*
 * Pick the predefined TCP/IP configuration for this sample.  See
 * LIB\TCPIP\TCP_CONFIG.LIB for instructions on how to set the
 * configuration.
 */
#define TCPCONFIG 1


#define PORT 23		// User macro for TCP port to use for processing

/*
 *		MACRO used to indicate when a key has been pressed.
 */

#define KEY_PRESSED	1

/*
 *		TIMEOUT is the number of seconds of no activity before closing
 *		the connection.
 *
 */

#define TIMEOUT		60


/*
 *		Led control MACRO's
 *
 */

#define LED_DELAY			100
#define BINARY_COUNTER	0
#define RESET_LEDS   	1

/*
 *		High current output channel that is used to indicate
 *    that there is an open network connection. Default setting
 *    is high current output OUT0.
 */
#define DIG_OUTPUT	0


/*
 *		Serial port Settings
 *		Serial PORT 1=A, 2=B, 3=C, 4=D
 */

#define SERIAL_PORT	2
#define BAUD_RATE		19200
#define INBUFSIZE		127
#define OUTBUFSIZE 	127


/********************************
 * End of configuration section *
 ********************************/

///////////////////////////////////////////////////////////////////////

#memmap xmem
#use "dcrtcp.lib"


#if (SERIAL_PORT==1)

	#define serXopen  	serAopen
	#define serXread  	serAread
	#define serXwrite 	serAwrite
	#define serXclose 	serAclose
	#define serXwrFlush	serAwrFlush
	#define serXrdFlush	serArdFlush
	#define serXwrFree	serAwrFree

	#define AINBUFSIZE	INBUFSIZE
	#define AOUTBUFSIZE	OUTBUFSIZE

#elif (SERIAL_PORT==2)

	#define serXopen  	serBopen
	#define serXread  	serBread
	#define serXwrite 	serBwrite
	#define serXclose 	serBclose
	#define serXwrFlush	serBwrFlush
	#define serXrdFlush	serBrdFlush
	#define serXwrFree	serBwrFree

	#define BINBUFSIZE	INBUFSIZE
	#define BOUTBUFSIZE	OUTBUFSIZE

#elif (SERIAL_PORT==3)

	#define serXopen  	serCopen
	#define serXread  	serCread
	#define serXwrite 	serCwrite
	#define serXclose 	serCclose
	#define serXwrFlush	serCwrFlush
	#define serXrdFlush	serCrdFlush
	#define serXwrFree	serCwrFree

	#define CINBUFSIZE	INBUFSIZE
	#define COUTBUFSIZE	OUTBUFSIZE

#elif (SERIAL_PORT==4)

	#define serXopen  	serDopen
	#define serXread  	serDread
	#define serXwrite 	serDwrite
	#define serXclose 	serDclose
	#define serXwrFlush	serDwrFlush
	#define serXrdFlush	serDrdFlush
	#define serXwrFree	serDwrFree

	#define DINBUFSIZE	INBUFSIZE
	#define DOUTBUFSIZE	OUTBUFSIZE

#endif

///////////////////////////////////////////////////////////////////////

/*
 *
 * Function to strip out telnet commands
 *
 */

int strip_telnet_cmds(char *ptr, int bytes_read) {

	int index;

	//write the NULL terminator at the end of the data string
	ptr[bytes_read] = '\0';

	index = 0;
	// Check for telnet command header, if none exit without altering the buffer
	if(ptr[index] == 0xff) {

		// Scan data looking for telnet commands
		while(ptr[index] == 0xff && index < bytes_read) {

			switch(ptr[index+1])
			{
				case 251:
				case 252:
				case 253:
				case 254:
					index += 3;
					break;

				case 255:
					break;

				default:
					index += 2;
					break;
			}
		}
		if(index >= bytes_read)
		{	//return 0 for no valid ascii data recieved in this packet
			return(0);
		}
		//overwrite the leading telnet commands with the valid ascii message
		strcpy(ptr, ptr+index);
	}

	//return with length of buffer
	return(strlen(ptr));
}

/*
 *		leds(void)
 *		This function displays a binary count on the LED's.
 *
 */

void leds(int mode)
{
	static int led, value, counter, mask;
	static unsigned long led_delay;

	if(mode == RESET_LEDS)
	{
		led_delay = (MS_TIMER+LED_DELAY);
	   counter = 0;
	   for(led = 1; led <= 7; led++)
		{
			ledOut(led, 0);
		}
	}
	else if((mode == BINARY_COUNTER) && (long) (MS_TIMER - led_delay) >= 0 )
	{
		for(led = 1, mask = 0x01; led <= 7; led++)
		{
			// check what the value should be for the given led
			mask = mask << 1;
			value = counter & mask;

			//update led
			if(value == 0)
			{	// turn led OFF
				ledOut(led, 0);
			}
			else
			{	// turn led ON
				ledOut(led, 1);
			}
		}
		// check if binary counter has reached 32, if so reset the counter
		if(++counter == 128)
		{
			counter = 0;
		}
		// set the interval for the next led update
		led_delay = (MS_TIMER+LED_DELAY);
	}
}

/*
 *		debounce_keys(void)
 *		This function checks for a valid keypress
 *
 */

int debounce_key(void)
{
	auto int status;
	static int state;
	static unsigned long debounce_period;

	// initialize state machine during system initialization
	#GLOBAL_INIT {state=0;}

	status = !KEY_PRESSED;
	switch(state)
	{
		case 0:
		   // wait for the switch to be pressed
			if(!digIn(0))
			{  //set debounce period
				state++;
				debounce_period = MS_TIMER + 50; // debounce period = 50ms
			}
			break;

		case 1:
			// check if switch is still being pressed, if not reset state machine
			if((long) (MS_TIMER-debounce_period) >= 0 )
			{
				if(!digIn(0))
				{
					state++;
					status = KEY_PRESSED;
				}
				else
				{
					state = 0;
				}
			}
			break;

		case 2:
		   // wait for the switch to be released
			if(digIn(0))
			{
				state++;
				debounce_period = MS_TIMER + 100;	// debounce period = 100ms
			}
			break;

		case 3:
			// verify that the switch is still released after debounce period
			if((long)(MS_TIMER-debounce_period) >= 0 )
			{
				if(digIn(0))
				{
					state = 0;
				}
				else
				{
					debounce_period = MS_TIMER + 20;	// debounce period = 20ms
				}
			}
			break;

		default: // should never get here!
			state = 0;
			break;
	}
	return(status);
}

/*
 *		main()
 *
 *		This function drives the state machine that manages
 *		the TCP->serial connection.
 *
 *		The INIT state sets a socket up for listening.
 *		The LISTEN state checks for a connection and sets
 *			up the data structures for an open socket.
 *		The OPEN state transfers as much bytes as we have
 *			buffer space from serial to TCP or vice-versa.
 *		The WAITCLOSED state waits for the socket to be
 *			available again.
 *
 */

#define CONNECTION_INIT			0
#define CONNECTION_LISTEN		1
#define CONNECTION_OPEN			2
#define CONNECTION_CLOSED		3

void main()
{
	int state;
	long timeout;
	int bytes_read;

	static char buffer[64];
	static tcp_Socket socket;

	brdInit();
	digOutConfig(0x0000); // Configure as sinking type outputs

	sock_init();						// initialize DCRTCP

	serXopen(BAUD_RATE);				// set up serial port

	tcp_reserveport(PORT);			// set up PORT for SYN Queueing
											// which will hold a pending connection
											// until we are ready to process it.
	state=CONNECTION_INIT;

	while(1) {
		if(state==CONNECTION_OPEN) {
			if(debounce_key()) {
				sock_close(&socket);
				state=CONNECTION_CLOSED;
			}
		}
		/*
		 *		Make sure that the connection hasn't closed on us.
		 *
		 */

		if(tcp_tick(&socket)==0 && state!=CONNECTION_INIT) {
				sock_close(&socket);
				state=CONNECTION_CLOSED;
		}

		switch(state) {
			case CONNECTION_INIT:
				tcp_listen(&socket,PORT,0,0,NULL,0);
				state=CONNECTION_LISTEN;
				break;

			case CONNECTION_LISTEN:
				if(sock_established(&socket)) {
					state=CONNECTION_OPEN;
					timeout=SEC_TIMER+TIMEOUT;
					serXrdFlush();
					serXwrFlush();
					leds(RESET_LEDS);
					digOut(DIG_OUTPUT, 1);
				}
				break;

			case CONNECTION_OPEN:

				/*
				 *		close the socket on a timeout
				 *
				 */
				if((long) (SEC_TIMER-timeout) >= 0) {
					sock_close(&socket);
					state=CONNECTION_CLOSED;
					break;
				}

				/*
				 *		read as many bytes from the socket as we have
				 *		room in the serial buffer. Also strip out the
				 *    telnet commands.
				 */
				bytes_read = 0;
				if(sock_bytesready(&socket) != -1)
				{
					bytes_read=sock_fastread(&socket, buffer, min(sizeof(buffer), serXwrFree()));
					bytes_read=strip_telnet_cmds(buffer, bytes_read);
				}

				/*
				 *		close the socket on an error
				 *
				 */

				if(bytes_read<0) {
					sock_close(&socket);
					state=CONNECTION_CLOSED;
					break;
				}

				/*
				 *		copy any bytes that we read
				 *
				 */

				if(bytes_read > 0) {
					timeout=SEC_TIMER+TIMEOUT;
					serXwrite(buffer, bytes_read);
				}

				/*
				 *		read as many bytes from the serial port as we
				 *		have room in the socket buffer.
				 *
				 */

				bytes_read=serXread(buffer,min(sizeof(buffer),sock_tbleft(&socket)),100);
				if(bytes_read>0) {
					timeout=SEC_TIMER+TIMEOUT;
					if(sock_fastwrite(&socket,buffer,bytes_read)<0) {
						sock_close(&socket);
						state=CONNECTION_CLOSED;
					}
				}
				leds(BINARY_COUNTER);
				break;

			case CONNECTION_CLOSED:
				serXrdFlush();
				serXwrFlush();
				sprintf(buffer, "\n\rConnection closed\n\r");
				serXwrite(buffer, strlen(buffer));
				leds(RESET_LEDS);
				digOut(DIG_OUTPUT, 0);
				state=CONNECTION_INIT;
				break;
		}
	}
}

