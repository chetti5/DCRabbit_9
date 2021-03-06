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
/*****************************************************************************
sflash_erase.c

This program is a handy utility for erasing a serial flash chip.  When it
starts it attempts to initialize a serial flash chip on serial port B.

If one is found then the user can confirm that they want to erase it.

****************************************************************************/

#define SFLASH_DEBUG

#define SF_SPI_CSPORT PEDR
#define SF_SPI_CSSHADOW PEDRShadow
#define SF_SPI_CSDD PEDDR
#define SF_SPI_CSDDSHADOW PEDDRShadow
#define SF_SPI_CSPIN 6

#use "sflash.lib"

char flash_buf[1056];

int main()
{
   int pagenum;

   brdInit();
   sfspi_init();
	if(sf_init())
   {
   	printf("Flash init failed\n");
      exit(-1);
   }
   else
   {
   	printf("Flash init OK\n");
      printf("# of blocks: %d\n", sf_blocks);
      printf("size of block: %d\n", sf_blocksize);
      
      if (sf_blocksize > sizeof flash_buf)
      {
      	printf("Error: update declaration of flash_buf to be %d bytes\n",
      		sf_blocksize);
      	exit(-1);
      }
   }
   
   printf("Enter YES to erase this device: ");
   gets(flash_buf);
   
   if (strcmp(flash_buf, "YES") != 0)
   {
   	printf("Canceled.\n");
   	return 0;
   }
   
   memset(flash_buf, 0xFF, sizeof flash_buf);
   sf_writeRAM(flash_buf, 0, sf_blocksize);
   
   for (pagenum = 0; pagenum < sf_blocks; ++pagenum)
   {
      printf("Clearing page %d...\r", pagenum);
      sf_RAMToPage(pagenum);
   }
   
   printf("\nErasure completed\n");
}




